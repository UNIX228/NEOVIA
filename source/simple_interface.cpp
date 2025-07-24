#include "simple_interface.h"
#include <cstring>
#include <fstream>

SimpleInterface::SimpleInterface() 
    : currentScreen(Screen::MAIN), selectedItem(0), hasUserIcon(false), 
      framebuffer(nullptr), width(0), height(0) {
}

SimpleInterface::~SimpleInterface() {
    cleanup();
}

bool SimpleInterface::initialize() {
    // Инициализация графики
    Result rc = gfxInitDefault();
    if (R_FAILED(rc)) return false;
    
    gfxConfigureResolution(1280, 720);
    framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);
    if (!framebuffer) return false;
    
    // Проверка иконки
    checkForIcon();
    
    return true;
}

void SimpleInterface::cleanup() {
    gfxExit();
}

void SimpleInterface::checkForIcon() {
    std::ifstream file("icon.jpg");
    hasUserIcon = file.good();
    file.close();
}

void SimpleInterface::render() {
    framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);
    
    // Очистка экрана
    uint32_t bgColor = Colors::WHITE.toRGBA();
    for (uint32_t i = 0; i < width * height; i++) {
        framebuffer[i] = bgColor;
    }
    
    switch (currentScreen) {
        case Screen::MAIN:
            renderMainScreen();
            break;
        case Screen::MENU:
            renderMenu();
            break;
        case Screen::SETTINGS:
            renderSettings();
            break;
        case Screen::ABOUT:
            renderAbout();
            break;
    }
    
    gfxFlushBuffers();
    gfxSwapBuffers();
    gfxWaitForVsync();
}

void SimpleInterface::renderMainScreen() {
    // Три полоски в левом верхнем углу
    drawText("☰", 50, 50, Colors::BLACK, 24);
    
    // Название по центру
    drawText("NEOVIA", 540, 200, Colors::BLACK, 48);
    
    // Кнопка "Улучшить" по центру
    drawButton("Улучшить", 540, 350, 200, 60, true);
}

void SimpleInterface::renderMenu() {
    // Три полоски (активные)
    drawText("☰", 50, 50, Colors::BLUE_ACCENT, 24);
    
    // Логотип и название
    if (hasUserIcon) {
        drawIcon(150, 150, 64);
    } else {
        drawRect(150, 150, 64, 64, Colors::BLUE_ACCENT);
        drawText("N", 175, 175, Colors::WHITE, 32);
    }
    
    drawText("NEOVIA", 230, 170, Colors::BLACK, 32);
    drawText("Graphics Enhancement System", 230, 200, Colors::TEXT_GRAY, 16);
    
    // Пункты меню
    drawButton("Настройки", 150, 280, 300, 50, selectedItem == 0);
    drawButton("О программе", 150, 350, 300, 50, selectedItem == 1);
}

void SimpleInterface::renderSettings() {
    drawText("☰", 50, 50, Colors::BLACK, 24);
    drawText("Настройки", 150, 150, Colors::BLACK, 32);
    drawText("Здесь будут настройки приложения", 150, 200, Colors::TEXT_GRAY, 16);
    drawText("Нажмите B для возврата", 150, 600, Colors::TEXT_GRAY, 14);
}

void SimpleInterface::renderAbout() {
    drawText("☰", 50, 50, Colors::BLACK, 24);
    drawText("О программе", 150, 150, Colors::BLACK, 32);
    drawText("NEOVIA v1.0.0", 150, 200, Colors::TEXT_GRAY, 20);
    drawText("Система улучшения графики для Nintendo Switch", 150, 230, Colors::TEXT_GRAY, 16);
    drawText("Нажмите B для возврата", 150, 600, Colors::TEXT_GRAY, 14);
}

bool SimpleInterface::handleInput(u64 kDown) {
    // Выход из программы
    if (kDown & (HidNpadButton_Plus | HidNpadButton_Minus)) {
        return false;
    }
    
    switch (currentScreen) {
        case Screen::MAIN:
            if (kDown & HidNpadButton_Y) { // Три полоски
                currentScreen = Screen::MENU;
                selectedItem = 0;
            } else if (kDown & HidNpadButton_A) { // Улучшить
                onEnhance();
            }
            break;
            
        case Screen::MENU:
            if (kDown & HidNpadButton_B) {
                currentScreen = Screen::MAIN;
            } else if (kDown & HidNpadButton_Up) {
                selectedItem = (selectedItem - 1 + 2) % 2;
            } else if (kDown & HidNpadButton_Down) {
                selectedItem = (selectedItem + 1) % 2;
            } else if (kDown & HidNpadButton_A) {
                if (selectedItem == 0) {
                    onSettings();
                } else {
                    onAbout();
                }
            }
            break;
            
        case Screen::SETTINGS:
        case Screen::ABOUT:
            if (kDown & HidNpadButton_B) {
                currentScreen = Screen::MENU;
            }
            break;
    }
    
    return true;
}

void SimpleInterface::onEnhance() {
    // Запуск улучшения игр
}

void SimpleInterface::onSettings() {
    currentScreen = Screen::SETTINGS;
}

void SimpleInterface::onAbout() {
    currentScreen = Screen::ABOUT;
}

// Простые функции рисования
void SimpleInterface::drawRect(float x, float y, float w, float h, const Color& color) {
    uint32_t col = color.toRGBA();
    int x1 = (int)x, y1 = (int)y;
    int x2 = (int)(x + w), y2 = (int)(y + h);
    
    for (int py = y1; py < y2 && py < (int)height; py++) {
        for (int px = x1; px < x2 && px < (int)width; px++) {
            if (px >= 0 && py >= 0) {
                framebuffer[py * width + px] = col;
            }
        }
    }
}

void SimpleInterface::drawText(const std::string& text, float x, float y, const Color& color, int size) {
    // Простое отображение текста (заглушка)
    // В реальной реализации здесь будет растровый шрифт
}

void SimpleInterface::drawButton(const std::string& text, float x, float y, float w, float h, bool selected) {
    Color bgColor = selected ? Colors::BLUE_ACCENT : Colors::GRAY_LIGHT;
    Color textColor = selected ? Colors::WHITE : Colors::BLACK;
    
    drawRect(x, y, w, h, bgColor);
    drawText(text, x + w/2 - text.length() * 4, y + h/2 - 8, textColor, 16);
}

void SimpleInterface::drawIcon(float x, float y, float size) {
    // Отображение пользовательской иконки
    drawRect(x, y, size, size, Colors::BLUE_ACCENT);
    drawText("USER", x + 10, y + size/2, Colors::WHITE, 12);
}