#pragma once
#include <switch.h>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <cmath>

// Цветовая схема NEOVIA
struct Color {
    uint8_t r, g, b, a;
    
    Color(uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255, uint8_t alpha = 255) 
        : r(red), g(green), b(blue), a(alpha) {}
    
    uint32_t toRGBA() const {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }
};

// Цветовая палитра
namespace Colors {
    const Color PRIMARY(64, 128, 255);      // Синий
    const Color SECONDARY(128, 64, 255);    // Фиолетовый  
    const Color ACCENT(255, 64, 128);       // Розовый
    const Color SUCCESS(64, 255, 128);      // Зеленый
    const Color WARNING(255, 192, 64);      // Оранжевый
    const Color ERROR(255, 64, 64);         // Красный
    const Color BACKGROUND(18, 18, 24);     // Темный фон
    const Color SURFACE(28, 28, 36);        // Поверхности
    const Color TEXT(240, 240, 245);        // Основной текст
    const Color TEXT_SECONDARY(180, 180, 190); // Вторичный текст
    const Color WHITE(255, 255, 255);
    const Color BLACK(0, 0, 0);
    const Color TRANSPARENT(0, 0, 0, 0);
}

// Анимация
struct Animation {
    float duration;
    float elapsed;
    float startValue;
    float endValue;
    bool active;
    
    Animation(float dur = 0.3f) : duration(dur), elapsed(0), startValue(0), endValue(0), active(false) {}
    
    void start(float from, float to) {
        startValue = from;
        endValue = to;
        elapsed = 0;
        active = true;
    }
    
    float update(float deltaTime) {
        if (!active) return endValue;
        
        elapsed += deltaTime;
        if (elapsed >= duration) {
            active = false;
            return endValue;
        }
        
        // Easing функция (ease-out-cubic)
        float t = elapsed / duration;
        t = 1 - pow(1 - t, 3);
        return startValue + (endValue - startValue) * t;
    }
    
    bool isActive() const { return active; }
};

// Базовый элемент UI
class UIElement {
public:
    float x, y, width, height;
    Color backgroundColor;
    Color borderColor;
    float borderWidth;
    float cornerRadius;
    bool visible;
    Animation fadeAnimation;
    Animation scaleAnimation;
    
    UIElement(float x = 0, float y = 0, float w = 100, float h = 50) 
        : x(x), y(y), width(w), height(h), 
          backgroundColor(Colors::SURFACE), borderColor(Colors::PRIMARY),
          borderWidth(0), cornerRadius(8), visible(true) {}
    
    virtual ~UIElement() = default;
    virtual void render(float deltaTime) = 0;
    virtual void update(float deltaTime) {}
    virtual bool handleInput(u64 kDown, float touchX = -1, float touchY = -1) { return false; }
    
    bool isPointInside(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

// Кнопка с анимацией
class Button : public UIElement {
public:
    std::string text;
    Color textColor;
    bool pressed;
    bool hovered;
    Animation pressAnimation;
    Animation hoverAnimation;
    std::function<void()> onClick;
    
    Button(const std::string& txt, float x = 0, float y = 0, float w = 200, float h = 60);
    void render(float deltaTime) override;
    void update(float deltaTime) override;
    bool handleInput(u64 kDown, float touchX = -1, float touchY = -1) override;
};

// Панель с градиентом
class Panel : public UIElement {
public:
    Color gradientStart;
    Color gradientEnd;
    bool useGradient;
    std::vector<std::unique_ptr<UIElement>> children;
    
    Panel(float x = 0, float y = 0, float w = 400, float h = 300);
    void render(float deltaTime) override;
    void update(float deltaTime) override;
    bool handleInput(u64 kDown, float touchX = -1, float touchY = -1) override;
    void addChild(std::unique_ptr<UIElement> child);
};

// Текстовый элемент
class Label : public UIElement {
public:
    std::string text;
    Color textColor;
    int fontSize;
    bool centered;
    
    Label(const std::string& txt, float x = 0, float y = 0);
    void render(float deltaTime) override;
};

// Прогресс бар с анимацией
class ProgressBar : public UIElement {
public:
    float progress; // 0.0 - 1.0
    Color fillColor;
    Color backgroundColor;
    Animation progressAnimation;
    bool showPercentage;
    
    ProgressBar(float x = 0, float y = 0, float w = 300, float h = 20);
    void render(float deltaTime) override;
    void update(float deltaTime) override;
    void setProgress(float value, bool animate = true);
};

// Менеджер графики
class GraphicsManager {
private:
    static GraphicsManager* instance;
    uint32_t* framebuffer;
    uint32_t width, height;
    float lastFrameTime;
    
public:
    static GraphicsManager* getInstance();
    
    bool initialize();
    void cleanup();
    
    void beginFrame();
    void endFrame();
    float getDeltaTime() const { return lastFrameTime; }
    
    // Базовые примитивы
    void drawPixel(int x, int y, const Color& color);
    void drawLine(int x1, int y1, int x2, int y2, const Color& color, float thickness = 1.0f);
    void drawRect(float x, float y, float width, float height, const Color& color);
    void drawRoundedRect(float x, float y, float width, float height, float radius, const Color& color);
    void drawCircle(float x, float y, float radius, const Color& color);
    void drawGradient(float x, float y, float width, float height, const Color& startColor, const Color& endColor, bool vertical = true);
    
    // Текст
    void drawText(const std::string& text, float x, float y, const Color& color, int fontSize = 16);
    void drawTextCentered(const std::string& text, float x, float y, float width, const Color& color, int fontSize = 16);
    
    // Эффекты
    void drawShadow(float x, float y, float width, float height, float radius = 8, float opacity = 0.3f);
    void drawGlow(float x, float y, float width, float height, const Color& color, float intensity = 0.5f);
    
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
};

// Макросы для удобства
#define GFX GraphicsManager::getInstance()