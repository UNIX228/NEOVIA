#include "config.h"
#include "neovia.h"
#include <switch.h>
#include <fstream>
#include <map>

Result loadConfig(Config& config) {
    // Пытаемся загрузить конфигурацию
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
    // Сохраняем конфигурацию
    return 0;
}

std::string getLanguageString(Language lang, const std::string& key) {
    static std::map<std::string, std::map<Language, std::string>> translations = {
        {"app_title", {
            {LANG_RU, "NEOVIA"},
            {LANG_EN, "NEOVIA"},
            {LANG_UK, "NEOVIA"}
        }},
        {"settings", {
            {LANG_RU, "Настройки"},
            {LANG_EN, "Settings"},
            {LANG_UK, "Налаштування"}
        }}
    };
    
    auto keyIt = translations.find(key);
    if (keyIt != translations.end()) {
        auto langIt = keyIt->second.find(lang);
        if (langIt != keyIt->second.end()) {
            return langIt->second;
        }
    }
    
    return key;
}
