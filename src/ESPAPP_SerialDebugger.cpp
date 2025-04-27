#include "ESPAPP_SerialDebugger.h"

#ifdef DEBUG

ESPAPP_SerialDebugger::ESPAPP_SerialDebugger() {};

void ESPAPP_SerialDebugger::setColor(ESPAPP_DEBUGGER_MESSAGE_COLOR color)
{

  Serial << (color == ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD ? F(ESPAPP_TERMINAL_FONT_RESET_COLOR)
                                                              : (color == ESPAPP_DEBUGGER_MESSAGE_COLOR::GREEN
                                                                     ? F(ESPAPP_TERMINAL_FONT_GREEN)
                                                                     : (color == ESPAPP_DEBUGGER_MESSAGE_COLOR::BLUE
                                                                            ? F(ESPAPP_TERMINAL_FONT_BLUE)
                                                                            : F(ESPAPP_TERMINAL_FONT_RED))));
}

void ESPAPP_SerialDebugger::print(const char *text,
                                  const __FlashStringHelper *messageCategory,
                                  ESPAPP_DEBUGGER_MESSAGE_TYPE type, uint8_t newLineBefore,
                                  uint8_t newLineAfter, uint8_t intent, ESPAPP_DEBUGGER_MESSAGE_COLOR color)
{
  addMessageHeader(type, messageCategory, newLineBefore, intent);
  if (color != ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD)
  {
    this->setColor(color);
  }
  Serial << text;
  if (color != ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD)
  {
    this->setColor(ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD);
  }
  addNewLines(newLineAfter);
}

