#include "modern_gui.h"
#include "downloader.h"
#include "game_database.h"
#include "icon_loader.h"
#include <cmath>
#include <random>

// Forward declaration of advanced effects
namespace AdvancedEffects {
    void drawHolographicPanel(float x, float y, float width, float height, float time, float intensity);
    void drawNeonText(const std::string& text, float x, float y, const Color& color, int fontSize, float glowSize, float intensity);
    void drawPlasmaBackground(float x, float y, float width, float height, float time);
    void drawConstellation(float x, float y, float width, float height, float time, int starCount);
    void drawCyberpunkGrid(float x, float y, float width, float height, float time, float gridSize);
    void drawAnimatedLogo(float x, float y, float time, float scale);
}

ModernGUI::ModernGUI() 
    : config(nullptr), currentScreen(Screen::MAIN_MENU), previousScreen(Screen::MAIN_MENU),
      selectedButton(0), isTransitioning(false), backgroundOffset(0) {
    
    // Инициализация анимаций
    screenTransition = Animation(0.5f);
    backgroundPulse = Animation(2.0f);
    backgroundPulse.start(0, 1);
    
    // Создание частиц
    particles.reserve(50);
    for (int i = 0; i < 30; i++) {
        spawnParticle();
    }
}

ModernGUI::~ModernGUI() = default;

bool ModernGUI::initialize(Config* cfg) {
    config = cfg;
    
    if (!GFX->initialize()) {
        return false;
    }
    
    // Initialize icon system and create beautiful default icons
    DefaultIcons::initializeDefaultIcons();
    
    // Try to load user's custom JPG icon if it exists
    if (!ICON_LOADER->loadIcon("/switch/NEOVIA/icon.jpg", "user_icon")) {
        // If no custom icon, try other common locations
        if (!ICON_LOADER->loadIcon("/switch/NEOVIA/logo.jpg", "user_icon")) {
            ICON_LOADER->loadIcon("/switch/NEOVIA/neovia.jpg", "user_icon");
        }
    }
    
    createMainMenu();
    createSettingsMenu();
    createAboutMenu();
    createEnhancementMenu();
    createLoadingMenu();
    
    return true;
}

void ModernGUI::render() {
    GFX->beginFrame();
    
    float deltaTime = GFX->getDeltaTime();
    backgroundOffset += deltaTime;
    
    // Enhanced animated background with multiple effects
    renderBackground(deltaTime);
    
    // Add cyberpunk grid overlay
    AdvancedEffects::drawCyberpunkGrid(0, 0, 1280, 720, backgroundOffset, 40.0f);
    
    // Add constellation effect in corners
    AdvancedEffects::drawConstellation(0, 0, 300, 200, backgroundOffset, 8);
    AdvancedEffects::drawConstellation(980, 520, 300, 200, backgroundOffset + 1.0f, 6);
    
    renderParticles(deltaTime);
    
    // Рендер текущего экрана
    switch (currentScreen) {
        case Screen::MAIN_MENU:
            renderMainMenu(deltaTime);
            break;
        case Screen::SETTINGS:
            renderSettingsMenu(deltaTime);
            break;
        case Screen::ABOUT:
            renderAboutMenu(deltaTime);
            break;
        case Screen::GAME_ENHANCEMENT:
            renderEnhancementMenu(deltaTime);
            break;
        case Screen::LOADING:
            renderLoadingMenu(deltaTime);
            break;
    }
    
    GFX->endFrame();
}

void ModernGUI::update(float deltaTime) {
    updateParticles(deltaTime);
    backgroundOffset += deltaTime * 10.0f;
    
    // Обновление анимаций
    screenTransition.update(deltaTime);
    backgroundPulse.update(deltaTime);
    
    // Перезапуск анимации пульсации
    if (!backgroundPulse.isActive()) {
        backgroundPulse.start(0, 1);
    }
    
    // Обновление UI элементов
    if (mainPanel) mainPanel->update(deltaTime);
    if (settingsPanel) settingsPanel->update(deltaTime);
    if (aboutPanel) aboutPanel->update(deltaTime);
    if (enhancementPanel) enhancementPanel->update(deltaTime);
    if (loadingPanel) loadingPanel->update(deltaTime);
}

