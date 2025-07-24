#pragma once
#include <switch.h>
#include <string>
#include <functional>

// Простые цвета Switch
struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255, uint8_t alpha = 255) 
        : r(red), g(green), b(blue), a(alpha) {}
    uint32_t toRGBA() const { return (r << 24) | (g << 16) | (b << 8) | a; }
};

namespace Colors {
    const Color WHITE(255, 255, 255);
    const Color BLACK(0, 0, 0);
    const Color GRAY_LIGHT(240, 240, 240);
    const Color BLUE_ACCENT(0, 122, 255);
    const Color TEXT_GRAY(128, 128, 128);
}

enum class Screen {
    MAIN,
    MENU,
    SETTINGS,
    ABOUT
};

class SimpleInterface {
private:
    Screen currentScreen;
    int selectedItem;
    bool hasUserIcon;
    
    // Графика
    uint32_t* framebuffer;
    uint32_t width, height;
    
public:
    SimpleInterface();
    ~SimpleInterface();
    
    bool initialize();
    void cleanup();
    
    void render();
    bool handleInput(u64 kDown);
    
private:
    void renderMainScreen();
    void renderMenu();
    void renderSettings();
    void renderAbout();
    
    void drawRect(float x, float y, float w, float h, const Color& color);
    void drawText(const std::string& text, float x, float y, const Color& color, int size = 16);
    void drawButton(const std::string& text, float x, float y, float w, float h, bool selected = false);
    void drawIcon(float x, float y, float size);
    
    void checkForIcon();
    void onEnhance();
    void onSettings();
    void onAbout();
};