#ifndef LOG
#define LOG

enum LogLevel {
    LOG_LEVEL_NONE = 0x00, 
    LOG_LEVEL_ERROR = 0x01, 
    LOG_LEVEL_INFO = 0x03, 
    LOG_LEVEL_DEBUG = 0x07, 
    LOG_LEVEL_ALL = 0xFF
};

void AddLog(enum LogLevel level, const char* value);
void AddLog(enum LogLevel level, String value);
void AddLog(LogLevel level);

#define LOGDATASIZE 520

extern char log_data[LOGDATASIZE];

#endif