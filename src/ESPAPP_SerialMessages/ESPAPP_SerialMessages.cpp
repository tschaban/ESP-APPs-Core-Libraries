#include "ESPAPP_SerialMessages.h"

#ifdef DEBUG

ESPAPP_SerialMessages::ESPAPP_SerialMessages(){};

void ESPAPP_SerialMessages::print(const char *text,
                        const __FlashStringHelper *messageCategory,
                        uint8_t type, uint8_t newLineBefore,
                        uint8_t newLineAfter, uint8_t intent) {
  addMessageHeader(type, messageCategory, newLineBefore, intent);
  Serial << text;
  addNewLines(newLineAfter);
}

void ESPAPP_SerialMessages::print(const __FlashStringHelper *text,
                        const __FlashStringHelper *messageCategory,
                        uint8_t type, uint8_t newLineBefore,
                        uint8_t newLineAfter, uint8_t intent) {
  addMessageHeader(type, messageCategory, newLineBefore, intent);
  Serial << text;
  addNewLines(newLineAfter);
}

void ESPAPP_SerialMessages::printValue(uint8_t number, const __FlashStringHelper *text,
                             uint8_t newLineAfter) {
  Serial << number;
  addAdditionalText(text, newLineAfter);
}

void ESPAPP_SerialMessages::printValue(uint8_t number, uint8_t newLineAfter) {
  printValue(number, F(""), newLineAfter);
}

void ESPAPP_SerialMessages::printValue(unsigned long number,
                             const __FlashStringHelper *text,
                             uint8_t newLineAfter) {
  Serial << number;
  addAdditionalText(text, newLineAfter);
}

void ESPAPP_SerialMessages::printValue(unsigned long number, uint8_t newLineAfter) {
  printValue(number, F(""), newLineAfter);
}

void ESPAPP_SerialMessages::printValue(const char *text, uint8_t newLineBefore,
                             uint8_t newLineAfter) {
  print(text, F(""), ESP_APP_DEBUG_TYPE_LINE, newLineBefore, newLineAfter, 0);
}

void ESPAPP_SerialMessages::printValue(const __FlashStringHelper *text,
                             uint8_t newLineBefore, uint8_t newLineAfter) {
  print(text, F(""), ESP_APP_DEBUG_TYPE_LINE, newLineBefore, newLineAfter, 0);
}

void ESPAPP_SerialMessages::printValue(float number, const __FlashStringHelper *text,
                             uint8_t newLineAfter) {
  Serial << number;
  addAdditionalText(text, newLineAfter);
}

void ESPAPP_SerialMessages::printValue(float number, uint8_t newLineAfter) {
  printValue(number, F(""), newLineAfter);
}

void ESPAPP_SerialMessages::addNewLines(uint8_t noOfLines) {
  for (uint8_t i = 0; i < noOfLines; i++) {
    Serial << endl;
  }
}

void ESPAPP_SerialMessages::addMessageHeader(uint8_t type,
                                   const __FlashStringHelper *messageCategory,
                                   uint8_t newLineBefore, uint8_t noOfIntents) {
  addNewLines(newLineBefore);

  for (uint8_t i = 0; i < noOfIntents; i++) {
    Serial << F(" ");
  }

  Serial << (type == ESP_APP_DEBUG_TYPE_INFORMATION
                 ? F(ESP_APP_GREEN)
                 : (type == ESP_APP_DEBUG_TYPE_WARNING
                        ? F(ESP_APP_BLUE)
                        : (type == ESP_APP_DEBUG_TYPE_ERROR
                               ?  F(ESP_APP_RED)
                               : F(""))));
  

  Serial << (type == ESP_APP_DEBUG_TYPE_INFORMATION
                 ? F("INFO")
                 : (type == ESP_APP_DEBUG_TYPE_WARNING
                        ? F("WARN")
                        : (type == ESP_APP_DEBUG_TYPE_ERROR
                               ? F("ERROR")
                               : (type == ESP_APP_DEBUG_TYPE_BULLET_POINT
                                      ? F("* ")
                                      : F("")))));


  Serial << F(ESP_APP_RESET_COLOR); 
  
  if (type == ESP_APP_DEBUG_TYPE_ERROR || type == ESP_APP_DEBUG_TYPE_INFORMATION ||
      type == ESP_APP_DEBUG_TYPE_WARNING) {
    Serial << F(": ") << messageCategory << F(": ");
  }
}

void ESPAPP_SerialMessages::addAdditionalText(const __FlashStringHelper *text,
                                    uint8_t newLineAfter) {
  if (text) {
    Serial << F(" ") << text;
  }

  addNewLines(newLineAfter);
}

