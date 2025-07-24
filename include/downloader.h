#pragma once

#include "neovia.h"
#include <switch.h>
#include <string>

struct DownloadProgress {
    std::string currentFile;
    size_t currentBytes;
    size_t totalBytes;
    int percentage;
    bool completed;
    bool error;
    std::string errorMessage;
};

Result downloadToString(const std::string& url, std::string& response);
Result downloadFile(const std::string& url, const std::string& filePath);
Result downloadModsForGame(const std::string& titleId);
Result downloadExtras();
DownloadProgress getDownloadProgress();
void resetDownloadProgress();
