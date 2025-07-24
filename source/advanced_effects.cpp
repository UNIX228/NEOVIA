#include "graphics.h"
#include "icon_loader.h"
#include <cmath>
#include <random>
#include <vector>

// Advanced visual effects for NEOVIA
namespace AdvancedEffects {
    
    // Holographic effect with rainbow colors
    void drawHolographicPanel(float x, float y, float width, float height, float time, float intensity = 1.0f) {
        // Create holographic shimmer effect
        for (int py = 0; py < (int)height; py += 2) {
            for (int px = 0; px < (int)width; px += 2) {
                float fx = (float)px / width;
                float fy = (float)py / height;
                
                // Create rainbow effect
                float hue = fmod(time * 0.5f + fx * 2.0f + fy * 1.0f, 1.0f) * 360.0f;
                float sat = 0.7f + 0.3f * sin(time * 3.0f + fx * 10.0f);
                float val = 0.3f + 0.2f * sin(time * 2.0f + fy * 8.0f);
                
                // HSV to RGB conversion
                float c = val * sat;
                float x_val = c * (1 - abs(fmod(hue / 60.0f, 2) - 1));
                float m = val - c;
                
                float r, g, b;
                if (hue < 60) { r = c; g = x_val; b = 0; }
                else if (hue < 120) { r = x_val; g = c; b = 0; }
                else if (hue < 180) { r = 0; g = c; b = x_val; }
                else if (hue < 240) { r = 0; g = x_val; b = c; }
                else if (hue < 300) { r = x_val; g = 0; b = c; }
                else { r = c; g = 0; b = x_val; }
                
                uint8_t red = (uint8_t)((r + m) * 255 * intensity);
                uint8_t green = (uint8_t)((g + m) * 255 * intensity);
                uint8_t blue = (uint8_t)((b + m) * 255 * intensity);
                uint8_t alpha = (uint8_t)(100 * intensity);
                
                Color holoColor(red, green, blue, alpha);
                GFX->drawPixel(x + px, y + py, holoColor);
            }
        }
    }
    
    // Neon glow text effect
    void drawNeonText(const std::string& text, float x, float y, const Color& color, 
                     int fontSize, float glowSize = 8.0f, float intensity = 1.0f) {
        // Draw multiple glow layers
        for (int layer = (int)glowSize; layer > 0; layer--) {
            float alpha = intensity * (1.0f - (float)layer / glowSize) * 0.3f;
            Color glowColor(color.r, color.g, color.b, (uint8_t)(255 * alpha));
            
            // Draw text at multiple offsets for glow effect
            for (int dx = -layer; dx <= layer; dx++) {
                for (int dy = -layer; dy <= layer; dy++) {
                    if (dx * dx + dy * dy <= layer * layer) {
                        GFX->drawText(text, x + dx, y + dy, glowColor, fontSize);
                    }
                }
            }
        }
        
        // Draw main text
        GFX->drawText(text, x, y, color, fontSize);
    }
    
    // Plasma effect background
    void drawPlasmaBackground(float x, float y, float width, float height, float time) {
        for (int py = 0; py < (int)height; py += 2) {
            for (int px = 0; px < (int)width; px += 2) {
                float fx = (float)px / width * 8.0f;
                float fy = (float)py / height * 8.0f;
                
                // Plasma equation
                float value = sin(fx + time) + 
                             sin(fy + time) + 
                             sin((fx + fy + time) / 2.0f) + 
                             sin(sqrt(fx * fx + fy * fy) + time);
                
                value = (value + 4.0f) / 8.0f; // Normalize to 0-1
                
                // Convert to color
                uint8_t r = (uint8_t)(128 + 127 * sin(value * M_PI * 2 + 0));
                uint8_t g = (uint8_t)(128 + 127 * sin(value * M_PI * 2 + 2));
                uint8_t b = (uint8_t)(128 + 127 * sin(value * M_PI * 2 + 4));
                uint8_t a = 80; // Semi-transparent
                
                Color plasmaColor(r, g, b, a);
                GFX->drawPixel(x + px, y + py, plasmaColor);
            }
        }
    }
    
