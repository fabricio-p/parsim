#include <cmath>
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <functional>
#include <sstream>
#include <iomanip>

#include <raylib.h>

#include "simulation.hpp"
#include "util.hpp"

#include <iostream>

#define DEBUGGING

#define DUMP_FR(expr)                                               \
    std::cerr<<"["<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"] "\
             <<(#expr)<<" = "<<(expr)<<std::endl;

#ifdef DEBUGGING
#define DUMP(expr) DUMP_FR(expr)
#else
#define DUMP(expr)
#endif

// PHYSICS CONSTANTS {{{
// PHYSICS CONSTANTS }}}

float calcMass(float radius, float density) {
    return M_PI * radius * radius * density;
}

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetWindowMonitor(GetCurrentMonitor());
    InitWindow(900, 700, "Particle simulator");
    SetTargetFPS(60);
    SetWindowMonitor(GetCurrentMonitor());

    Rectangle viewport;
    viewport.x = -GetScreenWidth() * 3.f;
    viewport.y = -GetScreenHeight() * 3.f;
    viewport.width = GetScreenWidth() * 6.f;
    viewport.height = GetScreenHeight() * 6.f;
    Simulation simulation(
        {
            MaterialInfo {"A", 2.e8f},
            MaterialInfo {"B", 1.5e3f},
        },
        viewport,
        0.5f,
        Vector2 {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f}
    );
    simulation.scale = 0.2f;

    Vector2 center = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
    // float velocityScale = 7.5f;

    simulation.add(
        center,
        Vector2 {0.f, 0.f},
        250.f,
        YELLOW,
        0
    );

    simulation.add(
        center + Vector2 {200.f + 400.f, 0.f},
        Vector2 {0.f, -25.f},
        10.f,
        GRAY,
        1
    );

    simulation.add(
        center + Vector2 {200.f + 600.f, 0.f},
        Vector2 {0.f, -35.f},
        22.f,
        RED,
        1
    );

    simulation.add(
        center + Vector2 {200.f + 1500.f, 0.f},
        Vector2 {0.f, -25.f},
        30.f,
        BLUE,
        1
    );

    if (!IsWindowReady()) {
        return 1;
    }

    float dt = 1.f / 200.f;
    while (!WindowShouldClose()) {
        // TODO: Must be `Simulation::Commands`, put the construction in a
        //       function
        struct {
            size_t zoomIn : 1;
            size_t zoomOut : 1;
            size_t up : 1;
            size_t down : 1;
            size_t left : 1;
            size_t right : 1;
            size_t stop : 1;
            size_t faster : 1;
            size_t slower : 1;
            size_t ptrUp : 1;
            size_t ptrDown : 1;
            size_t ptrLeft : 1;
            size_t ptrRight : 1;
        } commands = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        commands.zoomIn =
            IsKeyDown(KEY_EQUAL) && IsKeyDown(KEY_LEFT_SHIFT);
        commands.zoomOut =
            IsKeyDown(KEY_MINUS) && IsKeyDown(KEY_LEFT_SHIFT);
        commands.up = IsKeyDown(KEY_UP);
        commands.down = IsKeyDown(KEY_DOWN);
        commands.left = IsKeyDown(KEY_LEFT);
        commands.right = IsKeyDown(KEY_RIGHT);
        commands.stop = IsKeyDown(KEY_SPACE);
        commands.faster = IsKeyDown(KEY_RIGHT_BRACKET);
        commands.slower = IsKeyDown(KEY_LEFT_BRACKET);
        commands.ptrUp = IsKeyDown(KEY_W);
        commands.ptrDown = IsKeyDown(KEY_S);
        commands.ptrLeft = IsKeyDown(KEY_A);
        commands.ptrRight = IsKeyDown(KEY_D);

        // TODO: Move this inside the simulation
        constexpr float const
            forceAmount = 1.e14f
        ,   pointerDX = 5.f
        ;
        simulation.externalForce = {0.f, 0.f};
        if (commands.zoomIn) {
            simulation.scale += 0.01f;
        }
        if (commands.zoomOut) {
            simulation.scale -= 0.01f;
        }
        if (commands.up) {
            simulation.externalForce.y -= forceAmount;
        }
        if (commands.down) {
            simulation.externalForce.y += forceAmount;
        }
        if (commands.left) {
            simulation.externalForce.x -= forceAmount;
        }
        if (commands.right) {
            simulation.externalForce.x += forceAmount;
        }

        if (commands.stop) {
            simulation.externalForce = {0.f, 0.f};
            simulation.velocities[0] = {0.f, 0.f};
        }
        if (commands.faster) {
            dt *= 1.1f;
        }
        if (commands.slower) {
            dt /= 1.2f;
        }

        if (commands.ptrUp) {
            simulation.pointer.y -= pointerDX;
        }
        if (commands.ptrDown) {
            simulation.pointer.y += pointerDX;
        }
        if (commands.ptrLeft) {
            simulation.pointer.x -= pointerDX;
        }
        if (commands.ptrRight) {
            simulation.pointer.x += pointerDX;
        }

        if (commands.zoomIn || commands.zoomOut) {
            simulation.referencePoint = simulation.pointer;
        }

        std::ostringstream ss;
        ss<<"dt = "<<std::fixed<<std::setprecision(6)<<dt;
        std::string dtMsg = ss.str();
        ss.seekp(0);
        ss.clear();
        ss<<"scale = "<<std::fixed<<std::setprecision(2)<<simulation.scale;
        std::string scaleMsg = ss.str();
        BeginDrawing();
        simulation.update(dt);
        ClearBackground(RAYWHITE);
        simulation.draw();
        DrawText(dtMsg.c_str(), 5, 0, 20, BLACK);
        DrawText(scaleMsg.c_str(), 5, 23, 20, BLACK);
        EndDrawing();
    }
    return 0;
}