#pragma once

#include <switch.h>

// Функции UI
Result initializeUI();
void finalizeUI();
void updateUI();
void handleUIInput(u64 kDown);
void renderUI();

// Внутренние функции отрисовки
void drawHeader();
void drawMainMenu();
void drawProgressWindow();