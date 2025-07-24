#pragma once

#include "neovia.h"
#include <switch.h>
#include <vector>
#include <string>

Result scanInstalledGames(std::vector<GameInfo>& games);
bool getGameInfo(const std::string& titleId, GameInfo& gameInfo);
bool isGameSupported(const std::string& titleId);
std::vector<std::string> getSupportedGames();
std::string getGameName(const std::string& titleId);
