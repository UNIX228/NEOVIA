#include "simple_interface.h"
#include <cstring>

// Простая функция отрисовки текста без сложного шрифта
void GraphicsManager::drawText(const std::string& text, float x, float y, const Color& color, int fontSize) {
    // Простая заглушка для отрисовки текста
    // В реальной реализации здесь будет bitmap шрифт
    
    if (!framebuffer) return;
    
    int charWidth = fontSize / 2;
    int charHeight = fontSize;
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        
        // Простой прямоугольник вместо символа (для совместимости)
        int startX = (int)(x + i * charWidth);
        int startY = (int)y;
        
        // Рисуем простой прямоугольник для каждого символа
        for (int dy = 0; dy < charHeight && startY + dy < height; dy++) {
            for (int dx = 0; dx < charWidth && startX + dx < width; dx++) {
                if (startX + dx >= 0 && startY + dy >= 0) {
                    int pixelIndex = (startY + dy) * width + (startX + dx);
                    if (pixelIndex < width * height) {
                        // Простая отрисовка - рамка символа
                        if (dx == 0 || dx == charWidth-1 || dy == 0 || dy == charHeight-1) {
                            framebuffer[pixelIndex] = color.toRGBA();
                        }
                    }
                }
            }
        }
    }
}

// Простая функция получения размера текста
void GraphicsManager::getTextSize(const std::string& text, int fontSize, int& width, int& height) {
    width = text.length() * (fontSize / 2);
    height = fontSize;
}