#include "config.h"
#include "neovia.h"
#include <switch.h>
#include <fstream>
// #include <json/json.h> // Убрано для упрощения

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
    
    // Простое чтение конфигурации (без JSON)
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("firstRun=") == 0) {
            config.firstRun = (line.substr(9) == "true");
        } else if (line.find("language=") == 0) {
            config.language = static_cast<Language>(std::stoi(line.substr(9)));
        } else if (line.find("priority=") == 0) {
            config.priority = static_cast<Priority>(std::stoi(line.substr(9)));
        } else if (line.find("dynamicResolution=") == 0) {
            config.dynamicResolution = (line.substr(18) == "true");
        } else if (line.find("downloadAllMods=") == 0) {
            config.downloadAllMods = (line.substr(16) == "true");
        } else if (line.find("autoStart=") == 0) {
            config.autoStart = (line.substr(10) == "true");
        } else if (line.find("extrasInstalled=") == 0) {
            config.extrasInstalled = (line.substr(16) == "true");
        }
    }
    
    file.close();
    
    return 0;
}

// Сохранение конфигурации в файл
Result saveConfig(const Config& config) {
    std::ofstream file(SETTINGS_PATH);
    if (!file.is_open()) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    // Простое сохранение конфигурации (без JSON)
    file << "firstRun=" << (config.firstRun ? "true" : "false") << std::endl;
    file << "language=" << static_cast<int>(config.language) << std::endl;
    file << "priority=" << static_cast<int>(config.priority) << std::endl;
    file << "dynamicResolution=" << (config.dynamicResolution ? "true" : "false") << std::endl;
    file << "downloadAllMods=" << (config.downloadAllMods ? "true" : "false") << std::endl;
    file << "autoStart=" << (config.autoStart ? "true" : "false") << std::endl;
    file << "extrasInstalled=" << (config.extrasInstalled ? "true" : "false") << std::endl;
    
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