#include "ui.h"
#include "neovia.h"
#include "config.h"
#include "downloader.h"
#include <switch.h>
#include <string>

// Глобальные переменные UI
static Config g_currentConfig;
static int g_selectedMenuItem = 0;
static bool g_inSettings = false;
static bool g_showingProgress = false;
static DownloadProgress g_currentProgress;

// Цвета в стиле Nintendo Switch (темная тема)
#define COLOR_BACKGROUND    0xFF2D2D2D
#define COLOR_SURFACE       0xFF3C3C3C
#define COLOR_PRIMARY       0xFF0099FF
#define COLOR_TEXT_PRIMARY  0xFFFFFFFF
#define COLOR_TEXT_SECONDARY 0xFFCCCCCC
#define COLOR_ACCENT        0xFF00D4AA
#define COLOR_WARNING       0xFFFF6B00
#define COLOR_ERROR         0xFFFF5555

// Инициализация UI
Result initializeUI() {
    Result rc = loadConfig(g_currentConfig);
    if (R_FAILED(rc)) {
        // Используем настройки по умолчанию
        g_currentConfig.firstRun = true;
        g_currentConfig.language = LANG_RU;
        g_currentConfig.priority = PRIORITY_GRAPHICS;
        g_currentConfig.dynamicResolution = true;
        g_currentConfig.downloadAllMods = true;
        g_currentConfig.autoStart = false;
        g_currentConfig.extrasInstalled = false;
    }
    
    return 0;
}

// Завершение работы UI
void finalizeUI() {
    // Сохраняем конфигурацию при выходе
    saveConfig(g_currentConfig);
}

// Отрисовка заголовка
void drawHeader() {
    // Фон заголовка
    // В реальной реализации здесь был бы код отрисовки через libnx graphics
    printf("\033[2J\033[H"); // Очистка экрана и переход в начало
    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                  NEOVIA                                    ║\n");
    printf("║                        Graphics Enhancement Tool                           ║\n");
    printf("║                              v%s by %s                              ║\n", NEOVIA_VERSION, NEOVIA_AUTHOR);
    printf("╚════════════════════════════════════════════════════════════════════════════╝\n\n");
}

// Отрисовка главного меню
void drawMainMenu() {
    std::string lang_settings = getLanguageString(g_currentConfig.language, "settings");
    std::string lang_priority = getLanguageString(g_currentConfig.language, "priority");
    std::string lang_dynamic_res = getLanguageString(g_currentConfig.language, "dynamic_resolution");
    std::string lang_download_all = getLanguageString(g_currentConfig.language, "download_all_mods");
    std::string lang_auto_start = getLanguageString(g_currentConfig.language, "auto_start");
    std::string lang_download_extras = getLanguageString(g_currentConfig.language, "download_extras");
    std::string lang_language = getLanguageString(g_currentConfig.language, "language");
    
    printf("┌─ %s ─────────────────────────────────────────────────────────────────────┐\n", lang_settings.c_str());
    printf("│                                                                            │\n");
    
    // Язык
    printf("│ %s %s: ", (g_selectedMenuItem == 0) ? "►" : " ", lang_language.c_str());
    switch (g_currentConfig.language) {
        case LANG_RU: printf("Русский"); break;
        case LANG_EN: printf("English"); break;
        case LANG_UK: printf("Українська"); break;
    }
    printf("%*s│\n", (int)(50 - lang_language.length()), "");
    
    // Приоритет
    printf("│ %s %s: ", (g_selectedMenuItem == 1) ? "►" : " ", lang_priority.c_str());
    switch (g_currentConfig.priority) {
        case PRIORITY_FPS: 
            printf("%s", getLanguageString(g_currentConfig.language, "priority_fps").c_str()); 
            break;
        case PRIORITY_STABILITY: 
            printf("%s", getLanguageString(g_currentConfig.language, "priority_stability").c_str()); 
            break;
        case PRIORITY_GRAPHICS: 
            printf("%s", getLanguageString(g_currentConfig.language, "priority_graphics").c_str()); 
            break;
    }
    printf("%*s│\n", (int)(45 - lang_priority.length()), "");
    
    // Динамическое разрешение
    printf("│ %s %s: %s", (g_selectedMenuItem == 2) ? "►" : " ", 
           lang_dynamic_res.c_str(), 
           g_currentConfig.dynamicResolution ? "ON " : "OFF");
    printf("%*s│\n", (int)(35 - lang_dynamic_res.length()), "");
    
    // Загрузка модов для всех игр
    printf("│ %s %s: %s", (g_selectedMenuItem == 3) ? "►" : " ", 
           lang_download_all.c_str(), 
           g_currentConfig.downloadAllMods ? "ON " : "OFF");
    printf("%*s│\n", (int)(20 - lang_download_all.length()), "");
    
    // Автозапуск
    printf("│ %s %s: %s", (g_selectedMenuItem == 4) ? "►" : " ", 
           lang_auto_start.c_str(), 
           g_currentConfig.autoStart ? "ON " : "OFF");
    printf("%*s│\n", (int)(35 - lang_auto_start.length()), "");
    
    // Скачать дополнительные возможности
    printf("│ %s %s", (g_selectedMenuItem == 5) ? "►" : " ", lang_download_extras.c_str());
    if (g_currentConfig.extrasInstalled) {
        printf(" ✓");
    }
    printf("%*s│\n", (int)(40 - lang_download_extras.length()), "");
    
    printf("│                                                                            │\n");
    printf("└────────────────────────────────────────────────────────────────────────────┘\n\n");
    
    printf("Управление: ↑↓ - выбор, A - изменить, B - выход, X - применить настройки\n");
}

