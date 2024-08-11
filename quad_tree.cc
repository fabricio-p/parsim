#include "quad_tree.hpp"

using Node = QuadTree::Node;
std::ostream& printIndent(std::ostream& o, size_t level);

QuadTree::QuadTree(Rectangle viewport) : nodes({Node(viewport)}) {}

void QuadTree::clear() {
    nodes.resize(1);
    root() = Node(root().bounds);
}
Node& QuadTree::root() {
    return nodes[0];
}

void QuadTree::insert(
    Entity e,
    std::vector<Vector2> const& positions,
    Node::Index i
) {
    // DUMP(i);
    QuadTree::Node *node = &nodes.at(i);
    if (!node->hasEntity() && !node->hasChildren()) {
        node->e = e;
        return;
    }
    if (node->hasEntity() && !node->hasChildren()) {
        auto const partition = node->findPartition(positions[node->e]);
        QuadTree::Node child(std::get<1>(partition));
        size_t j = nodes.size();
        Entity e0 = node->e;
        node->children[std::get<0>(partition)] = j;
        node->e = -1;
        nodes.push_back(child);
        insert(e0, positions, j);
        // reassign because the pointer to the element gets invalidated
        // after the push
        node = &nodes.at(i);
    }
    auto const partition = node->findPartition(positions[e]);
    if (!node->hasChild(std::get<0>(partition))) {
        size_t j = nodes.size();
        QuadTree::Node child(std::get<1>(partition));
        node->children[std::get<0>(partition)] = j;
        nodes.push_back(child);
        // reassign again after the push
        node = &nodes.at(i);
    }
    insert(e, positions, node->children[std::get<0>(partition)]);
}

std::ostream& QuadTree::printNode(
    QuadTree::Node::Index i,
    std::ostream& o,
    size_t level
) const {
    Node const& node = nodes[i];
    o<<"Node {"<<std::endl;
    printIndent(o, level + 1)<<"entity: "<<node.e<<","<<std::endl;
    printIndent(o, level + 1)<<"bounds: {"
        <<"x: "<<node.bounds.x<<", "
        <<"y: "<<node.bounds.y<<", "
        <<"width: "<<node.bounds.width<<", "
        <<"height: "<<node.bounds.height<<"},"<<std::endl;
    printIndent(o, level + 1)<<"children: [";
    if (node.hasChildren()) {
        o<<std::endl;
        for (auto const childDesc : node) {
            printIndent(o, level + 1)
                <<"<"
                <<(childDesc & Node::bottom ? "bottom" : "top")
                <<", "
                <<(childDesc & Node::right ? "right" : "left")
                <<">: ";
            printNode(childDesc, o, level + 2)<<std::endl;
        }
        printIndent(o, level + 1);
    }
    return printIndent(o<<"]"<<std::endl, level)<<"}";
}

Node::Node(
    Rectangle _bounds,
    Entity _e,
    std::array<Node::Index, 4> _children
)
: children(_children)
, bounds(_bounds)
, e(_e)
, mass(-1)
, massCenter({-1, -1}) {}

size_t Node::countChildren() const {
    return hasChild(top | left) +
        hasChild(top | right) +
        hasChild(bottom | left) +
        hasChild(bottom | right);
}

bool Node::hasChildren() const {
    return countChildren() > 0;
}

bool Node::hasChild(ssize_t i) const {
    return children[i] >= 0;
}

bool Node::hasEntity() const {
    return e >= 0;
}

std::pair<ssize_t, Rectangle> Node::findPartition(Vector2 pos) const {
    float x_m = bounds.x + bounds.width / 2.f;
    float y_m = bounds.y + bounds.height / 2.f;
    ssize_t index = 0;
    Rectangle rect;
    rect.x = bounds.x;
    rect.y = bounds.y;
    rect.width = bounds.width / 2.f;
    rect.height = bounds.height / 2.f;
    index  |= pos.x <= x_m ? left : right;
    index  |= pos.y <= y_m ? top : bottom;
    rect.x += pos.x <= x_m ? 0.f : rect.width;
    rect.y += pos.y <= y_m ? 0.f : rect.height;
    return {index, rect};
}

Node::iterator Node::begin() const {
    if (!hasChildren()) {
        return end();
    }
    size_t first = 0;
    for (; first < 4 && !hasChild(first); first++) {}
    return iterator(*this, first);
}
Node::iterator Node::end() const {
    return iterator(*this, 4);
}

Node::iterator::iterator(Node const& _node, size_t _i)
: node(_node), i(_i) {
    for (; i < 4 && !node.hasChild(i); i++) {}
}

Node::iterator& Node::iterator::operator++() {
    i++;
    for (; i < 4 && !node.hasChild(i); i++) {}
    return *this;
}

Node::iterator Node::iterator::operator++(int) {
    iterator ret = *this;
    ++(*this);
    return ret;
}

bool Node::iterator::operator==(iterator const& other) const {
    return std::addressof(node) == std::addressof(other.node) &&
        i == other.i;
}

bool Node::iterator::operator!=(iterator const& other) const {
    return !this->operator==(other);
}

Node::iterator::reference Node::iterator::operator*() const {
    return *this;
}

template <typename T>
Node& Node::iterator::getFrom(T& collection) const {
    return collection.get(i);
}

Node::iterator::operator size_t() const {
    return node.children[i];
}

std::ostream& printIndent(std::ostream& o, size_t level) {
    for (size_t i = 0; i < level; i++) {
        o<<"  ";
    }
    return o;
}