#include "simple_gui.h"
#include <stdio.h>
#include <string>
#include "downloader.h"
#include "game_database.h"

SimpleGUI::SimpleGUI() : config(nullptr), selectedMenuItem(0), inSideMenu(false) {}
    
bool SimpleGUI::initialize(Config* cfg) {
    config = cfg;
    return true;
}

void SimpleGUI::render() {
        consoleClear();
        
        if (inSideMenu) {
            renderSideMenu();
        } else {
            renderMainMenu();
        }
        
        consoleUpdate(NULL);
}

void SimpleGUI::renderMainMenu() {
        printf("\x1b[2J\x1b[H"); // Очистка экрана
        printf("\x1b[47m\x1b[30m"); // Серый фон, черный текст
        
        // Имитация серого фона
        for (int i = 0; i < 45; i++) {
            printf("                                                                                \n");
        }
        
        printf("\x1b[10;35H\x1b[44m\x1b[37m NEOVIA v1.0.0 \x1b[0m\n");
        printf("\x1b[15;30H\x1b[46m\x1b[30m [ Улучшить графику ] \x1b[0m\n");
        printf("\x1b[20;35H\x1b[37mГотов к работе\x1b[0m\n");
        
        // Нижняя панель
        printf("\x1b[43;1H\x1b[40m\x1b[37m");
        for (int i = 0; i < 80; i++) printf(" ");
        printf("\x1b[43;35H🏠 Главная\x1b[0m");
        
        // Боковая панель
        printf("\x1b[5;5H\x1b[40m\x1b[37m☰\x1b[0m");
        
        printf("\x1b[45;1H\x1b[37mA - Выбрать  ☰ - Меню  + - Выход\x1b[0m");
}

