/* Class for streamlining Inforamtion, Warnings and Errors to Serial port
* Requires 
* - Streaming.h
* - FS.h
* - LITTLEFS.h
* Following must be defined in your app
* - DEBUG
* - ESPAPP_FILE_SYSTEM = ESPAPP_FS_SPIFFS or ESPAPP_FS_LITTLEFS
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

#define ESPAPP_FS_SPIFFS 0
#define ESPAPP_FS_LITTLEFS 1


#ifndef ESPAPP_FILE_SYSTEM
#define ESPAPP_FILE_SYSTEM ESPAPP_FS_LITTLEFS
// #define ESPAPP_FILE_SYSTEM ESPAPP_FS_SPIFFS
#endif

/* Unknown ID */
#define ESPAPP_UNKNWON 255

/* Type for messages */
#define ESPAPP_MSG_TYPE_LINE 0
#define ESPAPP_MSG_TYPE_BULLET_POINT 1
#define ESPAPP_MSG_TYPE_INFORMATION 7
#define ESPAPP_MSG_TYPE_WARNING 8
#define ESPAPP_MSG_TYPE_ERROR 9

/* Header default length */
#define ESPAPP_MSG_HEADER_DEFAULT_LENGTH 72

#ifndef UPDATE_SIZE_UNKNOWN
#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF
#endif

#ifdef ESP32
#include <LITTLEFS.h>
#else /* ESP8266 */
#include <FS.h>
#if ESPAPP_FILE_SYSTEM == ESPAPP_FS_LITTLEFS
#include <LITTLEFS.h>
#endif
#endif // ESP32/ESP8266



class ESPAPP_SerialMessages {

private:


#if ESPAPP_FILE_SYSTEM == ESPAPP_FS_SPIFFS
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
 
  void printValue(unsigned int number, uint8_t newLineAfter = 0);
  void printValue(unsigned int number, const __FlashStringHelper *text,
                  uint8_t newLineAfter = 0);

  void printValue(int number, uint8_t newLineAfter = 0);
  void printValue(int number, const __FlashStringHelper *text,
                  uint8_t newLineAfter = 0);


  void printValue(unsigned long number, uint8_t newLineAfter = 0);
  void printValue(unsigned long number, const __FlashStringHelper *text,
                  uint8_t newLineAfter = 0);
 
  void printValue(float number, uint8_t newLineAfter = 0);
  void printValue(float number, const __FlashStringHelper *text,
                  uint8_t newLineAfter = 0);


  void printValue(bool number, uint8_t newLineAfter = 0);


  void printHeader(uint8_t newLineBefore = 1, uint8_t newLineAfter = 1,
                   uint8_t length = ESPAPP_MSG_HEADER_DEFAULT_LENGTH,
                   uint8_t type = ESPAPP_MSG_HEADER_TYPE_HASH);

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
