#include "neocore.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>

NeoCoreManager g_neoCore;

NeoCoreManager::NeoCoreManager() : status(NeoCoreStatus::NOT_INITIALIZED) {
    // Инициализация конфигурации по умолчанию
    config.force_fps_60 = true;
    config.enable_fxaa = true;
    config.enable_shadows = true;
    config.enable_overlay = false;
    config.show_fps = false;
    config.priority = QualityPriority::GRAPHICS;
    config.log_level = 2;
    config.quality_preset = "ultra";
}

NeoCoreManager::~NeoCoreManager() {
    cleanup();
}

bool NeoCoreManager::initialize() {
    status = NeoCoreStatus::INITIALIZING;
    logMessage("NeoCore v" + std::string(NEOCORE_VERSION) + " инициализация...");
    
    // Создаем структуру папок
    if (!createDirectoryStructure()) {
        lastError = "Не удалось создать структуру папок NeoCore";
        status = NeoCoreStatus::ERROR;
        return false;
    }
    
    // Загружаем конфигурацию
    loadConfig();
    
    // Проверяем наличие основных файлов
    if (!checkCoreFiles()) {
        logMessage("Основные файлы NeoCore отсутствуют, попытка загрузки...");
        if (!downloadCoreFiles()) {
            lastError = "Не удалось загрузить файлы NeoCore";
            status = NeoCoreStatus::ERROR;
            return false;
        }
    }
    
    status = NeoCoreStatus::READY;
    logMessage("NeoCore готов к работе");
    return true;
}

bool NeoCoreManager::createDirectoryStructure() {
    std::vector<std::string> directories = {
        "/graphics/",
        NEOCORE_PATH,
        NEOCORE_PLUGINS_PATH,
        NEOCORE_LOGS_PATH,
        NEOCORE_SYSTEM_PATH,
        NEOCORE_TEMP_CACHE,
        NEOCORE_FALLBACK,
        "/graphics/NeoCore/system/defaults/",
        "/graphics/mods/"
    };
    
    for (const auto& dir : directories) {
        Result rc = fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), dir.c_str());
        if (R_FAILED(rc) && rc != 0x402) { // 0x402 = уже существует
            logMessage("Ошибка создания папки: " + std::string(dir));
            return false;
        }
    }
    
    return true;
}

bool NeoCoreManager::checkCoreFiles() {
    std::ifstream coreFile(NEOCORE_CORE_BIN);
    std::ifstream loaderFile(NEOCORE_LOADER_CFG);
    
    return coreFile.good() && loaderFile.good();
}

bool NeoCoreManager::downloadCoreFiles() {
    // Создаем базовый loader.cfg
    std::ofstream loaderFile(NEOCORE_LOADER_CFG);
    if (loaderFile.is_open()) {
        loaderFile << "# NeoCore v" << NEOCORE_VERSION << " - Конфигурация загрузки\n";
        loaderFile << "# Автор: " << NEOCORE_AUTHOR << "\n";
        loaderFile << "# Модульное графическое ядро для максимального качества\n\n";
        
        loaderFile << "[core]\n";
        loaderFile << "force_fps=60\n";
        loaderFile << "quality=" << config.quality_preset << "\n";
        loaderFile << "enable_fxaa=" << (config.enable_fxaa ? "true" : "false") << "\n";
        loaderFile << "enable_shadows=" << (config.enable_shadows ? "true" : "false") << "\n\n";
        
        loaderFile << "[overlay]\n";
        loaderFile << "enabled=" << (config.enable_overlay ? "true" : "false") << "\n";
        loaderFile << "show_fps=" << (config.show_fps ? "true" : "false") << "\n\n";
        
        loaderFile << "[debug]\n";
        loaderFile << "log_level=" << config.log_level << "\n";
        
        loaderFile.close();
    }
    
    // Создаем заглушку core.bin (будет заменена реальным ядром)
    std::ofstream coreFile(NEOCORE_CORE_BIN, std::ios::binary);
    if (coreFile.is_open()) {
        // Записываем заголовок бинарного файла
        std::string header = "NEOCORE_BIN_v1.0.0_by_Unix228";
        coreFile.write(header.c_str(), header.length());
        coreFile.close();
    }
    
    // Создаем базовые модули
    createDefaultModules();
    
    return true;
}

