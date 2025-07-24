#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <curl/curl.h>
// #include <json/json.h> // Убрано для упрощения
#include "neovia.h"
#include "ui.h"
#include "downloader.h"
#include "game_database.h"
#include "config.h"

// Глобальные переменные
AppletHookCookie g_appletHookCookie;
bool g_exitRequested = false;

// Инициализация приложения
Result initializeApp() {
    Result rc = 0;
    
    // Инициализация сервисов
    rc = romfsInit();
    if (R_FAILED(rc)) {
        printf("romfsInit() failed: 0x%x\n", rc);
        return rc;
    }
    
    rc = socketInitializeDefault();
    if (R_FAILED(rc)) {
        printf("socketInitializeDefault() failed: 0x%x\n", rc);
        return rc;
    }
    
    rc = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (rc != CURLE_OK) {
        printf("curl_global_init() failed: %d\n", rc);
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    rc = nsInitialize();
    if (R_FAILED(rc)) {
        printf("nsInitialize() failed: 0x%x\n", rc);
        return rc;
    }
    
    // Инициализация файловой системы
    rc = fsInitialize();
    if (R_FAILED(rc)) {
        printf("fsInitialize() failed: 0x%x\n", rc);
        return rc;
    }
    
    return 0;
}

// Завершение работы приложения
void finalizeApp() {
    curl_global_cleanup();
    socketExit();
    nsExit();
    fsExit();
    romfsExit();
}

// Обработчик событий applet
void appletHookFunc(AppletHookType hook, void* param) {
    switch (hook) {
        case AppletHookType_OnExitRequest:
            g_exitRequested = true;
            break;
        default:
            break;
    }
}

// Создание базовой структуры папок
Result createDirectoryStructure() {
    Result rc = 0;
    
    // Создаем основную папку graphics
    rc = fsMkdir(fsdevGetDeviceFileSystem("sdmc"), "/graphics");
    if (R_FAILED(rc) && rc != 0x402) { // 0x402 = уже существует
        printf("Failed to create /graphics directory: 0x%x\n", rc);
        return rc;
    }
    
    // Создаем папку для NEOVIA
    rc = fsMkdir(fsdevGetDeviceFileSystem("sdmc"), "/switch");
    if (R_FAILED(rc) && rc != 0x402) {
        printf("Failed to create /switch directory: 0x%x\n", rc);
        return rc;
    }
    
    rc = fsMkdir(fsdevGetDeviceFileSystem("sdmc"), "/switch/NEOVIA");
    if (R_FAILED(rc) && rc != 0x402) {
        printf("Failed to create /switch/NEOVIA directory: 0x%x\n", rc);
        return rc;
    }
    
    rc = fsMkdir(fsdevGetDeviceFileSystem("sdmc"), "/switch/NEOVIA/extras");
    if (R_FAILED(rc) && rc != 0x402) {
        printf("Failed to create /switch/NEOVIA/extras directory: 0x%x\n", rc);
        return rc;
    }
    
    return 0;
}

// Первичная настройка при первом запуске
Result firstTimeSetup() {
    printf("Выполняется первичная настройка NEOVIA...\n");
    consoleUpdate(NULL);
    
    // Создаем структуру папок
    Result rc = createDirectoryStructure();
    if (R_FAILED(rc)) {
        return rc;
    }
    
    // Сканируем установленные игры
    printf("Сканирование установленных игр...\n");
    consoleUpdate(NULL);
    
    std::vector<GameInfo> installedGames;
    rc = scanInstalledGames(installedGames);
    if (R_FAILED(rc)) {
        printf("Ошибка сканирования игр: 0x%x\n", rc);
        return rc;
    }
    
    printf("Найдено игр: %zu\n", installedGames.size());
    consoleUpdate(NULL);
    
    // Загружаем моды для найденных игр
    for (const auto& game : installedGames) {
        printf("Загрузка модов для: %s\n", game.name.c_str());
        consoleUpdate(NULL);
        
        rc = downloadModsForGame(game.titleId);
        if (R_FAILED(rc)) {
            printf("Ошибка загрузки модов для %s: 0x%x\n", game.name.c_str(), rc);
        }
    }
    
    // Создаем конфигурационный файл
    Config config;
    config.firstRun = false;
    config.language = LANG_RU;
    config.priority = PRIORITY_GRAPHICS;
    config.dynamicResolution = true;
    config.downloadAllMods = true;
    config.autoStart = false;
    
    rc = saveConfig(config);
    if (R_FAILED(rc)) {
        printf("Ошибка сохранения конфигурации: 0x%x\n", rc);
        return rc;
    }
    
    printf("Первичная настройка завершена!\n");
    consoleUpdate(NULL);
    sleep(2);
    
    return 0;
}

int main(int argc, char* argv[]) {
    Result rc = 0;
    
    // Инициализация консоли для отладки
    consoleInit(NULL);
    
    // Инициализация приложения
    rc = initializeApp();
    if (R_FAILED(rc)) {
        printf("Ошибка инициализации: 0x%x\n", rc);
        printf("Нажмите + для выхода.\n");
        
        while (appletMainLoop()) {
            hidScanInput();
            u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
            if (kDown & KEY_PLUS) break;
            consoleUpdate(NULL);
        }
        
        consoleExit(NULL);
        return 1;
    }
    
    // Регистрируем обработчик событий applet
    appletHook(&g_appletHookCookie, appletHookFunc, NULL);
    
    // Загружаем конфигурацию
    Config config;
    rc = loadConfig(config);
    
    // Если первый запуск или конфиг не найден
    if (R_FAILED(rc) || config.firstRun) {
        rc = firstTimeSetup();
        if (R_FAILED(rc)) {
            printf("Ошибка первичной настройки: 0x%x\n", rc);
            printf("Нажмите + для выхода.\n");
            
            while (appletMainLoop()) {
                hidScanInput();
                u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
                if (kDown & KEY_PLUS) break;
                consoleUpdate(NULL);
            }
            
            goto cleanup;
        }
        
        // Перезагружаем конфигурацию
        rc = loadConfig(config);
        if (R_FAILED(rc)) {
            printf("Ошибка загрузки конфигурации: 0x%x\n", rc);
            goto cleanup;
        }
    }
    
    // Инициализация UI
    rc = initializeUI();
    if (R_FAILED(rc)) {
        printf("Ошибка инициализации UI: 0x%x\n", rc);
        goto cleanup;
    }
    
    // Основной цикл приложения
    while (appletMainLoop() && !g_exitRequested) {
        // Обновление UI
        updateUI();
        
        // Обработка входных данных
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        
        if (kDown & KEY_PLUS) {
            g_exitRequested = true;
        }
        
        // Обработка UI событий
        handleUIInput(kDown);
        
        // Рендеринг
        renderUI();
    }
    
cleanup:
    // Очистка ресурсов
    finalizeUI();
    appletUnhook(&g_appletHookCookie);
    finalizeApp();
    consoleExit(NULL);
    
    return 0;
}