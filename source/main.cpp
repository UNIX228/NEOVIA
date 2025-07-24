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
    
    logToGraphics("NEOVIA", "========== NEOVIA ЗАПУСК ==========");
    logToGraphics("NEOVIA", "Версия: 1.0.0");
    logToGraphics("NEOVIA", "Автор: Unix228");
    
    // Инициализация пада
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    logToGraphics("NEOVIA", "Система управления инициализирована");
    
    // Загрузка конфигурации
    Config config = loadConfig();
    logToGraphics("NEOVIA", "Конфигурация загружена");
    
    // Инициализация интерфейса
    if (!g_interface.initialize()) {
        logToGraphics("NEOVIA", "КРИТИЧЕСКАЯ ОШИБКА: Не удалось инициализировать интерфейс!");
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
    
    logToGraphics("NEOVIA", "Интерфейс успешно загружен, переход в основной цикл");
    
    // Основной игровой цикл
    while (appletMainLoop()) {
        // Обновление ввода
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        
        // Обработка ввода интерфейсом
        if (!g_interface.handleInput(kDown)) {
            logToGraphics("NEOVIA", "Получен сигнал выхода из приложения");
            break; // Выход из программы
        }
        
        // Рендеринг
        g_interface.render();
    }
    
    // Очистка ресурсов
    logToGraphics("NEOVIA", "Завершение работы приложения, очистка ресурсов...");
    g_interface.cleanup();
    logToGraphics("NEOVIA", "========== NEOVIA ЗАВЕРШЕН ==========");
    consoleExit(NULL);
    return 0;
}