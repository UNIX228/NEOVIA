#pragma once
#include "graphics.h"
#include "config.h"
#include <memory>
#include <vector>

enum class Screen {
    MAIN_MENU,
    SETTINGS,
    ABOUT,
    GAME_ENHANCEMENT,
    LOADING
};

class ModernGUI {
private:
    Config* config;
    Screen currentScreen;
    Screen previousScreen;
    
    // UI элементы для разных экранов
    std::unique_ptr<Panel> mainPanel;
    std::unique_ptr<Panel> settingsPanel;
    std::unique_ptr<Panel> aboutPanel;
    std::unique_ptr<Panel> enhancementPanel;
    std::unique_ptr<Panel> loadingPanel;
    
    // Анимации переходов
    Animation screenTransition;
    Animation backgroundPulse;
    
    // Состояние
    int selectedButton;
    bool isTransitioning;
    float backgroundOffset;
    
    // Частицы для фона
    struct Particle {
        float x, y;
        float vx, vy;
        float size;
        Color color;
        float life;
        float maxLife;
    };
    std::vector<Particle> particles;
    
public:
    ModernGUI();
    ~ModernGUI();
    
    bool initialize(Config* cfg);
    void render();
    void update(float deltaTime);
    bool handleInput(u64 kDown);
    
    void switchScreen(Screen newScreen);
    
private:
    void createMainMenu();
    void createSettingsMenu();
    void createAboutMenu();
    void createEnhancementMenu();
    void createLoadingMenu();
    
    void renderBackground(float deltaTime);
    void renderParticles(float deltaTime);
    void updateParticles(float deltaTime);
    void spawnParticle();
    
    void renderMainMenu(float deltaTime);
    void renderSettingsMenu(float deltaTime);
    void renderAboutMenu(float deltaTime);
    void renderEnhancementMenu(float deltaTime);
    void renderLoadingMenu(float deltaTime);
    
    void handleMainMenuInput(u64 kDown);
    void handleSettingsInput(u64 kDown);
    void handleAboutInput(u64 kDown);
    void handleEnhancementInput(u64 kDown);
    
    void startGameEnhancement();
    void showLanguageSettings();
    void showPrioritySettings();
    
    // Анимированные эффекты
    void drawNeoviaBranding(float x, float y, float scale = 1.0f);
    void drawAnimatedLogo(float x, float y, float time);
    void drawStatusIndicator(float x, float y, bool active);
};