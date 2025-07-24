#include "game_database.h"
#include "neovia.h"
#include <switch.h>
#include <vector>
#include <map>
#include <string>

static const std::map<std::string, std::string> GAME_DATABASE = {
    {"0100ABF008968000", "The Legend of Zelda: Breath of the Wild"},
    {"01007EF00011E000", "The Legend of Zelda: Tears of the Kingdom"},
    {"0100000000010000", "Super Mario Odyssey"},
    {"01006F8002326000", "Animal Crossing: New Horizons"},
    {"0100EA80032EA000", "Mario Kart 8 Deluxe"},
    {"010025400AECE000", "The Witcher 3: Wild Hunt"},
    {"0100744001588000", "DOOM"},
    {"01002620045F4000", "DOOM Eternal"},
    {"01006C300AE7E004", "Metro 2033 Redux"},
    {"0100E8E016284004", "Metro: Last Light Redux"},
    {"0100C5500DC1C005", "Metro Exodus"},
    {"0100B04011742000", "Hollow Knight"},
    {"0100770008DD8000", "Celeste"},
    {"0100A8C00A5AE000", "Hades"}
};

Result scanInstalledGames(std::vector<GameInfo>& games) {
    // Заглушка - добавляем несколько игр для демонстрации
    for (const auto& pair : GAME_DATABASE) {
        GameInfo game;
        game.titleId = pair.first;
        game.name = pair.second;
        game.hasIcon = false;
        game.iconData = nullptr;
        game.iconSize = 0;
        games.push_back(game);
        
        if (games.size() >= 5) break; // Ограничиваем для демо
    }
    
    return 0;
}

bool getGameInfo(const std::string& titleId, GameInfo& gameInfo) {
    auto it = GAME_DATABASE.find(titleId);
    if (it != GAME_DATABASE.end()) {
        gameInfo.titleId = titleId;
        gameInfo.name = it->second;
        gameInfo.hasIcon = false;
        gameInfo.iconData = nullptr;
        gameInfo.iconSize = 0;
        return true;
    }
    return false;
}

bool isGameSupported(const std::string& titleId) {
    return GAME_DATABASE.find(titleId) != GAME_DATABASE.end();
}

std::vector<std::string> getSupportedGames() {
    std::vector<std::string> games;
    for (const auto& pair : GAME_DATABASE) {
        games.push_back(pair.first);
    }
    return games;
}

std::string getGameName(const std::string& titleId) {
    auto it = GAME_DATABASE.find(titleId);
    if (it != GAME_DATABASE.end()) {
        return it->second;
    }
    return "Unknown Game";
}
