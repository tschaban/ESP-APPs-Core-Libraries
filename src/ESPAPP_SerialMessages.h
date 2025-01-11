/* Class for streamlining Inforamtion, Warnings and Errors to Serial port
* Requires 
* - Streaming.h
* - FS.h
* - LITTLEFS.h
* Following must be defined in your app
* - DEBUG
* - ESP_APP_FILE_SYSTEM = ESP_APP_FS_SPIFFS or ESP_APP_FS_LITTLEFS
*/

#ifndef _ESPAPP_SerialMessages_h
#define _ESPAPP_SerialMessages_h

#ifdef DEBUG

#ifndef ESP32
extern "C" {
#include "user_interface.h"
}
#endif 

#include <ESPAPP_Parameters.h>
#include <Streaming.h>

/* Define in you app if you diff thean LittleFS*/

#define ESP_APP_FS_SPIFFS 0
#define ESP_APP_FS_LITTLEFS 1


#ifndef ESP_APP_FILE_SYSTEM
#define ESP_APP_FILE_SYSTEM ESP_APP_FS_LITTLEFS
// #define ESP_APP_FILE_SYSTEM ESP_APP_FS_SPIFFS
#endif

/* Unknown ID */
#define ESP_APP_UNKNWON 255

/* Type for messages */
#define ESP_APP_MSG_TYPE_LINE 0
#define ESP_APP_MSG_TYPE_BULLET_POINT 1
#define ESP_APP_MSG_TYPE_INFORMATION 7
#define ESP_APP_MSG_TYPE_WARNING 8
#define ESP_APP_MSG_TYPE_ERROR 9

/* Header default length */
#define ESP_APP_MSG_HEADER_DEFAULT_LENGTH 72

#ifndef UPDATE_SIZE_UNKNOWN
#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF
#endif

#ifdef ESP32
#include <LITTLEFS.h>
#else /* ESP8266 */
#include <FS.h>
#if ESP_APP_FILE_SYSTEM == ESP_APP_FS_LITTLEFS
#include <LITTLEFS.h>
#endif
#endif // ESP32/ESP8266



class ESPAPP_SerialMessages {

private:


#if ESP_APP_FILE_SYSTEM == ESP_APP_FS_SPIFFS
  FSInfo fileSystem;
#endif

  void print(const char *text, const __FlashStringHelper *messageCategory,
             uint8_t type, uint8_t newLineBefore, uint8_t newLineAfter,
             uint8_t intent);
  void print(const __FlashStringHelper *text,
             const __FlashStringHelper *messageCategory, uint8_t type,
             uint8_t newLineBefore, uint8_t newLineAfter, uint8_t intent);
  void addMessageHeader(uint8_t type,
                        const __FlashStringHelper *messageCategory,
                        uint8_t newLineBefore, uint8_t noOfIntents);
  void addNewLines(uint8_t noOfLines);
  void addAdditionalText(const __FlashStringHelper *text, uint8_t newLineAfter);

public:
  ESPAPP_SerialMessages();

  void printInformation(const char *text,
                        const __FlashStringHelper *messageCategory,
                        uint8_t newLineBefore = 1, uint8_t newLineAfter = 0);
  /**
   * @brief Generate debugge message of a type: INFO
   *
   * @param  text             Message
   * @param  messageCategory  Category, Location, Group
   * @param  newLineBefore    Default: 1
   * @param  newLineAfter     Defualt: 0
   */
  void printInformation(const __FlashStringHelper *text,
                        const __FlashStringHelper *messageCategory,
                        uint8_t newLineBefore = 1, uint8_t newLineAfter = 0);

  void printWarning(const char *text,
                    const __FlashStringHelper *messageCategory,
                    uint8_t newLineBefore = 1, uint8_t newLineAfter = 0);
  void printWarning(const __FlashStringHelper *text,
                    const __FlashStringHelper *messageCategory,
                    uint8_t newLineBefore = 1, uint8_t newLineAfter = 0);

  void printError(const char *text, const __FlashStringHelper *messageCategory,
                  uint8_t newLineBefore = 1, uint8_t newLineAfter = 0);
  void printError(const __FlashStringHelper *text,
                  const __FlashStringHelper *messageCategory,
                  uint8_t newLineBefore = 1, uint8_t newLineAfter = 0);

  void printBulletPoint(const char *text, uint8_t newLineAfter = 0);
  void printBulletPoint(const __FlashStringHelper *text,
                        uint8_t newLineAfter = 0);

  void printValue(const char *text, uint8_t newLineBefore = 0,
                  uint8_t newLineAfter = 0);
  void printValue(const __FlashStringHelper *text, uint8_t newLineBefore = 0,
                  uint8_t newLineAfter = 0);
  void printValue(uint8_t number, uint8_t newLineAfter = 0);
  void printValue(uint8_t number, const __FlashStringHelper *text,
                  uint8_t newLineAfter = 0);
  void printValue(bool number, uint8_t newLineAfter = 0);
  void printValue(unsigned long number, uint8_t newLineAfter = 0);
  void printValue(unsigned long number, const __FlashStringHelper *text,
                  uint8_t newLineAfter = 0);
  void printValue(float number, uint8_t newLineAfter = 0);
  void printValue(float number, const __FlashStringHelper *text,
                  uint8_t newLineAfter = 0);

  void printHeader(uint8_t newLineBefore = 1, uint8_t newLineAfter = 1,
                   uint8_t length = ESP_APP_MSG_HEADER_DEFAULT_LENGTH,
                   uint8_t type = ESP_APP_MSG_HEADER_TYPE_HASH);

  void getFreeMemorySize();
  void getFileSystemDubugInformation();
  void getESPHardwareInformation();
  void getFirmwareFlashInformation();

  void getFirmwareAllDebugInformation();

  void printProcessingRequest(const __FlashStringHelper *deviceItemName,
                              uint8_t deviceId,
                              const __FlashStringHelper *type);
};
#endif
#endif
