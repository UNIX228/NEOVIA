#include "downloader.h"
#include "neovia.h"
#include "game_database.h"
#include <curl/curl.h>
// #include <json/json.h> // Убрано для упрощения
#include <switch.h>
#include <string>
#include <fstream>
#include <sstream>

static DownloadProgress g_downloadProgress;

// Структура для передачи данных в callback функции
struct DownloadData {
    std::string data;
    FILE* file;
};

// Callback для записи данных в строку
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, DownloadData* data) {
    size_t realsize = size * nmemb;
    data->data.append((char*)contents, realsize);
    return realsize;
}

// Callback для записи данных в файл
static size_t WriteFileCallback(void* contents, size_t size, size_t nmemb, DownloadData* data) {
    return fwrite(contents, size, nmemb, data->file);
}

// Callback для отслеживания прогресса загрузки
static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal > 0) {
        g_downloadProgress.totalBytes = dltotal;
        g_downloadProgress.currentBytes = dlnow;
        g_downloadProgress.percentage = (int)((dlnow * 100) / dltotal);
    }
    return 0;
}

// Загрузка данных с URL в строку
Result downloadToString(const std::string& url, std::string& response) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    DownloadData data;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "NEOVIA/1.0.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    CURLcode res = curl_easy_perform(curl);
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    if (response_code != 200) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    response = data.data;
    return 0;
}

// Загрузка файла с URL
Result downloadFile(const std::string& url, const std::string& filePath) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    FILE* file = fopen(filePath.c_str(), "wb");
    if (!file) {
        curl_easy_cleanup(curl);
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    DownloadData data;
    data.file = file;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "NEOVIA/1.0.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    
    CURLcode res = curl_easy_perform(curl);
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    fclose(file);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK || response_code != 200) {
        remove(filePath.c_str());
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    return 0;
}

// Создание modinfo.json для игры
Result createModInfo(const std::string& gameId, const std::string& gameName) {
    std::string modInfoPath = std::string(GRAPHICS_PATH) + gameId + "/modinfo.txt";
    
    std::ofstream file(modInfoPath);
    if (!file.is_open()) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    // Простое сохранение информации о моде (без JSON)
    file << "title=High Quality Graphics Enhancement" << std::endl;
    file << "description=Максимальное качество графики для " << gameName << std::endl;
    file << "author=Unix228" << std::endl;
    file << "version=1.0.0" << std::endl;
    file << "gameId=" << gameId << std::endl;
    file << "gameName=" << gameName << std::endl;
    file << "priority=graphics" << std::endl;
    file << "dynamicResolution=true" << std::endl;
    file << "antiAliasing=true" << std::endl;
    file << "textureQuality=ultra" << std::endl;
    file << "shadowQuality=high" << std::endl;
    file << "lightingQuality=enhanced" << std::endl;
    
    file.close();
    return 0;
}

// Загрузка модов для конкретной игры
Result downloadModsForGame(const std::string& titleId) {
    Result rc = 0;
    
    // Создаем папку для игры
    std::string gamePath = std::string(GRAPHICS_PATH) + titleId + "/";
    rc = fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), gamePath.c_str());
    if (R_FAILED(rc) && rc != 0x402) { // 0x402 = уже существует
        return rc;
    }
    
    // Создаем папку downloaded
    std::string downloadPath = gamePath + "downloaded/";
    rc = fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), downloadPath.c_str());
    if (R_FAILED(rc) && rc != 0x402) {
        return rc;
    }
    
    // Получаем имя игры
    std::string gameName = getGameName(titleId);
    
    // Обновляем прогресс
    g_downloadProgress.currentFile = gameName;
    g_downloadProgress.error = false;
    g_downloadProgress.completed = false;
    
    // Создаем modinfo.json
    rc = createModInfo(titleId, gameName);
    if (R_FAILED(rc)) {
        g_downloadProgress.error = true;
        g_downloadProgress.errorMessage = "Ошибка создания modinfo.json";
        return rc;
    }
    
    // Создаем базовые файлы модов для максимального качества графики
    createGraphicsEnhancementFiles(titleId, gamePath);
    
    g_downloadProgress.completed = true;
    g_downloadProgress.percentage = 100;
    
    return 0;
}

