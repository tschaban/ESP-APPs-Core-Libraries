#include "ESPAPP_API_Flash.h"

#ifdef DEBUG
ESPAPP_API_Flash::ESPAPP_API_Flash(ESPAPP_SerialDebugger *_Debugger)
{
  this->Debugger = _Debugger;
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
      this->Debugger->printBulletPoint(F("Deserialisation error: "), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
      this->Debugger->printValue(error.c_str());
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
  if (!success)
   {
    this->Debugger->printBulletPoint(F("Failed to save JSON"), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
  }
#endif

  return success;
}

bool ESPAPP_API_Flash::init(void)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Initializing file system"), F("FS"));
#endif
  fileSystemReady = this->mountFileSystem();

  if (fileSystemReady)
  {

    sprintf(this->fileName, "%s%s%s", FPSTR(path_boot), FPSTR(path_root), F(ESPAPP_FILE_SYSTEM_INITIALIZED));

    fileSystemReady = this->fileExist(this->fileName);
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Files structure exists"));
#endif
  }

  if (!fileSystemReady)
  {
    fileSystemReady = formatFileSystem();
  }

#ifdef DEBUG
  if (fileSystemReady)
  {
    this->Debugger->printBulletPoint(F("File system is ready"));
  }
  else
  {
    this->Debugger->printBulletPoint(F("File system is NOT ready"), ESPAPP_DEBUGGER_MESSAGE_COLOR::BLUE);
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
    this->Debugger->printBulletPoint(F("File system mounted"));
  }
  else
  {
    this->Debugger->printBulletPoint(F("File system NOT mounted"), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
  }
#endif

  return success;
}

bool ESPAPP_API_Flash::formatFileSystem(void)
{
  bool success = false;
#ifdef DEBUG
  this->Debugger->printBulletPoint(F("Formatting File System"));
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
    this->Debugger->printBulletPoint(F("Completed"));
  }
  else
  {
    this->Debugger->printBulletPoint(F("Formatting failed"), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
#endif
  }

  return success;
}

bool ESPAPP_API_Flash::fileExist(const char *path)
{
  bool _ret = fileSystem.exists(path);
#ifdef DEBUG
  if (!_ret)
  {
    this->Debugger->printBulletPoint(F("File not found: "),ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
  }
#endif
  return _ret;
}

bool ESPAPP_API_Flash::createFile(const char *path)
{
  boolean success = false;
#ifdef DEBUG
  this->Debugger->printBulletPoint(F("File: "));
  this->Debugger->printValue(path);
  this->Debugger->printValue(F(" - Created: "));
#endif

  File createFile = fileSystem.open(path, ESPAPP_OPEN_FILE_WRITING);

  if (createFile)
  {
    success = true;
    createFile.close();
  }

#ifdef DEBUG
  this->Debugger->printValue(success);
#endif

  return success;
}

bool ESPAPP_API_Flash::openFile(File &openedFile, const char *mode,
                                const char *path, uint8_t id, boolean createIfNotExists)
{

  bool success = false;

#ifdef DEBUG
  this->Debugger->printBulletPoint(F("Opening file: "));
  this->Debugger->printValue(path);
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
  if (!success)
  {
    this->Debugger->printBulletPoint(F("Failed to open file: "), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
  }
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
  this->Debugger->printBulletPoint(F("Listing directory: "));
  this->Debugger->printValue(listedDirectory);
#endif

  File root = fileSystem.open(listedDirectory);
  if (!root || !root.isDirectory())
  {
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Failed to open directory: "), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
    this->Debugger->printValue(listedDirectory);
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
        this->Debugger->printBulletPoint(F("Directory: "));
        this->Debugger->printValue(files[count - 1].name);
        this->Debugger->printValue(F(" - Files count: "));
        this->Debugger->printValue(files[count - 1].size);
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
        this->Debugger->printBulletPoint(F("File: "));
        this->Debugger->printValue(files[count - 1].name);
        this->Debugger->printValue(F(" - Size: "));
        this->Debugger->printValue(files[count - 1].size);
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
  this->Debugger->printInformation(F("Uploading file"), F("uploadFile"));
  this->Debugger->printBulletPoint(F("Directory: "));
  this->Debugger->printValue(directory);
  this->Debugger->printBulletPoint(F("Filename: "));
  this->Debugger->printValue(filename);
  this->Debugger->printBulletPoint(F("Data length: "));
  this->Debugger->printValue(length);
#endif

  sprintf(this->fileName, "%s%s", FPSTR(path_root), directory);

  // Ensure directory exists
  if (!fileSystem.exists(this->fileName))
  {
    fileSystem.mkdir(this->fileName);
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Directory created: "));
    this->Debugger->printValue(this->fileName);
#endif
  }

  this->getPathToFile(this->fileName, directory, filename);

#ifdef DEBUG
  this->Debugger->printBulletPoint(F("Path: "));
  this->Debugger->printValue(this->fileName);
#endif

  File file = fileSystem.open(this->fileName, ESPAPP_OPEN_FILE_WRITING);
  if (!file)
  {
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("File not saved: "), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
    this->Debugger->printValue(this->fileName);
#endif
    return false;
  }
  file.write(data, length);
  file.close();
#ifdef DEBUG
  this->Debugger->printBulletPoint(F("File saved: "));
  this->Debugger->printValue(this->fileName);
#endif
  return true;
}

bool ESPAPP_API_Flash::deleteFile(const char *directory, const char *path)
{
  this->getPathToFile(this->fileName, directory, path);

#ifdef DEBUG
  this->Debugger->printBulletPoint(F("Deleting file: "));
  this->Debugger->printValue(this->fileName);
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
  this->Debugger->printBulletPoint(F("Creating folder: "));
  this->Debugger->printValue(path);
#endif
  bool success = fileSystem.mkdir(path);
#ifdef DEBUG
  if (success)
  {
    this->Debugger->printBulletPoint(F("Folder created successfully"));
  }
  else
  {
    this->Debugger->printBulletPoint(F("Failed to create folder: "), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
    this->Debugger->printValue(path);
  }
#endif
  return success;
}

bool ESPAPP_API_Flash::deleteFolder(const char *path)
{

  this->getPathToFile(this->fileName, "", path);

#ifdef DEBUG
  this->Debugger->printBulletPoint(F("Deleting folder: "));
  this->Debugger->printValue(this->fileName);
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
  this->Debugger->printBulletPoint(F("Deleting all files in directory: "));
  this->Debugger->printValue(directory);
#endif

  File dir = fileSystem.open(directory);
  if (!dir || !dir.isDirectory())
  {
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Failed to open directory: "), ESPAPP_DEBUGGER_MESSAGE_COLOR::RED);
    this->Debugger->printValue(directory);
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
