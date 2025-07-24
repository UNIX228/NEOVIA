#pragma once

#include <switch.h>

Result initializeUI();
void finalizeUI();
void updateUI();
void handleUIInput(u64 kDown);
void renderUI();
void drawHeader();
void drawMainMenu();
void drawProgressWindow();