void NeoCoreManager::createDefaultModules() {
    // Модуль теней
    std::ofstream shadowMod(std::string(NEOCORE_PLUGINS_PATH) + "shadows.mod");
    if (shadowMod.is_open()) {
        shadowMod << "# NeoCore Shadows Module v1.0.0\n";
        shadowMod << "# Модуль динамических теней\n";
        shadowMod << "shadow_quality=ultra\n";
        shadowMod << "shadow_distance=high\n";
        shadowMod << "cascade_count=4\n";
        shadowMod.close();
    }
    
    // Модуль FXAA
    std::ofstream fxaaMod(std::string(NEOCORE_PLUGINS_PATH) + "fxaa.mod");
    if (fxaaMod.is_open()) {
        fxaaMod << "# NeoCore FXAA Module v1.0.0\n";
        fxaaMod << "# Модуль сглаживания FXAA\n";
        fxaaMod << "fxaa_quality=ultra\n";
        fxaaMod << "edge_threshold=0.125\n";
        fxaaMod.close();
    }
}

bool NeoCoreManager::startCore(const GameInfo& gameInfo) {
    if (status != NeoCoreStatus::READY) {
        lastError = "NeoCore не готов к запуску";
        return false;
    }
    
    status = NeoCoreStatus::RUNNING;
    logMessage("Запуск NeoCore для игры: " + gameInfo.gameName);
    
    // Отправляем информацию об игре
    if (!sendGameInfo(gameInfo)) {
        lastError = "Не удалось отправить информацию об игре";
        status = NeoCoreStatus::ERROR;
        return false;
    }
    
    // Создаем профиль игры если его нет
    if (!hasGameProfile(gameInfo.gameId)) {
        createGameProfile(gameInfo.gameId);
    }
    
    // Ждем готовности ядра
    if (!waitForReady()) {
        lastError = "Тайм-аут ожидания готовности ядра";
        status = NeoCoreStatus::ERROR;
        return false;
    }
    
    logMessage("NeoCore успешно запущен для " + gameInfo.gameName);
    return true;
}

bool NeoCoreManager::sendGameInfo(const GameInfo& gameInfo) {
    std::ofstream commFile(NEOCORE_COMM_FILE);
    if (!commFile.is_open()) {
        return false;
    }
    
    commFile << "# NeoCore Communication File\n";
    commFile << "game_id=" << gameInfo.gameId << "\n";
    commFile << "game_name=" << gameInfo.gameName << "\n";
    commFile << "has_profile=" << (gameInfo.hasCustomProfile ? "true" : "false") << "\n";
    commFile << "active_mods=" << gameInfo.activeMods.size() << "\n";
    
    for (size_t i = 0; i < gameInfo.activeMods.size(); i++) {
        commFile << "mod_" << i << "=" << gameInfo.activeMods[i] << "\n";
    }
    
    commFile << "neocore_ready=false\n";
    commFile.close();
    
    return true;
}

bool NeoCoreManager::waitForReady(int timeoutMs) {
    int elapsed = 0;
    const int checkInterval = 100; // мс
    
    while (elapsed < timeoutMs) {
        std::ifstream commFile(NEOCORE_COMM_FILE);
        if (commFile.is_open()) {
            std::string line;
            while (std::getline(commFile, line)) {
                if (line.find("neocore_ready=true") != std::string::npos) {
                    return true;
                }
            }
            commFile.close();
        }
        
        // Имитация ожидания
        elapsed += checkInterval;
    }
    
    return false;
}

bool NeoCoreManager::createGameProfile(const std::string& gameId) {
    std::string profilePath = "/graphics/mods/" + gameId + "/";
    
    // Создаем папку игры
    Result rc = fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), profilePath.c_str());
    if (R_FAILED(rc) && rc != 0x402) {
        return false;
    }
    
    // Создаем конфигурацию игры
    std::ofstream configFile(profilePath + "neocore_profile.cfg");
    if (configFile.is_open()) {
        configFile << "# NeoCore Profile for Game: " << gameId << "\n";
        configFile << "# Создан автоматически NEOVIA\n";
        configFile << "# Powered by NeoCore Engine v" << NEOCORE_VERSION << "\n\n";
        
        configFile << "[graphics]\n";
        configFile << "shadows=true\n";
        configFile << "fxaa=true\n";
        configFile << "quality=ultra\n";
        configFile << "fps_target=60\n\n";
        
        configFile << "[profile]\n";
        configFile << "created_by=NEOVIA\n";
        configFile << "engine=NeoCore\n";
        configFile << "version=" << NEOCORE_VERSION << "\n";
        
        configFile.close();
    }
    
    // Создаем папки для ресурсов
    std::vector<std::string> subDirs = {
        profilePath + "shaders/",
        profilePath + "textures/",
        profilePath + "effects/"
    };
    
    for (const auto& dir : subDirs) {
        fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), dir.c_str());
    }
    
    logMessage("Создан профиль NeoCore для игры: " + gameId);
    return true;
}