bool ModernGUI::handleInput(u64 kDown) {
    if (isTransitioning) return false;
    
    switch (currentScreen) {
        case Screen::MAIN_MENU:
            handleMainMenuInput(kDown);
            break;
        case Screen::SETTINGS:
            handleSettingsInput(kDown);
            break;
        case Screen::ABOUT:
            handleAboutInput(kDown);
            break;
        case Screen::GAME_ENHANCEMENT:
            handleEnhancementInput(kDown);
            break;
        case Screen::LOADING:
            // В режиме загрузки ввод не обрабатывается
            break;
    }
    
    return true;
}

void ModernGUI::switchScreen(Screen newScreen) {
    if (currentScreen == newScreen || isTransitioning) return;
    
    previousScreen = currentScreen;
    currentScreen = newScreen;
    isTransitioning = true;
    screenTransition.start(0, 1);
}

void ModernGUI::createMainMenu() {
    mainPanel = std::make_unique<Panel>(0, 0, 1280, 720);
    mainPanel->useGradient = true;
    mainPanel->gradientStart = Color(18, 18, 24, 200);
    mainPanel->gradientEnd = Color(12, 12, 18, 200);
    
    // Главная кнопка "Улучшить игры"
    auto enhanceButton = std::make_unique<Button>("🎮 УЛУЧШИТЬ ИГРЫ", 440, 300, 400, 80);
    enhanceButton->backgroundColor = Colors::PRIMARY;
    enhanceButton->textColor = Colors::WHITE;
    enhanceButton->onClick = [this]() { startGameEnhancement(); };
    mainPanel->addChild(std::move(enhanceButton));
    
    // Кнопка настроек
    auto settingsButton = std::make_unique<Button>("⚙️ НАСТРОЙКИ", 440, 400, 180, 60);
    settingsButton->backgroundColor = Colors::SECONDARY;
    settingsButton->onClick = [this]() { switchScreen(Screen::SETTINGS); };
    mainPanel->addChild(std::move(settingsButton));
    
    // Кнопка "О программе"
    auto aboutButton = std::make_unique<Button>("ℹ️ О НАС", 660, 400, 180, 60);
    aboutButton->backgroundColor = Colors::ACCENT;
    aboutButton->onClick = [this]() { switchScreen(Screen::ABOUT); };
    mainPanel->addChild(std::move(aboutButton));
    
    // Статус индикатор
    auto statusLabel = std::make_unique<Label>("СИСТЕМА ГОТОВА К РАБОТЕ", 440, 520);
    statusLabel->textColor = Colors::SUCCESS;
    statusLabel->fontSize = 18;
    statusLabel->centered = true;
    statusLabel->width = 400;
    mainPanel->addChild(std::move(statusLabel));
}

