#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_interface.h"
#include "config.h"

SimpleInterface g_interface;

int main(int argc, char* argv[]) {
    // Инициализация системы
    consoleInit(NULL);
    
    // Инициализация пада
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    // Загрузка конфигурации
    Config config = loadConfig();
    
    // Инициализация интерфейса
    if (!g_interface.initialize()) {
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
    
    // Основной игровой цикл
    while (appletMainLoop()) {
        // Обновление ввода
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        
        // Обработка ввода интерфейсом
        if (!g_interface.handleInput(kDown)) {
            break; // Выход из программы
        }
        
        // Рендеринг
        g_interface.render();
    }
    
    // Очистка ресурсов
    g_interface.cleanup();
    consoleExit(NULL);
    return 0;
}