#include "config.h"
#include "neovia.h"
#include <switch.h>

Result loadConfig(Config& config) {
    config.firstRun = true;
    config.language = LANG_RU;
    config.priority = PRIORITY_GRAPHICS;
    config.dynamicResolution = true;
    config.downloadAllMods = true;
    config.autoStart = false;
    config.extrasInstalled = false;
    return 0;
}

Result saveConfig(const Config& config) {
    return 0;
}

std::string getLanguageString(Language lang, const std::string& key) {
    if (key == "app_title") return "NEOVIA";
    if (key == "settings") return "Настройки";
    return key;
}
