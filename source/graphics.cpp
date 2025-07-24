#include "graphics.h"
#include <cstring>
#include <algorithm>

// Глобальная система графики
std::unique_ptr<GraphicsSystem> g_graphics;

// Простые шейдеры для 2D рендеринга
static const char* vertexShaderSource = R"(
#version 460

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;

layout (location = 0) out vec4 fragColor;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    fragColor = color;
}
)";

static const char* fragmentShaderSource = R"(
#version 460

layout (location = 0) in vec4 fragColor;
layout (location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}
)";

GraphicsSystem::GraphicsSystem() : initialized(false) {
}

GraphicsSystem::~GraphicsSystem() {
    shutdown();
}

bool GraphicsSystem::initialize() {
    if (initialized) return true;
    
    // Создаем устройство deko3d
    device = dk::DeviceMaker{}.create();
    if (!device) return false;
    
    // Создаем очередь команд
    queue = dk::QueueMaker{device}.setFlags(DkQueueFlags_Graphics).create();
    if (!queue) return false;
    
    // Выделяем память для команд
    cmdMemBlock = dk::MemBlockMaker{device, 0x10000}
        .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
        .create();
    if (!cmdMemBlock) return false;
    
    // Создаем командный буфер
    cmdbuf = dk::CmdBufMaker{device}.create();
    if (!cmdbuf) return false;
    
    cmdbuf.addMemory(cmdMemBlock, 0, cmdMemBlock.getSize());
    
    // Создаем изображения для swapchain
    static constexpr uint32_t NumFramebuffers = 2;
    dk::UniqueImage framebuffers[NumFramebuffers];
    
    // Создаем layout для изображений
    dk::ImageLayout layout;
    dk::ImageLayoutMaker{device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(SCREEN_WIDTH, SCREEN_HEIGHT)
        .initialize(layout);
    
    uint32_t fbSize = layout.getSize();
    uint32_t fbAlign = layout.getAlignment();
    
    // Выделяем память для framebuffer
    fbMemBlock = dk::MemBlockMaker{device, fbSize * NumFramebuffers}
        .setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
        .create();
    
    if (!fbMemBlock) return false;
    
    // Создаем изображения
    for (uint32_t i = 0; i < NumFramebuffers; i++) {
        framebuffers[i] = dk::ImageMaker{device, layout}
            .setMemory(fbMemBlock, fbSize * i)
            .create();
    }
    
    // Создаем swapchain с изображениями
    std::array<DkImage const*, NumFramebuffers> swapchainImages;
    for (uint32_t i = 0; i < NumFramebuffers; i++) {
        swapchainImages[i] = &framebuffers[i];
    }
    
    swapchain = dk::SwapchainMaker{device, nwindowGetDefault(), swapchainImages}
        .create();
    
    if (!swapchain) return false;
    
    initialized = true;
    return true;
}

void GraphicsSystem::shutdown() {
    if (!initialized) return;
    
    elements.clear();
    
    cmdbuf = nullptr;
    cmdMemBlock = nullptr;
    fbMemBlock = nullptr;
    depthBuffer = nullptr;
    swapchain = nullptr;
    queue = nullptr;
    device = nullptr;
    
    initialized = false;
}

void GraphicsSystem::beginFrame() {
    if (!initialized) return;
    
    // Начинаем запись команд
    cmdbuf.clear();
    
    // Получаем текущее изображение
    int imageSlot;
    DkFence fence{};
    swapchain.acquireImage(imageSlot, fence);
    
    // Очищаем экран серым цветом (упрощенная версия)
    cmdbuf.setScissors(0, DkScissor{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    cmdbuf.clearColor(0, DkColorMask_RGBA, 0.26f, 0.26f, 0.26f, 1.0f); // Серый фон
    
    // Устанавливаем viewport
    cmdbuf.setViewports(0, DkViewport{0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f});
}

void GraphicsSystem::endFrame() {
    if (!initialized) return;
    
    // Рендерим все элементы
    for (auto& element : elements) {
        if (element->visible) {
            element->render();
        }
    }
    
    // Завершаем запись команд
    cmdlist = cmdbuf.finishList();
}

void GraphicsSystem::present() {
    if (!initialized) return;
    
    // Отправляем команды на выполнение
    queue.submitCommands(cmdlist);
    queue.presentImage(swapchain, 0);
    queue.waitIdle();
}

void GraphicsSystem::addElement(std::unique_ptr<UIElement> element) {
    elements.push_back(std::move(element));
}

void GraphicsSystem::removeAllElements() {
    elements.clear();
}

void GraphicsSystem::handleInput(u64 kDown, Point touchPos) {
    // Обрабатываем ввод для интерактивных элементов
    for (auto& element : elements) {
        if (element->interactive && element->visible) {
            if (element->handleInput(kDown, touchPos)) {
                break; // Элемент обработал ввод
            }
        }
    }
}

void GraphicsSystem::drawRect(Rect rect, Color color) {
    if (!initialized) return;
    
    // Заглушка для отрисовки прямоугольников
    // В полной реализации здесь будет создание vertex buffer и рендеринг
}

void GraphicsSystem::drawText(Point pos, const std::string& text, Color color, float size) {
    // Заглушка для отрисовки текста
    // В реальной реализации нужно использовать библиотеку шрифтов
}

void GraphicsSystem::drawIcon(Point pos, const std::string& iconName, Color color, float size) {
    // Заглушка для отрисовки иконок
    // В реальной реализации нужно загружать и отрисовывать PNG иконки
}

// Реализация Panel
void Panel::render() {
    if (g_graphics) {
        g_graphics->drawRect(bounds, color);
    }
}

// Реализация Button
void Button::render() {
    if (!g_graphics) return;
    
    // Рисуем фон кнопки
    Color buttonColor = hovered ? Color(COLOR_BUTTON_HOVER) : color;
    g_graphics->drawRect(bounds, buttonColor);
    
    // Рисуем текст по центру
    Point textPos = {
        bounds.x + bounds.width / 2.0f,
        bounds.y + bounds.height / 2.0f
    };
    g_graphics->drawText(textPos, text, Color(COLOR_TEXT_WHITE));
}

bool Button::handleInput(u64 kDown, Point touchPos) {
    // Проверяем попадание курсора/касания
    bool inBounds = touchPos.x >= bounds.x && touchPos.x <= bounds.x + bounds.width &&
                   touchPos.y >= bounds.y && touchPos.y <= bounds.y + bounds.height;
    
    setHovered(inBounds);
    
    if (inBounds && (kDown & HidNpadButton_A)) {
        if (onClick) {
            onClick();
        }
        return true;
    }
    
    return false;
}

// Реализация TextElement
void TextElement::render() {
    if (g_graphics) {
        g_graphics->drawText({bounds.x, bounds.y}, text, color, fontSize);
    }
}