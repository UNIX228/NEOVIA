#include "graphics.h"
#include <cmath>
#include <algorithm>
#include <cstring>
#include <functional>

GraphicsManager* GraphicsManager::instance = nullptr;

GraphicsManager* GraphicsManager::getInstance() {
    if (!instance) {
        instance = new GraphicsManager();
    }
    return instance;
}

bool GraphicsManager::initialize() {
    gfxInitDefault();
    width = 1280;
    height = 720;
    framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);
    lastFrameTime = 0.016f; // 60 FPS по умолчанию
    return framebuffer != nullptr;
}

void GraphicsManager::cleanup() {
    gfxExit();
}

void GraphicsManager::beginFrame() {
    static u64 lastTime = armGetSystemTick();
    u64 currentTime = armGetSystemTick();
    lastFrameTime = (float)(currentTime - lastTime) / armGetSystemTickFreq();
    lastTime = currentTime;
    
    framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);
    
    // Очистка экрана с градиентом
    drawGradient(0, 0, width, height, Colors::BACKGROUND, Color(12, 12, 18), true);
}

void GraphicsManager::endFrame() {
    gfxFlushBuffers();
    gfxSwapBuffers();
    gfxWaitForVsync();
}

void GraphicsManager::drawPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= (int)width || y < 0 || y >= (int)height || !framebuffer) return;
    
    uint32_t* pixel = framebuffer + y * width + x;
    if (color.a == 255) {
        *pixel = color.toRGBA();
    } else {
        // Alpha blending
        uint32_t existing = *pixel;
        uint8_t er = (existing >> 24) & 0xFF;
        uint8_t eg = (existing >> 16) & 0xFF;
        uint8_t eb = (existing >> 8) & 0xFF;
        
        float alpha = color.a / 255.0f;
        uint8_t nr = (uint8_t)(color.r * alpha + er * (1.0f - alpha));
        uint8_t ng = (uint8_t)(color.g * alpha + eg * (1.0f - alpha));
        uint8_t nb = (uint8_t)(color.b * alpha + eb * (1.0f - alpha));
        
        *pixel = (nr << 24) | (ng << 16) | (nb << 8) | 0xFF;
    }
}