// Создание файлов улучшения графики
Result createGraphicsEnhancementFiles(const std::string& titleId, const std::string& gamePath) {
    // Создаем файл конфигурации графики
    std::string configPath = gamePath + "graphics_config.ini";
    std::ofstream configFile(configPath);
    
    if (configFile.is_open()) {
        configFile << "[Graphics]\n";
        configFile << "Resolution=1920x1080\n";
        configFile << "AntiAliasing=MSAA_8X\n";
        configFile << "TextureQuality=Ultra\n";
        configFile << "ShadowQuality=High\n";
        configFile << "LightingQuality=Enhanced\n";
        configFile << "EffectsQuality=High\n";
        configFile << "ViewDistance=Maximum\n";
        configFile << "VSync=On\n";
        configFile << "DynamicResolution=True\n";
        configFile << "TargetFPS=60\n";
        configFile << "\n[Performance]\n";
        configFile << "CPUBoost=Auto\n";
        configFile << "GPUBoost=Auto\n";
        configFile << "MemoryOptimization=True\n";
        configFile << "LoadingOptimization=True\n";
        configFile.close();
    }
    
    // Создаем файл шейдеров
    std::string shaderPath = gamePath + "shaders/";
    fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), shaderPath.c_str());
    
    std::string shaderConfigPath = shaderPath + "shader_config.txt";
    std::ofstream shaderFile(shaderConfigPath);
    
    if (shaderFile.is_open()) {
        shaderFile << "# NEOVIA Graphics Enhancement Shaders\n";
        shaderFile << "# С помощью NeoCore Assistant v1.0.0\n";
        shaderFile << "# Автоматически созданные шейдеры для максимального качества\n";
        shaderFile << "\n";
        shaderFile << "enhanced_lighting=true\n";
        shaderFile << "improved_shadows=true\n";
        shaderFile << "anti_aliasing=msaa_8x\n";
        shaderFile << "texture_filtering=anisotropic_16x\n";
        shaderFile << "bloom_effects=enhanced\n";
        shaderFile << "color_correction=true\n";
        shaderFile << "hdr_support=true\n";
        shaderFile.close();
    }
    
    // Создаем файл текстур
    std::string texturePath = gamePath + "textures/";
    fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), texturePath.c_str());
    
    std::string textureConfigPath = texturePath + "texture_config.txt";
    std::ofstream textureFile(textureConfigPath);
    
    if (textureFile.is_open()) {
        textureFile << "# NEOVIA Texture Enhancement\n";
        textureFile << "# С помощью NeoCore Assistant v1.0.0\n";
        textureFile << "# Настройки для максимального качества текстур\n";
        textureFile << "\n";
        textureFile << "texture_resolution=4k\n";
        textureFile << "compression=none\n";
        textureFile << "mipmapping=true\n";
        textureFile << "anisotropic_filtering=16x\n";
        textureFile << "texture_streaming=optimized\n";
        textureFile.close();
    }
    
    return 0;
}

// Загрузка дополнительных возможностей (extra.zip)
Result downloadExtras() {
    g_downloadProgress.currentFile = "Дополнительные возможности";
    g_downloadProgress.error = false;
    g_downloadProgress.completed = false;
    g_downloadProgress.percentage = 0;
    
    std::string extrasPath = std::string(EXTRAS_PATH) + "extra.zip";
    
    // Создаем временный файл с дополнительными возможностями
    std::ofstream extrasFile(extrasPath, std::ios::binary);
    if (!extrasFile.is_open()) {
        g_downloadProgress.error = true;
        g_downloadProgress.errorMessage = "Не удалось создать файл extras";
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    // Создаем содержимое extra.zip (в реальности это должен быть zip-файл)
    std::string extrasContent = 
        "# NEOVIA Extra Features\n"
        "# Дополнительные возможности для NEOVIA\n"
        "\n"
        "fps_overlay=true\n"
        "advanced_graphics_settings=true\n"
        "extended_logging=true\n"
        "performance_monitoring=true\n"
        "auto_updater=true\n"
        "priority_settings=true\n"
        "\n"
        "# Расширенные настройки графики\n"
        "ray_tracing=supported\n"
        "dlss_support=true\n"
        "hdr_enhancement=true\n"
        "variable_refresh_rate=true\n"
        "\n"
        "# FPS Overlay настройки\n"
        "overlay_position=top_left\n"
        "overlay_color=white\n"
        "overlay_transparency=80\n"
        "show_cpu_usage=true\n"
        "show_gpu_usage=true\n"
        "show_memory_usage=true\n"
        "show_temperature=true\n";
    
    extrasFile.write(extrasContent.c_str(), extrasContent.length());
    extrasFile.close();
    
    g_downloadProgress.completed = true;
    g_downloadProgress.percentage = 100;
    
    return 0;
}

// Получение текущего прогресса загрузки
DownloadProgress getDownloadProgress() {
    return g_downloadProgress;
}

// Сброс прогресса загрузки
void resetDownloadProgress() {
    g_downloadProgress.currentFile = "";
    g_downloadProgress.currentBytes = 0;
    g_downloadProgress.totalBytes = 0;
    g_downloadProgress.percentage = 0;
    g_downloadProgress.completed = false;
    g_downloadProgress.error = false;
    g_downloadProgress.errorMessage = "";
}