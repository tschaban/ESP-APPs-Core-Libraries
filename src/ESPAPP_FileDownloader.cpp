#include "ESPAPP_FileDownloader.h"

ESPAPP_FileDownloader::ESPAPP_FileDownloader(ESPAPP_Core *_System)
{
    this->System = _System;

#ifdef DEBUG
    this->System->Debugger->printInformation(F("File downloader initialized"), F("DOWNLOAD"));
#endif
}

ESPAPP_FileDownloader::~ESPAPP_FileDownloader()
{
    // Ensure HTTP client is properly closed
    http.end();
}

bool ESPAPP_FileDownloader::checkAvailableSpace(size_t size)
{
    // Get available space from the file system
    size_t totalBytes = this->System->Flash->fileSystem.totalBytes();
    size_t usedBytes = this->System->Flash->fileSystem.usedBytes();
    size_t freeBytes = totalBytes - usedBytes;

    return freeBytes >= size;
}

ESPAPP_DOWNLOAD_STATUS ESPAPP_FileDownloader::downloadFile(const char *url, const char *directory, const char *filename)
{
#ifdef DEBUG
    this->System->Debugger->printInformation(F("Downloading file from URL"), F("DOWNLOAD"));
    this->System->Debugger->printBulletPoint(F("URL: "));
    this->System->Debugger->printValue(url);
    this->System->Debugger->printBulletPoint(F("Target directory: "));
    this->System->Debugger->printValue(directory);
    this->System->Debugger->printBulletPoint(F("Filename: "));
    this->System->Debugger->printValue(filename);
#endif

    // Set timeout for connection
    http.setTimeout(CONNECTION_TIMEOUT);
    http.setConnectTimeout(CONNECTION_TIMEOUT);

    // Parse the URL to get host, port, and path
    char host[100];
    int port;
    char path[200];

    if (!parseUrl(url, host, port, path))
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Invalid URL format"), F("DOWNLOAD"));
#endif
        return DOWNLOAD_ERROR_CANNOT_CONNECT;
    }

#ifdef DEBUG
    this->System->Debugger->printBulletPoint(F("Host: "));
    this->System->Debugger->printValue(host);
    this->System->Debugger->printBulletPoint(F("Port: "));
    this->System->Debugger->printValue(port);
    this->System->Debugger->printBulletPoint(F("Path: "));
    this->System->Debugger->printValue(path);
#endif

    // Begin HTTP connection
    http.begin(host, port, path);

    // Send HTTP GET request
    int httpCode = http.GET();

    // Check if the request was successful
    if (httpCode <= 0)
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Connection failed: "), F("DOWNLOAD"));
        this->System->Debugger->printValue(http.errorToString(httpCode).c_str());
#endif
        http.end();
        return DOWNLOAD_ERROR_CANNOT_CONNECT;
    }

    // Check the HTTP response code
    if (httpCode != HTTP_CODE_OK)
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("HTTP error code: "), F("DOWNLOAD"));
        this->System->Debugger->printValue(httpCode);
#endif
        http.end();

        if (httpCode == HTTP_CODE_NOT_FOUND)
        {
            return DOWNLOAD_ERROR_FILE_NOT_FOUND;
        }
        else if (httpCode >= 500)
        {
            return DOWNLOAD_ERROR_SERVER_ERROR;
        }
        else
        {
            return DOWNLOAD_ERROR_UNKNOWN;
        }
    }

    // Get the content length
    int contentLength = http.getSize();

#ifdef DEBUG
    this->System->Debugger->printBulletPoint(F("Content length: "));
    this->System->Debugger->printValue(contentLength, F(" bytes"));
#endif

    // Check if content length is valid
    if (contentLength <= 0)
    {
#ifdef DEBUG
        this->System->Debugger->printWarning(F("Content length is invalid or unknown"), F("DOWNLOAD"));
#endif
    }
    else if (contentLength > MAX_DOWNLOAD_SIZE)
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("File too large: "), F("DOWNLOAD"));
        this->System->Debugger->printValue(contentLength, F(" bytes"));
        this->System->Debugger->printValue(F(" > Maximum allowed: "));
        this->System->Debugger->printValue(MAX_DOWNLOAD_SIZE, F(" bytes"));
