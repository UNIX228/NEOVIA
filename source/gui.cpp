#include "gui.h"
#include "neovia.h"
#include <algorithm>
#include <cmath>

// Глобальный GUI
std::unique_ptr<NEOVIA_GUI> g_gui;

NEOVIA_GUI::NEOVIA_GUI() 
    : currentState(GUIState::MainMenu)
    , previousState(GUIState::MainMenu)
    , sideMenuVisible(false)
    , overlayVisible(false)
    , sideMenuAnimation(0.0f)
    , config(nullptr) {
}

NEOVIA_GUI::~NEOVIA_GUI() {
    shutdown();
}

bool NEOVIA_GUI::initialize(Config* cfg) {
    config = cfg;
    
    // Создаем основные панели
    backgroundPanel = std::make_unique<Panel>(
        Rect{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 
        Color(COLOR_BACKGROUND)
    );
    
    // Боковая панель (левая черная полоса)
    sidebarPanel = std::make_unique<Panel>(
        Rect{0, 0, SIDEBAR_WIDTH, SCREEN_HEIGHT}, 
        Color(COLOR_PANEL)
    );
    
    // Нижняя панель (черная полоса внизу)
    bottomPanel = std::make_unique<Panel>(
        Rect{0, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT, SCREEN_WIDTH, BOTTOM_PANEL_HEIGHT}, 
        Color(COLOR_PANEL)
    );
    
    createMainMenuUI();
    return true;
}

void NEOVIA_GUI::shutdown() {
    // Очищаем все элементы
    backgroundPanel.reset();
    sidebarPanel.reset();
    bottomPanel.reset();
    overlayPanel.reset();
    
    enhanceButton.reset();
    hamburgerButton.reset();
    settingsButton.reset();
    aboutButton.reset();
    
    titleText.reset();
    statusText.reset();
}

void NEOVIA_GUI::createMainMenuUI() {
    // Кнопка гамбургер меню (3 полоски в левом верхнем углу)
    hamburgerButton = std::make_unique<Button>(
        Rect{10, 10, 60, 40},
        "☰", // Unicode символ гамбургер меню
        [this]() { onHamburgerPressed(); }
    );
    
    // Заголовок "Главная" с иконкой домика в нижней панели
    titleText = std::make_unique<TextElement>(
        Point{SCREEN_WIDTH / 2.0f - 50, SCREEN_HEIGHT - 40},
        "🏠 Главная"
    );
    
    // Главная кнопка "Улучшить" по центру экрана
    enhanceButton = std::make_unique<Button>(
        Rect{SCREEN_WIDTH / 2.0f - BUTTON_WIDTH / 2.0f, 
             SCREEN_HEIGHT / 2.0f - BUTTON_HEIGHT / 2.0f - 50, 
             BUTTON_WIDTH, BUTTON_HEIGHT},
        "Улучшить",
        [this]() { onEnhancePressed(); }
    );
    
    // Статус текст
    statusText = std::make_unique<TextElement>(
        Point{SCREEN_WIDTH / 2.0f - 100, SCREEN_HEIGHT / 2.0f + 50},
        "Готов к работе"
    );
}

void NEOVIA_GUI::createSideMenu() {
    if (overlayVisible) return;
    
    // Создаем оверлей панель (затемнение фона)
    overlayPanel = std::make_unique<Panel>(
        Rect{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 
        Color(0x80000000) // Полупрозрачный черный
    );
    
    // Панель бокового меню
    float menuWidth = 300;
    auto menuPanel = std::make_unique<Panel>(
        Rect{0, 0, menuWidth, SCREEN_HEIGHT}, 
        Color(COLOR_PANEL)
    );
    
    // Логотип NEOVIA вверху меню
    auto logoText = std::make_unique<TextElement>(
        Point{20, 50},
        "NEOVIA v1.0.0"
    );
    
    // Разделительная линия
    auto separatorPanel = std::make_unique<Panel>(
        Rect{20, 100, menuWidth - 40, 2}, 
        Color(COLOR_TEXT_GRAY)
    );
    
    // Кнопка настроек
    settingsButton = std::make_unique<Button>(
        Rect{20, 130, menuWidth - 40, 50},
        "⚙️ Настройки",
        [this]() { onSettingsPressed(); }
    );
    
    // Кнопка "О нас"
    aboutButton = std::make_unique<Button>(
        Rect{20, 190, menuWidth - 40, 50},
        "ℹ️ О нас",
        [this]() { onAboutPressed(); }
    );
    
    overlayVisible = true;
}

void NEOVIA_GUI::update(float deltaTime) {
    updateAnimations(deltaTime);
}

void NEOVIA_GUI::updateAnimations(float deltaTime) {
    // Анимация бокового меню
    if (sideMenuVisible && sideMenuAnimation < 1.0f) {
        sideMenuAnimation = std::min(1.0f, sideMenuAnimation + deltaTime * 4.0f);
    } else if (!sideMenuVisible && sideMenuAnimation > 0.0f) {
        sideMenuAnimation = std::max(0.0f, sideMenuAnimation - deltaTime * 4.0f);
        
        if (sideMenuAnimation <= 0.0f) {
            overlayVisible = false;
            overlayPanel.reset();
            settingsButton.reset();
            aboutButton.reset();
        }
    }
}

void NEOVIA_GUI::render() {
    if (!g_graphics) return;
    
    g_graphics->beginFrame();
    
    // Рендерим основные элементы
    switch (currentState) {
        case GUIState::MainMenu:
            renderMainMenu();
            break;
        case GUIState::Settings:
            renderSettings();
            break;
        case GUIState::About:
            renderAbout();
            break;
        default:
            renderMainMenu();
            break;
    }
    
    // Рендерим боковое меню поверх всего
    if (overlayVisible) {
        renderOverlay();
        renderSideMenu();
    }
    
    g_graphics->endFrame();
    g_graphics->present();
}

void NEOVIA_GUI::renderMainMenu() {
    // Фон
    if (backgroundPanel) backgroundPanel->render();
    
    // Боковая панель
    if (sidebarPanel) sidebarPanel->render();
    
    // Нижняя панель
    if (bottomPanel) bottomPanel->render();
    
    // Кнопка гамбургер
    if (hamburgerButton) hamburgerButton->render();
    
    // Заголовок
    if (titleText) titleText->render();
    
    // Главная кнопка
    if (enhanceButton) enhanceButton->render();
    
    // Статус
    if (statusText) statusText->render();
}

void NEOVIA_GUI::renderSettings() {
    // Фон настроек
    if (backgroundPanel) backgroundPanel->render();
    
    // Заголовок настроек
    g_graphics->drawText(
        Point{SCREEN_WIDTH / 2.0f - 50, 100},
        "⚙️ Настройки",
        Color(COLOR_TEXT_WHITE),
        32.0f
    );
    
    // Настройки языка
    std::string langText = "Язык: ";
    switch (config->language) {
        case LANG_RU: langText += "Русский"; break;
        case LANG_EN: langText += "English"; break;
        case LANG_UA: langText += "Українська"; break;
    }
    
    g_graphics->drawText(
        Point{100, 200},
        langText,
        Color(COLOR_TEXT_WHITE),
        24.0f
    );
    
    // Настройки приоритета
    std::string priorityText = "Приоритет: ";
    switch (config->priority) {
        case PRIORITY_GRAPHICS: priorityText += "Графика"; break;
        case PRIORITY_PERFORMANCE: priorityText += "Производительность"; break;
        case PRIORITY_BALANCED: priorityText += "Сбалансированный"; break;
    }
    
    g_graphics->drawText(
        Point{100, 250},
        priorityText,
        Color(COLOR_TEXT_WHITE),
        24.0f
    );
    
    // Инструкция
    g_graphics->drawText(
        Point{100, 350},
        "Нажмите B для возврата",
        Color(COLOR_TEXT_GRAY),
        20.0f
    );
}

void NEOVIA_GUI::renderAbout() {
    // Фон
    if (backgroundPanel) backgroundPanel->render();
    
    // Заголовок
    g_graphics->drawText(
        Point{SCREEN_WIDTH / 2.0f - 50, 100},
        "ℹ️ О нас",
        Color(COLOR_TEXT_WHITE),
        32.0f
    );
    
    // Информация о приложении
    g_graphics->drawText(
        Point{100, 200},
        "NEOVIA v1.0.0",
        Color(COLOR_TEXT_WHITE),
        24.0f
    );
    
    g_graphics->drawText(
        Point{100, 240},
        "Автор: UNIX228",
        Color(COLOR_TEXT_WHITE),
        20.0f
    );
    
    g_graphics->drawText(
        Point{100, 280},
        "Система улучшения графики",
        Color(COLOR_TEXT_WHITE),
        20.0f
    );
    
    g_graphics->drawText(
        Point{100, 320},
        "для Nintendo Switch",
        Color(COLOR_TEXT_WHITE),
        20.0f
    );
}

void NEOVIA_GUI::renderOverlay() {
    if (overlayPanel && sideMenuAnimation > 0.0f) {
        // Применяем альфа анимацию
        Color overlayColor = overlayPanel->color;
        overlayColor.a *= sideMenuAnimation;
        
        g_graphics->drawRect(overlayPanel->bounds, overlayColor);
    }
}

void NEOVIA_GUI::renderSideMenu() {
    if (sideMenuAnimation <= 0.0f) return;
    
    float menuWidth = 300 * sideMenuAnimation;
    
    // Панель меню
    g_graphics->drawRect(
        Rect{0, 0, menuWidth, SCREEN_HEIGHT},
        Color(COLOR_PANEL)
    );
    
    if (sideMenuAnimation > 0.8f) { // Показываем текст только когда меню почти открыто
        // Логотип
        g_graphics->drawText(
            Point{20, 50},
            "NEOVIA v1.0.0",
            Color(COLOR_TEXT_WHITE),
            24.0f
        );
        
        // Разделитель
        g_graphics->drawRect(
            Rect{20, 100, menuWidth - 40, 2},
            Color(COLOR_TEXT_GRAY)
        );
        
        // Кнопки
        if (settingsButton) settingsButton->render();
        if (aboutButton) aboutButton->render();
    }
}

void NEOVIA_GUI::handleInput(u64 kDown, Point touchPos) {
    // Обработка глобальных кнопок
    if (kDown & HidNpadButton_B) {
        if (currentState != GUIState::MainMenu) {
            setState(GUIState::MainMenu);
            return;
        }
        if (sideMenuVisible) {
            hideSideMenu();
            return;
        }
    }
    
    // Обработка ввода для текущего состояния
    switch (currentState) {
        case GUIState::MainMenu:
            if (hamburgerButton) hamburgerButton->handleInput(kDown, touchPos);
            if (enhanceButton) enhanceButton->handleInput(kDown, touchPos);
            break;
            
        case GUIState::Settings:
            // Обработка настроек
            if (kDown & HidNpadButton_Left || kDown & HidNpadButton_Right) {
                // Переключение языка
                if (kDown & HidNpadButton_Left) {
                    config->language = static_cast<Language>((config->language + 2) % 3);
                } else {
                    config->language = static_cast<Language>((config->language + 1) % 3);
                }
                saveConfig(*config);
            }
            break;
            
        case GUIState::About:
            // Ничего не делаем в разделе "О нас"
            break;
    }
    
    // Обработка бокового меню
    if (overlayVisible) {
        if (settingsButton) settingsButton->handleInput(kDown, touchPos);
        if (aboutButton) aboutButton->handleInput(kDown, touchPos);
        
        // Закрытие меню при клике вне его
        if (touchPos.x > 300 && (kDown & HidNpadButton_A)) {
            hideSideMenu();
        }
    }
}

void NEOVIA_GUI::setState(GUIState newState) {
    previousState = currentState;
    currentState = newState;
    
    // Скрываем боковое меню при смене состояния
    if (sideMenuVisible) {
        hideSideMenu();
    }
}

void NEOVIA_GUI::showSideMenu() {
    if (!sideMenuVisible) {
        createSideMenu();
        sideMenuVisible = true;
    }
}

void NEOVIA_GUI::hideSideMenu() {
    sideMenuVisible = false;
}

void NEOVIA_GUI::toggleSideMenu() {
    if (sideMenuVisible) {
        hideSideMenu();
    } else {
        showSideMenu();
    }
}

void NEOVIA_GUI::showOverlay() {
    overlayVisible = true;
}

void NEOVIA_GUI::hideOverlay() {
    overlayVisible = false;
}

void NEOVIA_GUI::setStatusText(const std::string& text) {
    if (statusText) {
        statusText->setText(text);
    }
}

// Обработчики кнопок
void NEOVIA_GUI::onHamburgerPressed() {
    toggleSideMenu();
}

void NEOVIA_GUI::onEnhancePressed() {
    if (onEnhanceClicked) {
        onEnhanceClicked();
    }
}

void NEOVIA_GUI::onSettingsPressed() {
    setState(GUIState::Settings);
    hideSideMenu();
}

void NEOVIA_GUI::onAboutPressed() {
    setState(GUIState::About);
    hideSideMenu();
}

void NEOVIA_GUI::onBackPressed() {
    setState(previousState);
}