bool NeoCoreManager::hasGameProfile(const std::string& gameId) const {
    std::string profilePath = "/graphics/mods/" + gameId + "/neocore_profile.cfg";
    std::ifstream file(profilePath);
    return file.good();
}

void NeoCoreManager::logMessage(const std::string& message) {
    std::ofstream logFile(NEOCORE_LOG_FILE, std::ios::app);
    if (logFile.is_open()) {
        // Получаем текущее время
        time_t now = time(0);
        char* timeStr = ctime(&now);
        if (timeStr) {
            timeStr[strlen(timeStr) - 1] = '\0'; // Убираем \n
        }
        
        logFile << "[" << (timeStr ? timeStr : "Unknown") << "] " << message << std::endl;
        logFile.close();
    }
}

bool NeoCoreManager::loadConfig() {
    std::ifstream configFile(NEOCORE_LOADER_CFG);
    if (!configFile.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(configFile, line)) {
        if (line.find("force_fps=") == 0) {
            config.force_fps_60 = (line.substr(10) == "60");
        } else if (line.find("enable_fxaa=") == 0) {
            config.enable_fxaa = (line.substr(12) == "true");
        } else if (line.find("enable_shadows=") == 0) {
            config.enable_shadows = (line.substr(15) == "true");
        } else if (line.find("quality=") == 0) {
            config.quality_preset = line.substr(8);
        }
    }
    
    return true;
}

bool NeoCoreManager::saveConfig() {
    return downloadCoreFiles(); // Пересоздаем конфиг с текущими настройками
}

void NeoCoreManager::setQualityPriority(QualityPriority priority) {
    config.priority = priority;
    
    switch (priority) {
        case QualityPriority::FPS:
            config.quality_preset = "medium";
            config.enable_shadows = false;
            config.force_fps_60 = true;
            break;
        case QualityPriority::GRAPHICS:
            config.quality_preset = "ultra";
            config.enable_shadows = true;
            config.enable_fxaa = true;
            break;
        case QualityPriority::STABILITY:
            config.quality_preset = "high";
            config.enable_shadows = true;
            config.force_fps_60 = false;
            break;
    }
    
    saveConfig();
}

bool NeoCoreManager::isReady() const {
    return status == NeoCoreStatus::READY || status == NeoCoreStatus::RUNNING;
}

bool NeoCoreManager::stopCore() {
    if (status == NeoCoreStatus::RUNNING) {
        status = NeoCoreStatus::READY;
        logMessage("NeoCore остановлен");
    }
    return true;
}

void NeoCoreManager::cleanup() {
    stopCore();
    status = NeoCoreStatus::NOT_INITIALIZED;
}

void NeoCoreManager::enableModule(NeoCoreModule module, bool enable) {
    switch (module) {
        case NeoCoreModule::SHADOWS:
            config.enable_shadows = enable;
            break;
        case NeoCoreModule::FXAA:
            config.enable_fxaa = enable;
            break;
        case NeoCoreModule::OVERLAY:
            config.enable_overlay = enable;
            break;
        default:
            break;
    }
    
    saveConfig();
    logMessage("Модуль изменен: " + std::to_string((int)module) + " = " + (enable ? "включен" : "выключен"));
}

bool NeoCoreManager::isCoreBinaryValid() {
    std::ifstream coreFile(NEOCORE_CORE_BIN, std::ios::binary);
    if (!coreFile.is_open()) {
        return false;
    }
    
    std::string header;
    header.resize(28); // Размер нашего заголовка
    coreFile.read(&header[0], 28);
    
    return header.find("NEOCORE_BIN_v1.0.0_by_Unix228") == 0;
}