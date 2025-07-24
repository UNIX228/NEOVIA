#include "graphics.h"
#include <cmath>

namespace UIEffects {
    
    // Эффект появления элемента
    void drawFadeInEffect(float x, float y, float width, float height, float progress, const Color& color) {
        // progress от 0.0 до 1.0
        uint8_t alpha = (uint8_t)(color.a * progress);
        Color fadeColor(color.r, color.g, color.b, alpha);
        
        float scale = 0.8f + progress * 0.2f; // Масштабирование от 80% до 100%
        float scaledWidth = width * scale;
        float scaledHeight = height * scale;
        float offsetX = (width - scaledWidth) / 2;
        float offsetY = (height - scaledHeight) / 2;
        
        GFX->drawRoundedRect(x + offsetX, y + offsetY, scaledWidth, scaledHeight, 8, fadeColor);
    }
    
    // Эффект пульсации
    void drawPulseEffect(float x, float y, float radius, float time, const Color& color, float intensity = 1.0f) {
        float pulse = sin(time * 3.0f) * 0.5f + 0.5f; // 0.0 - 1.0
        float currentRadius = radius * (1.0f + pulse * 0.3f * intensity);
        uint8_t alpha = (uint8_t)(color.a * (0.5f + pulse * 0.5f));
        
        Color pulseColor(color.r, color.g, color.b, alpha);
        GFX->drawCircle(x, y, currentRadius, pulseColor);
        
        // Дополнительные кольца
        for (int i = 1; i <= 3; i++) {
            float ringRadius = currentRadius + i * 10;
            uint8_t ringAlpha = (uint8_t)(alpha * (1.0f - i * 0.3f));
            Color ringColor(color.r, color.g, color.b, ringAlpha);
            
            // Рисуем кольцо как разность кругов
            for (float angle = 0; angle < 2 * M_PI; angle += 0.1f) {
                float px = x + cos(angle) * ringRadius;
                float py = y + sin(angle) * ringRadius;
                GFX->drawPixel(px, py, ringColor);
            }
        }
    }
    
    // Эффект свечения текста
    void drawGlowText(const std::string& text, float x, float y, const Color& color, int fontSize, float glowIntensity = 0.5f) {
        // Рисуем свечение (несколько слоев с разной прозрачностью)
        for (int offset = 3; offset > 0; offset--) {
            uint8_t glowAlpha = (uint8_t)(color.a * glowIntensity * (float)offset / 3.0f);
            Color glowColor(color.r, color.g, color.b, glowAlpha);
            
            // Рисуем текст со смещением во все стороны
            GFX->drawText(text, x - offset, y, glowColor, fontSize);
            GFX->drawText(text, x + offset, y, glowColor, fontSize);
            GFX->drawText(text, x, y - offset, glowColor, fontSize);
            GFX->drawText(text, x, y + offset, glowColor, fontSize);
            GFX->drawText(text, x - offset, y - offset, glowColor, fontSize);
            GFX->drawText(text, x + offset, y + offset, glowColor, fontSize);
            GFX->drawText(text, x - offset, y + offset, glowColor, fontSize);
            GFX->drawText(text, x + offset, y - offset, glowColor, fontSize);
        }
        
        // Основной текст
        GFX->drawText(text, x, y, color, fontSize);
    }
    
    // Эффект искр
    void drawSparkles(float centerX, float centerY, float time, int count = 10) {
        for (int i = 0; i < count; i++) {
            float angle = (i * 2 * M_PI / count) + time * 2.0f;
            float distance = 30 + sin(time * 3 + i) * 15;
            
            float x = centerX + cos(angle) * distance;
            float y = centerY + sin(angle) * distance;
            
            float sparkleTime = time * 4 + i * 0.5f;
            float alpha = (sin(sparkleTime) + 1) * 0.5f;
            float size = 2 + alpha * 3;
            
            Color sparkleColor(255, 255, 200, (uint8_t)(255 * alpha));
            GFX->drawCircle(x, y, size, sparkleColor);
            
            // Дополнительный блеск
            if (alpha > 0.7f) {
                GFX->drawLine(x - size, y, x + size, y, sparkleColor, 1);
                GFX->drawLine(x, y - size, x, y + size, sparkleColor, 1);
            }
        }
    }
    
    // Эффект загрузки (спиннер)
    void drawLoadingSpinner(float centerX, float centerY, float time, float radius = 20) {
        const int segments = 8;
        
        for (int i = 0; i < segments; i++) {
            float angle = (i * 2 * M_PI / segments) - time * 4;
            float x = centerX + cos(angle) * radius;
            float y = centerY + sin(angle) * radius;
            
            // Альфа уменьшается для создания эффекта хвоста
            float alpha = 1.0f - (float)i / segments;
            Color dotColor(Colors::PRIMARY.r, Colors::PRIMARY.g, Colors::PRIMARY.b, (uint8_t)(255 * alpha));
            
            float dotSize = 3 + alpha * 2;
            GFX->drawCircle(x, y, dotSize, dotColor);
        }
    }
    
