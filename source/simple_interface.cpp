#include "simple_interface.h"
#include "neocore.h"
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
    logToGraphics("Interface", "Инициализация графического интерфейса NEOVIA...");
    
    // Инициализация графики
    Result rc = gfxInitDefault();
    if (R_FAILED(rc)) {
        logToGraphics("Interface", "Ошибка инициализации графики");
        return false;
    }
    
    gfxConfigureResolution(1280, 720);
    framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);
    if (!framebuffer) {
        logToGraphics("Interface", "Ошибка получения framebuffer");
        return false;
    }
    
    logToGraphics("Interface", "Графика инициализирована: 1280x720");
    
    // Инициализация NeoCore
    if (!g_neoCore.initialize()) {
        logToGraphics("Interface", "NeoCore не инициализирован, продолжаем без него");
    } else {
        logToGraphics("Interface", "NeoCore успешно интегрирован");
    }
    
    // Проверка иконки
    checkForIcon();
    
    logToGraphics("Interface", "Интерфейс NEOVIA готов к работе");
    return true;
}

void SimpleInterface::cleanup() {
    logToGraphics("Interface", "Очистка графического интерфейса...");
    gfxExit();
    logToGraphics("Interface", "Графический интерфейс завершен");
}

void SimpleInterface::checkForIcon() {
    std::ifstream file("icon.jpg");
    hasUserIcon = file.good();
    file.close();
    
    if (hasUserIcon) {
        logToGraphics("Interface", "Пользовательская иконка найдена: icon.jpg");
    } else {
        logToGraphics("Interface", "Пользовательская иконка не найдена, используется стандартная");
    }
}

void SimpleInterface::render() {
    framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);
    
    // Очистка экрана (серый фон как в Switch)
    uint32_t bgColor = Colors::SWITCH_GRAY.toRGBA();
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
    // Три полоски в левом верхнем углу (выделяемые)
    Color menuColor = (selectedItem == 1) ? Colors::BLUE_ACCENT : Colors::BLACK;
    drawText("☰", 50, 50, menuColor, 24);
    
    // Название по центру
    drawText("NEOVIA", 540, 200, Colors::BLACK, 48);
    
    // Кнопка "Улучшить" по центру
    drawButton("Улучшить", 540, 350, 200, 60, selectedItem == 0);
}

void SimpleInterface::renderMenu() {
    // Три полоски в левом верхнем углу
    drawText("☰", 50, 50, Colors::BLUE_ACCENT, 24);
    
    // Логотип и название
    if (hasUserIcon) {
        drawIcon(150, 150, 64);
    } else {
        drawRect(150, 150, 64, 64, Colors::BLUE_ACCENT);
        drawText("N", 175, 175, Colors::WHITE, 32);
    }
    
    drawText("NEOVIA", 230, 170, Colors::BLACK, 32);
    
    // Линия под логотипом
    drawRect(150, 240, 300, 2, Colors::TEXT_GRAY);
    
    // Пункты меню
    drawButton("Настройки", 150, 280, 300, 50, selectedItem == 0);
    drawButton("О нас", 150, 350, 300, 50, selectedItem == 1);
}

void SimpleInterface::renderSettings() {
    drawText("Настройки", 150, 150, Colors::BLACK, 32);
    drawText("Здесь будут настройки приложения", 150, 200, Colors::TEXT_GRAY, 16);
    drawText("Нажмите B для возврата", 150, 600, Colors::TEXT_GRAY, 14);
}

void SimpleInterface::renderAbout() {
    drawText("О нас", 150, 150, Colors::BLACK, 32);
    drawText("NEOVIA v1.0.0", 150, 200, Colors::TEXT_GRAY, 20);
    drawText("Система улучшения графики для Nintendo Switch", 150, 230, Colors::TEXT_GRAY, 16);
    
    // Информация о NeoCore как отдельном движке
    drawText("Powered by NeoCore Engine v1.0.0", 150, 280, Colors::BLUE_ACCENT, 16);
    drawText("Модульный графический движок без разгона", 150, 310, Colors::TEXT_GRAY, 14);
    
    // Статус NeoCore
    std::string status = "NeoCore Engine: ";
    Colors::Color statusColor = Colors::TEXT_GRAY;
    if (g_neoCore.isReady()) {
        status += "Готов";
        statusColor = Colors::GREEN_SUCCESS;
    } else {
        status += "Ошибка инициализации";
        statusColor = Colors::RED_ERROR;
    }
    drawText(status, 150, 340, statusColor, 14);
    
    drawText("Разработчик: Unix228", 150, 380, Colors::TEXT_GRAY, 14);
    drawText("Модульный графический движок без разгона", 150, 410, Colors::TEXT_GRAY, 12);
    drawText("Нажмите B для возврата", 150, 600, Colors::TEXT_GRAY, 14);
}

