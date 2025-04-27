/* Class for streamlining Inforamtion, Warnings and Errors to Serial port
 * Requires
 * - Streaming.h
 * - FS.h
 * - LITTLEFS.h
 * Following must be defined in your app
 * - DEBUG
 * - ESPAPP_FILE_SYSTEM = ESPAPP_FS_SPIFFS or ESPAPP_FS_LITTLEFS
 */

#ifndef _ESPAPP_SerialDebugger_h
#define _ESPAPP_SerialDebugger_h

#ifdef DEBUG

#ifndef ESP32
extern "C"
{
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
enum ESPAPP_DEBUGGER_MESSAGE_TYPE
{
  LINE = 0,
  BULLET_POINT = 1,
  INFORMATION = 7,
  WARNING = 8,
  ERROR = 9,
};

enum ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE
{
  HASH = 0,
  DASH = 1,
  SPACE = 2,
};

enum ESPAPP_DEBUGGER_MESSAGE_COLOR
{
  STANDARD = 0,
  RED = 1,
  GREEN = 2,
  BLUE = 3,
};

/* Colors of fonts for Teminal messages */
#define ESPAPP_TERMINAL_FONT_RESET_COLOR "\u001b[0m"
#define ESPAPP_TERMINAL_FONT_RED "\u001b[31m"
#define ESPAPP_TERMINAL_FONT_GREEN "\u001b[32m"
#define ESPAPP_TERMINAL_FONT_BLUE "\u001b[34m"

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

class ESPAPP_SerialDebugger
{

private:
#if ESPAPP_FILE_SYSTEM == ESPAPP_FS_SPIFFS
  FSInfo fileSystem;
#endif

  void print(const char *text, const __FlashStringHelper *messageCategory,
             ESPAPP_DEBUGGER_MESSAGE_TYPE type, uint8_t newLineBefore, uint8_t newLineAfter,
             uint8_t intent, ESPAPP_DEBUGGER_MESSAGE_COLOR color = ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD);
  void print(const __FlashStringHelper *text,
             const __FlashStringHelper *messageCategory, ESPAPP_DEBUGGER_MESSAGE_TYPE type,
             uint8_t newLineBefore, uint8_t newLineAfter, uint8_t intent, ESPAPP_DEBUGGER_MESSAGE_COLOR color = ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD);

  void addMessageHeader(ESPAPP_DEBUGGER_MESSAGE_TYPE type,
                        const __FlashStringHelper *messageCategory,
                        uint8_t newLineBefore, uint8_t noOfIntents);

  void addNewLines(uint8_t noOfLines);

  void addAdditionalText(const __FlashStringHelper *text, uint8_t newLineAfter);

  void setColor(ESPAPP_DEBUGGER_MESSAGE_COLOR color);

public:
  ESPAPP_SerialDebugger();

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

  void printBulletPoint(const char *text, ESPAPP_DEBUGGER_MESSAGE_COLOR color = ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD, uint8_t newLineAfter = 0);
  void printBulletPoint(const __FlashStringHelper *text,
                        ESPAPP_DEBUGGER_MESSAGE_COLOR color = ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD, uint8_t newLineAfter = 0);

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
                   ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE type = ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::HASH);

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
