/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#ifndef _ESPAPP_API_FLASH_h
#define _ESPAPP_API_FLASH_h

#include <ArduinoJson.h>
#include <ESPAPP_Parameters.h>
#include <LITTLEFS.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

/* File openig modes */
#define ESP_APP_OPEN_FILE_READING "r"
#define ESP_APP_OPEN_FILE_WRITING "w"
#define ESP_APP_OPEN_FILE_APPEND "a"

/* Directories */
#define ESP_APP_DIRECTORY_CONFIG "cfg"
#define ESP_APP_DIRECTORY_DATA "data"


/* File informing that system file is read */
#define ESP_APP_FILE_SYSTEM_INITIALIZED "/.token"

class ESPAPP_API_Flash
{
private:
#ifdef DEBUG
  ESPAPP_SerialMessages *Msg;
#endif

  bool mountFileSystem(void);
  bool formatFileSystem(void);
  bool fileExist(const char *path);
  bool createFile(const char *path);

  bool openFile(File &openedFile, const char *mode,
                const __FlashStringHelper *path, uint8_t id = ESP_APP_NONE,
                boolean createIfNotExists = true);

  bool openFile(File &openedFile, const char *mode, const char *path, uint8_t id,
                boolean createIfNotExists = true);



public:
#ifdef DEBUG
  ESPAPP_API_Flash(ESPAPP_SerialMessages *_Msg);
#else
  ESPAPP_API_Flash();
#endif

  bool init(void);
  bool getJSON(const __FlashStringHelper *fileName, JsonDocument &doc);
  bool saveJSON(const __FlashStringHelper *fileName, JsonDocument &doc);
  bool listAllFiles(ESPAPP_FILE files[], size_t capacity, size_t &count);
  bool listAllFiles(const char* directory, ESPAPP_FILE files[], size_t capacity, size_t &count);
  bool uploadFile(const char *directory, const char *filename, const uint8_t *data, size_t length);
};

#endif // _ESPAPP_API_FLASH_h