bool SimpleInterface::handleInput(u64 kDown) {
    // Выход из программы
    if (kDown & (HidNpadButton_Plus | HidNpadButton_Minus)) {
        return false;
    }
    
    switch (currentScreen) {
        case Screen::MAIN:
            if (kDown & HidNpadButton_Up) {
                selectedItem = (selectedItem - 1 + 2) % 2;
            } else if (kDown & HidNpadButton_Down) {
                selectedItem = (selectedItem + 1) % 2;
            } else if (kDown & HidNpadButton_A) {
                if (selectedItem == 0) {
                    onEnhance(); // Улучшить
                } else {
                    currentScreen = Screen::MENU; // Три полоски
                    selectedItem = 0;
                }
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
    logToGraphics("Interface", "Пользователь нажал кнопку 'Улучшить'");
    
    // Запуск улучшения игр через NeoCore
    if (g_neoCore.isReady()) {
        logToGraphics("Interface", "NeoCore готов, запуск улучшения графики...");
        
        // Создаем информацию об игре (пример)
        GameInfo gameInfo;
        gameInfo.gameId = "current_game";
        gameInfo.gameName = "Detected Game";
        gameInfo.hasCustomProfile = false;
        gameInfo.activeMods = {"shadows", "fxaa"};
        
        if (g_neoCore.startCore(gameInfo)) {
            logToGraphics("Interface", "Улучшение графики запущено успешно");
        } else {
            logToGraphics("Interface", "Ошибка запуска улучшения: " + g_neoCore.getLastError());
        }
    } else {
        logToGraphics("Interface", "NeoCore не готов, улучшение недоступно");
    }
}

void SimpleInterface::onSettings() {
    logToGraphics("Interface", "Переход в меню настроек");
    currentScreen = Screen::SETTINGS;
}

void SimpleInterface::onAbout() {
    logToGraphics("Interface", "Переход в меню 'О нас'");
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
    // Простой растровый шрифт 8x8
    static const uint8_t font8x8[128][8] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ' '
        {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // '!'
        {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '"'
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '#'
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '$'
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '%'
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '&'
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '''
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '('
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ')'
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '*'
        {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00}, // '+'
        {0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, 0x00}, // ','
        {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00}, // '-'
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00}, // '.'
        {0x00, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00}, // '/'
        {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00}, // '0'
        {0x18, 0x18, 0x38, 0x18, 0x18, 0x18, 0x7E, 0x00}, // '1'
        {0x3C, 0x66, 0x06, 0x0C, 0x30, 0x60, 0x7E, 0x00}, // '2'
        {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00}, // '3'
        {0x06, 0x0E, 0x1E, 0x66, 0x7F, 0x06, 0x06, 0x00}, // '4'
        {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00}, // '5'
        {0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00}, // '6'
        {0x7E, 0x66, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x00}, // '7'
        {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00}, // '8'
        {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C, 0x00}, // '9'
        {0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00}, // ':'
        {0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x30, 0x00}, // ';'
        {0x0E, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0E, 0x00}, // '<'
        {0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00}, // '='
        {0x70, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x70, 0x00}, // '>'
        {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00}, // '?'
        {0x3C, 0x66, 0x6E, 0x6E, 0x60, 0x62, 0x3C, 0x00}, // '@'
        {0x18, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00}, // 'A'
        {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00}, // 'B'
        {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00}, // 'C'
        {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00}, // 'D'
        {0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7E, 0x00}, // 'E'
        {0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x00}, // 'F'
        {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00}, // 'G'
        {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00}, // 'H'
        {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00}, // 'I'
        {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00}, // 'J'
        {0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00}, // 'K'
        {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00}, // 'L'
        {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00}, // 'M'
        {0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00}, // 'N'
        {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}, // 'O'
        {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00}, // 'P'
        {0x3C, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x0E, 0x00}, // 'Q'
        {0x7C, 0x66, 0x66, 0x7C, 0x78, 0x6C, 0x66, 0x00}, // 'R'
        {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00}, // 'S'
        {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00}, // 'T'
        {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}, // 'U'
        {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00}, // 'V'
        {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // 'W'
        {0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00}, // 'X'
        {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00}, // 'Y'
        {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00}, // 'Z'
    };
    
    int charWidth = size / 2;
    int charHeight = size;
    
    for (size_t i = 0; i < text.length(); i++) {
        unsigned char c = text[i];
        
        // Простая поддержка кириллицы
        if (c >= 192 && c <= 255) {
            // Русские буквы - используем латинские аналоги
            static const char cyrillicMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            c = cyrillicMap[(c - 192) % 26];
        } else if (c < 32 || c > 126) {
            c = '?';
        }
        
        const uint8_t* fontData = font8x8[c];
        
        for (int py = 0; py < 8; py++) {
            uint8_t row = fontData[py];
            for (int px = 0; px < 8; px++) {
                if (row & (1 << (7 - px))) {
                    // Масштабирование пикселя
                    for (int sy = 0; sy < charHeight / 8; sy++) {
                        for (int sx = 0; sx < charWidth / 8; sx++) {
                            int drawX = x + i * charWidth + px * (charWidth / 8) + sx;
                            int drawY = y + py * (charHeight / 8) + sy;
                            if (drawX >= 0 && drawX < (int)width && drawY >= 0 && drawY < (int)height) {
                                framebuffer[drawY * width + drawX] = color.toRGBA();
                            }
                        }
                    }
                }
            }
        }
    }
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