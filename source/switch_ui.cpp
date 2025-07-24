#include "switch_ui.h"
#include <cmath>
#include <cstring>

SwitchGraphics* SwitchGraphics::instance = nullptr;

SwitchGraphics* SwitchGraphics::getInstance() {
    if (!instance) {
        instance = new SwitchGraphics();
    }
    return instance;
}

bool SwitchGraphics::initialize() {
    // Инициализация framebuffer для Switch
    Result rc = gfxInitDefault();
    if (R_FAILED(rc)) return false;
    
    gfxConfigureResolution(1280, 720);
    
    framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);
    if (!framebuffer) return false;
    
    lastFrameTime = 1.0f / 60.0f; // 60 FPS по умолчанию
    return true;
}

void SwitchGraphics::cleanup() {
    gfxExit();
}

void SwitchGraphics::beginFrame() {
    framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);
    
    // Очистка экрана (светлый фон как в Switch)
    uint32_t bgColor = SwitchColors::BACKGROUND_LIGHT.toRGBA();
    for (uint32_t i = 0; i < width * height; i++) {
        framebuffer[i] = bgColor;
    }
}

void SwitchGraphics::endFrame() {
    gfxFlushBuffers();
    gfxSwapBuffers();
    gfxWaitForVsync();
}

void SwitchGraphics::drawPixel(int x, int y, const SwitchColor& color) {
    if (x >= 0 && x < (int)width && y >= 0 && y < (int)height) {
        framebuffer[y * width + x] = color.toRGBA();
    }
}

void SwitchGraphics::drawLine(int x1, int y1, int x2, int y2, const SwitchColor& color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        drawPixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void SwitchGraphics::drawRect(float x, float y, float width, float height, const SwitchColor& color) {
    int x1 = (int)x;
    int y1 = (int)y;
    int x2 = (int)(x + width);
    int y2 = (int)(y + height);
    
    for (int py = y1; py < y2; py++) {
        for (int px = x1; px < x2; px++) {
            drawPixel(px, py, color);
        }
    }
}

void SwitchGraphics::drawRoundedRect(float x, float y, float width, float height, float radius, const SwitchColor& color) {
    // Простые скругленные углы (как в Switch)
    int r = (int)radius;
    
    // Основной прямоугольник
    drawRect(x + r, y, width - 2 * r, height, color);
    drawRect(x, y + r, width, height - 2 * r, color);
    
    // Углы (упрощенные)
    for (int dy = 0; dy < r; dy++) {
        for (int dx = 0; dx < r; dx++) {
            float dist = sqrt(dx * dx + dy * dy);
            if (dist <= r) {
                // Верхний левый
                drawPixel(x + r - dx, y + r - dy, color);
                // Верхний правый
                drawPixel(x + width - r + dx, y + r - dy, color);
                // Нижний левый
                drawPixel(x + r - dx, y + height - r + dy, color);
                // Нижний правый
                drawPixel(x + width - r + dx, y + height - r + dy, color);
            }
        }
    }
}

void SwitchGraphics::drawCircle(float x, float y, float radius, const SwitchColor& color) {
    int r = (int)radius;
    int cx = (int)x;
    int cy = (int)y;
    
    for (int dy = -r; dy <= r; dy++) {
        for (int dx = -r; dx <= r; dx++) {
            if (dx * dx + dy * dy <= r * r) {
                drawPixel(cx + dx, cy + dy, color);
            }
        }
    }
}

// Простой растровый шрифт для Switch
static const uint8_t switchFont8x8[128][8] = {
    // Упрощенный набор символов
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ' '
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // '!'
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '"'
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // '#'
    // ... (добавим основные символы)
    {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00}, // 'O'
    // ... остальные символы
};

void SwitchGraphics::drawText(const std::string& text, float x, float y, const SwitchColor& color, int fontSize) {
    int charWidth = fontSize / 2;
    int charHeight = fontSize;
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        if (c < 32 || c > 126) c = '?';
        
        // Простое отображение символов
        const uint8_t* fontData = switchFont8x8[c];
        
        for (int py = 0; py < 8; py++) {
            uint8_t row = fontData[py];
            for (int px = 0; px < 8; px++) {
                if (row & (1 << (7 - px))) {
                    // Масштабирование
                    for (int sy = 0; sy < charHeight / 8; sy++) {
                        for (int sx = 0; sx < charWidth / 8; sx++) {
                            drawPixel(
                                x + i * charWidth + px * (charWidth / 8) + sx,
                                y + py * (charHeight / 8) + sy,
                                color
                            );
                        }
                    }
                }
            }
        }
    }
}

void SwitchGraphics::drawTextCentered(const std::string& text, float x, float y, float width, const SwitchColor& color, int fontSize) {
    float textWidth = text.length() * (fontSize / 2);
    float startX = x + (width - textWidth) / 2;
    drawText(text, startX, y, color, fontSize);
}

