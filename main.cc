#include <cmath>
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <functional>
#include <chrono>
#include <thread>

#include <raylib.h>

#include "quad_tree.hpp"
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

float calcGravity(float G, float m1, float m2, float r) {
    // return G * m1 * m2 / (r * r);
    return G * m1 / r * m2 / r;
}

struct MaterialInfo {
    std::string name;
    float density;
};

class Simulation {
public:
    std::vector<Vector2> positions;
    std::vector<float>   radii;
    std::vector<Vector2> velocities;
    std::vector<size_t>  materials;
    std::vector<Color>   colors;
    std::vector<Vector2> forces;

    QuadTree tree;
    float theta;
    float gamma = 6.674e-10;
    float radiusScale = 1.f;
    Vector2 externalForce = {0.f, 0.f};

    std::vector<MaterialInfo> materialsTable;

    Simulation(
        std::vector<MaterialInfo> materialsTable,
        Rectangle viewport,
        float _theta
    ) : tree(viewport), theta(_theta), materialsTable(materialsTable) {}

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
        forces.push_back({0, 0});
    }
    size_t size() const {
        return positions.size();
    }

    void update(float dt) {
        for (float ddt = 0.f; ddt < dt; ddt += dt / 100.f) {
            tree.clear();

            buildQuadTree();

            calculateForceVectors();

            applyForces(ddt);
        }
    }
    void draw() const {
        for (auto const& node : tree.nodes) {
            Rectangle bounds = node.bounds;
            DrawRectangleLines(
                bounds.x,
                bounds.y,
                bounds.width,
                bounds.height,
                LIME
            );
        }
        auto position = positions.begin();
        auto radius = radii.begin();
        auto color = colors.begin();
        for (; position != positions.end(); position++, radius++, color++) {
            DrawPoly(*position, 30, *radius * radiusScale, 0, *color);
            // DrawCircleV(*position, *radius, *color);
        }
    }

private:
    void buildQuadTree() {
        for (Entity e = 0; (size_t)e < size(); e++) {
            tree.insert(e, positions);
        }
    }

    void calculateForceVectors() {
        for (Entity e = 0; (size_t)e < size(); e++) {
            forces[e] = calculateForceFor(e);
            if (e == 0) {
                forces[e] += externalForce;
            }
        }
    }

    void applyForces(float dt) {
        for (Entity e = 0; (size_t)e < size(); e++) {
            Vector2 const force = forces[e];
            Vector2
                &position = positions[e]
            ,   &velocity = velocities[e]
            ;
            float const
                r       = radii[e]
            ,   area    = M_PIf * r * r
            ,   density = materialsTable[materials[e]].density
            ,   mass    = area * density
            ;
            // F = ma
            Vector2 acceleration = force / mass;
            position += velocity * dt;
            velocity += acceleration * dt;
        }
    }

    //       [ mass, center ]
    std::pair<float, Vector2> getNodeMassInfo(QuadTree::Node::Index i) {
        QuadTree::Node& node = tree.nodes.at(i);
        float mass = 0.f;
        Vector2 center = {0.f, 0.f};
        if (node.mass == -1 && !node.hasChildren()) {
            float r = radii[node.e];
            float area = M_PIf * r * r;
            node.mass = area * materialsTable[materials[node.e]].density;
            node.massCenter = positions[node.e];
        }
        if (node.mass != -1) {
            mass = node.mass;
            center = node.massCenter;
        } else if (node.hasChildren()) {
            for (auto const childDesc : node) {
                auto const childMassInfo = getNodeMassInfo(childDesc);
                mass += std::get<0>(childMassInfo);
                center =
                    std::get<1>(childMassInfo) * std::get<0>(childMassInfo)
                    + center;
            }
            center = center / mass;
            node.mass = mass;
            node.massCenter = center;
        }
        return {mass, center};
    }

    Vector2 calculateForceFor(Entity e, QuadTree::Node::Index i = 0) {
        Vector2 force = {0.f, 0.f};
        QuadTree::Node& node = tree.nodes.at(i);
        if (node.e == e) {
            return {0.f, 0.f};
        }
        Vector2 position = positions[e];

        float const entityRadius = radii[e];
        // float const entityArea = entityRadius * entityRadius * M_PIf;
        // assume sphere
        float const entityVolume =
            4.f * M_PIf * entityRadius / 3.f * entityRadius * entityRadius;
        float const entityMass =
            entityVolume * materialsTable[materials[e]].density;

        auto const massInfo = getNodeMassInfo(i);
        float const regionWidth = node.bounds.width;
        float const dist = abs(std::get<1>(massInfo) - position);

        if (regionWidth / dist < theta || !node.hasChildren()) {
            float const
                distX = std::get<1>(massInfo).x - position.x
            ,   distY = std::get<1>(massInfo).y - position.y
            ,   distCos = distX / dist
            ,   distSin = distY / dist
            ,   forceModulo =
                    calcGravity(gamma, entityMass, std::get<0>(massInfo), dist);
            force.y = forceModulo * distSin;
            force.x = forceModulo * distCos;
        } else {
            for (auto const childDesc : node) {
                force = force + calculateForceFor(e, childDesc);
            }
        }
        return force;
    }
};

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

    Vector2 offset = {300.f, 20.f};
    float velocityScale = 7.5f;

    simulation.add(
        Vector2 {100.f, 300.f} + offset,
        Vector2 {0.f, 0.f} * velocityScale,
        60.f,
        YELLOW,
        0
    );

    simulation.add(
        Vector2 {200.f, 300.f} + offset,
        Vector2 {0.f, -1.5f} * velocityScale,
        6.f,
        GRAY,
        1
    );

    simulation.add(
        Vector2 {300.f, 300.f} + offset,
        Vector2 {0.f, -1.6f} * velocityScale,
        15.f,
        RED,
        1
    );

    simulation.add(
        Vector2 {400.f, 300.f} + offset,
        Vector2 {0.f, -1.6f} * velocityScale,
        20.f,
        BLUE,
        1
    );

    if (!IsWindowReady()) {
        return 1;
    }
    while (!WindowShouldClose()) {
        struct {
            size_t zoomIn : 1;
            size_t zoomOut : 1;
            size_t up : 1;
            size_t down : 1;
            size_t left : 1;
            size_t right : 1;
        } commands = {0, 0, 0, 0, 0, 0};
        commands.zoomIn =
            IsKeyDown(KEY_EQUAL) && IsKeyDown(KEY_LEFT_SHIFT);
        commands.zoomOut =
            IsKeyDown(KEY_MINUS) && IsKeyDown(KEY_LEFT_SHIFT);
        commands.up = IsKeyDown(KEY_UP);
        commands.down = IsKeyDown(KEY_DOWN);
        commands.left = IsKeyDown(KEY_LEFT);
        commands.right = IsKeyDown(KEY_RIGHT);

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
        BeginDrawing();
        simulation.update(1.f / 200.f);
        ClearBackground(RAYWHITE);
        simulation.draw();
        EndDrawing();
    }
    return 0;
}