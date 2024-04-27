#include <cmath>
#include <vector>

#include <raylib.h>

#include "util.hpp"

float calcMass(float radius, float density) {
    return M_PI * radius * radius * density;
}

struct MaterialInfo {
    float density;
};

class Simulation {
public:
    std::vector<Vector2> positions;
    std::vector<float>   radii;
    std::vector<Vector2> velocities;
    std::vector<size_t>  materials;
    std::vector<Color>   colors;

    std::vector<MaterialInfo> materialsTable;

    Simulation(std::vector<MaterialInfo> materialsTable)
    : materialsTable(materialsTable) {}

    void add(
        Vector2 position,
        Vector2 velocity,
        float radius,
        Color color,
        size_t material
    ) {
        positions.push_back(position);
        velocities.push_back(velocity);
        radii.push_back(radius);
        colors.push_back(color);
        materials.push_back(material);
    }
    size_t size() const {
        return positions.size();
    }
    void update(float dt) {
        auto position = positions.begin();
        auto radius = radii.begin();
        auto velocity = velocities.begin();
        // auto material = materials.begin();

        for (; position != positions.end(); position++, radius++, velocity++) {
            *position += *velocity * dt;
            // *velocity += Vector2 {0.0f, 9.81f} * dt;
            // material++;
         }
    }
    void draw() const {
        auto position = positions.begin();
        auto radius = radii.begin();
        auto color = colors.begin();
        for (; position != positions.end(); position++, radius++, color++) {
            DrawCircleV(*position, *radius, *color);
        }
    }
};

int main(void) {
    Simulation simulation({ MaterialInfo {1.f} });
    simulation.add(Vector2 {0.f, 0.f}, Vector2 {10.f, 10.f}, 8.f, RED, 0);
    simulation.add(
        Vector2 {100.f, 200.f},
        Vector2 {0.f, -20.f},
        10.f,
        GREEN,
        0
    );
    InitWindow(640, 480, "Particle simulator");
    SetTargetFPS(60);
    if (!IsWindowReady()) {
        return 1;
    }
    while (!WindowShouldClose()) {
        BeginDrawing();
        for (size_t i = 0; i < 10; i++) {
            simulation.update(0.01f);
        }
        ClearBackground(RAYWHITE);
        simulation.draw();
        EndDrawing();
    }
    return 0;
}