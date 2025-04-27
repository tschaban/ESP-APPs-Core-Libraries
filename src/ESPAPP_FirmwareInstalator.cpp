#include "ESPAPP_FirmwareInstalator.h"

// Define static constants
const char *ESPAPP_FirmwareInstalator::TEMP_DIR = "tmp";
const char *ESPAPP_FirmwareInstalator::CONFIG_FILENAME = "configuration.json";

ESPAPP_FirmwareInstalator::ESPAPP_FirmwareInstalator(ESPAPP_Core *_System)
{
    this->System = _System;
    this->Downloader = new ESPAPP_FileDownloader(_System);
}

ESPAPP_FirmwareInstalator::~ESPAPP_FirmwareInstalator()
{
    if (this->Downloader)
    {
        delete this->Downloader;
    }
}

ESPAPP_INSTALL_STATUS ESPAPP_FirmwareInstalator::install(const char *configUrl, InstallationStats &stats)
{
    // Initialize stats
    stats.totalFiles = 0;
    stats.downloadedFiles = 0;
    stats.failedFiles = 0;

#ifdef DEBUG
    this->System->Debugger->printInformation(F("Starting firmware installation"), F("INSTALLER"));
    this->System->Debugger->printBulletPoint(F("Configuration URL: "));
    this->System->Debugger->printValue(configUrl);

    // Show available space on filesystem
    this->System->Debugger->printBulletPoint(F("Available space: "));
    this->System->Debugger->printValue(this->System->Flash->fileSystem.totalBytes() - this->System->Flash->fileSystem.usedBytes());
    this->System->Debugger->printValue(F(" bytes"));

    // Record start time for the entire installation
    unsigned long installStartTime = millis();
#endif
// @TODO change the messages saved to just an error #
    // Download the configuration file
    char configPath[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
    this->System->Flash->getPathToFile(configPath, TEMP_DIR, CONFIG_FILENAME);

    ESPAPP_DOWNLOAD_STATUS downloadStatus = this->Downloader->downloadFile(
        configUrl, TEMP_DIR, CONFIG_FILENAME);

    if (downloadStatus != DOWNLOAD_SUCCESS)
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Failed to download configuration file: "), F("INSTALLER"));
        this->System->Debugger->printValue(this->Downloader->getStatusText(downloadStatus));
#endif
        this->System->Message->addMessage("Firmware components installation failed: Failed to download configuration file");
        return INSTALL_ERROR_CONFIG_DOWNLOAD;
    }

    // Open and parse the configuration file
    File configFile;
    if (!this->System->Flash->openFile(configFile, ESPAPP_OPEN_FILE_READING, configPath))
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Failed to open configuration file"), F("INSTALLER"));
#endif
        this->System->Message->addMessage("Firmware components installation failed: Failed to open configuration file");
        return INSTALL_ERROR_CONFIG_PARSE;
    }

    // Determine file size for JSON buffer
    size_t fileSize = configFile.size();
    if (fileSize > 16384)
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Configuration file too large"), F("INSTALLER"));
#endif
        configFile.close();
        this->System->Message->addMessage("Firmware components installation failed: Configuration file too large");
        return INSTALL_ERROR_CONFIG_PARSE;
    }

    // Allocate JsonDocument
    DynamicJsonDocument doc(fileSize * 2); // Double the size for safety

    // Parse JSON
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    if (error)
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Failed to parse configuration file: "), F("INSTALLER"));
        this->System->Debugger->printValue(error.c_str());
