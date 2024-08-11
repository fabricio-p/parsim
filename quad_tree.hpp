#include <array>
#include <vector>
#include <iostream>
#include "common.hpp"

class QuadTree {
public:
    struct Node {
        using Index = ssize_t;
        static constexpr ssize_t const
            top = 0b00
        ,   bottom = 0b10
        ,   left = 0b00
        ,   right = 0b01
        ;
        // tl, tr, bl, br
        std::array<Index, 4> children;
        Rectangle bounds;
        Entity e;
        float mass;
        Vector2 massCenter;

        class iterator {
        public:
            using difference_type = iterator;
            using value_type = size_t;
            using pointer = iterator const *;
            using reference = iterator const &;
            using iterator_tag = std::output_iterator_tag;

            explicit iterator(Node const& _node, size_t _i);
            iterator& operator++();
            iterator operator++(int);
            bool operator==(iterator const& other) const;
            bool operator!=(iterator const& other) const;
            reference operator*() const;
            template <typename T> Node& getFrom(T& collection) const;
            operator size_t() const;
        private:
            Node const& node;
            size_t i;
        };

        Node(
            Rectangle _bounds = {-1, -1, -1, -1},
            Entity _e = -1,
            std::array<Node::Index, 4> _children = {-1, -1, -1, -1}
        );
        bool hasChild(ssize_t i) const;
        bool hasChildren() const;
        size_t countChildren() const;
        bool hasEntity() const;
        std::pair<ssize_t, Rectangle> findPartition(Vector2 pos) const;
        iterator begin() const;
        iterator end() const;
    };

    std::vector<Node> nodes;

    QuadTree(Rectangle viewport);
    void clear();
    Node& root();
    void insert(
        Entity e,
        std::vector<Vector2> const& positions,
        Node::Index i = 0
    );
    std::ostream& printNode(
        QuadTree::Node::Index i,
        std::ostream& o,
        size_t level = 0
    ) const;
};

static inline std::ostream&
operator<<(std::ostream& o, QuadTree const& tree) {
    return tree.printNode(0, o)<<std::endl;
}