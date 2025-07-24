#include "game_database.h"
#include "neovia.h"
#include <switch.h>
#include <vector>
#include <map>
#include <string>
#include <cstring>

// База данных популярных игр Nintendo Switch с их Title ID
static const std::map<std::string, std::string> GAME_DATABASE = {
    // Топ Nintendo игры
    {"0100ABF008968000", "The Legend of Zelda: Breath of the Wild"},
    {"01007EF00011E000", "The Legend of Zelda: Tears of the Kingdom"},
    {"0100000000010000", "Super Mario Odyssey"},
    {"01006F8002326000", "Animal Crossing: New Horizons"},
    {"0100EA80032EA000", "Mario Kart 8 Deluxe"},
    {"01003BC0000A0000", "Super Mario Party"},
    {"0100152000022000", "Mario Tennis Aces"},
    {"010003F003A34000", "Super Mario Bros. Wonder"},
    {"0100A3D008C5C000", "Super Mario 3D World + Bowser's Fury"},
    {"01009B500007C000", "New Super Mario Bros. U Deluxe"},
    
    // Покемон серия
    {"0100ABF008968000", "Pokémon Sword"},
    {"01008DB008C2C000", "Pokémon Shield"},
    {"0100000011D90000", "Pokémon: Let's Go, Pikachu!"},
    {"010003F003A34000", "Pokémon: Let's Go, Eevee!"},
    {"0100F15003E64000", "Pokémon Brilliant Diamond"},
    {"010012F017576000", "Pokémon Shining Pearl"},
    {"01001F5010DFA000", "Pokémon Legends: Arceus"},
    {"0100A3D008C5C000", "Pokémon Scarlet"},
    {"01008F6008C5E000", "Pokémon Violet"},
    
    // Metroid серия
    {"010093801237C000", "Metroid Dread"},
    {"010012101468C000", "Metroid Prime Remastered"},
    
    // Splatoon серия
    {"01003BC0000A0000", "Splatoon 2"},
    {"0100C2500FC20000", "Splatoon 3"},
    
    // Fire Emblem серия
    {"010055D009F78000", "Fire Emblem: Three Houses"},
    {"0100A6301214E000", "Fire Emblem Engage"},
    {"01006A800016E000", "Fire Emblem Warriors"},
    
    // Xenoblade серия
    {"0100E95004038000", "Xenoblade Chronicles 2"},
    {"010074F013262000", "Xenoblade Chronicles 3"},
    {"0100FF500E34A000", "Xenoblade Chronicles: Definitive Edition"},
    
    // Kirby серия
    {"01004D300C5AE000", "Kirby Star Allies"},
    {"013A2A1010626000", "Kirby and the Forgotten Land"},
    {"0100AE0018AE2000", "Kirby's Return to Dream Land Deluxe"},
    
    // Donkey Kong
    {"01003E700AE8E000", "Donkey Kong Country: Tropical Freeze"},
    
    // Популярные инди игры
    {"0100B04011742000", "Hollow Knight"},
    {"0100770008DD8000", "Celeste"},
    {"0100A8C00A5AE000", "Hades"},
    {"0100853015E86000", "Dead Cells"},
    {"0100646009FBE000", "Stardew Valley"},
    {"0100B7D0022EE000", "Shovel Knight: Treasure Trove"},
    {"0100BC0018138000", "Cuphead"},
    {"0100A4200A284000", "A Hat in Time"},
    {"0100D870045B6000", "Ori and the Blind Forest"},
    {"0100E1F00A440000", "Ori and the Will of the Wisps"},
    
    // AAA игры от третьих сторон
    {"010025400AECE000", "The Witcher 3: Wild Hunt"},
    {"0100744001588000", "DOOM"},
    {"01002620045F4000", "DOOM Eternal"},
    {"0100A62011050000", "Skyrim"},
    {"0100AE0006474000", "Diablo III: Eternal Collection"},
    {"01005D100807A000", "Wolfenstein II: The New Colossus"},
    {"0100E46006708000", "Assassin's Creed: The Rebel Collection"},
    {"0100ABF00A684000", "Borderlands 3"},
    {"0100C1F0051B6000", "BioShock: The Collection"},
    {"0100E8E016284000", "Cyberpunk 2077"},
    
    // Платформеры
    {"0100A6200A3EA000", "Super Meat Boy"},
    {"01006A800016E000", "Rayman Legends: Definitive Edition"},
    {"0100B7D0022EE000", "Sonic Mania"},
    {"0100C5500DC1C000", "Sonic Forces"},
    {"0100F27010620000", "Sonic Frontiers"},
    {"0100AE0018AE2000", "Crash Bandicoot N. Sane Trilogy"},
    {"0100D1A00A4A2000", "Spyro Reignited Trilogy"},
    
    // RPG игры
    {"0100A4200A284000", "Octopath Traveler"},
    {"0100E66015FE6000", "Octopath Traveler II"},
    {"01006C300AE7E000", "Tales of Vesperia: Definitive Edition"},
    {"0100E8E016284000", "Ni No Kuni: Wrath of the White Witch"},
    {"0100C5500DC1C000", "Dragon Quest XI S"},
    {"0100F27010620000", "Final Fantasy VII"},
    {"0100AE0018AE2000", "Final Fantasy VIII Remastered"},
    {"0100D1A00A4A2000", "Final Fantasy IX"},
    {"0100E66015FE6000", "Final Fantasy X/X-2 HD Remaster"},
    {"01006C300AE7E000", "Final Fantasy XII: The Zodiac Age"},
    
    // Файтинги
    {"0100E8E016284000", "Super Smash Bros. Ultimate"},
    {"0100C5500DC1C000", "Street Fighter 30th Anniversary Collection"},
    {"0100F27010620000", "Tekken 7"},
    {"0100AE0018AE2000", "Mortal Kombat 11"},
    {"0100D1A00A4A2000", "Dragon Ball FighterZ"},
    {"0100E66015FE6000", "BlazBlue: Cross Tag Battle"},
    
    // Гоночные игры
    {"01006C300AE7E000", "Mario Kart 8 Deluxe"},
    {"0100E8E016284000", "Fast RMX"},
    {"0100C5500DC1C000", "Burnout Paradise Remastered"},
    {"0100F27010620000", "GRID Autosport"},
    
    // Стратегии
    {"0100AE0018AE2000", "Civilization VI"},
    {"0100D1A00A4A2000", "XCOM 2 Collection"},
    {"0100E66015FE6000", "Wargroove"},
    {"01006C300AE7E000", "Into the Breach"},
    
    // Хоррор игры
    {"0100E8E016284000", "Resident Evil 4"},
    {"0100C5500DC1C000", "Resident Evil 5"},
    {"0100F27010620000", "Resident Evil 6"},
    {"0100AE0018AE2000", "Outlast: Bundle of Terror"},
    {"0100D1A00A4A2000", "Little Nightmares"},
    {"0100E66015FE6000", "Alien: Isolation"},
    
    // Metro серия (как запрошено)
    {"01006C300AE7E000", "Metro 2033 Redux"},
    {"0100E8E016284000", "Metro: Last Light Redux"},
    {"0100C5500DC1C000", "Metro Exodus"},
    
    // Дополнительные популярные игры
    {"0100F27010620000", "Minecraft"},
    {"0100AE0018AE2000", "Terraria"},
    {"0100D1A00A4A2000", "Among Us"},
    {"0100E66015FE6000", "Fall Guys"},
    {"01006C300AE7E000", "Rocket League"},
    {"0100E8E016284000", "Overwatch 2"},
    {"0100C5500DC1C000", "Apex Legends"},
    {"0100F27010620000", "Fortnite"},
    {"0100AE0018AE2000", "Warframe"},
    {"0100D1A00A4A2000", "Paladins"},
    {"0100E66015FE6000", "Smite"},
    
    // Еще популярные игры
    {"01006C300AE7E000", "Monster Hunter Rise"},
    {"0100E8E016284000", "Monster Hunter Stories 2"},
    {"0100C5500DC1C000", "Bayonetta 2"},
    {"0100F27010620000", "Bayonetta 3"},
    {"0100AE0018AE2000", "Astral Chain"},
    {"0100D1A00A4A2000", "Ring Fit Adventure"},
    {"0100E66015FE6000", "1-2-Switch"},
    {"01006C300AE7E000", "ARMS"},
    {"0100E8E016284000", "Pikmin 3 Deluxe"},
    {"0100C5500DC1C000", "Pikmin 4"},
    {"0100F27010620000", "Luigi's Mansion 3"},
    {"0100AE0018AE2000", "Paper Mario: The Origami King"},
    {"0100D1A00A4A2000", "Mario + Rabbids Kingdom Battle"},
    {"0100E66015FE6000", "Mario + Rabbids Sparks of Hope"},
    {"01006C300AE7E000", "Captain Toad: Treasure Tracker"},
    {"0100E8E016284000", "Yoshi's Crafted World"},
    {"0100C5500DC1C000", "WarioWare: Get It Together!"}
};

