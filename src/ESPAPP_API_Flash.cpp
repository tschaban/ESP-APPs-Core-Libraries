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
  // const char *input = "{\"p\":{\"s\":\"IoT World\",\"p\":\"iotiotiot\",\"d\":1,\"i\":\"\",\"g\":\"\",\"b\":\"\",\"d1\":\"8.8.8.8\",\"d2\":\"8.8.4.4\"},\"s\":{\"s\":\"\",\"p\":\"\",\"d\":1,\"i\":\"\",\"g\":\"\",\"b\":\"\",\"d1\":\"\",\"d2\":\"\"},\"n\":20,\"w\":1,\"ws\":1,\"nf\":2,\"m\":1}";
};

bool ESPAPP_API_Flash::init(void)
{
  bool success = false;
  success = mountFileSystem();
  success = formatFileSystem();
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
#ifdef DEBUG
  Msg->printInformation(F("Formatting"), F("FS"), 2);
#endif
  return LittleFS.format();
}

bool ESPAPP_API_Flash::fileExist(const char *path)
{
  bool _ret = LittleFS.exists(path);
#ifdef DEBUG
  Msg->printBulletPoint(F("Found: "));
  Msg->printValue(_ret);
#endif
  return _ret;
}

bool ESPAPP_API_Flash::createFile(const char *path)
{
  boolean _ret = false;
#ifdef DEBUG
  Msg->printBulletPoint(F("Created: "));
#endif

  File createFile = LittleFS.open(path, ESP_APP_OPEN_FILE_WRITING);

  if (createFile)
  {
    _ret = true;
    createFile.close();
  }

#ifdef DEBUG
  Msg->printValue(_ret);
#endif

  return _ret;
}

bool ESPAPP_API_Flash::openFile(File &openedFile, const char *mode,
                                const char *path, uint8_t id, boolean createIfNotExists)
{

#ifdef DEBUG
  Msg->printHeader(1, 0, 72, ESP_APP_MSG_HEADER_TYPE_DASH);
#endif

  bool _status = false;

#ifdef DEBUG
  Msg->printBulletPoint(F("Opening file: "));
  Msg->printValue(path);
#endif

  _status = fileExist(path);

  if (!_status && createIfNotExists)
  {
    _status = createFile(path);
  }

  if (_status)
  {
    openedFile = LittleFS.open(path, mode);
  }

  _status = openedFile ? true : false;

#ifdef DEBUG
  Msg->printBulletPoint(F("Opened: "));
  Msg->printValue(_status);
#endif

  return _status;
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
