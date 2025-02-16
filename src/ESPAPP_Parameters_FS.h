#ifndef _ESPAPP_Parameters_FS_h
#define _ESPAPP_Parameters_FS_h

/* File openig modes */
#define ESPAPP_OPEN_FILE_READING "r"
#define ESPAPP_OPEN_FILE_WRITING "w"
#define ESPAPP_OPEN_FILE_APPEND "a"

/* Directories */
#define ESPAPP_DIRECTORY_CONFIG "cfg"
#define ESPAPP_DIRECTORY_DATA "data"
#define ESPAPP_DIRECTORY_UI "ui"

/* File informing that system file is read */
#define ESPAPP_FILE_SYSTEM_INITIALIZED "/.token"

#define ESPAPP_FILE_MAX_FILE_NAME_LENGTH 30
#define ESPAPP_FILE_MAX_SIZE 10*1024 // 10kB

const char path_root[] PROGMEM = "";
const char path_configuration[] PROGMEM = "cfg";
const char path_data[] PROGMEM = "data";
const char *const ESPAPP_DIRECTORIES[] PROGMEM = {path_root, path_configuration, path_data};

#endif // _ESPAPP_Parameters_FS_h