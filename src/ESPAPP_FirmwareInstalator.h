#ifndef _ESPAPP_FIRMWARE_INSTALATOR_H
#define _ESPAPP_FIRMWARE_INSTALATOR_H

#include <ESPAPP_Core.h>
#include <ESPAPP_FileDownloader.h>
#include <ArduinoJson.h>

// Status codes for installation process
enum ESPAPP_INSTALL_STATUS {
    INSTALL_SUCCESS = 0,
    INSTALL_ERROR_CONFIG_DOWNLOAD,
    INSTALL_ERROR_CONFIG_PARSE,
    INSTALL_ERROR_FILE_DOWNLOAD,
    INSTALL_ERROR_FILESYSTEM
};

// Structure to hold statistics about the installation
struct InstallationStats {
    int totalFiles;
    int downloadedFiles;
    int failedFiles;
};

class ESPAPP_FirmwareInstalator {
private:
    ESPAPP_Core* System;
    ESPAPP_FileDownloader* Downloader;
    
    // Directory constants
    static const char* TEMP_DIR;
    static const char* CONFIG_FILENAME;
    
     // Download a single file from the configuration
    bool downloadFile(const char* fileName, const char* targetPath, const char* baseUrl);
    
public:
    ESPAPP_FirmwareInstalator(ESPAPP_Core* _System);
    ~ESPAPP_FirmwareInstalator();
    
    // Main installation method
    ESPAPP_INSTALL_STATUS install(const char* configUrl, InstallationStats& stats);
    
    // Get text description of install status
    const char* getStatusText(ESPAPP_INSTALL_STATUS status);
    

  
};

#endif // _ESPAPP_FIRMWARE_INSTALATOR_H