    // Crystal/glass effect
    void drawCrystalPanel(float x, float y, float width, float height, const Color& baseColor, 
                         float time, float refraction = 0.1f) {
        for (int py = 0; py < (int)height; py++) {
            for (int px = 0; px < (int)width; px++) {
                float fx = (float)px / width;
                float fy = (float)py / height;
                
                // Create refraction effect
                float offsetX = sin(fy * 10.0f + time) * refraction * width;
                float offsetY = cos(fx * 10.0f + time) * refraction * height;
                
                // Sample "background" with offset
                float intensity = 0.8f + 0.2f * sin((fx + fy) * 8.0f + time * 2.0f);
                
                uint8_t r = (uint8_t)(baseColor.r * intensity);
                uint8_t g = (uint8_t)(baseColor.g * intensity);
                uint8_t b = (uint8_t)(baseColor.b * intensity);
                uint8_t a = (uint8_t)(baseColor.a * 0.7f);
                
                Color crystalColor(r, g, b, a);
                GFX->drawPixel(x + px, y + py, crystalColor);
                
                // Add highlights
                if ((px + py) % 20 == 0) {
                    Color highlight(255, 255, 255, 100);
                    GFX->drawPixel(x + px, y + py, highlight);
                }
            }
        }
    }
    
    // Simple star struct for constellation
    struct Star {
        float x, y;
        float brightness;
        float phase;
    };

    // Animated constellation effect
    void drawConstellation(float x, float y, float width, float height, float time, int starCount = 20) {
        static std::vector<Star> stars;
        
        // Initialize stars
        if (stars.empty() || stars.size() != starCount) {
            stars.clear();
            stars.resize(starCount);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> xDist(0, width);
            std::uniform_real_distribution<float> yDist(0, height);
            std::uniform_real_distribution<float> phaseDist(0, M_PI * 2);
            
            for (auto& star : stars) {
                star.x = xDist(gen);
                star.y = yDist(gen);
                star.brightness = 0.5f + (gen() % 100) / 200.0f;
                star.phase = phaseDist(gen);
            }
        }
        
        // Draw stars
        for (size_t i = 0; i < stars.size(); i++) {
            const auto& star = stars[i];
            float twinkle = 0.5f + 0.5f * sin(time * 2.0f + star.phase);
            float brightness = star.brightness * twinkle;
            
            uint8_t intensity = (uint8_t)(255 * brightness);
            Color starColor(intensity, intensity, intensity, intensity);
            
            float starX = x + star.x;
            float starY = y + star.y;
            
            // Draw star with cross pattern
            GFX->drawPixel(starX, starY, starColor);
            GFX->drawPixel(starX - 1, starY, Color(intensity/2, intensity/2, intensity/2, intensity/2));
            GFX->drawPixel(starX + 1, starY, Color(intensity/2, intensity/2, intensity/2, intensity/2));
            GFX->drawPixel(starX, starY - 1, Color(intensity/2, intensity/2, intensity/2, intensity/2));
            GFX->drawPixel(starX, starY + 1, Color(intensity/2, intensity/2, intensity/2, intensity/2));
            
            // Draw connections to nearby stars
            for (size_t j = i + 1; j < stars.size(); j++) {
                const auto& other = stars[j];
                float dx = star.x - other.x;
                float dy = star.y - other.y;
                float distance = sqrt(dx * dx + dy * dy);
                
                if (distance < 100.0f) {
                    float alpha = (1.0f - distance / 100.0f) * 0.3f * brightness;
                    Color lineColor(Colors::PRIMARY.r, Colors::PRIMARY.g, Colors::PRIMARY.b, 
                                   (uint8_t)(255 * alpha));
                    
                    GFX->drawLine(x + star.x, y + star.y, x + other.x, y + other.y, lineColor);
                }
            }
        }
    }
    
    // Liquid metal effect
    void drawLiquidMetal(float x, float y, float width, float height, float time, 
                        const Color& metalColor) {
        for (int py = 0; py < (int)height; py += 2) {
            for (int px = 0; px < (int)width; px += 2) {
                float fx = (float)px / width;
                float fy = (float)py / height;
                
                // Create flowing liquid effect
                float wave1 = sin(fx * 6.0f + time * 2.0f) * 0.1f;
                float wave2 = cos(fy * 4.0f + time * 1.5f) * 0.1f;
                float wave3 = sin((fx + fy) * 8.0f + time * 3.0f) * 0.05f;
                
                float intensity = 0.7f + wave1 + wave2 + wave3;
                intensity = std::max(0.0f, std::min(1.0f, intensity));
                
                // Add metallic highlights
                float highlight = pow(intensity, 3.0f);
                
                uint8_t r = (uint8_t)(metalColor.r * intensity + 100 * highlight);
                uint8_t g = (uint8_t)(metalColor.g * intensity + 100 * highlight);
                uint8_t b = (uint8_t)(metalColor.b * intensity + 100 * highlight);
                uint8_t a = metalColor.a;
                
                r = std::min(255, (int)r);
                g = std::min(255, (int)g);
                b = std::min(255, (int)b);
                
                Color liquidColor(r, g, b, a);
                GFX->drawPixel(x + px, y + py, liquidColor);
            }
        }
    }
    