void ModernGUI::createSettingsMenu() {
    settingsPanel = std::make_unique<Panel>(200, 100, 880, 520);
    settingsPanel->borderWidth = 2;
    settingsPanel->borderColor = Colors::PRIMARY;
    
    // Заголовок
    auto titleLabel = std::make_unique<Label>("⚙️ НАСТРОЙКИ NEOVIA", 50, 40);
    titleLabel->textColor = Colors::PRIMARY;
    titleLabel->fontSize = 24;
    titleLabel->centered = true;
    titleLabel->width = 780;
    settingsPanel->addChild(std::move(titleLabel));
    
    // Настройка языка
    auto langLabel = std::make_unique<Label>("Язык интерфейса:", 50, 120);
    langLabel->textColor = Colors::TEXT;
    langLabel->fontSize = 18;
    settingsPanel->addChild(std::move(langLabel));
    
    auto langButton = std::make_unique<Button>("РУССКИЙ", 400, 110, 200, 40);
    langButton->backgroundColor = Colors::SURFACE;
    langButton->onClick = [this]() { showLanguageSettings(); };
    settingsPanel->addChild(std::move(langButton));
    
    // Настройка приоритета
    auto priorityLabel = std::make_unique<Label>("Приоритет оптимизации:", 50, 200);
    priorityLabel->textColor = Colors::TEXT;
    priorityLabel->fontSize = 18;
    settingsPanel->addChild(std::move(priorityLabel));
    
    auto priorityButton = std::make_unique<Button>("СБАЛАНСИРОВАННЫЙ", 400, 190, 200, 40);
    priorityButton->backgroundColor = Colors::SURFACE;
    priorityButton->onClick = [this]() { showPrioritySettings(); };
    settingsPanel->addChild(std::move(priorityButton));
    
    // Переключатель динамического разрешения
    auto dynResLabel = std::make_unique<Label>("Динамическое разрешение:", 50, 280);
    dynResLabel->textColor = Colors::TEXT;
    dynResLabel->fontSize = 18;
    settingsPanel->addChild(std::move(dynResLabel));
    
    auto dynResButton = std::make_unique<Button>("ВКЛЮЧЕНО", 400, 270, 200, 40);
    dynResButton->backgroundColor = Colors::SUCCESS;
    settingsPanel->addChild(std::move(dynResButton));
    
    // Кнопка возврата
    auto backButton = std::make_unique<Button>("← НАЗАД", 50, 450, 150, 50);
    backButton->backgroundColor = Colors::WARNING;
    backButton->onClick = [this]() { switchScreen(Screen::MAIN_MENU); };
    settingsPanel->addChild(std::move(backButton));
}

void ModernGUI::createAboutMenu() {
    aboutPanel = std::make_unique<Panel>(300, 150, 680, 420);
    aboutPanel->borderWidth = 2;
    aboutPanel->borderColor = Colors::ACCENT;
    
    // Логотип и название
    auto titleLabel = std::make_unique<Label>("NEOVIA v1.0.0", 50, 40);
    titleLabel->textColor = Colors::ACCENT;
    titleLabel->fontSize = 32;
    titleLabel->centered = true;
    titleLabel->width = 580;
    aboutPanel->addChild(std::move(titleLabel));
    
    // Подзаголовок
    auto subtitleLabel = std::make_unique<Label>("Система улучшения графики для Nintendo Switch", 50, 90);
    subtitleLabel->textColor = Colors::TEXT_SECONDARY;
    subtitleLabel->fontSize = 16;
    subtitleLabel->centered = true;
    subtitleLabel->width = 580;
    aboutPanel->addChild(std::move(subtitleLabel));
    
    // Информация об авторе
    auto authorLabel = std::make_unique<Label>("Разработчик: UNIX228", 50, 150);
    authorLabel->textColor = Colors::TEXT;
    authorLabel->fontSize = 18;
    authorLabel->centered = true;
    authorLabel->width = 580;
    aboutPanel->addChild(std::move(authorLabel));
    
    // Описание функций
    auto featuresLabel = std::make_unique<Label>("• Автоматическое улучшение графики\n• Поддержка множества игр\n• Оптимизация производительности\n• Простой и красивый интерфейс", 50, 200);
    featuresLabel->textColor = Colors::TEXT_SECONDARY;
    featuresLabel->fontSize = 14;
    aboutPanel->addChild(std::move(featuresLabel));
    
    // Кнопка возврата
    auto backButton = std::make_unique<Button>("← НАЗАД", 50, 350, 150, 50);
    backButton->backgroundColor = Colors::WARNING;
    backButton->onClick = [this]() { switchScreen(Screen::MAIN_MENU); };
    aboutPanel->addChild(std::move(backButton));
}

