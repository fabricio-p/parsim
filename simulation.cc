#include "simulation.hpp"

static float calcGravity(float G, float m1, float m2, float r);

Simulation::Simulation(
    std::vector<MaterialInfo> materialsTable,
    Rectangle viewport,
    float _theta,
    Vector2 _pointer
)
: tree(viewport)
, theta(_theta)
, pointer(_pointer)
, referencePoint(_pointer)
, materialsTable(materialsTable) {}

void Simulation::add(
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

size_t Simulation::size() const {
    return positions.size();
}

void Simulation::update(float dt) {
    for (float ddt = 0.f; ddt < dt; ddt += dt / 100.f) {
        tree.clear();

        buildQuadTree();

        calculateForceVectors();

        applyForces(ddt);
    }
}

void Simulation::draw() const {
    for (auto const& node : tree.nodes) {
        Rectangle bounds = node.bounds;
        Vector2 renderPosition = {bounds.x, bounds.y};
        renderPosition =
            referencePoint + (renderPosition - referencePoint) * scale;
        DrawRectangleLines(
            renderPosition.x,
            renderPosition.y,
            bounds.width * scale,
            bounds.height * scale,
            LIME
        );
    }
    auto position = positions.begin();
    auto radius = radii.begin();
    auto color = colors.begin();
    for (; position != positions.end(); position++, radius++, color++) {
        Vector2 renderPosition = *position;
        renderPosition =
            referencePoint + (renderPosition - referencePoint) * scale;
        DrawPoly(renderPosition, 30, *radius * scale, 0, *color);
        // DrawCircleV(*position, *radius, *color);
    }

    DrawRectangle(
        pointer.x - 10.f,
        pointer.y - 2.5f,
        20.f,
        5.f,
        GOLD
    );
    DrawRectangle(
        pointer.x - 2.5f,
        pointer.y - 10.f,
        5.f,
        20.f,
        GOLD
    );
}

void Simulation::buildQuadTree() {
    for (Entity e = 0; (size_t)e < size(); e++) {
        tree.insert(e, positions);
    }
}

void Simulation::calculateForceVectors() {
    for (Entity e = 0; (size_t)e < size(); e++) {
        forces[e] = calculateForceFor(e);
        if (e == 0) {
            forces[e] += externalForce;
        }
    }
}

void Simulation::applyForces(float dt) {
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

std::pair<float, Vector2> Simulation::getNodeMassInfo(QuadTree::Node::Index i) {
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

Vector2 Simulation::calculateForceFor(Entity e, QuadTree::Node::Index i) {
    Vector2 force = {0.f, 0.f};
    QuadTree::Node& node = tree.nodes.at(i);
    if (node.e == e) {
        return {0.f, 0.f};
    }
    Vector2 position = positions[e];

    float const entityRadius = radii[e];
    // float const entityArea = entityRadius * entityRadius * M_PIf;
    // assume sphere
    float const
        entityVolume =
            4.f * M_PIf * entityRadius / 3.f * entityRadius * entityRadius
    ,   entityMass =
            entityVolume * materialsTable[materials[e]].density;

    auto const massInfo = getNodeMassInfo(i);
    float const
        regionWidth = node.bounds.width
    ,   dist = abs(std::get<1>(massInfo) - position)
    ;

    if (regionWidth / dist < theta || !node.hasChildren()) {
        float const
            distX = std::get<1>(massInfo).x - position.x
        ,   distY = std::get<1>(massInfo).y - position.y
        ,   distCos = distX / dist
        ,   distSin = distY / dist
        ,   forceModulo =
                calcGravity(gamma, entityMass, std::get<0>(massInfo), dist)
        ;
        force.y = forceModulo * distSin;
        force.x = forceModulo * distCos;
    } else {
        for (auto const childDesc : node) {
            force = force + calculateForceFor(e, childDesc);
        }
    }
    return force;
}

static float calcGravity(float G, float m1, float m2, float r) {
    // return G * m1 * m2 / (r * r);
    return G * m1 / r * m2 / r;
}