#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include "Material.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Primitive.hpp"

class GridSubdivision {
public:
    GridSubdivision( SceneNode *root, int depth );
    void checkIntersection(
        int i, int j, int k, glm::vec3 origin, glm::vec3 slope,
        float &t, glm::vec3 &normal, Material *material, bool &result);

    void findSceneBorder(std::vector<SceneNode *> nodes, float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax);
    void do3DDDA(glm::vec3 origin, glm::vec3 slope, float &t, glm::vec3 &normal, Material *material, bool &result);
  
private:
    void subdivide(std::vector<uint> objIndices, int depth, int i, int j, int k, int totalDepth);
    void convertToNonHier(SceneNode *node, std::vector<SceneNode *> &converted, glm::mat4 ptrans);
	std::vector<std::vector<uint>> lattice;
    std::vector<SceneNode *> convertedNodes;
    std::vector<std::vector<float>> boundaries;
    float xmin, xmax, ymin, ymax, zmin, zmax;
    int totalDepth;
};
