#pragma once

#include <switch.h>
#include <deko3d.hpp>
#include <vector>
#include <memory>
#include <string>
#include <functional>

// Цвета интерфейса
#define COLOR_BACKGROUND    0xFF424242  // Серый фон
#define COLOR_PANEL         0xFF1A1A1A  // Черные панели
#define COLOR_TEXT_WHITE    0xFFFFFFFF  // Белый текст
#define COLOR_TEXT_GRAY     0xFFAAAAAA  // Серый текст
#define COLOR_BUTTON        0xFF0078D4  // Синяя кнопка
#define COLOR_BUTTON_HOVER  0xFF106EBE  // Синяя кнопка при наведении

// Размеры экрана
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

// Размеры элементов
#define SIDEBAR_WIDTH     80
#define BOTTOM_PANEL_HEIGHT 60
#define BUTTON_WIDTH      200
#define BUTTON_HEIGHT     50

// Структуры для UI элементов
struct Point {
    float x, y;
};

struct Size {
    float width, height;
};

struct Rect {
    float x, y, width, height;
};

struct Color {
    float r, g, b, a;
    
    Color(uint32_t rgba) {
        a = ((rgba >> 24) & 0xFF) / 255.0f;
        r = ((rgba >> 16) & 0xFF) / 255.0f;
        g = ((rgba >> 8) & 0xFF) / 255.0f;
        b = (rgba & 0xFF) / 255.0f;
    }
};

// Типы UI элементов
enum class UIElementType {
    Panel,
    Button,
    Text,
    Icon
};

// Базовый класс UI элемента
class UIElement {
public:
    UIElementType type;
    Rect bounds;
    Color color;
    bool visible;
    bool interactive;
    
    UIElement(UIElementType t, Rect b, Color c) 
        : type(t), bounds(b), color(c), visible(true), interactive(false) {}
    
    virtual ~UIElement() = default;
    virtual void render() = 0;
    virtual bool handleInput(u64 kDown, Point touchPos) { return false; }
};

// Панель
class Panel : public UIElement {
public:
    Panel(Rect bounds, Color color) 
        : UIElement(UIElementType::Panel, bounds, color) {}
    
    void render() override;
};

// Кнопка
class Button : public UIElement {
private:
    std::string text;
    bool hovered;
    std::function<void()> onClick;
    
public:
    Button(Rect bounds, const std::string& text, std::function<void()> callback)
        : UIElement(UIElementType::Button, bounds, Color(COLOR_BUTTON))
        , text(text), hovered(false), onClick(callback) {
        interactive = true;
    }
    
    void render() override;
    bool handleInput(u64 kDown, Point touchPos) override;
    void setHovered(bool hover) { hovered = hover; }
};

// Текст
class TextElement : public UIElement {
private:
    std::string text;
    float fontSize;
    
public:
    TextElement(Point pos, const std::string& text, float size = 24.0f)
        : UIElement(UIElementType::Text, {pos.x, pos.y, 0, 0}, Color(COLOR_TEXT_WHITE))
        , text(text), fontSize(size) {}
    
    void render() override;
    void setText(const std::string& newText) { text = newText; }
};

// Главный класс графической системы
class GraphicsSystem {
private:
    dk::UniqueDevice device;
    dk::UniqueQueue queue;
    dk::UniqueMemBlock depthBuffer;
    dk::UniqueMemBlock fbMemBlock;
    dk::UniqueMemBlock cmdMemBlock;
    dk::UniqueCmdBuf cmdbuf;
    
    dk::UniqueSwapchain swapchain;
    DkCmdList cmdlist;
    
    std::vector<std::unique_ptr<UIElement>> elements;
    
    bool initialized;
    
public:
    GraphicsSystem();
    ~GraphicsSystem();
    
    bool initialize();
    void shutdown();
    
    void beginFrame();
    void endFrame();
    void present();
    
    void addElement(std::unique_ptr<UIElement> element);
    void removeAllElements();
    
    void handleInput(u64 kDown, Point touchPos);
    
    // Примитивы рисования
    void drawRect(Rect rect, Color color);
    void drawText(Point pos, const std::string& text, Color color, float size = 24.0f);
    void drawIcon(Point pos, const std::string& iconName, Color color, float size = 24.0f);
};

// Глобальная система графики
extern std::unique_ptr<GraphicsSystem> g_graphics;