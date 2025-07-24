#include "graphics.h"
#include <random>
#include <cmath>

// Дополнительные эффекты частиц
namespace ParticleEffects {
    
    // Звездное поле
    void drawStarField(float time, float speed = 1.0f) {
        static std::vector<struct Star {
            float x, y, z;
            float brightness;
        }> stars;
        
        // Инициализация звезд
        if (stars.empty()) {
            stars.resize(100);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> xDist(0, 1280);
            std::uniform_real_distribution<float> yDist(0, 720);
            std::uniform_real_distribution<float> zDist(0.1f, 1.0f);
            std::uniform_real_distribution<float> brightDist(0.3f, 1.0f);
            
            for (auto& star : stars) {
                star.x = xDist(gen);
                star.y = yDist(gen);
                star.z = zDist(gen);
                star.brightness = brightDist(gen);
            }
        }
        
        // Рендер звезд
        for (auto& star : stars) {
            star.z -= speed * 0.01f;
            if (star.z <= 0) {
                star.z = 1.0f;
                star.x = rand() % 1280;
                star.y = rand() % 720;
            }
            
            float screenX = star.x + (star.x - 640) * (1.0f - star.z) * 0.5f;
            float screenY = star.y + (star.y - 360) * (1.0f - star.z) * 0.5f;
            
            if (screenX >= 0 && screenX < 1280 && screenY >= 0 && screenY < 720) {
                uint8_t alpha = (uint8_t)(255 * star.brightness * star.z);
                Color starColor(255, 255, 255, alpha);
                
                float size = (1.0f - star.z) * 3 + 1;
                GFX->drawCircle(screenX, screenY, size, starColor);
            }
        }
    }
    
    // Плавающие геометрические фигуры
    void drawFloatingShapes(float time) {
        const int shapeCount = 8;
        
        for (int i = 0; i < shapeCount; i++) {
            float phase = time * 0.3f + i * 0.8f;
            float x = 200 + i * 150 + sin(phase) * 80;
            float y = 200 + sin(phase * 0.7f + i) * 100;
            float rotation = time * 0.5f + i;
            float scale = 0.8f + sin(phase * 0.5f) * 0.3f;
            
            // Выбор цвета
            std::vector<Color> colors = {
                Colors::PRIMARY, Colors::SECONDARY, 
                Colors::ACCENT, Colors::SUCCESS
            };
            Color shapeColor = colors[i % colors.size()];
            shapeColor.a = 80;
            
            // Рисуем разные фигуры
            switch (i % 4) {
                case 0: // Круг
                    GFX->drawCircle(x, y, 20 * scale, shapeColor);
                    break;
                case 1: // Квадрат
                    GFX->drawRoundedRect(x - 15 * scale, y - 15 * scale, 
                                       30 * scale, 30 * scale, 5, shapeColor);
                    break;
                case 2: // Треугольник (упрощенный как ромб)
                    GFX->drawCircle(x, y, 18 * scale, shapeColor);
                    break;
                case 3: // Шестиугольник (упрощенный как круг)
                    GFX->drawCircle(x, y, 25 * scale, shapeColor);
                    break;
            }
        }
    }
    
    // Волновые эффекты
    void drawWaveEffect(float time, float amplitude = 50.0f) {
        const int waveCount = 3;
        
        for (int wave = 0; wave < waveCount; wave++) {
            Color waveColor = wave == 0 ? Colors::PRIMARY : 
                             wave == 1 ? Colors::SECONDARY : Colors::ACCENT;
            waveColor.a = 30;
            
            float offset = wave * 100;
            float frequency = 0.01f + wave * 0.005f;
            float speed = time * (1.0f + wave * 0.3f);
            
            // Рисуем волну как серию точек
            for (int x = 0; x < 1280; x += 4) {
                float y = 360 + sin(x * frequency + speed) * amplitude + offset;
                if (y >= 0 && y < 720) {
                    GFX->drawCircle(x, y, 2, waveColor);
                }
            }
        }
    }
    
    // Световые лучи
    void drawLightRays(float time, float centerX = 640, float centerY = 360) {
        const int rayCount = 12;
        
        for (int i = 0; i < rayCount; i++) {
            float angle = (i * 2 * M_PI / rayCount) + time * 0.2f;
            float length = 300 + sin(time + i) * 100;
            
            float x1 = centerX + cos(angle) * 50;
            float y1 = centerY + sin(angle) * 50;
            float x2 = centerX + cos(angle) * length;
            float y2 = centerY + sin(angle) * length;
            
            // Градиент луча
            for (int j = 0; j < 20; j++) {
                float t = j / 20.0f;
                float x = x1 + (x2 - x1) * t;
                float y = y1 + (y2 - y1) * t;
                
                uint8_t alpha = (uint8_t)(100 * (1.0f - t));
                Color rayColor(Colors::PRIMARY.r, Colors::PRIMARY.g, Colors::PRIMARY.b, alpha);
                
                GFX->drawCircle(x, y, 3 * (1.0f - t * 0.8f), rayColor);
            }
        }
    }
    
