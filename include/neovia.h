#pragma once

#include <switch.h>
#include <string>
#include <vector>

#define NEOVIA_VERSION "1.0.0"
#define NEOVIA_AUTHOR "Unix228"
#define GRAPHICS_PATH "/graphics/"

enum Language {
    LANG_RU = 0,
    LANG_EN = 1,
    LANG_UK = 2
};

enum Priority {
    PRIORITY_FPS = 0,
    PRIORITY_STABILITY = 1,
    PRIORITY_GRAPHICS = 2
};

struct GameInfo {
    std::string titleId;
    std::string name;
    std::string version;
    bool hasIcon;
    u8* iconData;
    size_t iconSize;
};

struct Config {
    bool firstRun;
    Language language;
    Priority priority;
    bool dynamicResolution;
    bool downloadAllMods;
    bool autoStart;
    bool extrasInstalled;
};

extern bool g_exitRequested;
Result initializeApp();
void finalizeApp();
Result createDirectoryStructure();
