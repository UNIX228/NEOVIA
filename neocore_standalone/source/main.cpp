#include <switch.h>
#include <iostream>
#include "neocore.h"

// NeoCore Standalone Demo Application
// Демонстрирует возможности NeoCore Engine как отдельного проекта

int main(int argc, char* argv[]) {
    // Инициализация консоли для вывода
    consoleInit(NULL);
    
    std::cout << "==================================" << std::endl;
    std::cout << "🎮 NeoCore Graphics Engine v1.0.0" << std::endl;
    std::cout << "   Standalone Demo Application" << std::endl;
    std::cout << "   Author: Unix228" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    
    // Инициализация NeoCore
    std::cout << "🔧 Инициализация NeoCore Engine..." << std::endl;
    
    NeoCoreManager neoCore;
    if (!neoCore.initialize()) {
        std::cout << "❌ Ошибка инициализации: " << neoCore.getLastError() << std::endl;
        std::cout << "Нажмите + для выхода" << std::endl;
        
        while (appletMainLoop()) {
            hidScanInput();
            u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
            
            if (kDown & KEY_PLUS) break;
            
            consoleUpdate(NULL);
        }
        
        consoleExit(NULL);
        return -1;
    }
    
    std::cout << "✅ NeoCore Engine успешно инициализирован!" << std::endl;
    std::cout << "📊 Статус: " << (neoCore.isReady() ? "Готов" : "Не готов") << std::endl;
    std::cout << "🎯 Версия: " << neoCore.getVersion() << std::endl;
    std::cout << "👤 Автор: " << neoCore.getAuthor() << std::endl;
    std::cout << std::endl;
    
    // Демонстрация возможностей
    std::cout << "🎮 Демонстрация возможностей:" << std::endl;
    std::cout << std::endl;
    
    // Создание тестовой игры
    GameInfo testGame;
    testGame.gameId = "0100000000000001";
    testGame.gameName = "NeoCore Demo Game";
    testGame.activeMods = {"shadows", "fxaa", "smaa"};
    testGame.hasCustomProfile = false;
    
    std::cout << "🚀 Запуск NeoCore для тестовой игры..." << std::endl;
    
    if (neoCore.startCore(testGame)) {
        std::cout << "✅ NeoCore успешно запущен!" << std::endl;
        std::cout << "🎯 Игра: " << testGame.gameName << std::endl;
        std::cout << "🆔 ID: " << testGame.gameId << std::endl;
        std::cout << "🔧 Активные моды: " << testGame.activeMods.size() << std::endl;
        
        for (const auto& mod : testGame.activeMods) {
            std::cout << "   • " << mod << std::endl;
        }
    } else {
        std::cout << "❌ Ошибка запуска: " << neoCore.getLastError() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "🎮 Тестирование модулей:" << std::endl;
    
    // Тест модулей
    neoCore.enableModule(NeoCoreModule::SHADOWS, true);
    std::cout << "✅ Модуль теней включен" << std::endl;
    
    neoCore.enableModule(NeoCoreModule::FXAA, true);
    std::cout << "✅ Модуль FXAA включен" << std::endl;
    
    neoCore.enableModule(NeoCoreModule::OVERLAY, true);
    std::cout << "✅ Модуль Overlay включен" << std::endl;
    
    // Тест приоритетов
    std::cout << std::endl;
    std::cout << "⚙️ Тестирование приоритетов качества:" << std::endl;
    
    neoCore.setQualityPriority(QualityPriority::GRAPHICS);
    std::cout << "🎨 Приоритет: Максимальная графика" << std::endl;
    
    neoCore.setQualityPriority(QualityPriority::FPS);
    std::cout << "⚡ Приоритет: Максимальный FPS" << std::endl;
    
    neoCore.setQualityPriority(QualityPriority::STABILITY);
    std::cout << "⚖️ Приоритет: Стабильность" << std::endl;
    
    std::cout << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "🎯 NeoCore Engine готов к работе!" << std::endl;
    std::cout << "📁 Файлы созданы в /graphics/NeoCore/" << std::endl;
    std::cout << "📝 Логи доступны в core_log.txt" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Управление:" << std::endl;
    std::cout << "🔄 A - Перезапустить NeoCore" << std::endl;
    std::cout << "⏹️ B - Остановить NeoCore" << std::endl;
    std::cout << "🎮 X - Переключить модуль теней" << std::endl;
    std::cout << "🔧 Y - Переключить FXAA" << std::endl;
    std::cout << "➕ + - Выход" << std::endl;
    std::cout << std::endl;
    
    bool shadowsEnabled = true;
    bool fxaaEnabled = true;
    
    // Основной цикл
    while (appletMainLoop()) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        
        if (kDown & KEY_PLUS) {
            std::cout << "👋 Завершение работы NeoCore..." << std::endl;
            break;
        }
        
        if (kDown & KEY_A) {
            std::cout << "🔄 Перезапуск NeoCore..." << std::endl;
            neoCore.stopCore();
            if (neoCore.startCore(testGame)) {
                std::cout << "✅ NeoCore перезапущен!" << std::endl;
            } else {
                std::cout << "❌ Ошибка перезапуска!" << std::endl;
            }
        }
        
        if (kDown & KEY_B) {
            std::cout << "⏹️ Остановка NeoCore..." << std::endl;
            neoCore.stopCore();
            std::cout << "✅ NeoCore остановлен!" << std::endl;
        }
        
        if (kDown & KEY_X) {
            shadowsEnabled = !shadowsEnabled;
            neoCore.enableModule(NeoCoreModule::SHADOWS, shadowsEnabled);
            std::cout << "🎮 Тени: " << (shadowsEnabled ? "Включены" : "Выключены") << std::endl;
        }
        
        if (kDown & KEY_Y) {
            fxaaEnabled = !fxaaEnabled;
            neoCore.enableModule(NeoCoreModule::FXAA, fxaaEnabled);
            std::cout << "🔧 FXAA: " << (fxaaEnabled ? "Включен" : "Выключен") << std::endl;
        }
        
        consoleUpdate(NULL);
    }
    
    // Очистка
    neoCore.cleanup();
    std::cout << "🧹 NeoCore очищен" << std::endl;
    std::cout << "👋 До свидания!" << std::endl;
    
    consoleExit(NULL);
    return 0;
}