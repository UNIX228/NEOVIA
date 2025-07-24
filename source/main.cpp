#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include "neovia.h"

bool g_exitRequested = false;

Result initializeApp() {
    Result rc = 0;
    
    rc = romfsInit();
    if (R_FAILED(rc)) return rc;
    
    rc = socketInitializeDefault();
    if (R_FAILED(rc)) return rc;
    
    rc = nsInitialize();
    if (R_FAILED(rc)) return rc;
    
    rc = fsInitialize();
    if (R_FAILED(rc)) return rc;
    
    return 0;
}

void finalizeApp() {
    fsExit();
    nsExit();
    socketExit();
    romfsExit();
}

Result createDirectoryStructure() {
    Result rc = 0;
    
    rc = fsMkdir(fsdevGetDeviceFileSystem("sdmc"), "/graphics");
    if (R_FAILED(rc) && rc != 0x402) {
        printf("Failed to create /graphics directory: 0x%x\n", rc);
        return rc;
    }
    
    rc = fsMkdir(fsdevGetDeviceFileSystem("sdmc"), "/switch");
    if (R_FAILED(rc) && rc != 0x402) return rc;
    
    rc = fsMkdir(fsdevGetDeviceFileSystem("sdmc"), "/switch/NEOVIA");
    if (R_FAILED(rc) && rc != 0x402) return rc;
    
    return 0;
}

int main(int argc, char* argv[]) {
    consoleInit(NULL);
    
    Result rc = initializeApp();
    if (R_FAILED(rc)) {
        printf("Ошибка инициализации: 0x%x\n", rc);
        printf("Нажмите + для выхода.\n");
        
        while (appletMainLoop()) {
            hidScanInput();
            u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
            if (kDown & KEY_PLUS) break;
            consoleUpdate(NULL);
        }
        
        consoleExit(NULL);
        return 1;
    }
    
    printf("🟪 NEOVIA v%s by %s\n", NEOVIA_VERSION, NEOVIA_AUTHOR);
    printf("================================\n");
    printf("Максимальная лучшая графика, без разгона!\n");
    printf("Все в одной папке графика (в корне SD карты)\n\n");
    
    rc = createDirectoryStructure();
    if (R_SUCCEEDED(rc)) {
        printf("✅ Папка /graphics/ создана!\n");
        printf("✅ NEOVIA готов к работе!\n\n");
    }
    
    printf("Нажмите + для выхода.\n");
    
    while (appletMainLoop() && !g_exitRequested) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        
        if (kDown & KEY_PLUS) {
            g_exitRequested = true;
        }
        
        consoleUpdate(NULL);
    }
    
    finalizeApp();
    consoleExit(NULL);
    
    return 0;
}
