#include "switch_main_screen.h"
#include <fstream>
#include <iostream>

SwitchMainScreen::SwitchMainScreen() 
    : config(nullptr), selectedButton(0), hasUserIcon(false) {
}

SwitchMainScreen::~SwitchMainScreen() = default;

bool SwitchMainScreen::initialize(Config* cfg) {
    config = cfg;
    
    // Проверяем наличие пользовательской иконки
    checkForUserIcon();
    
    // Создаем основной интерфейс
    createMainUI();
    createStatusBar();
    
    return true;
}

void SwitchMainScreen::checkForUserIcon() {
    // Проверяем файл icon.jpg в разных местах
    std::vector<std::string> iconPaths = {
        "/switch/NEOVIA/icon.jpg",
        "switch/NEOVIA/icon.jpg",
        "icon.jpg"
    };
    
    for (const auto& path : iconPaths) {
        std::ifstream file(path);
        if (file.good()) {
            hasUserIcon = true;
            break;
        }
    }
}

void SwitchMainScreen::createMainUI() {
    // Главная панель (в стиле Switch Home)
    mainPanel = std::make_unique<SwitchPanel>(200, 100, 880, 520);
    mainPanel->backgroundColor = SwitchColors::WHITE;
    mainPanel->borderColor = SwitchColors::GRAY_LIGHT;
    mainPanel->cornerRadius = 16;
    
    // Иконка приложения (большая, как в Switch)
    std::string iconName = hasUserIcon ? "user_icon" : "home";
    appIcon = std::make_unique<SwitchIcon>(iconName, 240, 140, 96);
    
    // Название приложения
    appTitle = std::make_unique<SwitchText>("NEOVIA", 360, 160);
    appTitle->fontSize = 32;
    appTitle->textColor = SwitchColors::TEXT_PRIMARY;
    
    // Подзаголовок
    appSubtitle = std::make_unique<SwitchText>("Graphics Enhancement System", 360, 200);
    appSubtitle->fontSize = 16;
    appSubtitle->textColor = SwitchColors::TEXT_SECONDARY;
    
    // Кнопки меню (в стиле Switch)
    enhanceGamesButton = std::make_unique<SwitchButton>("Enhance Games", 240, 280, 320, 60);
    enhanceGamesButton->onClick = [this]() { onEnhanceGames(); };
    enhanceGamesButton->selected = (selectedButton == 0);
    
    settingsButton = std::make_unique<SwitchButton>("Settings", 240, 360, 320, 60);
    settingsButton->onClick = [this]() { onSettings(); };
    settingsButton->selected = (selectedButton == 1);
    
    aboutButton = std::make_unique<SwitchButton>("About", 240, 440, 320, 60);
    aboutButton->onClick = [this]() { onAbout(); };
    aboutButton->selected = (selectedButton == 2);
    
    // Добавляем элементы в панель
    mainPanel->addChild(std::unique_ptr<SwitchUIElement>(appIcon.release()));
    mainPanel->addChild(std::unique_ptr<SwitchUIElement>(appTitle.release()));
    mainPanel->addChild(std::unique_ptr<SwitchUIElement>(appSubtitle.release()));
    mainPanel->addChild(std::unique_ptr<SwitchUIElement>(enhanceGamesButton.release()));
    mainPanel->addChild(std::unique_ptr<SwitchUIElement>(settingsButton.release()));
    mainPanel->addChild(std::unique_ptr<SwitchUIElement>(aboutButton.release()));
}

