#pragma once

#include <switch.h>
#include "config.h"

// Простой GUI на консоли
class SimpleGUI {
private:
    Config* config;
    int selectedMenuItem;
    bool inSideMenu;
    
public:
    SimpleGUI();
    
    bool initialize(Config* cfg);
    void render();
    void handleInput(u64 kDown);
    
private:
    void renderMainMenu();
    void renderSideMenu();
    void showSettings();
    void showAbout();
    void enhanceGames();
};

// Глобальный GUI
extern SimpleGUI g_simpleGui;