#endif
        this->System->Message->addMessage("Firmware components installation failed: Failed to parse configuration file");
        return INSTALL_ERROR_CONFIG_PARSE;
    }

    // Process configuration array
    JsonArray configArray = doc["configuration"];
    if (configArray.isNull())
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Invalid configuration format"), F("INSTALLER"));
#endif
        this->System->Message->addMessage("Firmware components installation failed: Invalid configuration format");
        return INSTALL_ERROR_CONFIG_PARSE;
    }

    // Iterate through configurations
    for (JsonObject config : configArray)
    {
        const char *configName = config["name"];
        const char *baseUrl = config["baseUrl"];
        const char *targetFolder = config["targetFolder"];

#ifdef DEBUG
        this->System->Debugger->printInformation(F("Processing configuration: "), F("INSTALLER"));
        this->System->Debugger->printValue(configName);
        this->System->Debugger->printBulletPoint(F("Target folder: "));
        this->System->Debugger->printValue(targetFolder);
#endif

        // Iterate through files
        JsonArray files = config["files"];
        stats.totalFiles += files.size();

        for (JsonObject file : files)
        {
            const char *fileName = file["name"];
            const char *targetFile = file["target"];

            // Construct download URL
            char url[256];
            snprintf(url, sizeof(url), "http://files.smartnydom.pl%s/%s", baseUrl, fileName);

#ifdef DEBUG
            this->System->Debugger->printInformation(F("Downloading file: "), F("INSTALLER"));
            this->System->Debugger->printValue(fileName);
            this->System->Debugger->printBulletPoint(F("URL: "));
            this->System->Debugger->printValue(url);
            this->System->Debugger->printBulletPoint(F("Target folder: "));
            this->System->Debugger->printValue(targetFolder);
            this->System->Debugger->printBulletPoint(F("Target file: "));
            this->System->Debugger->printValue(targetFile);
#endif

            // Download the file and save it with the target name in the target folder
            ESPAPP_DOWNLOAD_STATUS status = this->Downloader->downloadFile(url, targetFolder, targetFile);

            if (status == DOWNLOAD_SUCCESS)
            {
                stats.downloadedFiles++;

#ifdef DEBUG
                this->System->Debugger->printInformation(F("File downloaded successfully"), F("INSTALLER"));
#endif
            }
            else
            {
                stats.failedFiles++;
#ifdef DEBUG
                this->System->Debugger->printError(F("Failed to download file: "), F("INSTALLER"));
                this->System->Debugger->printValue(fileName);
                this->System->Debugger->printBulletPoint(F("Status: "));
                this->System->Debugger->printValue(this->Downloader->getStatusText(status));
#endif
            }

            // Small delay between downloads to avoid overwhelming the server
            delay(100);
        }
    }

#ifdef DEBUG
    // Calculate total installation time
    unsigned long installTime = millis() - installStartTime;

    this->System->Debugger->printInformation(F("Installation completed"), F("INSTALLER"));
    this->System->Debugger->printBulletPoint(F("Total files: "));
    this->System->Debugger->printValue(stats.totalFiles);
    this->System->Debugger->printBulletPoint(F("Downloaded files: "));
    this->System->Debugger->printValue(stats.downloadedFiles);
    this->System->Debugger->printBulletPoint(F("Failed files: "));
    this->System->Debugger->printValue(stats.failedFiles);
    this->System->Debugger->printBulletPoint(F("Time taken: "));
    this->System->Debugger->printValue((float)(installTime / 1000.0));
    this->System->Debugger->printValue(F(" seconds"));

#endif

    // Return appropriate status based on installation results
    if (stats.failedFiles > 0)
    {
        return INSTALL_ERROR_FILE_DOWNLOAD;
    }

    this->System->Message->addMessage("Firmware components installed successfully");
    this->System->Message->addMessage("Restart the device to apply changes");

    return INSTALL_SUCCESS;
}

#ifdef DEBUG
const char *ESPAPP_FirmwareInstalator::getStatusText(ESPAPP_INSTALL_STATUS status)
{
    switch (status)
    {
    case INSTALL_SUCCESS:
        return "Installation completed successfully";
    case INSTALL_ERROR_CONFIG_DOWNLOAD:
        return "Failed to download configuration file";
    case INSTALL_ERROR_CONFIG_PARSE:
        return "Failed to parse configuration file";
    case INSTALL_ERROR_FILE_DOWNLOAD:
        return "Failed to download one or more files";
    case INSTALL_ERROR_FILESYSTEM:
        return "Filesystem error";
    default:
        return "Unknown error";
    }
}
#endif