void ModernGUI::createEnhancementMenu() {
    enhancementPanel = std::make_unique<Panel>(200, 100, 880, 520);
    enhancementPanel->borderWidth = 2;
    enhancementPanel->borderColor = Colors::SUCCESS;
    
    // Заголовок
    auto titleLabel = std::make_unique<Label>("🎮 УЛУЧШЕНИЕ ИГР", 50, 40);
    titleLabel->textColor = Colors::SUCCESS;
    titleLabel->fontSize = 24;
    titleLabel->centered = true;
    titleLabel->width = 780;
    enhancementPanel->addChild(std::move(titleLabel));
    
    // Прогресс бар
    auto progressBar = std::make_unique<ProgressBar>(50, 150, 780, 30);
    progressBar->fillColor = Colors::SUCCESS;
    enhancementPanel->addChild(std::move(progressBar));
    
    // Статус
    auto statusLabel = std::make_unique<Label>("Сканирование установленных игр...", 50, 200);
    statusLabel->textColor = Colors::TEXT;
    statusLabel->fontSize = 16;
    statusLabel->centered = true;
    statusLabel->width = 780;
    enhancementPanel->addChild(std::move(statusLabel));
    
    // Список найденных игр
    auto gamesLabel = std::make_unique<Label>("Найденные игры:", 50, 250);
    gamesLabel->textColor = Colors::TEXT;
    gamesLabel->fontSize = 18;
    enhancementPanel->addChild(std::move(gamesLabel));
    
    // Кнопка возврата
    auto backButton = std::make_unique<Button>("← НАЗАД", 50, 450, 150, 50);
    backButton->backgroundColor = Colors::WARNING;
    backButton->onClick = [this]() { switchScreen(Screen::MAIN_MENU); };
    enhancementPanel->addChild(std::move(backButton));
}

void ModernGUI::createLoadingMenu() {
    loadingPanel = std::make_unique<Panel>(440, 260, 400, 200);
    loadingPanel->cornerRadius = 20;
    
    // Анимированный индикатор загрузки
    auto loadingLabel = std::make_unique<Label>("ЗАГРУЗКА...", 50, 80);
    loadingLabel->textColor = Colors::PRIMARY;
    loadingLabel->fontSize = 20;
    loadingLabel->centered = true;
    loadingLabel->width = 300;
    loadingPanel->addChild(std::move(loadingLabel));
    
    // Прогресс бар
    auto progressBar = std::make_unique<ProgressBar>(50, 120, 300, 20);
    progressBar->fillColor = Colors::PRIMARY;
    loadingPanel->addChild(std::move(progressBar));
}

// renderBackground реализован в particle_effects.cpp

void ModernGUI::renderParticles(float deltaTime) {
    for (const auto& particle : particles) {
        if (particle.life > 0) {
            float alpha = particle.life / particle.maxLife;
            Color particleColor(particle.color.r, particle.color.g, particle.color.b, (uint8_t)(255 * alpha));
            GFX->drawCircle(particle.x, particle.y, particle.size, particleColor);
        }
    }
}

void ModernGUI::updateParticles(float deltaTime) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    for (auto& particle : particles) {
        if (particle.life > 0) {
            particle.x += particle.vx * deltaTime;
            particle.y += particle.vy * deltaTime;
            particle.life -= deltaTime;
            
            // Границы экрана
            if (particle.x < 0 || particle.x > 1280 || particle.y < 0 || particle.y > 720) {
                particle.life = 0;
            }
        }
    }
    
    // Удаление мертвых частиц и создание новых
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.life <= 0; }), particles.end());
    
    // Добавление новых частиц
    if (particles.size() < 30) {
        spawnParticle();
    }
}

void ModernGUI::spawnParticle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> xDist(0, 1280);
    static std::uniform_real_distribution<float> yDist(0, 720);
    static std::uniform_real_distribution<float> vDist(-50, 50);
    static std::uniform_real_distribution<float> sizeDist(1, 4);
    static std::uniform_real_distribution<float> lifeDist(3, 8);
    
    Particle particle;
    particle.x = xDist(gen);
    particle.y = yDist(gen);
    particle.vx = vDist(gen);
    particle.vy = vDist(gen);
    particle.size = sizeDist(gen);
    particle.maxLife = lifeDist(gen);
    particle.life = particle.maxLife;
    
    // Случайный цвет из палитры
    std::vector<Color> colors = {Colors::PRIMARY, Colors::SECONDARY, Colors::ACCENT, Colors::SUCCESS};
    particle.color = colors[gen() % colors.size()];
    particle.color.a = 100;
    
    particles.push_back(particle);
}