void SimpleGUI::renderSideMenu() {
        printf("\x1b[2J\x1b[H"); // Очистка экрана
        
        // Затемненный фон
        printf("\x1b[40m\x1b[37m");
        for (int i = 0; i < 45; i++) {
            printf("                                                                                \n");
        }
        
        // Боковое меню
        printf("\x1b[5;5H\x1b[47m\x1b[30m NEOVIA v1.0.0 \x1b[0m");
        printf("\x1b[7;5H\x1b[37m" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─" "─\x1b[0m");
        
        if (selectedMenuItem == 0) {
            printf("\x1b[9;5H\x1b[46m\x1b[30m ⚙️ Настройки \x1b[0m");
        } else {
            printf("\x1b[9;5H\x1b[40m\x1b[37m ⚙️ Настройки \x1b[0m");
        }
        
        if (selectedMenuItem == 1) {
            printf("\x1b[11;5H\x1b[46m\x1b[30m ℹ️ О нас \x1b[0m");
        } else {
            printf("\x1b[11;5H\x1b[40m\x1b[37m ℹ️ О нас \x1b[0m");
        }
        
        printf("\x1b[45;1H\x1b[37mA - Выбрать  B - Назад  ↕ - Навигация\x1b[0m");
}

void SimpleGUI::handleInput(u64 kDown) {
        if (inSideMenu) {
            if (kDown & HidNpadButton_Up) {
                selectedMenuItem = (selectedMenuItem - 1 + 2) % 2;
            }
            if (kDown & HidNpadButton_Down) {
                selectedMenuItem = (selectedMenuItem + 1) % 2;
            }
            if (kDown & HidNpadButton_A) {
                if (selectedMenuItem == 0) {
                    showSettings();
                } else if (selectedMenuItem == 1) {
                    showAbout();
                }
            }
            if (kDown & HidNpadButton_B) {
                inSideMenu = false;
                selectedMenuItem = 0;
            }
        } else {
            if (kDown & HidNpadButton_A) {
                enhanceGames();
            }
            if (kDown & HidNpadButton_L) { // L как гамбургер меню
                inSideMenu = true;
                selectedMenuItem = 0;
            }
        }
}

void SimpleGUI::showSettings() {
        printf("\x1b[2J\x1b[H");
        printf("\x1b[5;30H\x1b[46m\x1b[30m ⚙️ Настройки \x1b[0m\n\n");
        
        printf("\x1b[10;10HЯзык: ");
        switch (config->language) {
            case LANG_RU: printf("Русский"); break;
            case LANG_EN: printf("English"); break;
            case LANG_UA: printf("Українська"); break;
        }
        
        printf("\x1b[12;10HПриоритет: ");
        switch (config->priority) {
            case PRIORITY_GRAPHICS: printf("Графика"); break;
            case PRIORITY_PERFORMANCE: printf("Производительность"); break;
            case PRIORITY_BALANCED: printf("Сбалансированный"); break;
        }
        
        printf("\x1b[15;10HДинамическое разрешение: %s", 
               config->dynamicResolution ? "Включено" : "Выключено");
        
        printf("\x1b[40;1H\x1b[37mB - Назад  ← → - Изменить язык\x1b[0m");
        consoleUpdate(NULL);
        
        // Ждем ввод
        PadState pad;
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        padInitializeDefault(&pad);
        
        while (true) {
            padUpdate(&pad);
            u64 kDown = padGetButtonsDown(&pad);
            
            if (kDown & HidNpadButton_B) {
                inSideMenu = false;
                break;
            }
            if (kDown & HidNpadButton_Left) {
                config->language = static_cast<Language>((config->language + 2) % 3);
                saveConfig(*config);
                showSettings(); // Перерисовываем
                break;
            }
            if (kDown & HidNpadButton_Right) {
                config->language = static_cast<Language>((config->language + 1) % 3);
                saveConfig(*config);
                showSettings(); // Перерисовываем
                break;
            }
        }
}

void SimpleGUI::showAbout() {
        printf("\x1b[2J\x1b[H");
        printf("\x1b[5;30H\x1b[46m\x1b[30m ℹ️ О нас \x1b[0m\n\n");
        
        printf("\x1b[10;25HNEOVIA v1.0.0\n");
        printf("\x1b[12;25HАвтор: UNIX228\n");
        printf("\x1b[14;20HСистема улучшения графики\n");
        printf("\x1b[15;22Hдля Nintendo Switch\n");
        
        printf("\x1b[40;1H\x1b[37mB - Назад\x1b[0m");
        consoleUpdate(NULL);
        
        // Ждем ввод
        PadState pad;
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        padInitializeDefault(&pad);
        
        while (true) {
            padUpdate(&pad);
            u64 kDown = padGetButtonsDown(&pad);
            
            if (kDown & HidNpadButton_B) {
                inSideMenu = false;
                break;
            }
        }
}

void SimpleGUI::enhanceGames() {
        printf("\x1b[2J\x1b[H");
        printf("\x1b[10;30HСканирование игр...\n");
        consoleUpdate(NULL);
        
        std::vector<GameInfo> games;
        Result rc = scanInstalledGames(games);
        
        if (R_SUCCEEDED(rc) && !games.empty()) {
            printf("\x1b[12;25HНайдено игр: %zu\n", games.size());
            printf("\x1b[14;25HЗагрузка модов...\n");
            consoleUpdate(NULL);
            
            int processed = 0;
            for (const auto& game : games) {
                if (processed >= 5) break;
                
                printf("\x1b[16;20HОбработка: %s\n", game.name.c_str());
                consoleUpdate(NULL);
                
                rc = downloadModsForGame(game.titleId);
                if (R_SUCCEEDED(rc)) {
                    processed++;
                }
            }
            
            printf("\x1b[20;25H\x1b[42m\x1b[30m Готово! Обработано: %d \x1b[0m\n", processed);
        } else {
            printf("\x1b[12;30H\x1b[41m\x1b[37m Игры не найдены \x1b[0m\n");
        }
        
        printf("\x1b[40;1H\x1b[37mA - Продолжить\x1b[0m");
        consoleUpdate(NULL);
        
        // Ждем ввод
        PadState pad;
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        padInitializeDefault(&pad);
        
        while (true) {
            padUpdate(&pad);
            u64 kDown = padGetButtonsDown(&pad);
            
            if (kDown & HidNpadButton_A) {
                break;
            }
                  }
      }
}

// Глобальный GUI
SimpleGUI g_simpleGui;