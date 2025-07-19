#pragma once

#include "neovia.h"
#include <switch.h>
#include <string>

// Функции загрузки
Result downloadToString(const std::string& url, std::string& response);
Result downloadFile(const std::string& url, const std::string& filePath);
Result downloadModsForGame(const std::string& titleId);
Result downloadExtras();
Result createModInfo(const std::string& gameId, const std::string& gameName);
Result createGraphicsEnhancementFiles(const std::string& titleId, const std::string& gamePath);

// Функции прогресса
DownloadProgress getDownloadProgress();
void resetDownloadProgress();