void SwitchGraphics::drawIcon(const std::string& iconName, float x, float y, float size, const SwitchColor& tint) {
    // Простые встроенные иконки в стиле Switch
    if (iconName == "settings") {
        // Иконка настроек (шестеренка)
        drawCircle(x + size/2, y + size/2, size/3, tint);
        drawCircle(x + size/2, y + size/2, size/6, SwitchColors::BACKGROUND_LIGHT);
    } else if (iconName == "home") {
        // Иконка дома
        drawLine(x + size/2, y + size/4, x + size/4, y + size/2, tint);
        drawLine(x + size/2, y + size/4, x + 3*size/4, y + size/2, tint);
        drawRect(x + size/3, y + size/2, size/3, size/2, tint);
    } else if (iconName == "user_icon" && iconName != "") {
        // Пользовательская иконка (если есть)
        drawRoundedRect(x, y, size, size, size/8, SwitchColors::BLUE_ACCENT);
        drawTextCentered("USER", x, y + size/2 - 8, size, SwitchColors::WHITE, 16);
    } else {
        // Дефолтная иконка приложения
        drawRoundedRect(x, y, size, size, size/8, SwitchColors::GRAY_MEDIUM);
        drawTextCentered("APP", x, y + size/2 - 8, size, SwitchColors::WHITE, 16);
    }
}

// Реализация SwitchButton
SwitchButton::SwitchButton(const std::string& txt, float x, float y, float w, float h) 
    : SwitchUIElement(x, y, w, h), text(txt), pressed(false) {
    backgroundColor = SwitchColors::WHITE;
    textColor = SwitchColors::TEXT_PRIMARY;
    borderColor = SwitchColors::GRAY_MEDIUM;
}

void SwitchButton::render(float deltaTime) {
    if (!visible) return;
    
    float pressOffset = pressAnimation.update(deltaTime);
    
    // Фон кнопки (простой, как в Switch)
    SwitchColor bgColor = backgroundColor;
    if (selected) {
        bgColor = SwitchColors::BLUE_ACCENT;
        textColor = SwitchColors::WHITE;
    } else if (pressed) {
        bgColor = SwitchColors::GRAY_LIGHT;
    }
    
    // Рисуем кнопку
    SWITCH_GFX->drawRoundedRect(x, y + pressOffset, width, height, 8, bgColor);
    
    // Граница
    SWITCH_GFX->drawRoundedRect(x - 1, y + pressOffset - 1, width + 2, height + 2, 9, borderColor);
    
    // Текст
    SWITCH_GFX->drawTextCentered(text, x, y + height/2 - 8 + pressOffset, width, textColor, 16);
}

void SwitchButton::update(float deltaTime) {
    pressAnimation.update(deltaTime);
}

bool SwitchButton::handleInput(u64 kDown) {
    if (kDown & HidNpadButton_A && selected) {
        pressed = true;
        pressAnimation.start(0, 2);
        if (onClick) onClick();
        return true;
    }
    return false;
}

// Реализация SwitchPanel
SwitchPanel::SwitchPanel(float x, float y, float w, float h) 
    : SwitchUIElement(x, y, w, h), borderWidth(1), cornerRadius(12) {
    backgroundColor = SwitchColors::WHITE;
    borderColor = SwitchColors::GRAY_MEDIUM;
}

void SwitchPanel::render(float deltaTime) {
    if (!visible) return;
    
    // Фон панели
    SWITCH_GFX->drawRoundedRect(x, y, width, height, cornerRadius, backgroundColor);
    
    // Граница
    if (borderWidth > 0) {
        SWITCH_GFX->drawRoundedRect(x - borderWidth, y - borderWidth, 
                                   width + 2*borderWidth, height + 2*borderWidth, 
                                   cornerRadius + borderWidth, borderColor);
    }
    
    // Дочерние элементы
    for (auto& child : children) {
        child->render(deltaTime);
    }
}

void SwitchPanel::update(float deltaTime) {
    for (auto& child : children) {
        child->update(deltaTime);
    }
}

bool SwitchPanel::handleInput(u64 kDown) {
    for (auto& child : children) {
        if (child->handleInput(kDown)) {
            return true;
        }
    }
    return false;
}

void SwitchPanel::addChild(std::unique_ptr<SwitchUIElement> child) {
    children.push_back(std::move(child));
}

// Реализация SwitchText
SwitchText::SwitchText(const std::string& txt, float x, float y) 
    : SwitchUIElement(x, y, 0, 0), text(txt), fontSize(16), centered(false) {
    textColor = SwitchColors::TEXT_PRIMARY;
}

void SwitchText::render(float deltaTime) {
    if (!visible) return;
    
    if (centered) {
        SWITCH_GFX->drawTextCentered(text, x, y, width, textColor, fontSize);
    } else {
        SWITCH_GFX->drawText(text, x, y, textColor, fontSize);
    }
}

// Реализация SwitchIcon
SwitchIcon::SwitchIcon(const std::string& name, float x, float y, float size) 
    : SwitchUIElement(x, y, size, size), iconName(name), scale(1.0f) {
    tintColor = SwitchColors::BLACK;
}

void SwitchIcon::render(float deltaTime) {
    if (!visible) return;
    
    SWITCH_GFX->drawIcon(iconName, x, y, width * scale, tintColor);
}