void SwitchMainScreen::createStatusBar() {
    // Статусная панель внизу (как в Switch)
    statusPanel = std::make_unique<SwitchPanel>(200, 640, 880, 60);
    statusPanel->backgroundColor = SwitchColors::GRAY_LIGHT;
    statusPanel->borderWidth = 0;
    statusPanel->cornerRadius = 8;
    
    // Статус системы
    systemStatus = std::make_unique<SwitchText>("System Ready", 220, 665);
    systemStatus->fontSize = 14;
    systemStatus->textColor = SwitchColors::TEXT_SECONDARY;
    
    // Версия
    versionText = std::make_unique<SwitchText>("v1.0.0", 980, 665);
    versionText->fontSize = 14;
    versionText->textColor = SwitchColors::TEXT_SECONDARY;
    
    statusPanel->addChild(std::unique_ptr<SwitchUIElement>(systemStatus.release()));
    statusPanel->addChild(std::unique_ptr<SwitchUIElement>(versionText.release()));
}

void SwitchMainScreen::render(float deltaTime) {
    // Фон экрана (светлый, как в Switch)
    SWITCH_GFX->drawRect(0, 0, 1280, 720, SwitchColors::BACKGROUND_LIGHT);
    
    // Заголовок экрана
    SWITCH_GFX->drawText("Nintendo Switch", 50, 50, SwitchColors::TEXT_SECONDARY, 20);
    SWITCH_GFX->drawText("Home", 50, 80, SwitchColors::TEXT_PRIMARY, 24);
    
    // Главная панель
    if (mainPanel) {
        mainPanel->render(deltaTime);
    }
    
    // Статусная панель
    if (statusPanel) {
        statusPanel->render(deltaTime);
    }
    
    // Подсказки управления (как в Switch)
    SWITCH_GFX->drawText("A Select", 1100, 680, SwitchColors::TEXT_SECONDARY, 14);
    SWITCH_GFX->drawText("+ Exit", 1180, 680, SwitchColors::TEXT_SECONDARY, 14);
    
    // Индикатор пользовательской иконки
    if (hasUserIcon) {
        SWITCH_GFX->drawText("Custom icon loaded", 600, 140, SwitchColors::GREEN_SUCCESS, 12);
    }
}

void SwitchMainScreen::update(float deltaTime) {
    if (mainPanel) {
        mainPanel->update(deltaTime);
    }
    if (statusPanel) {
        statusPanel->update(deltaTime);
    }
}

void SwitchMainScreen::updateSelection() {
    // Обновляем выделение кнопок
    if (enhanceGamesButton) enhanceGamesButton->selected = (selectedButton == 0);
    if (settingsButton) settingsButton->selected = (selectedButton == 1);
    if (aboutButton) aboutButton->selected = (selectedButton == 2);
}

bool SwitchMainScreen::handleInput(u64 kDown) {
    // Навигация вверх/вниз
    if (kDown & HidNpadButton_Up) {
        selectedButton = (selectedButton - 1 + 3) % 3;
        updateSelection();
        return true;
    }
    
    if (kDown & HidNpadButton_Down) {
        selectedButton = (selectedButton + 1) % 3;
        updateSelection();
        return true;
    }
    
    // Выбор кнопки
    if (kDown & HidNpadButton_A) {
        switch (selectedButton) {
            case 0: onEnhanceGames(); break;
            case 1: onSettings(); break;
            case 2: onAbout(); break;
        }
        return true;
    }
    
    // Передаем ввод панелям
    if (mainPanel && mainPanel->handleInput(kDown)) {
        return true;
    }
    
    return false;
}

void SwitchMainScreen::onEnhanceGames() {
    // Запуск улучшения игр
    if (systemStatus) {
        systemStatus->text = "Scanning games...";
        systemStatus->textColor = SwitchColors::BLUE_ACCENT;
    }
}

void SwitchMainScreen::onSettings() {
    // Открытие настроек
    if (systemStatus) {
        systemStatus->text = "Opening settings...";
        systemStatus->textColor = SwitchColors::BLUE_ACCENT;
    }
}

void SwitchMainScreen::onAbout() {
    // Показ информации
    if (systemStatus) {
        systemStatus->text = "NEOVIA v1.0.0 - Graphics Enhancement System";
        systemStatus->textColor = SwitchColors::TEXT_PRIMARY;
    }
}