#pragma once
#include <switch.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "graphics.h"

// Simple JPEG decoder for Nintendo Switch
class IconLoader {
private:
    static IconLoader* instance;
    std::unordered_map<std::string, std::unique_ptr<uint32_t[]>> iconCache;
    std::unordered_map<std::string, std::pair<int, int>> iconSizes;
    
    // Simple JPEG header parsing
    struct JPEGInfo {
        int width;
        int height;
        bool valid;
    };
    
    JPEGInfo parseJPEGHeader(const uint8_t* data, size_t size);
    std::unique_ptr<uint32_t[]> decodeJPEG(const uint8_t* data, size_t size, int& width, int& height);
    
public:
    static IconLoader* getInstance();
    
    // Load icon from file
    bool loadIcon(const std::string& path, const std::string& name);
    
    // Load icon from memory
    bool loadIconFromMemory(const uint8_t* data, size_t size, const std::string& name);
    
    // Get icon data
    uint32_t* getIcon(const std::string& name, int& width, int& height);
    
    // Check if icon exists
    bool hasIcon(const std::string& name);
    
    // Clear cache
    void clearCache();
    
    // Draw icon with scaling and effects
    void drawIcon(const std::string& name, float x, float y, float scale = 1.0f, 
                  float alpha = 1.0f, float rotation = 0.0f);
    
    // Draw icon with custom size
    void drawIconScaled(const std::string& name, float x, float y, float width, float height, 
                       float alpha = 1.0f);
    
    // Create default NEOVIA icon if no external icon is provided
    void createDefaultIcon();
    
    // Generate beautiful gradient icons programmatically
    void generateGradientIcon(const std::string& name, int size, const Color& color1, const Color& color2);
    
    // Generate geometric icons
    void generateGeometricIcon(const std::string& name, int size, const Color& color, const std::string& shape);
};

// Icon effects and animations
namespace IconEffects {
    // Pulsing icon effect
    void drawPulsingIcon(const std::string& iconName, float x, float y, float time, 
                        float baseScale = 1.0f, float pulseAmount = 0.2f);
    
    // Rotating icon effect
    void drawRotatingIcon(const std::string& iconName, float x, float y, float time, 
                         float scale = 1.0f, float speed = 1.0f);
    
    // Floating icon effect
    void drawFloatingIcon(const std::string& iconName, float x, float y, float time, 
                         float amplitude = 10.0f, float speed = 2.0f);
    
    // Glowing icon effect
    void drawGlowingIcon(const std::string& iconName, float x, float y, const Color& glowColor, 
                        float intensity = 0.5f, float scale = 1.0f);
    
    // Icon with particle trail
    void drawIconWithTrail(const std::string& iconName, float x, float y, float time, 
                          const Color& trailColor, int particleCount = 8);
    
    // 3D-style icon with shadow and highlight
    void draw3DIcon(const std::string& iconName, float x, float y, float depth = 5.0f, 
                   float scale = 1.0f, float lightAngle = 45.0f);
}

// Predefined beautiful icons
namespace DefaultIcons {
    // Create beautiful default icons
    void initializeDefaultIcons();
    
    // NEOVIA logo variations
    void createNEOVIALogo();
    void createNEOVIALogoGradient();
    void createNEOVIALogoGlow();
    
    // UI icons
    void createSettingsIcon();
    void createInfoIcon();
    void createGameIcon();
    void createDownloadIcon();
    void createToolsIcon();
    void createExitIcon();
    
    // Status icons
    void createSuccessIcon();
    void createWarningIcon();
    void createErrorIcon();
    void createLoadingIcon();
}

#define ICON_LOADER IconLoader::getInstance()