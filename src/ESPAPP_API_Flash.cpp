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

bool ESPAPP_API_Flash::getJSON(const __FlashStringHelper *fileName, JsonDocument &doc)
{

  boolean success = false;
  File configFile;
  if (openFile(configFile, ESPAPP_OPEN_FILE_READING,
               fileName, ESPAPP_NONE, false))
  {

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
    {
      this->Msg->printError(F("Deserialisation error: "), F("JSON"));
      this->Msg->printValue(error.c_str());
    }
  }
#endif

  return success;
};

bool ESPAPP_API_Flash::saveJSON(const __FlashStringHelper *fileName, JsonDocument &doc)
{
  boolean success = false;
  File configFile;

  if (openFile(configFile, ESPAPP_OPEN_FILE_WRITING,
               fileName, ESPAPP_NONE, true))
  {

    if (serializeJson(doc, configFile) != 0)
    {
      success = true;
    }

    configFile.close();
  }

#ifdef DEBUG
  if (success)
  {
    this->Msg->printInformation(F("JSON saved successfully"), F("JSON"));
  }
  else
  {
    this->Msg->printError(F("Failed to save JSON"), F("JSON"));
  }
#endif

  return success;
}

bool ESPAPP_API_Flash::init(void)
{
#ifdef DEBUG
  this->Msg->printInformation(F("Initializing file system"), F("FS"));
#endif
  fileSystemReady = mountFileSystem();

  if (fileSystemReady)
  {

    sprintf(this->fileName, "%s%s%s", FPSTR(path_boot), FPSTR(path_root), F(ESPAPP_FILE_SYSTEM_INITIALIZED));

    fileSystemReady = fileExist(this->fileName);
#ifdef DEBUG
    this->Msg->printBulletPoint(F("Files structure exists"));
#endif
  }

  if (!fileSystemReady)
  {
    fileSystemReady = formatFileSystem();
  }

#ifdef DEBUG
  if (fileSystemReady)
  {
    this->Msg->printBulletPoint(F("File system is ready"));
  }
  else
  {
    this->Msg->printError(F("File system is NOT ready"), F("FS"));
  }
#endif

  return fileSystemReady;
}

bool ESPAPP_API_Flash::initialized(void)
{
  return fileSystemReady;
}

bool ESPAPP_API_Flash::mountFileSystem(void)
{

  bool success = fileSystem.begin(false, (PGM_P)F(ESPAPP_FS_BASEPATH), ESPAPP_FS_MAX_OPEN_FILES, (PGM_P)F(ESPAPP_FS_PARTITION_NAME));

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

  success = fileSystem.format();
  {
    success = fileSystem.begin(false, (PGM_P)F(ESPAPP_FS_BASEPATH), ESPAPP_FS_MAX_OPEN_FILES, (PGM_P)F(ESPAPP_FS_PARTITION_NAME));
  }

  if (success)
  {

    for (uint8_t i = 0; i < sizeof(ESPAPP_DIRECTORIES) / sizeof(ESPAPP_DIRECTORIES[0]); i++)
    {
      strcpy_P(this->fileName, (char *)pgm_read_dword(&(ESPAPP_DIRECTORIES[i])));
      this->createFolder(this->fileName);
    }

    sprintf(this->fileName, "%s%s%s", FPSTR(path_boot), FPSTR(path_root), F(ESPAPP_FILE_SYSTEM_INITIALIZED));
    fileSystem.open(this->fileName, ESPAPP_OPEN_FILE_WRITING).close();

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
  bool _ret = fileSystem.exists(path);
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

  File createFile = fileSystem.open(path, ESPAPP_OPEN_FILE_WRITING);

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
  this->Msg->printHeader(1, 0, 72, ESPAPP_MSG_HEADER_TYPE_DASH);
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
    openedFile = fileSystem.open(path, mode);
  }

  success = openedFile ? true : false;

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Opened: "));
  this->Msg->printValue(success);
#endif

  return success;
}

