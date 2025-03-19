#include "ESPAPP_FileDownloader.h"

ESPAPP_FileDownloader::ESPAPP_FileDownloader(ESPAPP_Core* _System) {
    this->System = _System;
}

ESPAPP_FileDownloader::~ESPAPP_FileDownloader() {
    // Ensure HTTP client is properly closed
    http.end();
}

bool ESPAPP_FileDownloader::checkAvailableSpace(size_t size) {
    // Get available space from the file system
    #if defined(ESP32)
        size_t totalBytes = this->System->Flash->fileSystem.totalBytes();
        size_t usedBytes = this->System->Flash->fileSystem.usedBytes();
        size_t freeBytes = totalBytes - usedBytes;
    #else // ESP8266
        FSInfo fs_info;
        this->System->Flash->fileSystem.info(fs_info);
        size_t freeBytes = fs_info.totalBytes - fs_info.usedBytes;
    #endif
    
    return freeBytes >= size;
}

ESPAPP_DOWNLOAD_STATUS ESPAPP_FileDownloader::downloadFile(const char* url, const char* directory, const char* filename) {
#ifdef DEBUG
    this->System->Msg->printInformation(F("Downloading file from URL"), F("DOWNLOAD"));
    this->System->Msg->printBulletPoint(F("URL: "));
    this->System->Msg->printValue(url);
    this->System->Msg->printBulletPoint(F("Target directory: "));
    this->System->Msg->printValue(directory);
    this->System->Msg->printBulletPoint(F("Filename: "));
    this->System->Msg->printValue(filename);
#endif

    // Set timeout for connection
    http.setTimeout(CONNECTION_TIMEOUT);
    
    #ifdef ESP32
    http.setConnectTimeout(CONNECTION_TIMEOUT);
    #endif
    
    // Begin HTTP connection
    http.begin(url);
    
    // Send HTTP GET request
    int httpCode = http.GET();
    
    // Check if the request was successful
    if (httpCode <= 0) {
#ifdef DEBUG
        this->System->Msg->printError(F("Connection failed: "), F("DOWNLOAD"));
        this->System->Msg->printValue(http.errorToString(httpCode).c_str());
#endif
        http.end();
        return DOWNLOAD_ERROR_CANNOT_CONNECT;
    }
    
    // Check the HTTP response code
    if (httpCode != HTTP_CODE_OK) {
#ifdef DEBUG
        this->System->Msg->printError(F("HTTP error code: "), F("DOWNLOAD"));
        this->System->Msg->printValue(httpCode);
#endif
        http.end();
        
        if (httpCode == HTTP_CODE_NOT_FOUND) {
            return DOWNLOAD_ERROR_FILE_NOT_FOUND;
        } else if (httpCode >= 500) {
            return DOWNLOAD_ERROR_SERVER_ERROR;
        } else {
            return DOWNLOAD_ERROR_UNKNOWN;
        }
    }
    
    // Get the content length
    int contentLength = http.getSize();
    
#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Content length: "));
    this->System->Msg->printValue(contentLength, F(" bytes"));
#endif
    
    // Check if content length is valid
    if (contentLength <= 0) {
#ifdef DEBUG
        this->System->Msg->printWarning(F("Content length is invalid or unknown"), F("DOWNLOAD"));
#endif
    } else if (contentLength > MAX_DOWNLOAD_SIZE) {
#ifdef DEBUG
        this->System->Msg->printError(F("File too large: "), F("DOWNLOAD"));
        this->System->Msg->printValue(contentLength, F(" bytes"));
        this->System->Msg->printValue(F(" > Maximum allowed: "));
        this->System->Msg->printValue(MAX_DOWNLOAD_SIZE, F(" bytes"));
#endif
        http.end();
        return DOWNLOAD_ERROR_INSUFFICIENT_SPACE;
    }
    
    // Check available space
    if (contentLength > 0 && !checkAvailableSpace(contentLength)) {
#ifdef DEBUG
        this->System->Msg->printError(F("Insufficient space in filesystem"), F("DOWNLOAD"));
#endif
        http.end();
        return DOWNLOAD_ERROR_INSUFFICIENT_SPACE;
    }
    
    // Get the HTTP stream
    WiFiClient* stream = http.getStreamPtr();
    
    // Create the directory if it doesn't exist
    if (strlen(directory) > 0 && !this->System->Flash->fileSystem.exists(directory)) {
#ifdef DEBUG
        this->System->Msg->printBulletPoint(F("Creating directory: "));
        this->System->Msg->printValue(directory);
#endif
        this->System->Flash->fileSystem.mkdir(directory);
    }
    
    // Create the full file path
    char filePath[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
    this->System->Flash->getPathToFile(filePath, directory, filename);
    
#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Saving to path: "));
    this->System->Msg->printValue(filePath);
#endif
    
    // Open the file for writing
    File file = this->System->Flash->fileSystem.open(filePath, ESPAPP_OPEN_FILE_WRITING);
    if (!file) {
#ifdef DEBUG
        this->System->Msg->printError(F("Failed to open file for writing"), F("DOWNLOAD"));
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
    while (http.connected() && (contentLength == -1 || totalBytesRead < contentLength)) {
        // Get available data size
        size_t size = stream->available();
        
        if (size > 0) {
            // Read up to buffer size
            int c = stream->readBytes(buffer, ((size > sizeof(buffer)) ? sizeof(buffer) : size));
            
            // Write to file
            if (file.write(buffer, c) != c) {
#ifdef DEBUG
                this->System->Msg->printError(F("Write failed"), F("DOWNLOAD"));
#endif
                file.close();
                http.end();
                return DOWNLOAD_ERROR_WRITE_FAILED;
            }
            
            bytesRead = c;
            totalBytesRead += c;
            
            // Print progress every 3 seconds
            if (millis() - lastTimeUpdate > 3000) {
                lastTimeUpdate = millis();
#ifdef DEBUG
                if (contentLength > 0) {
                    this->System->Msg->printBulletPoint(F("Progress: "));
                    this->System->Msg->printValue((totalBytesRead * 100) / contentLength, F("%"));
                } else {
                    this->System->Msg->printBulletPoint(F("Downloaded: "));
                    this->System->Msg->printValue(totalBytesRead, F(" bytes"));
                }
#endif
            }
        } else {
            // Small delay to allow data to arrive
            delay(1);
        }
        
        // Check for timeout
        if (millis() - lastTimeUpdate > CONNECTION_TIMEOUT * 2) {
#ifdef DEBUG
            this->System->Msg->printError(F("Download timeout"), F("DOWNLOAD"));
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
    this->System->Msg->printInformation(F("Download complete: "), F("DOWNLOAD"));
    this->System->Msg->printValue(totalBytesRead, F(" bytes"));
#endif
    
    return DOWNLOAD_SUCCESS;
}

ESPAPP_DOWNLOAD_STATUS ESPAPP_FileDownloader::downloadFile(const __FlashStringHelper* url, const char* directory, const char* filename) {
    // Convert Flash string to char array
    char urlBuffer[200]; // Adjust size as needed
    strncpy_P(urlBuffer, (PGM_P)url, sizeof(urlBuffer) - 1);
    urlBuffer[sizeof(urlBuffer) - 1] = '\0';
    
    // Call the char* version of the method
    return downloadFile(urlBuffer, directory, filename);
}

const char* ESPAPP_FileDownloader::getStatusText(ESPAPP_DOWNLOAD_STATUS status) {
    switch (status) {
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

ESPAPP_DOWNLOAD_STATUS ESPAPP_FileDownloader::downloadFileList(const char* url, const char* filename) {
#ifdef DEBUG
    this->System->Msg->printInformation(F("Downloading file list JSON"), F("DOWNLOAD"));
    this->System->Msg->printBulletPoint(F("URL: "));
    this->System->Msg->printValue(url);
    this->System->Msg->printBulletPoint(F("Target filename: "));
    this->System->Msg->printValue(filename);
#endif

    // Use the /cfg directory for storing the file list
    return downloadFile(url, "/cfg", filename);
}

ESPAPP_DOWNLOAD_STATUS ESPAPP_FileDownloader::downloadFileList(const __FlashStringHelper* url, const char* filename) {
    // Convert Flash string to char array
    char urlBuffer[200]; // Adjust size as needed
    strncpy_P(urlBuffer, (PGM_P)url, sizeof(urlBuffer) - 1);
    urlBuffer[sizeof(urlBuffer) - 1] = '\0';
    
    // Call the char* version of the method
    return downloadFileList(urlBuffer, filename);
}

int ESPAPP_FileDownloader::parseFileList(const char* jsonFilePath, FileListItem* fileList, int maxFiles) {
    if (!fileList || maxFiles <= 0) {
        return -1;
    }
    
    // Create full path
    char fullPath[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
    this->System->Flash->getPathToFile(fullPath, "/cfg", jsonFilePath);
    
#ifdef DEBUG
    this->System->Msg->printInformation(F("Parsing file list JSON"), F("DOWNLOAD"));
    this->System->Msg->printBulletPoint(F("Path: "));
    this->System->Msg->printValue(fullPath);
#endif
    
    // Check if file exists
    if (!this->System->Flash->fileSystem.exists(fullPath)) {
#ifdef DEBUG
        this->System->Msg->printError(F("File list JSON not found"), F("DOWNLOAD"));
#endif
        return -2;
    }
    
    // Open file
    File file = this->System->Flash->fileSystem.open(fullPath, ESPAPP_OPEN_FILE_READING);
    if (!file) {
#ifdef DEBUG
        this->System->Msg->printError(F("Failed to open file list JSON"), F("DOWNLOAD"));
#endif
        return -3;
    }
    
    // Allocate memory for the JSON document
    // Calculate a reasonable buffer size based on file size
    size_t fileSize = file.size();
    
#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("File size: "));
    this->System->Msg->printValue(fileSize, F(" bytes"));
#endif
    
    // Use ArduinoJson's capacity calculator: https://arduinojson.org/v6/assistant/
    // This is a rough estimate, adjust based on your JSON structure
    const size_t capacity = JSON_ARRAY_SIZE(maxFiles) + maxFiles * JSON_OBJECT_SIZE(4) + maxFiles * 384;
    
    DynamicJsonDocument doc(capacity);
    
    // Parse JSON
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
#ifdef DEBUG
        this->System->Msg->printError(F("Failed to parse file list JSON: "), F("DOWNLOAD"));
        this->System->Msg->printValue(error.c_str());
#endif
        return -4;
    }
    
    // Ensure we have an array
    if (!doc.is<JsonArray>()) {
#ifdef DEBUG
        this->System->Msg->printError(F("File list JSON is not an array"), F("DOWNLOAD"));
#endif
        return -5;
    }
    
    JsonArray filesArray = doc.as<JsonArray>();
    int count = 0;
    
    for (JsonVariant file : filesArray) {
        if (count >= maxFiles) {
#ifdef DEBUG
            this->System->Msg->printWarning(F("Maximum files reached, stopping parsing"), F("DOWNLOAD"));
#endif
            break;
        }
        
        // Copy file information to the array
        if (file.containsKey("filename") && file.containsKey("version") && 
            file.containsKey("targetPath") && file.containsKey("url")) {
            
            strncpy(fileList[count].filename, file["filename"].as<const char*>(), sizeof(fileList[count].filename) - 1);
            fileList[count].filename[sizeof(fileList[count].filename) - 1] = '\0';
            
            strncpy(fileList[count].version, file["version"].as<const char*>(), sizeof(fileList[count].version) - 1);
            fileList[count].version[sizeof(fileList[count].version) - 1] = '\0';
            
            strncpy(fileList[count].targetPath, file["targetPath"].as<const char*>(), sizeof(fileList[count].targetPath) - 1);
            fileList[count].targetPath[sizeof(fileList[count].targetPath) - 1] = '\0';
            
            strncpy(fileList[count].url, file["url"].as<const char*>(), sizeof(fileList[count].url) - 1);
            fileList[count].url[sizeof(fileList[count].url) - 1] = '\0';
            
            count++;
            
#ifdef DEBUG
            this->System->Msg->printBulletPoint(F("Found file: "));
            this->System->Msg->printValue(file["filename"].as<const char*>());
            this->System->Msg->printBulletPoint(F("  Version: "));
            this->System->Msg->printValue(file["version"].as<const char*>());
            this->System->Msg->printBulletPoint(F("  Path: "));
            this->System->Msg->printValue(file["targetPath"].as<const char*>());
#endif
        } else {
#ifdef DEBUG
            this->System->Msg->printWarning(F("Skipping file - missing required fields"), F("DOWNLOAD"));
#endif
        }
    }
    
#ifdef DEBUG
    this->System->Msg->printInformation(F("Parsed files: "), F("DOWNLOAD"));
    this->System->Msg->printValue(count);
#endif
    
    return count;
}

int ESPAPP_FileDownloader::downloadFilesFromList(const char* jsonFilename, int maxFiles) {
#ifdef DEBUG
    this->System->Msg->printInformation(F("Starting batch download from file list"), F("DOWNLOAD"));
    this->System->Msg->printBulletPoint(F("JSON filename: "));
    this->System->Msg->printValue(jsonFilename);
    this->System->Msg->printBulletPoint(F("Max files: "));
    this->System->Msg->printValue(maxFiles);
#endif

    if (maxFiles <= 0) {
        maxFiles = 20; // Default value
    }
    
    // Allocate memory for file list
    FileListItem* fileList = new FileListItem[maxFiles];
    if (!fileList) {
#ifdef DEBUG
        this->System->Msg->printError(F("Failed to allocate memory for file list"), F("DOWNLOAD"));
#endif
        return -1;
    }
    
    // Parse the file list
    int fileCount = parseFileList(jsonFilename, fileList, maxFiles);
    if (fileCount <= 0) {
#ifdef DEBUG
        this->System->Msg->printError(F("Failed to parse file list or empty list"), F("DOWNLOAD"));
#endif
        delete[] fileList;
        return fileCount; // Return the error code
    }
    
    int successCount = 0;
    
    // Download each file
    for (int i = 0; i < fileCount; i++) {
#ifdef DEBUG
        this->System->Msg->printInformation(F("Downloading file "), F("DOWNLOAD"));
        this->System->Msg->printValue(i + 1);
        this->System->Msg->printValue(F(" of "));
        this->System->Msg->printValue(fileCount);
        this->System->Msg->printBulletPoint(F("Filename: "));
        this->System->Msg->printValue(fileList[i].filename);
        this->System->Msg->printBulletPoint(F("URL: "));
        this->System->Msg->printValue(fileList[i].url);
#endif
        
        ESPAPP_DOWNLOAD_STATUS status = downloadFile(fileList[i].url, fileList[i].targetPath, fileList[i].filename);
        
        if (status == DOWNLOAD_SUCCESS) {
            successCount++;
#ifdef DEBUG
            this->System->Msg->printBulletPoint(F("Download successful"));
#endif
        } else {
#ifdef DEBUG
            this->System->Msg->printError(F("Download failed: "), F("DOWNLOAD"));
            this->System->Msg->printValue(this->getStatusText(status));
#endif
        }
    }
    
    // Free the allocated memory
    delete[] fileList;
    
#ifdef DEBUG
    this->System->Msg->printInformation(F("Batch download complete. Successfully downloaded "), F("DOWNLOAD"));
    this->System->Msg->printValue(successCount);
    this->System->Msg->printValue(F(" of "));
    this->System->Msg->printValue(fileCount);
    this->System->Msg->printValue(F(" files."));
#endif
    
    return successCount;
}
