#pragma once
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#define PRINTF SEGGER_RTT_printf

#define LOGF(tag, format, ...)                                   \
do {                                                         \
PRINTF(0, "Fatal: [" tag "] " format "", ##__VA_ARGS__); \
} while (0)
#define LOGE(tag, format, ...)                                   \
do {                                                         \
PRINTF(0, "Error: [" tag "] " format "", ##__VA_ARGS__); \
} while (0)
#define LOGW(tag, format, ...)                                   \
do {                                                         \
PRINTF(0, "Warn:  [" tag "] " format "", ##__VA_ARGS__); \
} while (0)
    #define LOGI(tag, format, ...)                                   \
    do {                                                         \
    PRINTF(0, "Info:  [" tag "] " format "", ##__VA_ARGS__); \
    } while (0)
#define LOGD(tag, format, ...)                                   \
do {                                                         \
PRINTF(0, "Debug: [" tag "] " format "", ##__VA_ARGS__); \
} while (0)

#define LOG(format, ...)                  \
do {                                  \
PRINTF(0, format, ##__VA_ARGS__); \
} while (0)