void GraphicsManager::drawLine(int x1, int y1, int x2, int y2, const Color& color, float thickness) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    int x = x1, y = y1;
    int halfThickness = (int)(thickness / 2);
    
    while (true) {
        // Рисуем толстую линию
        for (int i = -halfThickness; i <= halfThickness; i++) {
            for (int j = -halfThickness; j <= halfThickness; j++) {
                drawPixel(x + i, y + j, color);
            }
        }
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void GraphicsManager::drawRect(float x, float y, float width, float height, const Color& color) {
    int ix = (int)x, iy = (int)y;
    int iw = (int)width, ih = (int)height;
    
    for (int py = iy; py < iy + ih; py++) {
        for (int px = ix; px < ix + iw; px++) {
            drawPixel(px, py, color);
        }
    }
}

void GraphicsManager::drawRoundedRect(float x, float y, float width, float height, float radius, const Color& color) {
    int ix = (int)x, iy = (int)y;
    int iw = (int)width, ih = (int)height;
    int ir = (int)radius;
    
    // Основной прямоугольник
    drawRect(x + radius, y, width - 2 * radius, height, color);
    drawRect(x, y + radius, width, height - 2 * radius, color);
    
    // Углы
    for (int py = 0; py < ir; py++) {
        for (int px = 0; px < ir; px++) {
            float dist = sqrt((px - ir) * (px - ir) + (py - ir) * (py - ir));
            if (dist <= ir) {
                // Верхний левый
                drawPixel(ix + px, iy + py, color);
                // Верхний правый
                drawPixel(ix + iw - ir + px, iy + py, color);
                // Нижний левый
                drawPixel(ix + px, iy + ih - ir + py, color);
                // Нижний правый
                drawPixel(ix + iw - ir + px, iy + ih - ir + py, color);
            }
        }
    }
}

void GraphicsManager::drawCircle(float x, float y, float radius, const Color& color) {
    int ix = (int)x, iy = (int)y;
    int ir = (int)radius;
    
    for (int py = -ir; py <= ir; py++) {
        for (int px = -ir; px <= ir; px++) {
            if (px * px + py * py <= ir * ir) {
                drawPixel(ix + px, iy + py, color);
            }
        }
    }
}

void GraphicsManager::drawGradient(float x, float y, float width, float height, 
                                 const Color& startColor, const Color& endColor, bool vertical) {
    int ix = (int)x, iy = (int)y;
    int iw = (int)width, ih = (int)height;
    
    for (int py = 0; py < ih; py++) {
        for (int px = 0; px < iw; px++) {
            float t = vertical ? (float)py / ih : (float)px / iw;
            
            uint8_t r = (uint8_t)(startColor.r + t * (endColor.r - startColor.r));
            uint8_t g = (uint8_t)(startColor.g + t * (endColor.g - startColor.g));
            uint8_t b = (uint8_t)(startColor.b + t * (endColor.b - startColor.b));
            uint8_t a = (uint8_t)(startColor.a + t * (endColor.a - startColor.a));
            
            drawPixel(ix + px, iy + py, Color(r, g, b, a));
        }
    }
}

// drawText реализован в font_renderer.cpp

void GraphicsManager::drawTextCentered(const std::string& text, float x, float y, float width, const Color& color, int fontSize) {
    float textWidth = text.length() * (fontSize / 2);
    float startX = x + (width - textWidth) / 2;
    drawText(text, startX, y, color, fontSize);
}

void GraphicsManager::drawShadow(float x, float y, float width, float height, float radius, float opacity) {
    Color shadowColor(0, 0, 0, (uint8_t)(255 * opacity));
    
    // Рисуем размытую тень
    for (int offset = 0; offset < (int)radius; offset++) {
        uint8_t alpha = (uint8_t)(255 * opacity * (1.0f - (float)offset / radius));
        Color blurColor(0, 0, 0, alpha);
        drawRoundedRect(x + offset, y + offset, width, height, 8, blurColor);
    }
}

void GraphicsManager::drawGlow(float x, float y, float width, float height, const Color& color, float intensity) {
    for (int offset = 5; offset > 0; offset--) {
        uint8_t alpha = (uint8_t)(255 * intensity * (float)offset / 5.0f);
        Color glowColor(color.r, color.g, color.b, alpha);
        drawRoundedRect(x - offset, y - offset, width + 2 * offset, height + 2 * offset, 8 + offset, glowColor);
    }
}

// Реализация Button
Button::Button(const std::string& txt, float x, float y, float w, float h) 
    : UIElement(x, y, w, h), text(txt), textColor(Colors::TEXT), pressed(false), hovered(false) {
    backgroundColor = Colors::PRIMARY;
    cornerRadius = 12;
}

// Button::render реализован в ui_effects.cpp

void Button::update(float deltaTime) {
    pressAnimation.update(deltaTime);
    hoverAnimation.update(deltaTime);
    scaleAnimation.update(deltaTime);
}

bool Button::handleInput(u64 kDown, float touchX, float touchY) {
    bool wasPressed = pressed;
    
    if (touchX >= 0 && touchY >= 0) {
        hovered = isPointInside(touchX, touchY);
        if (hovered && (kDown & HidNpadButton_A)) {
            pressed = true;
            pressAnimation.start(0, 1);
            if (onClick) onClick();
            return true;
        }
    } else {
        if (kDown & HidNpadButton_A) {
            pressed = true;
            pressAnimation.start(0, 1);
            if (onClick) onClick();
            return true;
        }
    }
    
    if (wasPressed && !pressed) {
        pressAnimation.start(1, 0);
    }
    
    return false;
}

// Реализация Panel
Panel::Panel(float x, float y, float w, float h) 
    : UIElement(x, y, w, h), useGradient(true) {
    gradientStart = Colors::SURFACE;
    gradientEnd = Color(22, 22, 30);
    cornerRadius = 16;
}

void Panel::render(float deltaTime) {
    if (!visible) return;
    
    // Тень панели
    GFX->drawShadow(x + 4, y + 8, width, height, 12, 0.4f);
    
    // Фон панели
    if (useGradient) {
        GFX->drawGradient(x, y, width, height, gradientStart, gradientEnd, true);
    } else {
        GFX->drawRoundedRect(x, y, width, height, cornerRadius, backgroundColor);
    }
    
    // Граница
    if (borderWidth > 0) {
        // Рисуем границу как несколько прямоугольников
        Color borderGlow(borderColor.r, borderColor.g, borderColor.b, 100);
        GFX->drawRoundedRect(x - 1, y - 1, width + 2, height + 2, cornerRadius + 1, borderGlow);
    }
    
    // Дочерние элементы
    for (auto& child : children) {
        child->render(deltaTime);
    }
}

void Panel::update(float deltaTime) {
    for (auto& child : children) {
        child->update(deltaTime);
    }
}

bool Panel::handleInput(u64 kDown, float touchX, float touchY) {
    for (auto& child : children) {
        if (child->handleInput(kDown, touchX, touchY)) {
            return true;
        }
    }
    return false;
}

void Panel::addChild(std::unique_ptr<UIElement> child) {
    children.push_back(std::move(child));
}

// Реализация Label
Label::Label(const std::string& txt, float x, float y) 
    : UIElement(x, y, 0, 0), text(txt), textColor(Colors::TEXT), fontSize(16), centered(false) {
    backgroundColor = Colors::TRANSPARENT;
}

void Label::render(float deltaTime) {
    if (!visible) return;
    
    if (centered) {
        GFX->drawTextCentered(text, x, y, width, textColor, fontSize);
    } else {
        GFX->drawText(text, x, y, textColor, fontSize);
    }
}

// Реализация ProgressBar
ProgressBar::ProgressBar(float x, float y, float w, float h) 
    : UIElement(x, y, w, h), progress(0), showPercentage(true) {
    fillColor = Colors::PRIMARY;
    backgroundColor = Colors::SURFACE;
    cornerRadius = h / 2;
}

void ProgressBar::render(float deltaTime) {
    if (!visible) return;
    
    float animatedProgress = progressAnimation.update(deltaTime);
    
    // Фон
    GFX->drawRoundedRect(x, y, width, height, cornerRadius, backgroundColor);
    
    // Заполнение
    if (animatedProgress > 0) {
        float fillWidth = width * animatedProgress;
        GFX->drawRoundedRect(x, y, fillWidth, height, cornerRadius, fillColor);
        
        // Свечение заполнения
        GFX->drawGlow(x, y, fillWidth, height, fillColor, 0.3f);
    }
    
    // Процент
    if (showPercentage) {
        char percentText[16];
        snprintf(percentText, sizeof(percentText), "%.0f%%", animatedProgress * 100);
        GFX->drawTextCentered(percentText, x, y + height/2 - 8, width, Colors::TEXT, 14);
    }
}

void ProgressBar::update(float deltaTime) {
    progressAnimation.update(deltaTime);
}

void ProgressBar::setProgress(float value, bool animate) {
    value = std::max(0.0f, std::min(1.0f, value));
    
    if (animate) {
        progressAnimation.start(progress, value);
    } else {
        progressAnimation.start(value, value);
        progressAnimation.active = false;
    }
    
    progress = value;
}