    // Энергетическая сетка
    void drawEnergyGrid(float time) {
        const int gridSize = 100;
        Color gridColor(Colors::SECONDARY.r, Colors::SECONDARY.g, Colors::SECONDARY.b, 20);
        
        // Вертикальные линии
        for (int x = 0; x < 1280; x += gridSize) {
            float offset = sin(time * 0.5f + x * 0.01f) * 20;
            for (int y = 0; y < 720; y += 10) {
                float nodeX = x + offset;
                if (nodeX >= 0 && nodeX < 1280) {
                    GFX->drawPixel(nodeX, y, gridColor);
                }
            }
        }
        
        // Горизонтальные линии
        for (int y = 0; y < 720; y += gridSize) {
            float offset = sin(time * 0.3f + y * 0.01f) * 15;
            for (int x = 0; x < 1280; x += 10) {
                float nodeY = y + offset;
                if (nodeY >= 0 && nodeY < 720) {
                    GFX->drawPixel(x, nodeY, gridColor);
                }
            }
        }
        
        // Узлы сетки
        for (int x = 0; x < 1280; x += gridSize) {
            for (int y = 0; y < 720; y += gridSize) {
                float offsetX = sin(time * 0.5f + x * 0.01f) * 20;
                float offsetY = sin(time * 0.3f + y * 0.01f) * 15;
                
                float nodeX = x + offsetX;
                float nodeY = y + offsetY;
                
                if (nodeX >= 0 && nodeX < 1280 && nodeY >= 0 && nodeY < 720) {
                    float pulse = sin(time * 2 + x * 0.01f + y * 0.01f) * 0.5f + 0.5f;
                    uint8_t alpha = (uint8_t)(150 * pulse);
                    Color nodeColor(Colors::ACCENT.r, Colors::ACCENT.g, Colors::ACCENT.b, alpha);
                    
                    GFX->drawCircle(nodeX, nodeY, 3 + pulse * 2, nodeColor);
                }
            }
        }
    }
    
    // Matrix-подобный эффект дождя
    void drawMatrixRain(float time) {
        static std::vector<struct Drop {
            float x, y, speed;
            char character;
            float life;
        }> drops;
        
        // Инициализация капель
        if (drops.size() < 50) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> xDist(0, 1280);
            std::uniform_real_distribution<float> speedDist(50, 200);
            std::uniform_int_distribution<int> charDist(33, 126);
            
            Drop drop;
            drop.x = xDist(gen);
            drop.y = -20;
            drop.speed = speedDist(gen);
            drop.character = charDist(gen);
            drop.life = 1.0f;
            drops.push_back(drop);
        }
        
        // Обновление и рендер капель
        for (auto& drop : drops) {
            drop.y += drop.speed * (1.0f / 60.0f);
            drop.life -= 0.01f;
            
            if (drop.y > 720 || drop.life <= 0) {
                drop.y = -20;
                drop.x = rand() % 1280;
                drop.life = 1.0f;
                drop.character = 33 + rand() % 94;
            }
            
            uint8_t alpha = (uint8_t)(100 * drop.life);
            Color textColor(Colors::SUCCESS.r, Colors::SUCCESS.g, Colors::SUCCESS.b, alpha);
            
            std::string charStr(1, drop.character);
            GFX->drawText(charStr, drop.x, drop.y, textColor, 12);
        }
    }
}

// Интеграция в ModernGUI
void ModernGUI::renderBackground(float deltaTime) {
    // Базовый градиент
    float pulse = backgroundPulse.update(deltaTime);
    Color bg1(18 + (int)(pulse * 5), 18 + (int)(pulse * 5), 24 + (int)(pulse * 8));
    Color bg2(12 - (int)(pulse * 2), 12 - (int)(pulse * 2), 18 - (int)(pulse * 3));
    GFX->drawGradient(0, 0, 1280, 720, bg1, bg2, true);
    
    // Эффекты частиц
    ParticleEffects::drawStarField(backgroundOffset, 0.5f);
    ParticleEffects::drawEnergyGrid(backgroundOffset);
    ParticleEffects::drawFloatingShapes(backgroundOffset);
    ParticleEffects::drawWaveEffect(backgroundOffset, 30.0f);
    
    // Световые эффекты только на главном экране
    if (currentScreen == Screen::MAIN_MENU) {
        ParticleEffects::drawLightRays(backgroundOffset);
    }
}