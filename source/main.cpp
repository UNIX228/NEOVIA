#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modern_gui.h"
#include "config.h"

extern ModernGUI g_modernGui;

int main(int argc, char* argv[]) {
    // Инициализация системы
    consoleInit(NULL);
    
    // Инициализация пада
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    // Загрузка конфигурации
    Config config = loadConfig();
    
    // Инициализация современного GUI
    if (!g_modernGui.initialize(&config)) {
        printf("Ошибка инициализации графической системы!\n");
        consoleUpdate(NULL);
        
        while (appletMainLoop()) {
            padUpdate(&pad);
            u64 kDown = padGetButtonsDown(&pad);
            if (kDown & HidNpadButton_Plus) break;
        }
        
        consoleExit(NULL);
        return -1;
    }
    
    printf("🎨 NEOVIA v1.0.0 - Система улучшения графики\n");
    printf("💫 Современный интерфейс загружен!\n");
    printf("🎮 Готов к работе...\n\n");
    consoleUpdate(NULL);
    
    // Основной игровой цикл
    while (appletMainLoop()) {
        // Обновление ввода
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        
        // Выход из приложения
        if (kDown & HidNpadButton_Plus) {
            break;
        }
        
        // Обновление GUI
        g_modernGui.update(1.0f / 60.0f); // 60 FPS
        
        // Обработка ввода
        g_modernGui.handleInput(kDown);
        
        // Рендеринг
        g_modernGui.render();
    }
    
    // Очистка ресурсов
    consoleExit(NULL);
    return 0;
}