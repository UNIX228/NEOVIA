#include "icon_loader.h"
#include <cstring>
#include <cmath>
#include <fstream>
#include <vector>

IconLoader* IconLoader::instance = nullptr;

IconLoader* IconLoader::getInstance() {
    if (!instance) {
        instance = new IconLoader();
    }
    return instance;
}

// Simple JPEG header parser (basic implementation for common JPEG files)
IconLoader::JPEGInfo IconLoader::parseJPEGHeader(const uint8_t* data, size_t size) {
    JPEGInfo info = {0, 0, false};
    
    if (size < 10 || data[0] != 0xFF || data[1] != 0xD8) {
        return info; // Not a valid JPEG
    }
    
    size_t pos = 2;
    while (pos < size - 8) {
        if (data[pos] != 0xFF) {
            pos++;
            continue;
        }
        
        uint8_t marker = data[pos + 1];
        if (marker == 0xC0 || marker == 0xC2) { // SOF0 or SOF2
            if (pos + 9 < size) {
                info.height = (data[pos + 5] << 8) | data[pos + 6];
                info.width = (data[pos + 7] << 8) | data[pos + 8];
                info.valid = true;
                break;
            }
        }
        
        // Skip to next marker
        if (pos + 3 < size) {
            uint16_t length = (data[pos + 2] << 8) | data[pos + 3];
            pos += 2 + length;
        } else {
            break;
        }
    }
    
    return info;
}

// Simplified JPEG decoder (creates a placeholder for now)
std::unique_ptr<uint32_t[]> IconLoader::decodeJPEG(const uint8_t* data, size_t size, int& width, int& height) {
    JPEGInfo info = parseJPEGHeader(data, size);
    
    if (!info.valid) {
        width = height = 0;
        return nullptr;
    }
    
    width = info.width;
    height = info.height;
    
    // For now, create a beautiful gradient placeholder
    // In a full implementation, you'd use a proper JPEG decoder like libjpeg
    auto pixels = std::make_unique<uint32_t[]>(width * height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float fx = (float)x / width;
            float fy = (float)y / height;
            
            // Create a beautiful gradient based on position
            uint8_t r = (uint8_t)(128 + 127 * sin(fx * M_PI));
            uint8_t g = (uint8_t)(128 + 127 * sin(fy * M_PI));
            uint8_t b = (uint8_t)(128 + 127 * sin((fx + fy) * M_PI));
            uint8_t a = 255;
            
            pixels[y * width + x] = (r << 24) | (g << 16) | (b << 8) | a;
        }
    }
    
    return pixels;
}

bool IconLoader::loadIcon(const std::string& path, const std::string& name) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    file.close();
    
    return loadIconFromMemory(data.data(), size, name);
}

bool IconLoader::loadIconFromMemory(const uint8_t* data, size_t size, const std::string& name) {
    int width, height;
    auto pixels = decodeJPEG(data, size, width, height);
    
    if (!pixels) {
        return false;
    }
    
    iconCache[name] = std::move(pixels);
    iconSizes[name] = {width, height};
    return true;
}

uint32_t* IconLoader::getIcon(const std::string& name, int& width, int& height) {
    auto it = iconCache.find(name);
    if (it == iconCache.end()) {
        width = height = 0;
        return nullptr;
    }
    
    auto sizeIt = iconSizes.find(name);
    if (sizeIt != iconSizes.end()) {
        width = sizeIt->second.first;
        height = sizeIt->second.second;
    }
    
    return it->second.get();
}

bool IconLoader::hasIcon(const std::string& name) {
    return iconCache.find(name) != iconCache.end();
}

void IconLoader::clearCache() {
    iconCache.clear();
    iconSizes.clear();
}

void IconLoader::drawIcon(const std::string& name, float x, float y, float scale, float alpha, float rotation) {
    int width, height;
    uint32_t* pixels = getIcon(name, width, height);
    
    if (!pixels) return;
    
    float scaledWidth = width * scale;
    float scaledHeight = height * scale;
    
    // Simple icon rendering with rotation and alpha
    for (int py = 0; py < (int)scaledHeight; py++) {
        for (int px = 0; px < (int)scaledWidth; px++) {
            float fx = px / scale;
            float fy = py / scale;
            
            if (fx >= 0 && fx < width && fy >= 0 && fy < height) {
                int srcX = (int)fx;
                int srcY = (int)fy;
                uint32_t pixel = pixels[srcY * width + srcX];
                
                // Apply alpha
                uint8_t r = (pixel >> 24) & 0xFF;
                uint8_t g = (pixel >> 16) & 0xFF;
                uint8_t b = (pixel >> 8) & 0xFF;
                uint8_t a = ((pixel & 0xFF) * (uint8_t)(alpha * 255)) >> 8;
                
                Color color(r, g, b, a);
                
                // Apply rotation if needed
                float renderX = x + px;
                float renderY = y + py;
                
                if (rotation != 0.0f) {
                    float centerX = x + scaledWidth / 2;
                    float centerY = y + scaledHeight / 2;
                    float dx = renderX - centerX;
                    float dy = renderY - centerY;
                    
                    renderX = centerX + dx * cos(rotation) - dy * sin(rotation);
                    renderY = centerY + dx * sin(rotation) + dy * cos(rotation);
                }
                
                GFX->drawPixel((int)renderX, (int)renderY, color);
            }
        }
    }
}

