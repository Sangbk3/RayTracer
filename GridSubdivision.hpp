#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <unordered_set> 
#include "Material.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Primitive.hpp"
#include "PhongMaterial.hpp"

struct ConvertedNode {
  ConvertedNode(SceneNode *node, std::vector<float> boundaries, glm::mat4 trans, glm::mat4 inverse)
    :node(node),boundaries(boundaries), trans(trans), invTrans(inverse){};
  
  SceneNode* node;
  std::vector<float> boundaries;
  glm::mat4 trans;
  glm::mat4 invTrans;
};

class GridSubdivision {
public:
    GridSubdivision( SceneNode *root, int depth, glm::vec3 eye, glm::vec3 view, float fov, float aspect );
    void checkIntersection(
        int i, int j, int k, glm::vec3 origin, glm::vec3 slope,
        float &t, glm::vec3 &normal, SceneNode **rNode,
        std::unordered_set<uint> &checkedIndices, bool &result);

    void findSceneBorder(float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax);
    void do3DDDA(glm::vec3 origin, glm::vec3 slope, float &t, glm::vec3 &normal, SceneNode **rNode, bool &result);
  
private:
    void subdivide(std::vector<uint> objIndices, int depth, int i, int j, int k, int totalDepth);
    void convertToNonHier(SceneNode *node, glm::mat4 ptrans, glm::mat4 pinv);
	std::vector<std::vector<uint>> lattice;
    std::vector<ConvertedNode *> convertedNodes;
    float xmin, xmax, ymin, ymax, zmin, zmax;
    int totalDepth;
};