bool ESPAPP_API_Flash::openFile(File &openedFile, const char *mode,
                                const __FlashStringHelper *path, uint8_t id,
                                boolean createIfNotExists)
{

  char fileName[strlen_P((PGM_P)path) + 1];
  if (id == ESPAPP_NONE)
  {
    sprintf(fileName, (PGM_P)path);
  }
  else
  {
    sprintf(fileName, (PGM_P)path, id);
  }

  return openFile(openedFile, mode, fileName, id, createIfNotExists);
}

bool ESPAPP_API_Flash::listFiles(ESPAPP_FILE files[], size_t capacity, size_t &count)
{
  return readFSElements((PGM_P)F(""), files, capacity, count, true, false);
}

bool ESPAPP_API_Flash::listFiles(const char *directory, ESPAPP_FILE files[], size_t capacity, size_t &count)
{
  return readFSElements(directory, files, capacity, count, true, false);
}

bool ESPAPP_API_Flash::listFolders(ESPAPP_FILE files[], size_t capacity, size_t &count)
{
  return readFSElements((PGM_P)F(""), files, capacity, count, false, true);
}

bool ESPAPP_API_Flash::listFolders(const char *directory, ESPAPP_FILE files[], size_t capacity, size_t &count)
{
  return readFSElements(directory, files, capacity, count, false, true);
}

