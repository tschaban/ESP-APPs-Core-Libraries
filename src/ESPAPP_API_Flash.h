/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#ifndef _ESPAPP_API_FLASH_h
#define _ESPAPP_API_FLASH_h

#include <ArduinoJson.h>
#include <ESPAPP_Parameters.h>
#include <LittleFS.h>
#include <FS.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

class ESPAPP_API_Flash
{
private:
#ifdef DEBUG
  ESPAPP_SerialMessages *Msg;
#endif

  bool fileSystemReady = false;

  bool mountFileSystem(void);
  bool formatFileSystem(void);

  bool createFile(const char *path);
  bool createFolder(const char *path);

  bool readFSElements(const char *directory, ESPAPP_FILE files[], size_t capacity, size_t &count, bool includeFiles, bool includeFolders);

public:
#ifdef DEBUG
  ESPAPP_API_Flash(ESPAPP_SerialMessages *_Msg);
#else
  ESPAPP_API_Flash();
#endif

  fs::LittleFSFS &fileSystem = LittleFS;

  /** Can be used in many places as allocated memory */
  char fileName[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];

  bool init(void);
  bool initialized(void);
  bool getJSON(const __FlashStringHelper *fileName, JsonDocument &doc);
  bool saveJSON(const __FlashStringHelper *fileName, JsonDocument &doc);

  void getPathToFile(char *path, const char *directory, const char *filename);

  bool deleteFile(const char *directory, const char *path);
  bool deleteFolder(const char *path);

  bool listFolders(ESPAPP_FILE files[], size_t capacity, size_t &count);
  bool listFolders(const char *directory, ESPAPP_FILE files[], size_t capacity, size_t &count);

  bool listFiles(ESPAPP_FILE files[], size_t capacity, size_t &count);
  bool listFiles(const char *directory, ESPAPP_FILE files[], size_t capacity, size_t &count);

  bool uploadFile(const char *directory, const char *filename, const uint8_t *data, size_t length);

  bool openFile(File &openedFile, const char *mode, const char *path, uint8_t id = ESPAPP_NONE,
                boolean createIfNotExists = true);
  bool fileExist(const char *path);

  bool openFile(File &openedFile, const char *mode,
                const __FlashStringHelper *path, uint8_t id = ESPAPP_NONE,
                boolean createIfNotExists = true);

  bool deleteAllFilesInDirectory(const __FlashStringHelper *directory);
};

#endif // _ESPAPP_API_FLASH_h
