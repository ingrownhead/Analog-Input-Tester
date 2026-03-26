#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>

// Display layout
#define OUTLINE         3
#define DISPLAY_RADIUS  80.0f
#define DOT_RADIUS      6.0f
#define CENTER_DOT_R    3.0f
#define CROSSHAIR_W     2.0f

// Circle pad — left side of bottom screen
#define CPAD_CENTER_X   80.0f
#define CPAD_CENTER_Y   120.0f
#define CPAD_MAX        155.0f

// C-stick — right side of bottom screen
#define CSTICK_CENTER_X 240.0f
#define CSTICK_CENTER_Y 120.0f
#define CSTICK_MAX      146.0f

int main() {
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    PrintConsole topScreen;
    consoleInit(GFX_TOP, &topScreen);
    consoleSelect(&topScreen);

    C3D_RenderTarget* bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    u32 gray   = C2D_Color32(128, 128, 128, 255);
    u32 black  = C2D_Color32(  0,   0,   0, 255);
    u32 red    = C2D_Color32(255,   0,   0, 255);
    u32 white  = C2D_Color32(255, 255, 255, 255);
    u32 green  = C2D_Color32(  0, 255,   0, 255);
    u32 yellow = C2D_Color32(255, 255,   0, 255);

    printf("=== Input Viewer v2.3 (C++) ===\n\n");
    printf("C-Stick X:    \n");
    printf("C-Stick Y:    \n\n");
    printf("Circle Pad X: \n");
    printf("Circle Pad Y: \n\n");
    printf("Touch X:      \n");
    printf("Touch Y:      \n\n");
    printf("Buttons:      \n\n");
    printf("Press START to exit\n");

    while (aptMainLoop()) {
        hidScanInput();

        circlePosition cstick, cpad;
        hidCstickRead(&cstick);
        hidCircleRead(&cpad);
        
        touchPosition touch;
        hidTouchRead(&touch);

        u32 keys = hidKeysHeld();
        if (keys & KEY_START) break;

        // --- Top screen updates (Shifted down by 1 row) ---
        printf("\x1b[3;15H%4d  ", cstick.dx);
        printf("\x1b[4;15H%4d  ", cstick.dy);
        printf("\x1b[6;15H%4d  ", cpad.dx);
        printf("\x1b[7;15H%4d  ", cpad.dy);
        
        // Only show touch coords if actually touching
        if (keys & KEY_TOUCH) {
            printf("\x1b[9;15H%4d  ", touch.px);
            printf("\x1b[10;15H%4d  ", touch.py);
        } else {
            printf("\x1b[9;15H----  ");
            printf("\x1b[10;15H----  ");
        }
        
        printf("\x1b[12;15H%08lX", keys);

        // Map raw input values
        float cpadDotX   = CPAD_CENTER_X   + (cpad.dx   / CPAD_MAX)   * DISPLAY_RADIUS;
        float cpadDotY   = CPAD_CENTER_Y   - (cpad.dy   / CPAD_MAX)   * DISPLAY_RADIUS;
        float cstickDotX = CSTICK_CENTER_X + (cstick.dx / CSTICK_MAX) * DISPLAY_RADIUS;
        float cstickDotY = CSTICK_CENTER_Y - (cstick.dy / CSTICK_MAX) * DISPLAY_RADIUS;

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(bot, black);
        C2D_SceneBegin(bot);

        float z_bg     = 0.1f;
        float z_dots   = 0.4f;
        float z_lines  = 0.7f;
        float z_center = 0.9f;

        // === CIRCLE PAD ===
        C2D_DrawCircleSolid(CPAD_CENTER_X, CPAD_CENTER_Y, z_bg, DISPLAY_RADIUS, gray);
        C2D_DrawCircleSolid(CPAD_CENTER_X, CPAD_CENTER_Y, z_bg, DISPLAY_RADIUS - OUTLINE, black);
        C2D_DrawCircleSolid(cpadDotX, cpadDotY, z_dots, DOT_RADIUS, red);
        C2D_DrawRectSolid(CPAD_CENTER_X - DISPLAY_RADIUS, CPAD_CENTER_Y - CROSSHAIR_W / 2, z_lines, DISPLAY_RADIUS * 2, CROSSHAIR_W, white);
        C2D_DrawRectSolid(CPAD_CENTER_X - CROSSHAIR_W / 2, CPAD_CENTER_Y - DISPLAY_RADIUS, z_lines, CROSSHAIR_W, DISPLAY_RADIUS * 2, white);
        C2D_DrawCircleSolid(CPAD_CENTER_X, CPAD_CENTER_Y, z_center, CENTER_DOT_R, green);

        // === C-STICK ===
        float sx = CSTICK_CENTER_X - DISPLAY_RADIUS;
        float sy = CSTICK_CENTER_Y - DISPLAY_RADIUS;
        float sw = DISPLAY_RADIUS * 2;
        C2D_DrawRectSolid(sx, sy, z_bg, sw, sw, gray);
        C2D_DrawRectSolid(sx + OUTLINE, sy + OUTLINE, z_bg, sw - OUTLINE * 2, sw - OUTLINE * 2, black);
        C2D_DrawCircleSolid(cstickDotX, cstickDotY, z_dots, DOT_RADIUS, red);
        C2D_DrawRectSolid(CSTICK_CENTER_X - DISPLAY_RADIUS, CSTICK_CENTER_Y - CROSSHAIR_W / 2, z_lines, DISPLAY_RADIUS * 2, CROSSHAIR_W, white);
        C2D_DrawRectSolid(CSTICK_CENTER_X - CROSSHAIR_W / 2, CSTICK_CENTER_Y - DISPLAY_RADIUS, z_lines, CROSSHAIR_W, DISPLAY_RADIUS * 2, white);
        C2D_DrawCircleSolid(CSTICK_CENTER_X, CSTICK_CENTER_Y, z_center, CENTER_DOT_R, green);

        // === TOUCH INDICATOR ===
        if (keys & KEY_TOUCH) {
            C2D_DrawCircleSolid(touch.px, touch.py, 1.0f, DOT_RADIUS, yellow);
        }

        C3D_FrameEnd(0);
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
