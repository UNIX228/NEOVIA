#include "ui.h"
#include "neovia.h"
#include "config.h"
#include <switch.h>
#include <string>

static Config g_currentConfig;
static int g_selectedMenuItem = 0;

Result initializeUI() {
    Result rc = loadConfig(g_currentConfig);
    if (R_FAILED(rc)) {
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

void finalizeUI() {
    saveConfig(g_currentConfig);
}

void updateUI() {
    // UI update logic
}

void handleUIInput(u64 kDown) {
    if (kDown & KEY_UP) {
        g_selectedMenuItem = (g_selectedMenuItem - 1 + 6) % 6;
    }
    if (kDown & KEY_DOWN) {
        g_selectedMenuItem = (g_selectedMenuItem + 1) % 6;
    }
    if (kDown & KEY_B) {
        g_exitRequested = true;
    }
}

void renderUI() {
    printf("\033[2J\033[H");
    printf("╔════════════════════════════════════════╗\n");
    printf("║              NEOVIA v%s              ║\n", NEOVIA_VERSION);
    printf("║        Graphics Enhancement Tool       ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    printf("Управление: ↑↓ - выбор, B - выход\n");
    consoleUpdate(NULL);
}

void drawHeader() {
    renderUI();
}

void drawMainMenu() {
    renderUI();
}

void drawProgressWindow() {
    printf("Загрузка...\n");
    consoleUpdate(NULL);
}