    // Эффект энергетического поля
    void drawEnergyField(float x, float y, float width, float height, float time, const Color& color) {
        // Основное поле
        uint8_t fieldAlpha = (uint8_t)(color.a * 0.3f);
        Color fieldColor(color.r, color.g, color.b, fieldAlpha);
        GFX->drawRoundedRect(x, y, width, height, 10, fieldColor);
        
        // Энергетические линии
        for (int i = 0; i < 5; i++) {
            float lineY = y + (height / 6) * (i + 1);
            float offset = sin(time * 2 + i) * 10;
            
            uint8_t lineAlpha = (uint8_t)(color.a * (0.5f + sin(time + i) * 0.3f));
            Color lineColor(color.r, color.g, color.b, lineAlpha);
            
            GFX->drawLine(x + 10 + offset, lineY, x + width - 10 + offset, lineY, lineColor, 2);
        }
        
        // Пульсирующие узлы
        for (int i = 0; i < 3; i++) {
            float nodeX = x + width * (0.25f + i * 0.25f);
            float nodeY = y + height * 0.5f;
            float pulse = sin(time * 3 + i * 1.5f) * 0.5f + 0.5f;
            
            uint8_t nodeAlpha = (uint8_t)(color.a * pulse);
            Color nodeColor(color.r, color.g, color.b, nodeAlpha);
            
            GFX->drawCircle(nodeX, nodeY, 5 + pulse * 3, nodeColor);
        }
    }
    
    // Эффект голограммы
    void drawHologramEffect(float x, float y, float width, float height, float time) {
        // Голографические линии
        for (int line = 0; line < (int)height; line += 3) {
            float lineY = y + line;
            float alpha = (sin(time * 5 + line * 0.1f) + 1) * 0.5f;
            
            if (alpha > 0.3f) {
                uint8_t lineAlpha = (uint8_t)(100 * alpha);
                Color holoColor(0, 255, 200, lineAlpha);
                
                GFX->drawLine(x, lineY, x + width, lineY, holoColor, 1);
            }
        }
        
        // Сканирующая линия
        float scanY = y + (sin(time * 2) + 1) * 0.5f * height;
        Color scanColor(0, 255, 255, 150);
        GFX->drawLine(x, scanY, x + width, scanY, scanColor, 3);
        
        // Искажения
        for (int i = 0; i < 3; i++) {
            float distortY = y + height * (0.2f + i * 0.3f);
            float distortX = x + sin(time * 3 + i) * 20;
            
            Color distortColor(100, 255, 200, 80);
            GFX->drawRect(distortX, distortY, width * 0.1f, 2, distortColor);
        }
    }
    
    // Эффект частиц при нажатии кнопки
    void drawButtonPressEffect(float centerX, float centerY, float time, int particleCount = 15) {
        for (int i = 0; i < particleCount; i++) {
            float angle = (i * 2 * M_PI / particleCount);
            float distance = time * 100; // Частицы разлетаются
            float life = 1.0f - time; // Время жизни частицы
            
            if (life <= 0) continue;
            
            float x = centerX + cos(angle) * distance;
            float y = centerY + sin(angle) * distance;
            
            uint8_t alpha = (uint8_t)(255 * life);
            Color particleColor(Colors::PRIMARY.r, Colors::PRIMARY.g, Colors::PRIMARY.b, alpha);
            
            float size = 3 * life;
            GFX->drawCircle(x, y, size, particleColor);
        }
    }
    
    // Эффект волны при переходе между экранами
    void drawTransitionWave(float progress, bool reverse = false) {
        // progress от 0.0 до 1.0
        float wavePos = reverse ? (1.0f - progress) : progress;
        float waveX = wavePos * 1280;
        
        // Рисуем волну
        for (int y = 0; y < 720; y++) {
            float waveOffset = sin(y * 0.02f + progress * 10) * 30;
            float currentX = waveX + waveOffset;
            
            // Градиент волны
            for (int i = 0; i < 50; i++) {
                float x = currentX - i;
                if (x >= 0 && x < 1280) {
                    uint8_t alpha = (uint8_t)(255 * (1.0f - i / 50.0f));
                    Color waveColor(Colors::PRIMARY.r, Colors::PRIMARY.g, Colors::PRIMARY.b, alpha);
                    GFX->drawPixel(x, y, waveColor);
                }
            }
        }
    }
}

// Интеграция эффектов в кнопки
void Button::render(float deltaTime) {
    if (!visible) return;
    
    // Базовая анимация
    float scale = scaleAnimation.update(deltaTime);
    float press = pressAnimation.update(deltaTime);
    float hover = hoverAnimation.update(deltaTime);
    
    // Эффект свечения при наведении
    if (hovered || pressed) {
        UIEffects::drawPulseEffect(x + width/2, y + height/2, width/2, 
                                  GFX->getDeltaTime() * 10, backgroundColor, 0.3f);
    }
    
    // Тень с эффектом
    GFX->drawShadow(x + 2, y + 4, width, height, 8, 0.3f);
    
    // Основная кнопка с эффектами
    Color currentBg = backgroundColor;
    if (hovered) {
        currentBg = Color(
            std::min(255, (int)(currentBg.r + 20)),
            std::min(255, (int)(currentBg.g + 20)),
            std::min(255, (int)(currentBg.b + 20))
        );
    }
    
    float renderX = x + press * 2;
    float renderY = y + press * 2;
    float renderW = width * (0.95f + scale * 0.05f);
    float renderH = height * (0.95f + scale * 0.05f);
    
    // Энергетическое поле для активных кнопок
    if (pressed) {
        UIEffects::drawEnergyField(renderX - 5, renderY - 5, renderW + 10, renderH + 10, 
                                  GFX->getDeltaTime() * 20, currentBg);
    }
    
    GFX->drawRoundedRect(renderX, renderY, renderW, renderH, cornerRadius, currentBg);
    
    // Текст с эффектом свечения
    if (hovered || pressed) {
        UIEffects::drawGlowText(text, renderX + renderW/2 - text.length() * 4, 
                               renderY + renderH/2 - 8, textColor, 16, 0.3f);
    } else {
        GFX->drawTextCentered(text, renderX, renderY + renderH/2 - 8, renderW, textColor, 16);
    }
    
    // Искры при нажатии
    if (pressed) {
        UIEffects::drawSparkles(renderX + renderW/2, renderY + renderH/2, 
                               GFX->getDeltaTime() * 30, 8);
    }
}