void ESPAPP_SerialDebugger::print(const __FlashStringHelper *text,
                                  const __FlashStringHelper *messageCategory,
                                  ESPAPP_DEBUGGER_MESSAGE_TYPE type, uint8_t newLineBefore,
                                  uint8_t newLineAfter, uint8_t intent, ESPAPP_DEBUGGER_MESSAGE_COLOR color)
{
  addMessageHeader(type, messageCategory, newLineBefore, intent);
  if (color != ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD)
  {
    this->setColor(color);
  }
  Serial << text;
  if (color != ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD)
  {
    this->setColor(ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD);
  }
  addNewLines(newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(uint8_t number, const __FlashStringHelper *text,
                                       uint8_t newLineAfter)
{
  Serial << number;
  addAdditionalText(text, newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(uint8_t number, uint8_t newLineAfter)
{
  printValue(number, F(""), newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(unsigned int number,
                                       const __FlashStringHelper *text,
                                       uint8_t newLineAfter)
{
  Serial << number;
  addAdditionalText(text, newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(unsigned int number, uint8_t newLineAfter)
{
  printValue(number, F(""), newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(int number,
                                       const __FlashStringHelper *text,
                                       uint8_t newLineAfter)
{
  Serial << number;
  addAdditionalText(text, newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(int number, uint8_t newLineAfter)
{
  printValue(number, F(""), newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(unsigned long number,
                                       const __FlashStringHelper *text,
                                       uint8_t newLineAfter)
{
  Serial << number;
  addAdditionalText(text, newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(unsigned long number, uint8_t newLineAfter)
{
  printValue(number, F(""), newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(const char *text, uint8_t newLineBefore,
                                       uint8_t newLineAfter)
{
  print(text, F(""), ESPAPP_DEBUGGER_MESSAGE_TYPE::LINE, newLineBefore, newLineAfter, 0);
}

void ESPAPP_SerialDebugger::printValue(const __FlashStringHelper *text,
                                       uint8_t newLineBefore, uint8_t newLineAfter)
{
  print(text, F(""), ESPAPP_DEBUGGER_MESSAGE_TYPE::LINE, newLineBefore, newLineAfter, 0);
}

void ESPAPP_SerialDebugger::printValue(float number, const __FlashStringHelper *text,
                                       uint8_t newLineAfter)
{
  Serial << number;
  addAdditionalText(text, newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(float number, uint8_t newLineAfter)
{
  printValue(number, F(""), newLineAfter);
}

void ESPAPP_SerialDebugger::printValue(bool number, uint8_t newLineAfter)
{
  printValue((number ? F("Yes") : F("No")), newLineAfter);
}

void ESPAPP_SerialDebugger::addNewLines(uint8_t noOfLines)
{
  for (uint8_t i = 0; i < noOfLines; i++)
  {
    Serial << endl;
  }
}

void ESPAPP_SerialDebugger::addMessageHeader(ESPAPP_DEBUGGER_MESSAGE_TYPE type,
                                             const __FlashStringHelper *messageCategory,
                                             uint8_t newLineBefore, uint8_t noOfIntents)
{
  addNewLines(newLineBefore);

  for (uint8_t i = 0; i < noOfIntents; i++)
  {
    Serial << F(" ");
  }

  switch (type)
  {
  case ESPAPP_DEBUGGER_MESSAGE_TYPE::INFORMATION:
    this->setColor(ESPAPP_DEBUGGER_MESSAGE_COLOR::GREEN);
    break;
  case ESPAPP_DEBUGGER_MESSAGE_TYPE::WARNING:
    this->setColor(ESPAPP_DEBUGGER_MESSAGE_COLOR::BLUE);
    break;
  case ESPAPP_DEBUGGER_MESSAGE_TYPE::ERROR:
    this->setColor(ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
    break;
  }

  Serial << (type == ESPAPP_DEBUGGER_MESSAGE_TYPE::INFORMATION
                 ? F("INFO")
                 : (type == ESPAPP_DEBUGGER_MESSAGE_TYPE::WARNING
                        ? F("WARN")
                        : (type == ESPAPP_DEBUGGER_MESSAGE_TYPE::ERROR
                               ? F("ERROR")
                               : (type == ESPAPP_DEBUGGER_MESSAGE_TYPE::BULLET_POINT
                                      ? F("• ")
                                      : F("")))));

  this->setColor(ESPAPP_DEBUGGER_MESSAGE_COLOR::STANDARD);

  if (type == ESPAPP_DEBUGGER_MESSAGE_TYPE::ERROR || type == ESPAPP_DEBUGGER_MESSAGE_TYPE::INFORMATION ||
      type == ESPAPP_DEBUGGER_MESSAGE_TYPE::WARNING)
  {
    Serial << F(": ") << messageCategory << F(": ");
  }
}

void ESPAPP_SerialDebugger::addAdditionalText(const __FlashStringHelper *text,
                                              uint8_t newLineAfter)
{
  if (text)
  {
    Serial << F(" ") << text;
  }

  addNewLines(newLineAfter);
}

void ESPAPP_SerialDebugger::printInformation(const char *text,
                                             const __FlashStringHelper *messageCategory,
                                             uint8_t newLineBefore,
                                             uint8_t newLineAfter)
{
  print(text, messageCategory, ESPAPP_DEBUGGER_MESSAGE_TYPE::INFORMATION, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialDebugger::printInformation(const __FlashStringHelper *text,
                                             const __FlashStringHelper *messageCategory,
                                             uint8_t newLineBefore,
                                             uint8_t newLineAfter)
{
  print(text, messageCategory, ESPAPP_DEBUGGER_MESSAGE_TYPE::INFORMATION, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialDebugger::printWarning(const char *text,
                                         const __FlashStringHelper *messageCategory,
                                         uint8_t newLineBefore, uint8_t newLineAfter)
{
  print(text, messageCategory, ESPAPP_DEBUGGER_MESSAGE_TYPE::WARNING, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialDebugger::printWarning(const __FlashStringHelper *text,
                                         const __FlashStringHelper *messageCategory,
                                         uint8_t newLineBefore, uint8_t newLineAfter)
{
  print(text, messageCategory, ESPAPP_DEBUGGER_MESSAGE_TYPE::WARNING, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialDebugger::printError(const char *text,
                                       const __FlashStringHelper *messageCategory,
                                       uint8_t newLineBefore, uint8_t newLineAfter)
{
  print(text, messageCategory, ESPAPP_DEBUGGER_MESSAGE_TYPE::ERROR, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialDebugger::printError(const __FlashStringHelper *text,
                                       const __FlashStringHelper *messageCategory,
                                       uint8_t newLineBefore, uint8_t newLineAfter)
{
  print(text, messageCategory, ESPAPP_DEBUGGER_MESSAGE_TYPE::ERROR, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialDebugger::printBulletPoint(const char *text, ESPAPP_DEBUGGER_MESSAGE_COLOR color, uint8_t newLineAfter)
{
  print(text, F(""), ESPAPP_DEBUGGER_MESSAGE_TYPE::BULLET_POINT, 1, newLineAfter, 2, color);
}

void ESPAPP_SerialDebugger::printBulletPoint(const __FlashStringHelper *text,
                                             ESPAPP_DEBUGGER_MESSAGE_COLOR color, uint8_t newLineAfter)
{
  print(text, F(""), ESPAPP_DEBUGGER_MESSAGE_TYPE::BULLET_POINT, 1, newLineAfter, 2, color);
}
void ESPAPP_SerialDebugger::printHeader(uint8_t newLineBefore, uint8_t newLineAfter,
                                        uint8_t length, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE type)
{
  addNewLines(newLineBefore);
  for (uint8_t i = 0; i < length; i++)
  {
    Serial << (type == ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH
                   ? F("-")
                   : (type == ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::SPACE ? F(" ") : F("#")));
  }
  addNewLines(newLineAfter);
}

void ESPAPP_SerialDebugger::getFreeMemorySize()
{
  printInformation(F("Free: "), F("RAM"));
#ifdef ESP32
  Serial << (esp_get_free_heap_size() / 1024) << F("kB");
#else
  Serial << (system_get_free_heap_size() / 1024) << F("kB");
#endif
}

void ESPAPP_SerialDebugger::getFileSystemDubugInformation()
{
  printInformation(F("Summary: "), F("FS"));
  printBulletPoint(F("Used: "));

#if ESPAPP_FILE_SYSTEM == ESPAPP_FS_SPIFFS
  SPIFFS.info(fileSystem);
  Serial << fileSystem.usedBytes / 1024 << F("/")
         << fileSystem.totalBytes / 1024 << F("kB");
#else
  Serial << LittleFS.usedBytes() / 1024 << F("/")
         << LittleFS.totalBytes() / 1024 << F("kB");
#endif
}

void ESPAPP_SerialDebugger::getESPHardwareInformation()
{
  printInformation(F("Hardware information"), F("ESP"));

#ifndef ESP32
  printBulletPoint(F("ID: "));
  Serial << ESP.getFlashChipId();
#endif

#ifdef ESP32
  printBulletPoint(F("Model: "));
  Serial << ESP.getChipModel();

  printBulletPoint(F("CPU cores: "));
  Serial << ESP.getChipCores();

#endif

#ifdef ESP32
  printBulletPoint(F("CPU: "));
  Serial << (ESP.getCpuFreqMHz() / 1000000) << F(" MHz");
#endif

  printBulletPoint(F("Flash size: "));
  if (ESP.getFlashChipSize() >= 1048576)
  {
    Serial << (ESP.getFlashChipSize() / 1048576) << F(" Mbits");
  }
  else
  {
    Serial << (ESP.getFlashChipSize() / 1024) << F(" Kbits");
  }

#ifndef ESP32
  printBulletPoint(F("Real Flash size: "));
  if (ESP.getFlashChipRealSize() >= 1048576)
  {
    Serial << (ESP.getFlashChipRealSize() / 1048576) << F(" Mbits");
  }
  else
  {
    Serial << (ESP.getFlashChipRealSize() / 1024) << F(" Kbits");
  }
#endif

  printBulletPoint(F("Flash Speed: "));
  Serial << (ESP.getFlashChipSpeed() / 1000000) << F(" MHz");

  printBulletPoint(F("Flash Chip Mode: "));
  // Serial << ESP.getFlashChipMode();

#ifdef ESP32
  printBulletPoint(F("Firmware size: "));
  Serial << ESP.getSketchSize();

  printBulletPoint(F("Firmware free space size: "));
  Serial << ESP.getFreeSketchSpace();
#endif
}

void ESPAPP_SerialDebugger::getFirmwareFlashInformation()
{

#ifdef ESP32
  uint32_t maxSketchSpace = UPDATE_SIZE_UNKNOWN;
#else  // ESP8266
  uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
#endif // ESP32

  printBulletPoint(F("Current: "));
  Serial << (ESP.getSketchSize() / 1024) << F("Kb");
  printBulletPoint(F("Available space: "));
  Serial << (ESP.getFreeSketchSpace() / 1024) << F("Kb");
  printBulletPoint(F("Max available space: "));
  Serial << (maxSketchSpace / 1024) << F("Kb");

#ifdef ESP32
  printBulletPoint(F("Max size: "));
  Serial << (UPDATE_SIZE_UNKNOWN / 1024 / 1024) << F("KB");
#endif // ESP32
}

void ESPAPP_SerialDebugger::getFirmwareAllDebugInformation()
{
  getFreeMemorySize();
  getFileSystemDubugInformation();
}

void ESPAPP_SerialDebugger::printProcessingRequest(
    const __FlashStringHelper *deviceItemName, uint8_t deviceId,
    const __FlashStringHelper *type)
{
  printInformation(F("Processing: "), type);
  Serial << deviceItemName;
  if (deviceId != ESPAPP_UNKNWON)
  {
    Serial << F(", Id: ") << deviceId;
  }
}

#endif
