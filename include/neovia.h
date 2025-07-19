#pragma once

#include <switch.h>
#include <string>
#include <vector>

// Версия приложения
#define NEOVIA_VERSION "1.0.0"
#define NEOVIA_AUTHOR "Unix228"

// Пути
#define GRAPHICS_PATH "/graphics/"
#define NEOVIA_PATH "/switch/NEOVIA/"
#define SETTINGS_PATH "/graphics/settings.cfg"
#define EXTRAS_PATH "/switch/NEOVIA/extras/"
#define MODS_REPO_URL "https://api.github.com/repos/UNIX228/NEOVIA-mods/contents/"
#define EXTRAS_DOWNLOAD_URL "https://github.com/UNIX228/NEOVIA-mods/releases/latest/download/extra.zip"

// Языки
enum Language {
    LANG_RU = 0,
    LANG_EN = 1,
    LANG_UK = 2
};

// Приоритеты
enum Priority {
    PRIORITY_FPS = 0,
    PRIORITY_STABILITY = 1,
    PRIORITY_GRAPHICS = 2
};

// Структура информации об игре
struct GameInfo {
    std::string titleId;
    std::string name;
    std::string version;
    bool hasIcon;
    u8* iconData;
    size_t iconSize;
};

// Структура информации о моде
struct ModInfo {
    std::string title;
    std::string description;
    std::string author;
    std::string version;
    std::string gameId;
    bool installed;
    bool enabled;
};

// Структура конфигурации
struct Config {
    bool firstRun;
    Language language;
    Priority priority;
    bool dynamicResolution;
    bool downloadAllMods;
    bool autoStart;
    bool extrasInstalled;
};

// Структура для прогресса загрузки
struct DownloadProgress {
    std::string currentFile;
    size_t currentBytes;
    size_t totalBytes;
    int percentage;
    bool completed;
    bool error;
    std::string errorMessage;
};

// Функции из main.cpp
extern bool g_exitRequested;
Result initializeApp();
void finalizeApp();
Result createDirectoryStructure();
Result firstTimeSetup();