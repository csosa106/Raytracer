#include "kdtree.hpp"

std::pair<vec3, vec3> split_point(vec3 minBounds, vec3 maxBounds, 
const std::vector<Shape*> &scenery, int split_axis)
{
    // Split halfway
    float loc = .5 * (maxBounds.coord[split_axis] + minBounds.coord[split_axis]);
    std::pair<vec3, vec3> result;
    vec3 rmin = minBounds;
    vec3 lmax = maxBounds;
    rmin.coord[split_axis] = loc;
    lmax.coord[split_axis] = loc;
    result.first = lmax;
    result.second = rmin;
    return result;
}

bool isLeaf(const std::vector<Shape*> &scenery, int leaf_size) 
{
    return scenery.size() <= (unsigned int) leaf_size;
}


Node init_node(vec3 minBounds, vec3 maxBounds, const std::vector<Shape*> 
&scenery, int leaf_size, int depth, int max_depth)
{
    Node result;
    result.minBounds = minBounds;
    result.maxBounds = maxBounds;
    result.depth = depth;
    if (isLeaf(scenery, leaf_size))
    {
        result.isLeaf = true;
        result.primitives = scenery;
        result.left = NULL;
        result.right = NULL;
    }
    else if (depth < max_depth)
    {
        std::vector<Shape*> l_prims;
        std::vector<Shape*> r_prims;
        int split_axis = rand() % 3;
        std::pair<vec3, vec3> split = split_point(minBounds, maxBounds, 
        scenery, split_axis);

        // Check if split went too deep for floating point precision
        if (minBounds == split.first || split.second == maxBounds)
        {
            result.isLeaf = true;
            result.primitives = scenery;
            result.left = NULL;
            result.right = NULL;
        }
        else
        {
            result.isLeaf = false;
            result.left = std::make_shared<Node>();
            result.right = std::make_shared<Node>();
            result.split_axis = split_axis;
            for (unsigned int i = 0; i < scenery.size(); i++)
            {
                
                if (scenery[i]->inBounds(minBounds, split.first))
                {
                    l_prims.push_back(scenery[i]);
                }
                if (scenery[i]->inBounds(split.second, maxBounds))
                {
                    r_prims.push_back(scenery[i]);
                }

            }
            *(result.left) = init_node(minBounds, split.first, l_prims, 
            leaf_size, depth + 1, max_depth);
            *(result.right) = init_node(split.second, maxBounds, r_prims, 
            leaf_size, depth + 1, max_depth);
        }

    }

    else
    {
        result.isLeaf = true;
        result.left = NULL;
        result.right = NULL;
        result.primitives = scenery;
    }

    return result;
}

kdtree::kdtree(vec3 minBounds, vec3 maxBounds, const std::vector<Shape*> 
&scenery, int leaf_size, int max_depth)
{
    root = std::make_shared<Node>();
    this->max_depth = max_depth;
    this->leaf_size = leaf_size;
    *root = init_node(minBounds, maxBounds, scenery, leaf_size, 0, max_depth);
}


std::ostream& operator<<(std::ostream& os, const Node n)
{
    if (n.isLeaf)
    {
        os << "PRIM_SIZE " << n.primitives.size() << " MIN AND MAX " << 
        n.minBounds << " " << n.maxBounds; 
    }
    else
    {
        os << "SPLIT AXIS " << n.split_axis;
    }
    return os;
}

std::stringstream display(std::shared_ptr<Node> n)
{
    std::stringstream result;
    if (n->isLeaf)
    {
        result << *n;
    }
    else
    {
        result << "[" << *n << " [" << display(n->left).str() << "] [" << 
        display(n->right).str() << "]]";
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const kdtree t)
{
    os << display(t.root).str();
    return os;
}


