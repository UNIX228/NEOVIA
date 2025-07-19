#include "config.h"
#include "neovia.h"
#include <switch.h>
#include <fstream>
#include <json/json.h>

// Загрузка конфигурации из файла
Result loadConfig(Config& config) {
    std::ifstream file(SETTINGS_PATH);
    if (!file.is_open()) {
        // Файл не существует, используем настройки по умолчанию
        config.firstRun = true;
        config.language = LANG_RU;
        config.priority = PRIORITY_GRAPHICS;
        config.dynamicResolution = true;
        config.downloadAllMods = true;
        config.autoStart = false;
        config.extrasInstalled = false;
        return MAKERESULT(Module_Libnx, LibnxError_NotFound);
    }
    
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    
    if (!Json::parseFromStream(builder, file, &root, &errors)) {
        file.close();
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    file.close();
    
    // Загружаем настройки
    config.firstRun = root.get("firstRun", true).asBool();
    config.language = static_cast<Language>(root.get("language", LANG_RU).asInt());
    config.priority = static_cast<Priority>(root.get("priority", PRIORITY_GRAPHICS).asInt());
    config.dynamicResolution = root.get("dynamicResolution", true).asBool();
    config.downloadAllMods = root.get("downloadAllMods", true).asBool();
    config.autoStart = root.get("autoStart", false).asBool();
    config.extrasInstalled = root.get("extrasInstalled", false).asBool();
    
    return 0;
}

// Сохранение конфигурации в файл
Result saveConfig(const Config& config) {
    Json::Value root;
    
    root["firstRun"] = config.firstRun;
    root["language"] = static_cast<int>(config.language);
    root["priority"] = static_cast<int>(config.priority);
    root["dynamicResolution"] = config.dynamicResolution;
    root["downloadAllMods"] = config.downloadAllMods;
    root["autoStart"] = config.autoStart;
    root["extrasInstalled"] = config.extrasInstalled;
    
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    
    std::ofstream file(SETTINGS_PATH);
    if (!file.is_open()) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    writer->write(root, &file);
    file.close();
    
    return 0;
}

// Получение строки языка
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
        }},
        {"priority", {
            {LANG_RU, "Приоритет"},
            {LANG_EN, "Priority"},
            {LANG_UK, "Пріоритет"}
        }},
        {"priority_fps", {
            {LANG_RU, "FPS"},
            {LANG_EN, "FPS"},
            {LANG_UK, "FPS"}
        }},
        {"priority_stability", {
            {LANG_RU, "Стабильность"},
            {LANG_EN, "Stability"},
            {LANG_UK, "Стабільність"}
        }},
        {"priority_graphics", {
            {LANG_RU, "Графика"},
            {LANG_EN, "Graphics"},
            {LANG_UK, "Графіка"}
        }},
        {"dynamic_resolution", {
            {LANG_RU, "Динамическое разрешение"},
            {LANG_EN, "Dynamic Resolution"},
            {LANG_UK, "Динамічна роздільність"}
        }},
        {"download_all_mods", {
            {LANG_RU, "Загрузка модов для всех игр"},
            {LANG_EN, "Download mods for all games"},
            {LANG_UK, "Завантаження модів для всіх ігор"}
        }},
        {"auto_start", {
            {LANG_RU, "Автозапуск NEOVIA"},
            {LANG_EN, "Auto-start NEOVIA"},
            {LANG_UK, "Автозапуск NEOVIA"}
        }},
        {"download_extras", {
            {LANG_RU, "Скачать дополнительные возможности"},
            {LANG_EN, "Download additional features"},
            {LANG_UK, "Завантажити додаткові можливості"}
        }},
        {"downloading", {
            {LANG_RU, "Загрузка..."},
            {LANG_EN, "Downloading..."},
            {LANG_UK, "Завантаження..."}
        }},
        {"completed", {
            {LANG_RU, "Завершено"},
            {LANG_EN, "Completed"},
            {LANG_UK, "Завершено"}
        }},
        {"error", {
            {LANG_RU, "Ошибка"},
            {LANG_EN, "Error"},
            {LANG_UK, "Помилка"}
        }},
        {"first_setup", {
            {LANG_RU, "Первичная настройка NEOVIA"},
            {LANG_EN, "NEOVIA Initial Setup"},
            {LANG_UK, "Первинне налаштування NEOVIA"}
        }},
        {"scanning_games", {
            {LANG_RU, "Сканирование установленных игр..."},
            {LANG_EN, "Scanning installed games..."},
            {LANG_UK, "Сканування встановлених ігор..."}
        }},
        {"downloading_mods", {
            {LANG_RU, "Загрузка модов..."},
            {LANG_EN, "Downloading mods..."},
            {LANG_UK, "Завантаження модів..."}
        }},
        {"setup_complete", {
            {LANG_RU, "Настройка завершена!"},
            {LANG_EN, "Setup completed!"},
            {LANG_UK, "Налаштування завершено!"}
        }},
        {"press_plus_to_exit", {
            {LANG_RU, "Нажмите + для выхода"},
            {LANG_EN, "Press + to exit"},
            {LANG_UK, "Натисніть + для виходу"}
        }},
        {"games_found", {
            {LANG_RU, "Найдено игр"},
            {LANG_EN, "Games found"},
            {LANG_UK, "Знайдено ігор"}
        }},
        {"language", {
            {LANG_RU, "Язык"},
            {LANG_EN, "Language"},
            {LANG_UK, "Мова"}
        }},
        {"russian", {
            {LANG_RU, "Русский"},
            {LANG_EN, "Russian"},
            {LANG_UK, "Російська"}
        }},
        {"english", {
            {LANG_RU, "Английский"},
            {LANG_EN, "English"},
            {LANG_UK, "Англійська"}
        }},
        {"ukrainian", {
            {LANG_RU, "Украинский"},
            {LANG_EN, "Ukrainian"},
            {LANG_UK, "Українська"}
        }}
    };
    
    auto keyIt = translations.find(key);
    if (keyIt != translations.end()) {
        auto langIt = keyIt->second.find(lang);
        if (langIt != keyIt->second.end()) {
            return langIt->second;
        }
    }
    
    return key; // Возвращаем ключ, если перевод не найден
}