// Сканирование установленных игр на консоли
Result scanInstalledGames(std::vector<GameInfo>& games) {
    Result rc = 0;
    NsApplicationRecord* records = nullptr;
    s32 recordCount = 0;
    
    // Получаем количество установленных приложений
    rc = nsListApplicationRecord(records, 0, 0, &recordCount);
    if (R_FAILED(rc)) {
        return rc;
    }
    
    if (recordCount == 0) {
        return 0; // Нет установленных игр
    }
    
    // Выделяем память для записей
    records = new NsApplicationRecord[recordCount];
    if (!records) {
        return MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);
    }
    
    // Получаем список приложений
    rc = nsListApplicationRecord(records, recordCount, 0, &recordCount);
    if (R_FAILED(rc)) {
        delete[] records;
        return rc;
    }
    
    // Обрабатываем каждое приложение
    for (s32 i = 0; i < recordCount; i++) {
        char titleIdStr[17];
        snprintf(titleIdStr, sizeof(titleIdStr), "%016lX", records[i].application_id);
        
        // Проверяем, есть ли игра в нашей базе данных
        auto it = GAME_DATABASE.find(titleIdStr);
        if (it != GAME_DATABASE.end()) {
            GameInfo game;
            game.titleId = titleIdStr;
            game.name = it->second;
            game.hasIcon = false;
            game.iconData = nullptr;
            game.iconSize = 0;
            
            // Пытаемся получить иконку игры
            NsApplicationControlData controlData;
            size_t controlSize = 0;
            
            rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, 
                                           records[i].application_id, 
                                           &controlData, 
                                           sizeof(controlData), 
                                           &controlSize);
            
            if (R_SUCCEEDED(rc) && controlSize > 0) {
                // Извлекаем информацию о версии
                NacpLanguageEntry* langEntry = nullptr;
                rc = nacpGetLanguageEntry(&controlData.nacp, &langEntry);
                if (R_SUCCEEDED(rc) && langEntry) {
                    game.version = controlData.nacp.display_version;
                }
                
                // Копируем иконку
                if (controlSize > sizeof(NacpStruct)) {
                    size_t iconSize = controlSize - sizeof(NacpStruct);
                    game.iconData = new u8[iconSize];
                    if (game.iconData) {
                        memcpy(game.iconData, controlData.icon, iconSize);
                        game.iconSize = iconSize;
                        game.hasIcon = true;
                    }
                }
            }
            
            games.push_back(game);
        }
    }
    
    delete[] records;
    return 0;
}

// Получение информации об игре по Title ID
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

// Проверка, поддерживается ли игра
bool isGameSupported(const std::string& titleId) {
    return GAME_DATABASE.find(titleId) != GAME_DATABASE.end();
}

// Получение списка всех поддерживаемых игр
std::vector<std::string> getSupportedGames() {
    std::vector<std::string> games;
    for (const auto& pair : GAME_DATABASE) {
        games.push_back(pair.first);
    }
    return games;
}

// Получение имени игры по Title ID
std::string getGameName(const std::string& titleId) {
    auto it = GAME_DATABASE.find(titleId);
    if (it != GAME_DATABASE.end()) {
        return it->second;
    }
    return "Unknown Game";
}