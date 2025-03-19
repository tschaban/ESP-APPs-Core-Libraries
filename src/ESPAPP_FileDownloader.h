#ifndef _ESPAPP_FILE_DOWNLOADER_h
#define _ESPAPP_FILE_DOWNLOADER_h

#include <ESPAPP_Core.h>
#include <ESPAPP_API_Flash.h>
#include <HTTPClient.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

/**
 * @brief Status codes for file download operations
 */
enum ESPAPP_DOWNLOAD_STATUS {
    DOWNLOAD_SUCCESS = 0,
    DOWNLOAD_ERROR_CANNOT_CONNECT,
    DOWNLOAD_ERROR_FILE_NOT_FOUND,
    DOWNLOAD_ERROR_SERVER_ERROR,
    DOWNLOAD_ERROR_WRITE_FAILED,
    DOWNLOAD_ERROR_INSUFFICIENT_SPACE,
    DOWNLOAD_ERROR_TIMEOUT,
    DOWNLOAD_ERROR_UNKNOWN
};

// Define the file list item structure
struct FileListItem {
    char filename[64];
    char version[16];
    char targetPath[64];
    char url[256];
};

class ESPAPP_FileDownloader {
private:
    ESPAPP_Core* System;
    HTTPClient http;

    // Default buffer size for downloading chunks
    static const size_t DOWNLOAD_BUFFER_SIZE = 4096;

    // Connection timeout in milliseconds
    static const uint32_t CONNECTION_TIMEOUT = 10000;

    // Maximum allowed file size for download (5MB by default)
    static const size_t MAX_DOWNLOAD_SIZE = 5 * 1024 * 1024;
    
    /**
     * @brief Checks if there is enough space in the filesystem for the download
     * 
     * @param size Size of the file to be downloaded
     * @return true If there is enough space
     * @return false If there is not enough space
     */
    bool checkAvailableSpace(size_t size);

    /**
     * @brief Parse a JSON file list and extract file information
     * 
     * @param jsonFilePath Path to the JSON file containing the file list
     * @param fileList Array to store the parsed file information
     * @param maxFiles Maximum number of files to parse
     * @return int Number of files parsed, negative value on error
     */
    int parseFileList(const char* jsonFilePath, FileListItem* fileList, int maxFiles);

public:
    ESPAPP_FileDownloader(ESPAPP_Core* _System);
    ~ESPAPP_FileDownloader();

    /**
     * @brief Download a file from a URL and save it to the specified directory
     * 
     * @param url The URL of the file to download
     * @param directory Target directory path in the filesystem (should start with '/')
     * @param filename Name of the file to save
     * @return ESPAPP_DOWNLOAD_STATUS Status of the operation
     */
    ESPAPP_DOWNLOAD_STATUS downloadFile(const char* url, const char* directory, const char* filename);

    /**
     * @brief Download a file from a URL and save it to the specified directory
     * 
     * @param url The URL of the file to download (Flash string)
     * @param directory Target directory path in the filesystem (should start with '/')
     * @param filename Name of the file to save
     * @return ESPAPP_DOWNLOAD_STATUS Status of the operation
     */
    ESPAPP_DOWNLOAD_STATUS downloadFile(const __FlashStringHelper* url, const char* directory, const char* filename);

    /**
     * @brief Download a JSON file containing a list of files to download
     * 
     * @param url URL of the JSON file list
     * @param filename Name to save the JSON file as (in the /cfg directory)
     * @return ESPAPP_DOWNLOAD_STATUS Status of the operation
     */
    ESPAPP_DOWNLOAD_STATUS downloadFileList(const char* url, const char* filename);

    /**
     * @brief Download a JSON file containing a list of files to download
     * 
     * @param url URL of the JSON file list (Flash string)
     * @param filename Name to save the JSON file as (in the /cfg directory)
     * @return ESPAPP_DOWNLOAD_STATUS Status of the operation
     */
    ESPAPP_DOWNLOAD_STATUS downloadFileList(const __FlashStringHelper* url, const char* filename);

    /**
     * @brief Download all files listed in a previously downloaded JSON file
     * 
     * @param jsonFilename Name of the JSON file (in the /cfg directory) containing the file list
     * @param maxFiles Maximum number of files to download (to avoid memory issues)
     * @return int Number of successfully downloaded files, negative value on error
     */
    int downloadFilesFromList(const char* jsonFilename, int maxFiles = 20);

    /**
     * @brief Get a text description of a download status code
     * 
     * @param status The status code
     * @return const char* Description of the status
     */
    const char* getStatusText(ESPAPP_DOWNLOAD_STATUS status);
};

#endif // _ESPAPP_FILE_DOWNLOADER_h
