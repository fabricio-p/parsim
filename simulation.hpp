#ifndef PARSIM_SIMULATION_H
#define PARSIM_SIMULATION_H

#include <vector>
#include <string>

#include "common.hpp"
#include "util.hpp"
#include "quad_tree.hpp"

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
    Vector2 pointer;
    Vector2 referencePoint = pointer;
    float gamma = 6.674e-10;
    float scale = 1.f;
    Vector2 externalForce = {0.f, 0.f};

    std::vector<MaterialInfo> materialsTable;

    Simulation(
        std::vector<MaterialInfo> materialsTable,
        Rectangle viewport,
        float _theta,
        Vector2 _pointer = {0.f, 0.f}
    );
    void add(
        Vector2 position,
        Vector2 velocity,
        float radius,
        Color color,
        size_t material
    );
    size_t size() const;
    void update(float dt);
    void draw() const;

private:
    void buildQuadTree();
    void calculateForceVectors();
    void applyForces(float dt);
    //       [ mass, center ]
    std::pair<float, Vector2> getNodeMassInfo(QuadTree::Node::Index i);
    Vector2 calculateForceFor(Entity e, QuadTree::Node::Index i = 0);
};

#endif /* PARSIM_SIMULATION_H */