#endif
        http.end();
        return DOWNLOAD_ERROR_INSUFFICIENT_SPACE;
    }

    // Check available space
    if (contentLength > 0 && !checkAvailableSpace(contentLength))
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Insufficient space in filesystem"), F("DOWNLOAD"));
#endif
        http.end();
        return DOWNLOAD_ERROR_INSUFFICIENT_SPACE;
    }

    // Get the HTTP stream
    WiFiClient *stream = http.getStreamPtr();

    // Create the full file path
    char filePath[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
    this->System->Flash->getPathToFile(filePath, directory, filename);

#ifdef DEBUG
    this->System->Debugger->printBulletPoint(F("Saving to path: "));
    this->System->Debugger->printValue(filePath);
#endif

    // Open the file for writing
    File file;
    this->System->Flash->openFile(file, ESPAPP_OPEN_FILE_WRITING, filePath);

    if (!file)
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Failed to open file for writing"), F("DOWNLOAD"));
#endif
        http.end();
        return DOWNLOAD_ERROR_WRITE_FAILED;
    }

    // Define buffer for data
    uint8_t buffer[DOWNLOAD_BUFFER_SIZE];
    size_t bytesRead = 0;
    size_t totalBytesRead = 0;
    uint32_t lastTimeUpdate = millis();

    // Read all data from server
    while (http.connected() && (contentLength == -1 || totalBytesRead < contentLength))
    {
        // Get available data size
        size_t size = stream->available();

        if (size > 0)
        {
            // Read up to buffer size
            int c = stream->readBytes(buffer, ((size > sizeof(buffer)) ? sizeof(buffer) : size));

            // Write to file
            if (file.write(buffer, c) != c)
            {
#ifdef DEBUG
                this->System->Debugger->printError(F("Write failed"), F("DOWNLOAD"));
#endif
                file.close();
                http.end();
                return DOWNLOAD_ERROR_WRITE_FAILED;
            }

            bytesRead = c;
            totalBytesRead += c;

            // Print progress every 3 seconds
            if (millis() - lastTimeUpdate > 3000)
            {
                lastTimeUpdate = millis();
#ifdef DEBUG
                if (contentLength > 0)
                {
                    this->System->Debugger->printBulletPoint(F("Progress: "));
                    this->System->Debugger->printValue((totalBytesRead * 100) / contentLength, F("%"));
                }
                else
                {
                    this->System->Debugger->printBulletPoint(F("Downloaded: "));
                    this->System->Debugger->printValue(totalBytesRead, F(" bytes"));
                }
#endif
            }
        }
        else
        {
            // Small delay to allow data to arrive
            delay(1);
        }

        // Check for timeout
        if (millis() - lastTimeUpdate > CONNECTION_TIMEOUT * 2)
        {
#ifdef DEBUG
            this->System->Debugger->printError(F("Download timeout"), F("DOWNLOAD"));
#endif
            file.close();
            http.end();
            return DOWNLOAD_ERROR_TIMEOUT;
        }
    }

    // Close the file
    file.close();

    // Disconnect from the server
    http.end();

#ifdef DEBUG
    this->System->Debugger->printInformation(F("Download complete: "), F("DOWNLOAD"));
    this->System->Debugger->printValue(totalBytesRead, F(" bytes"));
#endif

    return DOWNLOAD_SUCCESS;
}

bool ESPAPP_FileDownloader::parseUrl(const char *url, char *host, int &port, char *path)
{
    // Default values
    port = 80;
    strcpy(path, "/");

    // Check for null URL
    if (!url)
        return false;

    // Skip protocol part if present
    const char *hostStart = url;
    if (strncmp(url, "http://", 7) == 0)
    {
        hostStart = url + 7;
    }

    // Find end of host (port delimiter or path delimiter)
    const char *hostEnd = strchr(hostStart, ':');
    const char *pathStart = strchr(hostStart, '/');

    if (!pathStart)
    {
        // No path in URL, use default "/"
        pathStart = hostStart + strlen(hostStart);
    }
    else
    {
        // Copy the path
        strcpy(path, pathStart);
    }

    if (!hostEnd || (pathStart && hostEnd > pathStart))
    {
        // No port specified or port is part of the path
        hostEnd = pathStart;
    }
    else
    {
        // Extract port number
        port = atoi(hostEnd + 1);
        if (port <= 0)
        {
            port = 80;
        }
    }

    // Extract host
    size_t hostLen = hostEnd - hostStart;
    if (hostLen > 0)
    {
        strncpy(host, hostStart, hostLen);
        host[hostLen] = ESPAPP_EMPTY_STRING_TERMINATED;
        return true;
    }

    return false;
}

const char *ESPAPP_FileDownloader::getStatusText(ESPAPP_DOWNLOAD_STATUS status)
{
    switch (status)
    {
    case DOWNLOAD_SUCCESS:
        return "Success";
    case DOWNLOAD_ERROR_CANNOT_CONNECT:
        return "Cannot connect to server";
    case DOWNLOAD_ERROR_FILE_NOT_FOUND:
        return "File not found on server";
    case DOWNLOAD_ERROR_SERVER_ERROR:
        return "Server error";
    case DOWNLOAD_ERROR_WRITE_FAILED:
        return "Failed to write to filesystem";
    case DOWNLOAD_ERROR_INSUFFICIENT_SPACE:
        return "Insufficient space in filesystem";
    case DOWNLOAD_ERROR_TIMEOUT:
        return "Download timeout";
    case DOWNLOAD_ERROR_UNKNOWN:
    default:
        return "Unknown error";
    }
}