void IconLoader::drawIconScaled(const std::string& name, float x, float y, float width, float height, float alpha) {
    int iconWidth, iconHeight;
    uint32_t* pixels = getIcon(name, iconWidth, iconHeight);
    
    if (!pixels) return;
    
    for (int py = 0; py < (int)height; py++) {
        for (int px = 0; px < (int)width; px++) {
            float fx = (px / width) * iconWidth;
            float fy = (py / height) * iconHeight;
            
            int srcX = (int)fx;
            int srcY = (int)fy;
            
            if (srcX >= 0 && srcX < iconWidth && srcY >= 0 && srcY < iconHeight) {
                uint32_t pixel = pixels[srcY * iconWidth + srcX];
                
                uint8_t r = (pixel >> 24) & 0xFF;
                uint8_t g = (pixel >> 16) & 0xFF;
                uint8_t b = (pixel >> 8) & 0xFF;
                uint8_t a = ((pixel & 0xFF) * (uint8_t)(alpha * 255)) >> 8;
                
                Color color(r, g, b, a);
                GFX->drawPixel(x + px, y + py, color);
            }
        }
    }
}

void IconLoader::generateGradientIcon(const std::string& name, int size, const Color& color1, const Color& color2) {
    auto pixels = std::make_unique<uint32_t[]>(size * size);
    
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float t = (float)y / size;
            
            uint8_t r = (uint8_t)(color1.r * (1 - t) + color2.r * t);
            uint8_t g = (uint8_t)(color1.g * (1 - t) + color2.g * t);
            uint8_t b = (uint8_t)(color1.b * (1 - t) + color2.b * t);
            uint8_t a = (uint8_t)(color1.a * (1 - t) + color2.a * t);
            
            pixels[y * size + x] = (r << 24) | (g << 16) | (b << 8) | a;
        }
    }
    
    iconCache[name] = std::move(pixels);
    iconSizes[name] = {size, size};
}

void IconLoader::generateGeometricIcon(const std::string& name, int size, const Color& color, const std::string& shape) {
    auto pixels = std::make_unique<uint32_t[]>(size * size);
    
    // Clear to transparent
    std::fill(pixels.get(), pixels.get() + size * size, 0);
    
    float centerX = size / 2.0f;
    float centerY = size / 2.0f;
    float radius = size * 0.4f;
    
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            bool draw = false;
            float dx = x - centerX;
            float dy = y - centerY;
            float dist = sqrt(dx * dx + dy * dy);
            
            if (shape == "circle") {
                draw = dist <= radius;
            } else if (shape == "ring") {
                draw = (dist <= radius && dist >= radius * 0.7f);
            } else if (shape == "diamond") {
                draw = (abs(dx) + abs(dy)) <= radius;
            } else if (shape == "star") {
                float angle = atan2(dy, dx);
                float starRadius = radius * (0.6f + 0.4f * sin(angle * 5));
                draw = dist <= starRadius;
            } else if (shape == "hexagon") {
                // Simplified hexagon
                draw = (abs(dx) <= radius * 0.866f && 
                       abs(dy) <= radius && 
                       abs(dx * 0.866f + dy * 0.5f) <= radius &&
                       abs(dx * 0.866f - dy * 0.5f) <= radius);
            }
            
            if (draw) {
                // Add some gradient effect
                float intensity = 1.0f - (dist / radius) * 0.3f;
                uint8_t r = (uint8_t)(color.r * intensity);
                uint8_t g = (uint8_t)(color.g * intensity);
                uint8_t b = (uint8_t)(color.b * intensity);
                
                pixels[y * size + x] = (r << 24) | (g << 16) | (b << 8) | color.a;
            }
        }
    }
    
    iconCache[name] = std::move(pixels);
    iconSizes[name] = {size, size};
}

