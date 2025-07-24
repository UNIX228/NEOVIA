#include "downloader.h"
#include "neovia.h"
#include <switch.h>
#include <string>

static DownloadProgress g_downloadProgress;

Result downloadToString(const std::string& url, std::string& response) {
    // Заглушка для загрузки
    response = "Downloaded content";
    return 0;
}

Result downloadFile(const std::string& url, const std::string& filePath) {
    // Заглушка для загрузки файла
    return 0;
}

Result downloadModsForGame(const std::string& titleId) {
    g_downloadProgress.currentFile = "Game mods";
    g_downloadProgress.percentage = 100;
    g_downloadProgress.completed = true;
    g_downloadProgress.error = false;
    
    return 0;
}

Result downloadExtras() {
    g_downloadProgress.currentFile = "Extra features";
    g_downloadProgress.percentage = 100;
    g_downloadProgress.completed = true;
    g_downloadProgress.error = false;
    
    return 0;
}

DownloadProgress getDownloadProgress() {
    return g_downloadProgress;
}

void resetDownloadProgress() {
    g_downloadProgress.currentFile = "";
    g_downloadProgress.currentBytes = 0;
    g_downloadProgress.totalBytes = 0;
    g_downloadProgress.percentage = 0;
    g_downloadProgress.completed = false;
    g_downloadProgress.error = false;
    g_downloadProgress.errorMessage = "";
}
