#include <cmath>
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <functional>
#include <sstream>

#include <raylib.h>

#include "simulation.hpp"
#include "util.hpp"

#include <iostream>

// #define DEBUGGING

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
    viewport.x = -500.f;
    viewport.y = -500.f;
    viewport.width = GetScreenWidth() + 500.f;
    viewport.height = GetScreenHeight() + 500.f;
    Simulation simulation(
        {
            MaterialInfo {"A", 2.e8f},
            MaterialInfo {"B", 1.5e3f},
        },
        viewport,
        0.5f
    );

    Vector2 offset = {450.f, 20.f};
    float velocityScale = 7.5f;

    simulation.add(
        Vector2 {100.f, 300.f} + offset,
        Vector2 {0.f, 0.f} * velocityScale,
        60.f,
        YELLOW,
        0
    );

    simulation.add(
        Vector2 {220.f, 300.f} + offset,
        Vector2 {0.f, -1.5f} * velocityScale,
        6.f,
        GRAY,
        1
    );

    simulation.add(
        Vector2 {320.f, 300.f} + offset,
        Vector2 {0.f, -1.6f} * velocityScale,
        15.f,
        RED,
        1
    );

    simulation.add(
        Vector2 {175.f, 300.f} + offset,
        Vector2 {0.f, -2.88} * velocityScale,
        10.f,
        GREEN,
        1
    );

    simulation.add(
        Vector2 {420.f, 300.f} + offset,
        Vector2 {0.f, -1.6f} * velocityScale,
        20.f,
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
        } commands = {0, 0, 0, 0, 0, 0, 0, 0, 0};
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

        // TODO: Move this inside the simulation
        if (commands.zoomIn) {
            simulation.radiusScale += 0.01f;
        }
        if (commands.zoomOut) {
            simulation.radiusScale -= 0.01f;
        }
        constexpr float const forceAmount = 1.e12f;
        simulation.externalForce = {0.f, 0.f};
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
        std::ostringstream ss;
        ss<<"dt = "<<dt;
        std::string message = ss.str();
        BeginDrawing();
        simulation.update(dt);
        ClearBackground(RAYWHITE);
        simulation.draw();
        DrawText(message.c_str(), 0, 0, 20, BLACK);
        EndDrawing();
    }
    return 0;
}