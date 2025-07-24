#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "neovia.h"
#include "config.h"
#include "downloader.h"
#include "game_database.h"
#include "simple_gui.h"

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

// Обработчики убраны - теперь используется SimpleGUI

int main(int argc, char* argv[]) {
    Result rc = 0;
    
    // Инициализация консоли
    consoleInit(NULL);
    
    // Инициализация контроллера
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    // Инициализация приложения
    rc = initializeApp();
    if (R_FAILED(rc)) {
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
    
    // Инициализация простого GUI
    if (!g_simpleGui.initialize(&config)) {
        printf("Ошибка инициализации GUI\n");
        shutdownApp();
        return -1;
    }
    
    // Главный цикл с простым GUI
    while (appletMainLoop() && !g_exitRequested) {
        // Обработка ввода
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        
        // Глобальный выход
        if (kDown & HidNpadButton_Plus) {
            g_exitRequested = true;
            break;
        }
        
        // Передаем ввод в GUI
        g_simpleGui.handleInput(kDown);
        
        // Рендерим GUI
        g_simpleGui.render();
    }
    
    // Завершение работы
    shutdownApp();
    
    return 0;
}