void ModernGUI::renderMainMenu(float deltaTime) {
    // Add holographic panel effect behind main menu
    AdvancedEffects::drawHolographicPanel(400, 250, 480, 300, backgroundOffset, 0.3f);
    
    // Enhanced NEOVIA branding with neon effect
    AdvancedEffects::drawNeonText("NEOVIA", 540, 100, Colors::PRIMARY, 32, 12.0f, 1.0f);
    AdvancedEffects::drawNeonText("Graphics Enhancement System", 460, 140, Colors::SECONDARY, 16, 6.0f, 0.8f);
    
    // Use custom icon if available, otherwise use animated logo
    if (ICON_LOADER->hasIcon("user_icon")) {
        IconEffects::drawPulsingIcon("user_icon", 580, 50, backgroundOffset, 1.0f, 0.2f);
    } else {
        AdvancedEffects::drawAnimatedLogo(580, 50, backgroundOffset, 1.2f);
    }
    
    // Add version info with glow
    AdvancedEffects::drawNeonText("v1.0.0 - AIO Style", 520, 180, Colors::TEXT_SECONDARY, 12, 4.0f, 0.6f);
    
    // Рендер панели
    if (mainPanel) {
        mainPanel->render(deltaTime);
    }
    
    // Enhanced status indicators with icons
    if (ICON_LOADER->hasIcon("success")) {
        IconEffects::drawPulsingIcon("success", 100, 650, backgroundOffset, 0.8f, 0.1f);
    }
    drawStatusIndicator(120, 650, true);  // Система активна
    
    if (ICON_LOADER->hasIcon("settings")) {
        IconEffects::drawRotatingIcon("settings", 200, 650, backgroundOffset, 0.6f, 0.5f);
    }
    drawStatusIndicator(220, 650, config && config->dynamicResolution);  // Динамическое разрешение
    
    if (ICON_LOADER->hasIcon("download")) {
        IconEffects::drawFloatingIcon("download", 300, 650, backgroundOffset, 5.0f, 1.5f);
    }
    drawStatusIndicator(320, 650, false); // Моды загружены
}

void ModernGUI::renderSettingsMenu(float deltaTime) {
    if (settingsPanel) {
        settingsPanel->render(deltaTime);
    }
}

void ModernGUI::renderAboutMenu(float deltaTime) {
    if (aboutPanel) {
        aboutPanel->render(deltaTime);
    }
    
    // Анимированные элементы декора
    float time = backgroundOffset;
    for (int i = 0; i < 3; i++) {
        float x = 150 + i * 100 + sin(time + i) * 20;
        float y = 300 + cos(time * 0.8f + i * 0.7f) * 15;
        GFX->drawCircle(x, y, 5 + sin(time + i) * 2, Colors::ACCENT);
    }
}

void ModernGUI::renderEnhancementMenu(float deltaTime) {
    if (enhancementPanel) {
        enhancementPanel->render(deltaTime);
    }
}

void ModernGUI::renderLoadingMenu(float deltaTime) {
    if (loadingPanel) {
        loadingPanel->render(deltaTime);
    }
    
    // Анимированный спиннер
    float angle = backgroundOffset * 2;
    float centerX = 640, centerY = 360;
    for (int i = 0; i < 8; i++) {
        float a = angle + i * M_PI / 4;
        float x = centerX + cos(a) * 40;
        float y = centerY + sin(a) * 40;
        float alpha = (sin(angle + i * 0.5f) + 1) * 0.5f;
        Color spinnerColor(Colors::PRIMARY.r, Colors::PRIMARY.g, Colors::PRIMARY.b, (uint8_t)(255 * alpha));
        GFX->drawCircle(x, y, 8, spinnerColor);
    }
}

