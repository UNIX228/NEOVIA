#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_interface.h"
#include "config.h"
#include "neocore.h"

SimpleInterface g_interface;

int main(int argc, char* argv[]) {
    // Инициализация системы
    consoleInit(NULL);
    
    logToGraphics("NEOVIA", "========== NEOVIA STARTUP ==========");
    logToGraphics("NEOVIA", "Version: 1.0.0");
    logToGraphics("NEOVIA", "Author: Unix228");
    
    // Инициализация пада
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    logToGraphics("NEOVIA", "Input system initialized");
    
    // Загрузка конфигурации
    Config config = loadConfig();
    logToGraphics("NEOVIA", "Configuration loaded");
    
    // Инициализация интерфейса
    if (!g_interface.initialize()) {
        logToGraphics("NEOVIA", "CRITICAL ERROR: Failed to initialize interface!");
        printf("Ошибка инициализации интерфейса!\n");
        consoleUpdate(NULL);
        
        while (appletMainLoop()) {
            padUpdate(&pad);
            u64 kDown = padGetButtonsDown(&pad);
            if (kDown & HidNpadButton_Plus) break;
        }
        
        consoleExit(NULL);
        return -1;
    }
    
    printf("🎮 NEOVIA v1.0.0\n");
    printf("📱 Простой интерфейс загружен\n");
    printf("🖼️ Иконка: %s\n", "icon.jpg найдена" );
    consoleUpdate(NULL);
    
    logToGraphics("NEOVIA", "Interface loaded successfully, entering main loop");
    
    // Основной игровой цикл
    while (appletMainLoop()) {
        // Обновление ввода
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        
        // Обработка ввода интерфейсом
        if (!g_interface.handleInput(kDown)) {
            logToGraphics("NEOVIA", "Exit signal received from user");
            break; // Выход из программы
        }
        
        // Рендеринг
        g_interface.render();
    }
    
    // Очистка ресурсов
    logToGraphics("NEOVIA", "Application shutdown, cleaning up resources...");
    g_interface.cleanup();
    logToGraphics("NEOVIA", "========== NEOVIA SHUTDOWN ==========");
    consoleExit(NULL);
    return 0;
}