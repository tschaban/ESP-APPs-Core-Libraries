#include "ESPAPP_API_Flash.h"

#ifdef DEBUG
ESPAPP_API_Flash::ESPAPP_API_Flash(ESPAPP_SerialMessages *_Msg)
{
  Msg = _Msg;
}
#else
ESPAPP_API_Flash::ESPAPP_API_Flash()
{
}
#endif

// Improve performance of reading the files https://arduinojson.org/v6/how-to/improve-speed/

bool ESPAPP_API_Flash::getJSON(const __FlashStringHelper *fileName, JsonDocument &doc)
{
 
  // Hardcoded JSON string for testing
  const char *input = "{\"p\":{\"s\":\"IoT World\",\"p\":\"iotiotiot\",\"d\":1,\"i\":\"\",\"g\":\"\",\"b\":\"\",\"d1\":\"8.8.8.8\",\"d2\":\"8.8.4.4\"},\"s\":{\"s\":\"\",\"p\":\"\",\"d\":1,\"i\":\"\",\"g\":\"\",\"b\":\"\",\"d1\":\"\",\"d2\":\"\"},\"n\":20,\"w\":1,\"ws\":1,\"nf\":2,\"m\":1}";
  DeserializationError error = deserializeJson(doc, input);
  return true;

 
  boolean success = false;
  File configFile;
  if (openFile(configFile, ESP_APP_OPEN_FILE_READING,
               fileName, ESP_APP_NONE, false))
  {

#ifdef DEBUG
    // printFileContentInformation();
#endif

    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    DeserializationError error = deserializeJson(doc, buf.get());

    configFile.close();

    if (!error)
    {
      success = true;
    }
#ifdef DEBUG
    else
      Msg->printError(F("Deserialisation error: "), F("JSON"));
    Msg->printValue(error.c_str());
  }
#endif

  return success;
 };

bool ESPAPP_API_Flash::saveJSON(const __FlashStringHelper *fileName, JsonDocument &doc) {
  boolean success = false;
  File configFile;
  if (openFile(configFile, ESP_APP_OPEN_FILE_WRITING,
               fileName, ESP_APP_NONE, true))
  {
    serializeJson(doc, configFile);
    configFile.close();
    success = true;
  }
  return success;
}

bool ESPAPP_API_Flash::init(void)
{
  bool success = false;
#ifdef DEBUG
  Msg->printInformation(F("Initializing file system"), F("FS"));
#endif
  success = mountFileSystem();

  if (success)
  {
    success = fileExist(ESP_APP_FILE_SYSTEM_INITIALIZED);
#ifdef DEBUG
    Msg->printBulletPoint(F("Files structure exists"));
#endif
  }

  if (!success)
  {
    success = formatFileSystem();
  }

#ifdef DEBUG
  if (success)
  {
    Msg->printBulletPoint(F("File system is ready"));
  }
  else
  {
    Msg->printError(F("File system is NOT ready"), F("FS"));
  }
#endif

  return success;
}

bool ESPAPP_API_Flash::mountFileSystem(void)
{

  bool success = LittleFS.begin();

#ifdef DEBUG
  if (success)
  {
    Msg->printInformation(F("File system mounted: "), F("FS"));
  }
  else
  {
    Msg->printError(F("File system NOT mounted: "), F("FS"));
  }
#endif

  return success;
}

bool ESPAPP_API_Flash::formatFileSystem(void)
{
  bool success = false;
#ifdef DEBUG
  Msg->printBulletPoint(F("Formatting File System"));
#endif

  success = LittleFS.format();

  if (success)
  {
#ifdef DEBUG
    Msg->printBulletPoint(F("Creating directories structure"));
#endif
    LittleFS.mkdir(F(ESP_APP_DIRECTORY_CONFIG));
    LittleFS.mkdir(F(ESP_APP_DIRECTORY_DATA));
    LittleFS.open(F(ESP_APP_FILE_SYSTEM_INITIALIZED), ESP_APP_OPEN_FILE_WRITING).close();
#ifdef DEBUG
    Msg->printBulletPoint(F("Completed"));
  }
  else
  {
    Msg->printError(F("Formatting failed"), F("FS"));
#endif
  }

  return success;
}

bool ESPAPP_API_Flash::fileExist(const char *path)
{
  bool _ret = LittleFS.exists(path);
#ifdef DEBUG
  Msg->printBulletPoint(F("File: "));
  Msg->printValue(path);
  Msg->printValue(F(" - Found: "));
  Msg->printValue(_ret);
#endif
  return _ret;
}

bool ESPAPP_API_Flash::createFile(const char *path)
{
  boolean success = false;
#ifdef DEBUG
  Msg->printBulletPoint(F("File: "));
  Msg->printValue(path);
  Msg->printValue(F(" - Created: "));
#endif

  File createFile = LittleFS.open(path, ESP_APP_OPEN_FILE_WRITING);

  if (createFile)
  {
    success = true;
    createFile.close();
  }

#ifdef DEBUG
  Msg->printValue(success);
#endif

  return success;
}

bool ESPAPP_API_Flash::openFile(File &openedFile, const char *mode,
                                const char *path, uint8_t id, boolean createIfNotExists)
{

#ifdef DEBUG
  Msg->printHeader(1, 0, 72, ESP_APP_MSG_HEADER_TYPE_DASH);
#endif

  bool success = false;

#ifdef DEBUG
  Msg->printBulletPoint(F("Opening file: "));
  Msg->printValue(path);
#endif

  success = fileExist(path);

  if (!success && createIfNotExists)
  {
    success = createFile(path);
  }

  if (success)
  {
    openedFile = LittleFS.open(path, mode);
  }

  success = openedFile ? true : false;

#ifdef DEBUG
  Msg->printValue(F(" - Opened: "));
  Msg->printValue(success);
#endif

  return success;
}

bool ESPAPP_API_Flash::openFile(File &openedFile, const char *mode,
                                const __FlashStringHelper *path, uint8_t id,
                                boolean createIfNotExists)
{

  char fileName[strlen_P((PGM_P)path) + 1];
  if (id == ESP_APP_NONE)
  {
    sprintf(fileName, (PGM_P)path);
  }
  else
  {
    sprintf(fileName, (PGM_P)path, id);
  }

  return openFile(openedFile, mode, fileName, id, createIfNotExists);
}