    // Cyberpunk grid effect
    void drawCyberpunkGrid(float x, float y, float width, float height, float time, 
                          float gridSize = 20.0f) {
        Color gridColor = Colors::PRIMARY;
        
        // Draw horizontal lines
        for (float gy = 0; gy < height; gy += gridSize) {
            float intensity = 0.3f + 0.2f * sin(time * 2.0f + gy * 0.1f);
            Color lineColor(gridColor.r, gridColor.g, gridColor.b, (uint8_t)(255 * intensity));
            
            for (int px = 0; px < (int)width; px += 2) {
                float pulse = 0.8f + 0.2f * sin(time * 5.0f + px * 0.2f);
                Color pulseColor(lineColor.r, lineColor.g, lineColor.b, 
                               (uint8_t)(lineColor.a * pulse));
                GFX->drawPixel(x + px, y + gy, pulseColor);
            }
        }
        
        // Draw vertical lines
        for (float gx = 0; gx < width; gx += gridSize) {
            float intensity = 0.3f + 0.2f * sin(time * 1.8f + gx * 0.1f);
            Color lineColor(gridColor.r, gridColor.g, gridColor.b, (uint8_t)(255 * intensity));
            
            for (int py = 0; py < (int)height; py += 2) {
                float pulse = 0.8f + 0.2f * sin(time * 4.0f + py * 0.15f);
                Color pulseColor(lineColor.r, lineColor.g, lineColor.b, 
                               (uint8_t)(lineColor.a * pulse));
                GFX->drawPixel(x + gx, y + py, pulseColor);
            }
        }
        
        // Add intersection glow points
        for (float gy = 0; gy < height; gy += gridSize) {
            for (float gx = 0; gx < width; gx += gridSize) {
                float glowIntensity = 0.5f + 0.5f * sin(time * 3.0f + gx * 0.05f + gy * 0.05f);
                if (glowIntensity > 0.8f) {
                    Color glowColor(255, 255, 255, (uint8_t)(150 * glowIntensity));
                    
                    // Draw small glow
                    for (int dy = -2; dy <= 2; dy++) {
                        for (int dx = -2; dx <= 2; dx++) {
                            if (dx * dx + dy * dy <= 4) {
                                GFX->drawPixel(x + gx + dx, y + gy + dy, glowColor);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Animated logo with multiple effects
    void drawAnimatedLogo(float x, float y, float time, float scale = 1.0f) {
        // Use multiple effects combined
        float logoSize = 64 * scale;
        
        // Pulsing background glow
        float pulse = 0.7f + 0.3f * sin(time * 2.0f);
        Color glowColor(Colors::PRIMARY.r, Colors::PRIMARY.g, Colors::PRIMARY.b, 
                       (uint8_t)(100 * pulse));
        
        GFX->drawCircle(x + logoSize/2, y + logoSize/2, logoSize * 0.6f * pulse, glowColor);
        
        // Rotating elements
        for (int i = 0; i < 6; i++) {
            float angle = time + i * M_PI / 3.0f;
            float orbitRadius = logoSize * 0.3f;
            float orbX = x + logoSize/2 + cos(angle) * orbitRadius;
            float orbY = y + logoSize/2 + sin(angle) * orbitRadius;
            
            Color orbColor = (i % 2 == 0) ? Colors::SECONDARY : Colors::ACCENT;
            GFX->drawCircle(orbX, orbY, 4 * scale, orbColor);
        }
        
        // Central logo
        if (ICON_LOADER->hasIcon("neovia_gradient")) {
            IconEffects::drawPulsingIcon("neovia_gradient", x, y, time, scale, 0.1f);
        }
    }
}

// Integration with existing GUI
void enhanceModernGUI() {
    // This function can be called to add the new effects to the existing GUI
    // The effects can be integrated into the modern_gui.cpp render functions
}