// Icon Effects Implementation
namespace IconEffects {
    void drawPulsingIcon(const std::string& iconName, float x, float y, float time, 
                        float baseScale, float pulseAmount) {
        float pulse = sin(time * 3.0f) * 0.5f + 0.5f;
        float scale = baseScale + pulse * pulseAmount;
        ICON_LOADER->drawIcon(iconName, x, y, scale);
    }
    
    void drawRotatingIcon(const std::string& iconName, float x, float y, float time, 
                         float scale, float speed) {
        float rotation = time * speed;
        ICON_LOADER->drawIcon(iconName, x, y, scale, 1.0f, rotation);
    }
    
    void drawFloatingIcon(const std::string& iconName, float x, float y, float time, 
                         float amplitude, float speed) {
        float offsetY = sin(time * speed) * amplitude;
        ICON_LOADER->drawIcon(iconName, x, y + offsetY);
    }
    
    void drawGlowingIcon(const std::string& iconName, float x, float y, const Color& glowColor, 
                        float intensity, float scale) {
        // Draw glow layers
        for (int i = 3; i > 0; i--) {
            float glowScale = scale + i * 0.1f;
            float alpha = intensity * (1.0f - i * 0.3f);
            
            // This would need a colored version of the icon
            // For now, draw the regular icon with reduced alpha
            ICON_LOADER->drawIcon(iconName, x - i * 2, y - i * 2, glowScale, alpha);
        }
        
        // Draw main icon
        ICON_LOADER->drawIcon(iconName, x, y, scale);
    }
    
    void draw3DIcon(const std::string& iconName, float x, float y, float depth, 
                   float scale, float lightAngle) {
        // Draw shadow
        float shadowOffsetX = cos(lightAngle) * depth;
        float shadowOffsetY = sin(lightAngle) * depth;
        ICON_LOADER->drawIcon(iconName, x + shadowOffsetX, y + shadowOffsetY, scale, 0.3f);
        
        // Draw main icon
        ICON_LOADER->drawIcon(iconName, x, y, scale);
        
        // Draw highlight
        float highlightOffsetX = -cos(lightAngle) * (depth * 0.3f);
        float highlightOffsetY = -sin(lightAngle) * (depth * 0.3f);
        ICON_LOADER->drawIcon(iconName, x + highlightOffsetX, y + highlightOffsetY, scale, 0.5f);
    }
}

// Default Icons Implementation
namespace DefaultIcons {
    void initializeDefaultIcons() {
        createNEOVIALogo();
        createNEOVIALogoGradient();
        createSettingsIcon();
        createInfoIcon();
        createGameIcon();
        createDownloadIcon();
        createToolsIcon();
        createExitIcon();
        createSuccessIcon();
        createWarningIcon();
        createErrorIcon();
        createLoadingIcon();
    }
    
    void createNEOVIALogo() {
        ICON_LOADER->generateGeometricIcon("neovia_logo", 64, Colors::PRIMARY, "hexagon");
    }
    
    void createNEOVIALogoGradient() {
        ICON_LOADER->generateGradientIcon("neovia_gradient", 64, Colors::PRIMARY, Colors::SECONDARY);
    }
    
    void createSettingsIcon() {
        ICON_LOADER->generateGeometricIcon("settings", 32, Colors::TEXT, "star");
    }
    
    void createInfoIcon() {
        ICON_LOADER->generateGeometricIcon("info", 32, Colors::ACCENT, "circle");
    }
    
    void createGameIcon() {
        ICON_LOADER->generateGeometricIcon("game", 32, Colors::SUCCESS, "diamond");
    }
    
    void createDownloadIcon() {
        ICON_LOADER->generateGeometricIcon("download", 32, Colors::WARNING, "ring");
    }
    
    void createToolsIcon() {
        ICON_LOADER->generateGeometricIcon("tools", 32, Colors::SECONDARY, "hexagon");
    }
    
    void createExitIcon() {
        ICON_LOADER->generateGeometricIcon("exit", 32, Colors::ERROR, "circle");
    }
    
    void createSuccessIcon() {
        ICON_LOADER->generateGeometricIcon("success", 24, Colors::SUCCESS, "circle");
    }
    
    void createWarningIcon() {
        ICON_LOADER->generateGeometricIcon("warning", 24, Colors::WARNING, "diamond");
    }
    
    void createErrorIcon() {
        ICON_LOADER->generateGeometricIcon("error", 24, Colors::ERROR, "circle");
    }
    
    void createLoadingIcon() {
        ICON_LOADER->generateGeometricIcon("loading", 24, Colors::PRIMARY, "ring");
    }
}