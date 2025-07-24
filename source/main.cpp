#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "neovia.h"
#include "graphics.h"
#include "gui.h"
#include "config.h"
#include "downloader.h"
#include "game_database.h"

// Глобальные переменные
AppletHookCookie g_appletHookCookie;
bool g_exitRequested = false;

// Инициализация приложения
Result initializeApp() {
    Result rc = 0;
    
    // Инициализация сервисов
    rc = appletInitialize();
    if (R_FAILED(rc)) return rc;
    
    rc = hidInitialize();
    if (R_FAILED(rc)) return rc;
    
    rc = fsInitialize();
    if (R_FAILED(rc)) return rc;
    
    rc = nsInitialize();
    if (R_FAILED(rc)) return rc;
    
    rc = socketInitializeDefault();
    if (R_FAILED(rc)) return rc;
    
    rc = nifmInitialize(NifmServiceType_User);
    if (R_FAILED(rc)) return rc;
    
    // Инициализация curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Установка хука для выхода
    appletHook(&g_appletHookCookie, [](AppletHookType hook, void* param) {
        if (hook == AppletHookType_OnExitRequest) {
            g_exitRequested = true;
        }
    }, nullptr);
    
    return 0;
}

// Завершение работы приложения
void shutdownApp() {
    appletUnhook(&g_appletHookCookie);
    curl_global_cleanup();
    nifmExit();
    socketExit();
    nsExit();
    fsExit();
    hidExit();
    appletExit();
}

// Создание структуры папок
Result createDirectoryStructure() {
    Result rc = 0;
    
    // Создаем основную папку graphics
    rc = fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), "/graphics");
    if (R_FAILED(rc) && rc != 0x402) { // 0x402 = уже существует
        return rc;
    }
    
    // Создаем папку для NEOVIA
    rc = fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), "/switch");
    if (R_FAILED(rc) && rc != 0x402) {
        return rc;
    }
    
    rc = fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), "/switch/NEOVIA");
    if (R_FAILED(rc) && rc != 0x402) {
        return rc;
    }
    
    rc = fsFsCreateDirectory(fsdevGetDeviceFileSystem("sdmc"), "/switch/NEOVIA/extras");
    if (R_FAILED(rc) && rc != 0x402) {
        return rc;
    }
    
    return 0;
}

// Обработчик кнопки "Улучшить"
void onEnhanceButtonClicked() {
    g_gui->setStatusText("Сканирование игр...");
    
    // Получаем список установленных игр
    std::vector<GameInfo> games;
    Result rc = scanInstalledGames(games);
    
    if (R_SUCCEEDED(rc) && !games.empty()) {
        g_gui->setStatusText("Загрузка модов...");
        
        // Загружаем моды для первых 5 игр
        int processed = 0;
        for (const auto& game : games) {
            if (processed >= 5) break;
            
            rc = downloadModsForGame(game.titleId);
            if (R_SUCCEEDED(rc)) {
                processed++;
            }
        }
        
        if (processed > 0) {
            g_gui->setStatusText("Готово! Обработано игр: " + std::to_string(processed));
        } else {
            g_gui->setStatusText("Ошибка загрузки модов");
        }
    } else {
        g_gui->setStatusText("Игры не найдены");
    }
}

// Обработчик настроек
void onSettingsButtonClicked() {
    // Переходим в меню настроек
    g_gui->setState(GUIState::Settings);
}

// Обработчик "О нас"
void onAboutButtonClicked() {
    // Переходим в меню "О нас"
    g_gui->setState(GUIState::About);
}

int main(int argc, char* argv[]) {
    Result rc = 0;
    
    // Инициализация контроллера
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    // Инициализация графической системы
    g_graphics = std::make_unique<GraphicsSystem>();
    if (!g_graphics->initialize()) {
        // Откат к консольному режиму при ошибке
        consoleInit(NULL);
        printf("Ошибка инициализации графики\n");
        printf("Нажмите + для выхода\n");
        
        while (appletMainLoop() && !g_exitRequested) {
            padUpdate(&pad);
            u64 kDown = padGetButtonsDown(&pad);
            if (kDown & HidNpadButton_Plus) break;
            consoleUpdate(NULL);
        }
        return -1;
    }
    
    // Инициализация приложения
    rc = initializeApp();
    if (R_FAILED(rc)) {
        g_graphics->shutdown();
        consoleInit(NULL);
        printf("Ошибка инициализации: 0x%x\n", rc);
        printf("Нажмите + для выхода\n");
        
        while (appletMainLoop() && !g_exitRequested) {
            padUpdate(&pad);
            u64 kDown = padGetButtonsDown(&pad);
            if (kDown & HidNpadButton_Plus) break;
            consoleUpdate(NULL);
        }
        return rc;
    }
    
    // Создаем структуру папок
    createDirectoryStructure();
    
    // Загружаем конфигурацию
    Config config;
    loadConfig(config);
    
    // Инициализация GUI
    g_gui = std::make_unique<NEOVIA_GUI>();
    if (!g_gui->initialize(&config)) {
        g_graphics->shutdown();
        shutdownApp();
        return -1;
    }
    
    // Устанавливаем обработчики событий
    g_gui->setOnEnhanceClicked(onEnhanceButtonClicked);
    g_gui->setOnSettingsClicked(onSettingsButtonClicked);
    g_gui->setOnAboutClicked(onAboutButtonClicked);
    
    // Главный цикл с GUI
    u64 lastTime = armGetSystemTick();
    
    while (appletMainLoop() && !g_exitRequested) {
        // Вычисляем delta time
        u64 currentTime = armGetSystemTick();
        float deltaTime = (float)(currentTime - lastTime) / armGetSystemTickFreq();
        lastTime = currentTime;
        
        // Обработка ввода
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        
        // Глобальный выход
        if (kDown & HidNpadButton_Plus) {
            g_exitRequested = true;
            break;
        }
        
        // Передаем ввод в GUI
        Point touchPos = {640, 360}; // Центр экрана по умолчанию
        g_gui->handleInput(kDown, touchPos);
        
        // Обновляем GUI
        g_gui->update(deltaTime);
        
        // Рендерим GUI
        g_gui->render();
    }
    
    // Завершение работы
    g_gui->shutdown();
    g_graphics->shutdown();
    shutdownApp();
    
    return 0;
}