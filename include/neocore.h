#pragma once
#include <switch.h>
#include <string>
#include <vector>

// NeoCore - Модульный графический движок для Nintendo Switch
// Независимый движок для максимального качества графики без разгона
#define NEOCORE_VERSION "1.0.0"
#define NEOCORE_AUTHOR "Unix228"
#define NEOCORE_PROJECT "NeoCore Graphics Engine"

// Пути NeoCore
#define NEOCORE_PATH "/graphics/NeoCore/"
#define NEOCORE_CORE_BIN "/graphics/NeoCore/core.bin"
#define NEOCORE_LOADER_CFG "/graphics/NeoCore/loader.cfg"
#define NEOCORE_PLUGINS_PATH "/graphics/NeoCore/plugins/"
#define NEOCORE_LOGS_PATH "/graphics/NeoCore/logs/"
#define NEOCORE_SYSTEM_PATH "/graphics/NeoCore/system/"
#define NEOCORE_TEMP_CACHE "/graphics/NeoCore/system/temp_cache/"
#define NEOCORE_FALLBACK "/graphics/NeoCore/system/fallback/"
#define NEOCORE_COMM_FILE "/graphics/neocomm.msg"
#define NEOCORE_LOG_FILE "/graphics/logs.txt"

// Статусы NeoCore
enum class NeoCoreStatus {
    NOT_INITIALIZED,
    INITIALIZING,
    READY,
    RUNNING,
    ERROR,
    UPDATING
};

// Модули NeoCore
enum class NeoCoreModule {
    SHADOWS,
    FXAA,
    SMAA,
    DOF,
    OVERLAY,
    SHADER_CACHE,
    FALLBACK
};

// Приоритеты качества
enum class QualityPriority {
    FPS,        // Приоритет FPS
    GRAPHICS,   // Приоритет графики
    STABILITY   // Приоритет стабильности
};

// Конфигурация NeoCore
struct NeoCoreConfig {
    bool force_fps_60;
    bool enable_fxaa;
    bool enable_shadows;
    bool enable_overlay;
    bool show_fps;
    QualityPriority priority;
    int log_level;
    std::string quality_preset; // ultra, high, medium, low
};

// Информация об игре для NeoCore
struct GameInfo {
    std::string gameId;
    std::string gameName;
    std::vector<std::string> activeMods;
    bool hasCustomProfile;
};

// Класс управления NeoCore
class NeoCoreManager {
private:
    NeoCoreStatus status;
    NeoCoreConfig config;
    std::string lastError;
    
public:
    NeoCoreManager();
    ~NeoCoreManager();
    
    // Основные функции
    bool initialize();
    bool isReady() const;
    bool startCore(const GameInfo& gameInfo);
    bool stopCore();
    void cleanup();
    
    // Конфигурация
    bool loadConfig();
    bool saveConfig();
    void setQualityPriority(QualityPriority priority);
    void enableModule(NeoCoreModule module, bool enable);
    
    // Система файлов
    bool createDirectoryStructure();
    bool checkCoreFiles();
    bool downloadCoreFiles();
    bool updateFromExtraZip(const std::string& zipPath);
    
    // Коммуникация
    bool sendGameInfo(const GameInfo& gameInfo);
    bool waitForReady(int timeoutMs = 5000);
    bool isCoreBinaryValid();
    
    // Профили игр
    bool createGameProfile(const std::string& gameId);
    bool hasGameProfile(const std::string& gameId) const;
    
    // Логирование
    void logMessage(const std::string& message);
    std::string getLastError() const { return lastError; }
    NeoCoreStatus getStatus() const { return status; }
    
    // Информация
    std::string getVersion() const { return NEOCORE_VERSION; }
    std::string getAuthor() const { return NEOCORE_AUTHOR; }
    
private:
    // Вспомогательные функции
    void createDefaultModules();
    std::string getCurrentTimeString();
};

// Глобальный экземпляр NeoCore
extern NeoCoreManager g_neoCore;

// Глобальная функция логирования для всего приложения
void logToGraphics(const std::string& component, const std::string& message);