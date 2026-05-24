#include "raylib.h"
#include "rlgl.h"
#include <cmath>

int main()
{
    InitWindow(1300, 800, "Hands-on 4, Actividad Integradora - Movimientos Basicos en 3D");

    Camera3D camera = { 0 };
    camera.position = { 0.0f, 18.0f, 18.0f };
    camera.target   = { 0.0f,  0.0f,  0.0f };
    camera.up       = { 0.0f,  1.0f,  0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);

    // --- variables rebote ---
    float reboteY    =  1.0f;
    float velocidadY =  0.08f;

    // --- variables traslacion ---
    float trasX = -2.5f;

    // --- variables rotacion ---
    float angulo = 0.0f;

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_ORBITAL);

        float t = (float)GetTime();

        // 4. Senoidal
        float senoY = 2.0f + sinf(t * 2.0f) * 1.5f;

        // 3. Rebote
        reboteY += velocidadY;
        if (reboteY > 4.0f || reboteY < 1.0f) velocidadY *= -1.0f;

        // 6. Órbita
        float radio   = 2.0f;
        float orbitaX = cosf(t) * radio;
        float orbitaZ = sinf(t) * radio;

        // 5. Trayectoria ondulada
        float trayectoriaX = -2.5f + fmodf(t * 1.5f, 5.0f);
        float trayectoriaZ = sinf(t * 3.0f) * 1.2f;

        // 1. Traslación
        trasX += 0.03f;
        if (trasX > 2.5f) trasX = -2.5f;

        // 2. Rotación
        angulo += 1.0f;

        // ── Distribución de zonas ─────────────────────────────────────
        // FILA FRONTAL  (Z = 4)
        //   Zona 1 · Traslación  : X = -9
        //   Zona 2 · Rotación    : X = -3
        //   Zona 3 · Rebote      : X =  3
        // FILA TRASERA  (Z = -4)
        //   Zona 4 · Senoidal    : X = -6
        //   Zona 5 · Trayectoria : X =  0
        //   Zona 6 · Órbita      : X =  6

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawGrid(30, 1.0f);

        // ════════════════════════════════════════════════════════════════
        //  FILA FRONTAL  (Z = 4)
        // ════════════════════════════════════════════════════════════════

        // ── Zona 1 · Traslación ────────────────────────────────────────
        DrawCube({ -9.0f, 0.05f, 4.0f }, 5.0f, 0.1f, 5.0f, LIGHTGRAY);
        DrawLine3D({ -9.0f - 2.5f, 1.0f, 4.0f },
                   { -9.0f + 2.5f, 1.0f, 4.0f }, PURPLE);
        DrawSphere({ -9.0f + trasX, 1.0f, 4.0f }, 0.45f, PURPLE);

        // ── Zona 2 · Rotación ──────────────────────────────────────────
        DrawCube({ -3.0f, 0.05f, 4.0f }, 5.0f, 0.1f, 5.0f, LIGHTGRAY);
        DrawSphere({ -3.0f, 1.0f, 4.0f }, 0.15f, MAROON);
        rlPushMatrix();
            rlTranslatef(-3.0f, 1.0f, 4.0f);
            rlRotatef(angulo, 0.0f, 1.0f, 0.0f);
            DrawSphere({ 2.0f, 0.0f, 0.0f }, 0.45f, MAROON);
            DrawLine3D({ 0.0f, 0.0f, 0.0f }, { 2.0f, 0.0f, 0.0f }, MAROON);
        rlPopMatrix();
        DrawSphereWires({ -3.0f, 1.0f, 4.0f }, 2.0f, 24, 6, MAROON);

        // ── Zona 3 · Rebote ────────────────────────────────────────────
        DrawCube({ 3.0f, 0.05f, 4.0f }, 5.0f, 0.1f, 5.0f, LIGHTGRAY);
        DrawSphere({ 3.0f, reboteY, 4.0f }, 0.45f, RED);
        DrawCube({ 3.0f, 0.15f, 4.0f }, 2.0f, 0.3f, 2.0f, GRAY);
        DrawLine3D({ 3.0f, 0.3f, 4.0f }, { 3.0f, 4.0f, 4.0f }, RED);

        // ════════════════════════════════════════════════════════════════
        //  FILA TRASERA  (Z = -4)
        // ════════════════════════════════════════════════════════════════

        // ── Zona 4 · Senoidal ──────────────────────────────────────────
        DrawCube({ -6.0f, 0.05f, -4.0f }, 5.0f, 0.1f, 5.0f, LIGHTGRAY);
        DrawSphere({ -6.0f, senoY, -4.0f }, 0.45f, BLUE);
        DrawLine3D({ -6.0f, 0.0f, -4.0f }, { -6.0f, 4.0f, -4.0f }, BLUE);
        DrawSphere({ -6.0f, 4.0f, -4.0f }, 0.12f, BLUE);
        DrawSphere({ -6.0f, 1.0f, -4.0f }, 0.12f, BLUE);

        // ── Zona 5 · Trayectoria ───────────────────────────────────────
        DrawCube({ 0.0f, 0.05f, -4.0f }, 5.0f, 0.1f, 5.0f, LIGHTGRAY);
        for (int i = 0; i < 80; i++)
        {
            float x1 = -2.5f +  i      * (5.0f / 80.0f);
            float x2 = -2.5f + (i + 1) * (5.0f / 80.0f);
            float z1 = sinf(x1 * 3.0f) * 1.2f;
            float z2 = sinf(x2 * 3.0f) * 1.2f;
            DrawLine3D({ 0.0f + x1, 1.0f, -4.0f + z1 },
                       { 0.0f + x2, 1.0f, -4.0f + z2 }, ORANGE);
        }
        DrawSphere({ 0.0f + trayectoriaX, 1.0f, -4.0f + trayectoriaZ }, 0.45f, ORANGE);

        // ── Zona 6 · Órbita ────────────────────────────────────────────
        DrawCube({ 6.0f, 0.05f, -4.0f }, 5.0f, 0.1f, 5.0f, LIGHTGRAY);
        DrawSphere({ 6.0f, 1.0f, -4.0f }, 0.2f, DARKGREEN);
        DrawSphereWires({ 6.0f, 1.0f, -4.0f }, radio, 24, 24, DARKGREEN);
        DrawSphere({ 6.0f + orbitaX, 1.0f, -4.0f + orbitaZ }, 0.45f, GREEN);

        EndMode3D();

        // ── HUD ────────────────────────────────────────────────────────
        DrawText("Hands-on 4, Actividad Integradora", 20, 20, 30, DARKGRAY);

        // Fila 1 (frontal)
        DrawText("1. Traslacion",               80,  90, 22, PURPLE);
        DrawText("Desplazamiento lineal en X",  80, 115, 17, DARKGRAY);

        DrawText("2. Rotacion",                370,  90, 22, MAROON);
        DrawText("Gira sobre el eje Y",        370, 115, 17, DARKGRAY);

        DrawText("3. Rebote",                  660,  90, 22, RED);
        DrawText("Cambia direccion en limites",660, 115, 17, DARKGRAY);

        // Fila 2 (trasera)
        DrawText("4. Senoidal",                80, 155, 22, BLUE);
        DrawText("Sube y baja suavemente",     80, 180, 17, DARKGRAY);

        DrawText("5. Trayectoria",            370, 155, 22, ORANGE);
        DrawText("Sigue un camino ondulado",  370, 180, 17, DARKGRAY);

        DrawText("6. Orbita",                 660, 155, 22, GREEN);
        DrawText("Gira alrededor de un centro", 660, 180, 17, DARKGRAY);

        DrawText("La escena muestra 6 tipos de movimiento en 3D.", 20, 730, 20, GRAY);
        DrawText("ESC para salir", 20, 758, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}