void ESPAPP_SerialMessages::printInformation(const char *text,
                                   const __FlashStringHelper *messageCategory,
                                   uint8_t newLineBefore,
                                   uint8_t newLineAfter) {
  print(text, messageCategory, ESP_APP_DEBUG_TYPE_INFORMATION, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialMessages::printInformation(const __FlashStringHelper *text,
                                   const __FlashStringHelper *messageCategory,
                                   uint8_t newLineBefore,
                                   uint8_t newLineAfter) {
  print(text, messageCategory, ESP_APP_DEBUG_TYPE_INFORMATION, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialMessages::printWarning(const char *text,
                               const __FlashStringHelper *messageCategory,
                               uint8_t newLineBefore, uint8_t newLineAfter) {
  print(text, messageCategory, ESP_APP_DEBUG_TYPE_WARNING, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialMessages::printWarning(const __FlashStringHelper *text,
                               const __FlashStringHelper *messageCategory,
                               uint8_t newLineBefore, uint8_t newLineAfter) {
  print(text, messageCategory, ESP_APP_DEBUG_TYPE_WARNING, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialMessages::printError(const char *text,
                             const __FlashStringHelper *messageCategory,
                             uint8_t newLineBefore, uint8_t newLineAfter) {
  print(text, messageCategory, ESP_APP_DEBUG_TYPE_ERROR, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialMessages::printError(const __FlashStringHelper *text,
                             const __FlashStringHelper *messageCategory,
                             uint8_t newLineBefore, uint8_t newLineAfter) {
  print(text, messageCategory, ESP_APP_DEBUG_TYPE_ERROR, newLineBefore,
        newLineAfter, 0);
}

void ESPAPP_SerialMessages::printBulletPoint(const char *text, uint8_t newLineAfter) {
  print(text, F(""), ESP_APP_DEBUG_TYPE_BULLET_POINT, 1, newLineAfter, 2);
}

void ESPAPP_SerialMessages::printBulletPoint(const __FlashStringHelper *text,
                                   uint8_t newLineAfter) {
  print(text, F(""), ESP_APP_DEBUG_TYPE_BULLET_POINT, 1, newLineAfter, 2);
}
void ESPAPP_SerialMessages::printHeader(uint8_t newLineBefore, uint8_t newLineAfter,
                              uint8_t length, uint8_t type) {
  addNewLines(newLineBefore);
  for (uint8_t i = 0; i < length; i++) {
    Serial << (type == ESP_APP_DEBUG_HEADER_TYPE_DASH
                   ? F("-")
                   : (type == ESP_APP_DEBUG_HEADER_TYPE_SPACE ? F(" ") : F("#")));
  }
  addNewLines(newLineAfter);
}

void ESPAPP_SerialMessages::getFreeMemorySize() {
  printInformation(F("Free: "), F("RAM"));
#ifdef ESP32
  Serial << (esp_get_free_heap_size() / 1024) << F("kB");
#else
  Serial << (system_get_free_heap_size() / 1024) << F("kB");
#endif
}

void ESPAPP_SerialMessages::getFileSystemDubugInformation() {
  printInformation(F("Summary: "), F("FS"));
  printBulletPoint(F("Used: "));

#if ESP_APP_FILE_SYSTEM == ESP_APP_FS_SPIFFS
  SPIFFS.info(fileSystem);
  Serial << fileSystem.usedBytes / 1024 << F("/")
         << fileSystem.totalBytes / 1024 << F("kB");
#else
  Serial << LittleFS.usedBytes() / 1024 << F("/")
         << LittleFS.totalBytes() / 1024 << F("kB");
#endif
}

void ESPAPP_SerialMessages::getESPHardwareInformation() {
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
  if (ESP.getFlashChipSize() >= 1048576) {
    Serial << (ESP.getFlashChipSize() / 1048576) << F(" Mbits");
  } else {
    Serial << (ESP.getFlashChipSize() / 1024) << F(" Kbits");
  }

#ifndef ESP32
  printBulletPoint(F("Real Flash size: "));
  if (ESP.getFlashChipRealSize() >= 1048576) {
    Serial << (ESP.getFlashChipRealSize() / 1048576) << F(" Mbits");
  } else {
    Serial << (ESP.getFlashChipRealSize() / 1024) << F(" Kbits");
  }
#endif

  printBulletPoint(F("Flash Speed: "));
  Serial << (ESP.getFlashChipSpeed() / 1000000) << F(" MHz");

  printBulletPoint(F("Flash Chip Mode: "));
  Serial << ESP.getFlashChipMode();

#ifdef ESP32
  printBulletPoint(F("Firmware size: "));
  Serial << ESP.getSketchSize();

  printBulletPoint(F("Firmware free space size: "));
  Serial << ESP.getFreeSketchSpace();
#endif
}

void ESPAPP_SerialMessages::getFirmwareFlashInformation() {

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

void ESPAPP_SerialMessages::getFirmwareAllDebugInformation() {
  getFreeMemorySize();
  getFileSystemDubugInformation();
}

void ESPAPP_SerialMessages::printProcessingRequest(
    const __FlashStringHelper *deviceItemName, uint8_t deviceId,
    const __FlashStringHelper *type) {
  printInformation(F("Processing: "), type);
  Serial << deviceItemName;
  if (deviceId != ESP_APP_UNKNWON) {
    Serial << F(", Id: ") << deviceId;
  }
}

#endif
