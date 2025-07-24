#pragma once
#include <switch.h>
#include <vector>
#include <string>
#include <memory>

// Nintendo Switch цветовая схема (точные цвета)
struct SwitchColor {
    uint8_t r, g, b, a;
    
    SwitchColor(uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255, uint8_t alpha = 255) 
        : r(red), g(green), b(blue), a(alpha) {}
    
    uint32_t toRGBA() const {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }
};

// Оригинальные цвета Nintendo Switch
namespace SwitchColors {
    const SwitchColor WHITE(255, 255, 255);
    const SwitchColor BLACK(0, 0, 0);
    const SwitchColor GRAY_LIGHT(235, 235, 235);
    const SwitchColor GRAY_MEDIUM(200, 200, 200);
    const SwitchColor GRAY_DARK(80, 80, 80);
    const SwitchColor BLUE_ACCENT(0, 122, 255);        // Системный синий
    const SwitchColor GREEN_SUCCESS(52, 199, 89);      // Системный зеленый
    const SwitchColor RED_ERROR(255, 59, 48);          // Системный красный
    const SwitchColor ORANGE_WARNING(255, 149, 0);     // Системный оранжевый
    const SwitchColor BACKGROUND_LIGHT(245, 245, 245); // Светлый фон
    const SwitchColor BACKGROUND_DARK(45, 45, 45);     // Темный фон
    const SwitchColor TEXT_PRIMARY(0, 0, 0);           // Основной текст
    const SwitchColor TEXT_SECONDARY(128, 128, 128);   // Вторичный текст
}

// Простая анимация без эффектов
struct SimpleAnimation {
    float duration;
    float elapsed;
    float startValue;
    float endValue;
    bool active;
    
    SimpleAnimation(float dur = 0.2f) : duration(dur), elapsed(0), startValue(0), endValue(0), active(false) {}
    
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
        
        // Простая линейная интерполяция (как в Switch)
        float t = elapsed / duration;
        return startValue + (endValue - startValue) * t;
    }
    
    bool isActive() const { return active; }
};

// Базовый элемент UI в стиле Switch
class SwitchUIElement {
public:
    float x, y, width, height;
    SwitchColor backgroundColor;
    bool visible;
    bool selected;
    SimpleAnimation fadeAnimation;
    
    SwitchUIElement(float x = 0, float y = 0, float w = 100, float h = 50) 
        : x(x), y(y), width(w), height(h), 
          backgroundColor(SwitchColors::WHITE), visible(true), selected(false) {}
    
    virtual ~SwitchUIElement() = default;
    virtual void render(float deltaTime) = 0;
    virtual void update(float deltaTime) {}
    virtual bool handleInput(u64 kDown) { return false; }
    
    bool isPointInside(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

// Кнопка в стиле Switch (плоская, минималистичная)
class SwitchButton : public SwitchUIElement {
public:
    std::string text;
    SwitchColor textColor;
    SwitchColor borderColor;
    bool pressed;
    SimpleAnimation pressAnimation;
    std::function<void()> onClick;
    
    SwitchButton(const std::string& txt, float x = 0, float y = 0, float w = 200, float h = 60);
    void render(float deltaTime) override;
    void update(float deltaTime) override;
    bool handleInput(u64 kDown) override;
};

// Панель в стиле Switch (простая, чистая)
class SwitchPanel : public SwitchUIElement {
public:
    std::vector<std::unique_ptr<SwitchUIElement>> children;
    SwitchColor borderColor;
    float borderWidth;
    float cornerRadius;
    
    SwitchPanel(float x = 0, float y = 0, float w = 400, float h = 300);
    void render(float deltaTime) override;
    void update(float deltaTime) override;
    bool handleInput(u64 kDown) override;
    void addChild(std::unique_ptr<SwitchUIElement> child);
};

// Текст в стиле Switch
class SwitchText : public SwitchUIElement {
public:
    std::string text;
    SwitchColor textColor;
    int fontSize;
    bool centered;
    
    SwitchText(const std::string& txt, float x = 0, float y = 0);
    void render(float deltaTime) override;
};

// Иконка в стиле Switch
class SwitchIcon : public SwitchUIElement {
public:
    std::string iconName;
    SwitchColor tintColor;
    float scale;
    
    SwitchIcon(const std::string& name, float x = 0, float y = 0, float size = 48);
    void render(float deltaTime) override;
};

// Менеджер графики в стиле Switch
class SwitchGraphics {
private:
    static SwitchGraphics* instance;
    uint32_t* framebuffer;
    uint32_t width, height;
    float lastFrameTime;
    
public:
    static SwitchGraphics* getInstance();
    
    bool initialize();
    void cleanup();
    
    void beginFrame();
    void endFrame();
    float getDeltaTime() const { return lastFrameTime; }
    
    // Базовые примитивы (без эффектов, как в Switch)
    void drawPixel(int x, int y, const SwitchColor& color);
    void drawLine(int x1, int y1, int x2, int y2, const SwitchColor& color);
    void drawRect(float x, float y, float width, float height, const SwitchColor& color);
    void drawRoundedRect(float x, float y, float width, float height, float radius, const SwitchColor& color);
    void drawCircle(float x, float y, float radius, const SwitchColor& color);
    
    // Текст (простой, как в Switch)
    void drawText(const std::string& text, float x, float y, const SwitchColor& color, int fontSize = 16);
    void drawTextCentered(const std::string& text, float x, float y, float width, const SwitchColor& color, int fontSize = 16);
    
    // Иконки
    void drawIcon(const std::string& iconName, float x, float y, float size = 48, const SwitchColor& tint = SwitchColors::BLACK);
    
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
};

#define SWITCH_GFX SwitchGraphics::getInstance()