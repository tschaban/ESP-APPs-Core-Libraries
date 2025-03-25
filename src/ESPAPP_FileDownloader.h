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
     * @brief Parse a URL into host, port, and path components
     * 
     * @param url The URL to parse
     * @param host Buffer to store the host
     * @param port Variable to store the port
     * @param path Buffer to store the path
     * @return true If the URL was successfully parsed
     * @return false Otherwise
     */
    bool parseUrl(const char* url, char* host, int& port, char* path);

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
     * @brief Get a text description of a download status code
     * 
     * @param status The status code
     * @return const char* Description of the status
     */
    const char* getStatusText(ESPAPP_DOWNLOAD_STATUS status);
};

#endif // _ESPAPP_FILE_DOWNLOADER_h