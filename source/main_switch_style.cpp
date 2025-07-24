#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "switch_main_screen.h"
#include "config.h"

// Глобальные переменные
SwitchMainScreen g_mainScreen;

int main(int argc, char* argv[]) {
    // Инициализация системы
    consoleInit(NULL);
    
    // Инициализация пада
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    // Загрузка конфигурации
    Config config = loadConfig();
    
    // Инициализация графической системы Switch
    if (!SWITCH_GFX->initialize()) {
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
    
    // Инициализация главного экрана
    if (!g_mainScreen.initialize(&config)) {
        printf("Ошибка инициализации главного экрана!\n");
        SWITCH_GFX->cleanup();
        consoleExit(NULL);
        return -1;
    }
    
    printf("🎮 NEOVIA v1.0.0 - Nintendo Switch Style\n");
    printf("✨ Графическая система инициализирована\n");
    printf("🖼️ Поиск пользовательской иконки...\n\n");
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
        
        // Обновление главного экрана
        g_mainScreen.update(1.0f / 60.0f); // 60 FPS
        
        // Обработка ввода
        g_mainScreen.handleInput(kDown);
        
        // Рендеринг в стиле Switch
        SWITCH_GFX->beginFrame();
        g_mainScreen.render(1.0f / 60.0f);
        SWITCH_GFX->endFrame();
    }
    
    // Очистка ресурсов
    SWITCH_GFX->cleanup();
    consoleExit(NULL);
    return 0;
}