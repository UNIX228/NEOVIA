#pragma once

#include "graphics.h"
#include "config.h"
#include <memory>
#include <functional>

// Состояния GUI
enum class GUIState {
    MainMenu,
    Settings,
    About,
    Progress,
    GameSelection
};

// Главный класс GUI
class NEOVIA_GUI {
private:
    GUIState currentState;
    GUIState previousState;
    
    // UI элементы
    std::unique_ptr<Panel> backgroundPanel;
    std::unique_ptr<Panel> sidebarPanel;
    std::unique_ptr<Panel> bottomPanel;
    std::unique_ptr<Panel> overlayPanel;
    
    std::unique_ptr<Button> enhanceButton;
    std::unique_ptr<Button> hamburgerButton;
    std::unique_ptr<Button> settingsButton;
    std::unique_ptr<Button> aboutButton;
    
    std::unique_ptr<TextElement> titleText;
    std::unique_ptr<TextElement> statusText;
    
    // Состояние меню
    bool sideMenuVisible;
    bool overlayVisible;
    float sideMenuAnimation;
    
    // Конфигурация
    Config* config;
    
    // Колбэки
    std::function<void()> onEnhanceClicked;
    std::function<void()> onSettingsClicked;
    std::function<void()> onAboutClicked;
    
public:
    NEOVIA_GUI();
    ~NEOVIA_GUI();
    
    bool initialize(Config* cfg);
    void shutdown();
    
    void update(float deltaTime);
    void render();
    void handleInput(u64 kDown, Point touchPos);
    
    // Управление состояниями
    void setState(GUIState newState);
    GUIState getState() const { return currentState; }
    
    // Управление боковым меню
    void showSideMenu();
    void hideSideMenu();
    void toggleSideMenu();
    
    // Управление оверлеем
    void showOverlay();
    void hideOverlay();
    
    // Установка колбэков
    void setOnEnhanceClicked(std::function<void()> callback) { onEnhanceClicked = callback; }
    void setOnSettingsClicked(std::function<void()> callback) { onSettingsClicked = callback; }
    void setOnAboutClicked(std::function<void()> callback) { onAboutClicked = callback; }
    
    // Обновление текста
    void setStatusText(const std::string& text);
    
private:
    void createMainMenuUI();
    void createSettingsUI();
    void createAboutUI();
    void createSideMenu();
    
    void updateAnimations(float deltaTime);
    void renderMainMenu();
    void renderSettings();
    void renderAbout();
    void renderSideMenu();
    void renderOverlay();
    
    // Обработчики кнопок
    void onHamburgerPressed();
    void onEnhancePressed();
    void onSettingsPressed();
    void onAboutPressed();
    void onBackPressed();
};

// Глобальный GUI
extern std::unique_ptr<NEOVIA_GUI> g_gui;