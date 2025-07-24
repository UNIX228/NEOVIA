#pragma once
#include "switch_ui.h"
#include "config.h"
#include <memory>
#include <vector>

class SwitchMainScreen {
private:
    Config* config;
    
    // UI элементы в стиле Switch
    std::unique_ptr<SwitchPanel> mainPanel;
    std::unique_ptr<SwitchIcon> appIcon;
    std::unique_ptr<SwitchText> appTitle;
    std::unique_ptr<SwitchText> appSubtitle;
    
    // Кнопки главного меню
    std::unique_ptr<SwitchButton> enhanceGamesButton;
    std::unique_ptr<SwitchButton> settingsButton;
    std::unique_ptr<SwitchButton> aboutButton;
    
    // Статусная панель
    std::unique_ptr<SwitchPanel> statusPanel;
    std::unique_ptr<SwitchText> systemStatus;
    std::unique_ptr<SwitchText> versionText;
    
    // Навигация
    int selectedButton;
    bool hasUserIcon;
    
public:
    SwitchMainScreen();
    ~SwitchMainScreen();
    
    bool initialize(Config* cfg);
    void render(float deltaTime);
    void update(float deltaTime);
    bool handleInput(u64 kDown);
    
private:
    void createMainUI();
    void createStatusBar();
    void updateSelection();
    void checkForUserIcon();
    
    // Обработчики кнопок
    void onEnhanceGames();
    void onSettings();
    void onAbout();
};