bool ESPAPP_API_Flash::readFSElements(const char *directory, ESPAPP_FILE files[], size_t capacity, size_t &count, bool includeFiles, bool includeFolders)
{
  count = 0;
  char listedDirectory[strlen(directory) + 2];
  char listedFilename[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
  sprintf(listedDirectory, "%s%s", FPSTR(path_root), directory);

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Listing directory: "));
  this->Msg->printValue(listedDirectory);
#endif

  File root = fileSystem.open(listedDirectory);
  if (!root || !root.isDirectory())
  {
#ifdef DEBUG
    this->Msg->printError(F("Failed to open directory: "), F("FS"));
    this->Msg->printValue(listedDirectory);
#endif
    return false;
  }

  // First, list directories if includeFolders = true
  if (includeFolders)
  {
    root.rewindDirectory();
    File entry = root.openNextFile();
    while (entry && count < capacity)
    {
      if (entry.isDirectory())
      {
        strncpy(files[count].name, entry.name(), sizeof(files[count].name) - 1);
        files[count].name[sizeof(files[count].name) - 1] = '\0';
        files[count].isDirectory = true;
        sprintf(listedFilename, "%s%s", FPSTR(path_root), entry.name());

        // Count how many files are in this directory
        File subDir = fileSystem.open(listedFilename);
        size_t fileCount = 0;
        if (subDir && subDir.isDirectory())
        {
          File subEntry = subDir.openNextFile();
          while (subEntry)
          {
            if (!subEntry.isDirectory())
            {
              fileCount++;
            }
            subEntry = subDir.openNextFile();
          }
          subDir.close();
        }
        files[count].size = fileCount; // store number of files in 'size'
        count++;

#ifdef DEBUG
        this->Msg->printBulletPoint(F("Directory: "));
        this->Msg->printValue(files[count - 1].name);
        this->Msg->printValue(F(" - Files count: "));
        this->Msg->printValue(files[count - 1].size);
#endif
      }
      entry = root.openNextFile();
    }
  }

  // Then, list files if includeFiles = true
  if (includeFiles)
  {
    root.rewindDirectory();
    File entry = root.openNextFile();
    while (entry && count < capacity)
    {
      if (!entry.isDirectory())
      {
        strncpy(files[count].name, entry.name(), sizeof(files[count].name) - 1);
        files[count].name[sizeof(files[count].name) - 1] = '\0';
        files[count].isDirectory = false;
        files[count].size = entry.size(); // store file size in bytes
        count++;

#ifdef DEBUG
        this->Msg->printBulletPoint(F("File: "));
        this->Msg->printValue(files[count - 1].name);
        this->Msg->printValue(F(" - Size: "));
        this->Msg->printValue(files[count - 1].size);
#endif
      }
      entry = root.openNextFile();
    }
  }

  root.close();
  return true;
}

bool ESPAPP_API_Flash::uploadFile(const char *directory, const char *filename, const uint8_t *data, size_t length)
{

#ifdef DEBUG
  this->Msg->printInformation(F("Uploading file"), F("uploadFile"));
  this->Msg->printBulletPoint(F("Directory: "));
  this->Msg->printValue(directory);
  this->Msg->printBulletPoint(F("Filename: "));
  this->Msg->printValue(filename);
  this->Msg->printBulletPoint(F("Data length: "));
  this->Msg->printValue(length);
#endif

  sprintf(this->fileName, "%s%s", FPSTR(path_root), directory);

  // Ensure directory exists
  if (!fileSystem.exists(this->fileName))
  {
    fileSystem.mkdir(this->fileName);
#ifdef DEBUG
    this->Msg->printBulletPoint(F("Directory created: "));
    this->Msg->printValue(this->fileName);
#endif
  }

  this->getPathToFile(this->fileName, directory, filename);

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Path: "));
  this->Msg->printValue(this->fileName);
#endif

  File file = fileSystem.open(this->fileName, ESPAPP_OPEN_FILE_WRITING);
  if (!file)
  {
#ifdef DEBUG
    this->Msg->printError(F("File not saved: "), F("FS"));
    this->Msg->printValue(this->fileName);
#endif
    return false;
  }
  file.write(data, length);
  file.close();
#ifdef DEBUG
  this->Msg->printBulletPoint(F("File saved: "));
  this->Msg->printValue(this->fileName);
#endif
  return true;
}

bool ESPAPP_API_Flash::deleteFile(const char *directory, const char *path)
{
  this->getPathToFile(this->fileName, directory, path);

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Deleting file: "));
  this->Msg->printValue(this->fileName);
#endif
  if (fileSystem.exists(this->fileName))
  {
    return fileSystem.remove(this->fileName);
  }
  return false;
}

void ESPAPP_API_Flash::getPathToFile(char *path, const char *directory, const char *filename)
{
  sprintf(path, "%s", FPSTR(path_root));
  if (strlen(directory) > 0)
  {
    sprintf(path, "%s%s%s", path, directory, FPSTR(path_root));
  }
  sprintf(path, "%s%s", path, filename);
}

/** Folders related  */

bool ESPAPP_API_Flash::createFolder(const char *path)
{

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Creating folder: "));
  this->Msg->printValue(path);
#endif
  bool success = fileSystem.mkdir(path);
#ifdef DEBUG
  if (success)
  {
    this->Msg->printBulletPoint(F("Folder created successfully"));
  }
  else
  {
    this->Msg->printError(F("Failed to create folder: "), F("FS"));
    this->Msg->printValue(path);
  }
#endif
  return success;
}

bool ESPAPP_API_Flash::deleteFolder(const char *path)
{

  this->getPathToFile(this->fileName, "", path);

#ifdef DEBUG
  this->Msg->printBulletPoint(F("Deleting folder: "));
  this->Msg->printValue(this->fileName);
#endif

  File dir = fileSystem.open(this->fileName);
  if (!dir || !dir.isDirectory())
  {
    return false;
  }

  dir.rewindDirectory();
  File entry = dir.openNextFile();
  bool isEmpty = !entry;
  entry.close();
  dir.close();

  if (isEmpty)
  {
    return fileSystem.rmdir(this->fileName);
  }
  return false;
}

bool ESPAPP_API_Flash::deleteAllFilesInDirectory(const __FlashStringHelper *directory)
{
#ifdef DEBUG
  this->Msg->printBulletPoint(F("Deleting all files in directory: "));
  this->Msg->printValue(directory);
#endif
  


  File dir = fileSystem.open(directory);
  if (!dir || !dir.isDirectory())
  {
#ifdef DEBUG
    this->Msg->printError(F("Failed to open directory: "), F("FS"));
    this->Msg->printValue(directory);
#endif
    return false;
  }

  char fileToDelete[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
  dir.rewindDirectory();
  File entry = dir.openNextFile();
  
  while (entry)
  {
    if (!entry.isDirectory())
    {
      sprintf(fileToDelete, entry.name());
      entry.close();
      this->deleteFile((PGM_P)directory, fileToDelete);       
    }
    entry = dir.openNextFile();
  }

  dir.close();
  return true;
}
