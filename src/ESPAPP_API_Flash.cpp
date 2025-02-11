#include "ESPAPP_API_Flash.h"

#ifdef DEBUG
ESPAPP_API_Flash::ESPAPP_API_Flash(ESPAPP_SerialMessages *_Msg)
{
  this->Msg = _Msg;
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
      this->Msg->printError(F("Deserialisation error: "), F("JSON"));
    this->Msg->printValue(error.c_str());
  }
#endif

  return success;
};

bool ESPAPP_API_Flash::saveJSON(const __FlashStringHelper *fileName, JsonDocument &doc)
{
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
  this->Msg->printInformation(F("Initializing file system"), F("FS"));
#endif
  success = mountFileSystem();

  if (success)
  {
    success = fileExist(ESP_APP_FILE_SYSTEM_INITIALIZED);
#ifdef DEBUG
    this->Msg->printBulletPoint(F("Files structure exists"));
#endif
  }

  if (!success)
  {
    success = formatFileSystem();
  }

#ifdef DEBUG
  if (success)
  {
    this->Msg->printBulletPoint(F("File system is ready"));
  }
  else
  {
    this->Msg->printError(F("File system is NOT ready"), F("FS"));
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
    this->Msg->printInformation(F("File system mounted: "), F("FS"));
  }
  else
  {
    this->Msg->printError(F("File system NOT mounted: "), F("FS"));
  }
#endif

  return success;
}

bool ESPAPP_API_Flash::formatFileSystem(void)
{
  bool success = false;
#ifdef DEBUG
  this->Msg->printBulletPoint(F("Formatting File System"));
#endif

  success = LittleFS.format();

  if (success)
  {
#ifdef DEBUG
    this->Msg->printBulletPoint(F("Creating directories structure"));
#endif
    LittleFS.mkdir(F(ESP_APP_DIRECTORY_CONFIG));
    LittleFS.mkdir(F(ESP_APP_DIRECTORY_DATA));
    LittleFS.open(F(ESP_APP_FILE_SYSTEM_INITIALIZED), ESP_APP_OPEN_FILE_WRITING).close();
#ifdef DEBUG
    this->Msg->printBulletPoint(F("Completed"));
  }
  else
  {
    this->Msg->printError(F("Formatting failed"), F("FS"));
#endif
  }

  return success;
}

bool ESPAPP_API_Flash::fileExist(const char *path)
{
  bool _ret = LittleFS.exists(path);
#ifdef DEBUG
  this->Msg->printBulletPoint(F("File: "));
  this->Msg->printValue(path);
  this->Msg->printValue(F(" - Found: "));
  this->Msg->printValue(_ret);
#endif
  return _ret;
}

bool ESPAPP_API_Flash::createFile(const char *path)
{
  boolean success = false;
#ifdef DEBUG
  this->Msg->printBulletPoint(F("File: "));
  this->Msg->printValue(path);
  this->Msg->printValue(F(" - Created: "));
#endif

  File createFile = LittleFS.open(path, ESP_APP_OPEN_FILE_WRITING);

  if (createFile)
  {
    success = true;
    createFile.close();
  }

#ifdef DEBUG
  this->Msg->printValue(success);
#endif

  return success;
}

bool ESPAPP_API_Flash::openFile(File &openedFile, const char *mode,
                                const char *path, uint8_t id, boolean createIfNotExists)
{

#ifdef DEBUG
  this->Msg->printHeader(1, 0, 72, ESP_APP_MSG_HEADER_TYPE_DASH);
#endif

  bool success = false;

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Opening file: "));
  this->Msg->printValue(path);
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
  this->Msg->printValue(F(" - Opened: "));
  this->Msg->printValue(success);
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

bool ESPAPP_API_Flash::listAllFiles(ESPAPP_FILE files[], size_t capacity, size_t &count)
{
  return this->listAllFiles((PGM_P)F(ESP_APP_EMPTY_STRING), files, capacity, count);
}

bool ESPAPP_API_Flash::listAllFiles(const char *directory, ESPAPP_FILE files[], size_t capacity, size_t &count)
{
  count = 0;
  char listedDirectory[strlen(directory) + 1];
  sprintf(listedDirectory, "/%s", directory);

#ifdef DEBUG
  this->Msg->printInformation(F("Listing files in directory"), F("FS"));
  this->Msg->printBulletPoint(F("Directory: "));
  this->Msg->printValue(listedDirectory);
#endif

  File root = LittleFS.open(listedDirectory);
  if (!root || !root.isDirectory())
  {
#ifdef DEBUG
    this->Msg->printBulletPoint(F("Directory not found"));
#endif
    return false;
  }

  File file = root.openNextFile();
  while (file && count < capacity)
  {
    strncpy(files[count].name, file.name(), sizeof(files[count].name) - 1);
    files[count].name[sizeof(files[count].name) - 1] = '\0';
    files[count].isDirectory = file.isDirectory();
    files[count].size = file.size();
    count++;
    file = root.openNextFile();
  }

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Files in the directory: "));
  this->Msg->printValue(count);
#endif

  root.close();
  return true;
}

bool ESPAPP_API_Flash::uploadFile(const char *directory, const char *filename, const uint8_t *data, size_t length)
{

  this->Msg->printInformation(F("Uploading file"), F("uploadFile"));
  this->Msg->printBulletPoint(F("Directory: "));
  this->Msg->printValue(directory);
  this->Msg->printBulletPoint(F("Filename: "));
  this->Msg->printValue(filename);
  this->Msg->printBulletPoint(F("Data length: "));
  this->Msg->printValue(length);

  char uploadDirectroy[strlen(directory) + 1];
  char uploadFilename[strlen(uploadDirectroy) + strlen(filename) + 1];
  sprintf(uploadDirectroy, "/%s", directory);
  sprintf(uploadFilename, "%s/%s", uploadDirectroy, filename);

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Directory: "));
  this->Msg->printValue(uploadDirectroy);
  this->Msg->printBulletPoint(F("Upload path: "));
  this->Msg->printValue(uploadFilename);
#endif

  // Ensure directory exists
  if (!LittleFS.exists(uploadDirectroy))
  {
    LittleFS.mkdir(uploadDirectroy);
#ifdef DEBUG
    this->Msg->printBulletPoint(F("Directory created: "));
    this->Msg->printValue(uploadDirectroy);
#endif
  }

  File file = LittleFS.open(uploadFilename, ESP_APP_OPEN_FILE_WRITING);
  if (!file)
  {
#ifdef DEBUG
    this->Msg->printError(F("File not saved: "), F("FS"));
    this->Msg->printValue(uploadFilename);
#endif
    return false;
  }
  file.write(data, length);
  file.close();
#ifdef DEBUG
  this->Msg->printBulletPoint(F("File saved: "));
  this->Msg->printValue(uploadFilename);
#endif
  return true;
}