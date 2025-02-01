#ifndef KDTREE_H
#define KDTREE_H
#include <vector>
#include "vec3.hpp"
#include <sstream>
#include <memory>
#include "geometry.hpp"

class Node
{
    public:
        bool isLeaf;
        std::vector<Shape*> primitives;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        vec3 minBounds;
        vec3 maxBounds;
        int split_axis;
        int depth;
};


class kdtree
{
    public:
        int leaf_size;
        int max_depth;
        std::shared_ptr<Node> root;
        kdtree(vec3 minBounds, vec3 maxBounds, 
        const std::vector<Shape*> &scenery, int leaf_size, int max_depth);
};

std::pair<vec3, vec3> split_point(vec3 minBounds, vec3 maxBounds, 
const std::vector<Shape*> &scenery, int split_axis);


Node init_node(vec3 minBounds, vec3 maxBounds, const std::vector<Shape*> 
&scenery, int leaf_size, int depth, int max_depth);


std::ostream& operator<<(std::ostream& os, const Node n);

std::stringstream display(std::shared_ptr<Node> n);

std::ostream& operator<<(std::ostream& os, const kdtree t);
#endif