// Отрисовка окна прогресса загрузки
void drawProgressWindow() {
    std::string lang_downloading = getLanguageString(g_currentConfig.language, "downloading");
    
    printf("┌─ %s ─────────────────────────────────────────────────────────────────┐\n", lang_downloading.c_str());
    printf("│                                                                            │\n");
    printf("│ Файл: %-60s │\n", g_currentProgress.currentFile.c_str());
    printf("│                                                                            │\n");
    
    // Прогресс бар
    printf("│ [");
    int barWidth = 60;
    int progress = (g_currentProgress.percentage * barWidth) / 100;
    for (int i = 0; i < barWidth; i++) {
        if (i < progress) {
            printf("█");
        } else {
            printf("░");
        }
    }
    printf("] %3d%% │\n", g_currentProgress.percentage);
    
    printf("│                                                                            │\n");
    
    if (g_currentProgress.error) {
        printf("│ ERROR: %-64s │\n", g_currentProgress.errorMessage.c_str());
    } else if (g_currentProgress.completed) {
        std::string lang_completed = getLanguageString(g_currentConfig.language, "completed");
        printf("│ %s%*s │\n", lang_completed.c_str(), (int)(70 - lang_completed.length()), "");
    }
    
    printf("│                                                                            │\n");
    printf("└────────────────────────────────────────────────────────────────────────────┘\n\n");
    
    if (g_currentProgress.completed || g_currentProgress.error) {
        printf("Нажмите A для продолжения\n");
    }
}

// Обновление UI
void updateUI() {
    if (g_showingProgress) {
        g_currentProgress = getDownloadProgress();
    }
}

// Обработка ввода
void handleUIInput(u64 kDown) {
    if (g_showingProgress) {
        if (kDown & HidNpadButton_A) {
            if (g_currentProgress.completed || g_currentProgress.error) {
                g_showingProgress = false;
                resetDownloadProgress();
            }
        }
        return;
    }
    
    if (kDown & HidNpadButton_Up) {
        g_selectedMenuItem = (g_selectedMenuItem - 1 + 6) % 6;
    }
    
    if (kDown & HidNpadButton_Down) {
        g_selectedMenuItem = (g_selectedMenuItem + 1) % 6;
    }
    
    if (kDown & HidNpadButton_A) {
        switch (g_selectedMenuItem) {
            case 0: // Язык
                g_currentConfig.language = static_cast<Language>((g_currentConfig.language + 1) % 3);
                break;
                
            case 1: // Приоритет
                g_currentConfig.priority = static_cast<Priority>((g_currentConfig.priority + 1) % 3);
                break;
                
            case 2: // Динамическое разрешение
                g_currentConfig.dynamicResolution = !g_currentConfig.dynamicResolution;
                break;
                
            case 3: // Загрузка модов для всех игр
                g_currentConfig.downloadAllMods = !g_currentConfig.downloadAllMods;
                break;
                
            case 4: // Автозапуск
                g_currentConfig.autoStart = !g_currentConfig.autoStart;
                break;
                
            case 5: // Скачать дополнительные возможности
                if (!g_currentConfig.extrasInstalled) {
                    g_showingProgress = true;
                    resetDownloadProgress();
                    
                    // Запускаем загрузку в фоновом режиме
                    Result rc = downloadExtras();
                    if (R_SUCCEEDED(rc)) {
                        g_currentConfig.extrasInstalled = true;
                        saveConfig(g_currentConfig);
                    }
                }
                break;
        }
    }
    
    if (kDown & HidNpadButton_X) {
        // Применить настройки
        saveConfig(g_currentConfig);
    }
    
    if (kDown & HidNpadButton_B) {
        g_exitRequested = true;
    }
}

// Основная функция рендеринга
void renderUI() {
    drawHeader();
    
    if (g_showingProgress) {
        drawProgressWindow();
    } else {
        drawMainMenu();
    }
    
    consoleUpdate(NULL);
}