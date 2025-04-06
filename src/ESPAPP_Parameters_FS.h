#ifndef _ESPAPP_Parameters_FS_h
#define _ESPAPP_Parameters_FS_h

/** Disk parameters */
#define ESPAPP_FS_BASEPATH "/littlefs"
#define ESPAPP_FS_MAX_OPEN_FILES 10
#define ESPAPP_FS_PARTITION_NAME "spiffs"

/* File openig modes */
#define ESPAPP_OPEN_FILE_READING "r"
#define ESPAPP_OPEN_FILE_WRITING "w"
#define ESPAPP_OPEN_FILE_APPEND "a"

/* File informing that system file is read */
#define ESPAPP_FILE_SYSTEM_INITIALIZED ".token"

#define ESPAPP_FILE_MAX_FILE_NAME_LENGTH 30
#define ESPAPP_FILE_MAX_SIZE 10*1024 // 10kB

/* Directories */
const char path_root[] PROGMEM = "/";

const char path_configuration[] PROGMEM = "/etc";
const char path_data[] PROGMEM = "/var";
const char path_ui[] PROGMEM = "/srv";
const char path_temp[] PROGMEM = "/tmp";
const char path_sys[] PROGMEM = "/sys";
const char path_boot[] PROGMEM = "/boot";
const char *const ESPAPP_DIRECTORIES[] PROGMEM = {path_boot, path_configuration, path_ui, path_sys, path_temp, path_data};

/** Files */

#define ESPAPP_SYS_FILE_MESSAGES "/sys/messages.txt"


#endif // _ESPAPP_Parameters_FS_h