void ModernGUI::handleMainMenuInput(u64 kDown) {
    if (mainPanel) {
        mainPanel->handleInput(kDown);
    }
    
    // Дополнительные горячие клавиши
    if (kDown & HidNpadButton_X) {
        switchScreen(Screen::SETTINGS);
    }
    if (kDown & HidNpadButton_Y) {
        switchScreen(Screen::ABOUT);
    }
}

void ModernGUI::handleSettingsInput(u64 kDown) {
    if (settingsPanel) {
        settingsPanel->handleInput(kDown);
    }
    
    if (kDown & HidNpadButton_B) {
        switchScreen(Screen::MAIN_MENU);
    }
}

void ModernGUI::handleAboutInput(u64 kDown) {
    if (aboutPanel) {
        aboutPanel->handleInput(kDown);
    }
    
    if (kDown & HidNpadButton_B) {
        switchScreen(Screen::MAIN_MENU);
    }
}

void ModernGUI::handleEnhancementInput(u64 kDown) {
    if (enhancementPanel) {
        enhancementPanel->handleInput(kDown);
    }
    
    if (kDown & HidNpadButton_B) {
        switchScreen(Screen::MAIN_MENU);
    }
}

void ModernGUI::startGameEnhancement() {
    switchScreen(Screen::GAME_ENHANCEMENT);
    
    // Здесь будет логика улучшения игр
    // Пока что просто имитируем процесс
}

void ModernGUI::showLanguageSettings() {
    // Переключение языка
    if (config) {
        config->language = static_cast<Language>((config->language + 1) % 3);
        saveConfig(*config);
    }
}

void ModernGUI::showPrioritySettings() {
    // Переключение приоритета
    if (config) {
        config->priority = static_cast<Priority>((config->priority + 1) % 3);
        saveConfig(*config);
    }
}

void ModernGUI::drawNeoviaBranding(float x, float y, float scale) {
    // Стилизованный логотип NEOVIA
    GFX->drawTextCentered("NEOVIA", x - 100, y, 200, Colors::PRIMARY, (int)(32 * scale));
    
    // Декоративные элементы
    Color accentColor = Colors::ACCENT;
    GFX->drawCircle(x - 120, y + 10, 4 * scale, accentColor);
    GFX->drawCircle(x + 120, y + 10, 4 * scale, accentColor);
}

void ModernGUI::drawAnimatedLogo(float x, float y, float time) {
    float scale = 1.0f + sin(time * 0.5f) * 0.1f;
    
    // Свечение логотипа
    GFX->drawGlow(x - 100, y - 20, 200, 40, Colors::PRIMARY, 0.3f);
    
    // Основной логотип
    drawNeoviaBranding(x, y, scale);
    
    // Анимированные частицы вокруг логотипа
    for (int i = 0; i < 6; i++) {
        float angle = time + i * M_PI / 3;
        float px = x + cos(angle) * (60 + sin(time + i) * 10);
        float py = y + sin(angle) * (30 + cos(time + i) * 5);
        
        Color particleColor = Colors::SECONDARY;
        particleColor.a = (uint8_t)(128 + sin(time * 2 + i) * 127);
        GFX->drawCircle(px, py, 3, particleColor);
    }
}

void ModernGUI::drawStatusIndicator(float x, float y, bool active) {
    Color indicatorColor = active ? Colors::SUCCESS : Colors::ERROR;
    
    // Фон индикатора
    GFX->drawCircle(x, y, 12, Colors::SURFACE);
    
    // Активный индикатор
    if (active) {
        GFX->drawGlow(x, y, 20, 20, indicatorColor, 0.5f);
    }
    
    GFX->drawCircle(x, y, 8, indicatorColor);
}

// Глобальный экземпляр
ModernGUI g_modernGui;