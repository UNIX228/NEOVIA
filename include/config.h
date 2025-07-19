#pragma once

#include "neovia.h"
#include <switch.h>
#include <string>
#include <map>

// Функции для работы с конфигурацией
Result loadConfig(Config& config);
Result saveConfig(const Config& config);
std::string getLanguageString(Language lang, const std::string& key);