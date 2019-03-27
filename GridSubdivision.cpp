
#include <glm/ext.hpp>

#include "GridSubdivision.hpp"

using namespace std;

static float PI = 3.14159265359f;

GridSubdivision::GridSubdivision( SceneNode * root, int depth, glm::vec3 eye, glm::vec3 view, float fov, float aspect ) {
    //make nonhier copy
    GridSubdivision::convertToNonHier(root, glm::mat4(1.f), glm::mat4(1.f));

    std::vector<uint> indices(convertedNodes.size());
    for (int i = 0; i < indices.size(); i++) {
        indices.at(i) = i;
    }

    findSceneBorder(xmin, xmax, ymin, ymax, zmin, zmax);
    if (view[2] >= 0) {
        zmin = eye[2];
    } else {
        zmax = eye[2];
    }
	float height = (zmax - zmin)*tanf(PI * fov/360);
    ymax = eye[1] + height;
    ymin = eye[1] - height;
    xmax = eye[0] + height*aspect;
    xmin = eye[0] - height*aspect;

    lattice.resize(pow(8, depth));
    subdivide(indices, depth, 0, 0, 0, depth);

    totalDepth = depth;
}

void GridSubdivision::do3DDDA(glm::vec3 origin, glm::vec3 slope, float &t, glm::vec3 &normal, SceneNode **rNode, bool &result) {
    glm::vec3 position;
    
	std::unordered_set<uint> checkedIndices;

    if (origin[0] > xmax || origin[0] < xmin || origin[1] > ymax || origin[1] < ymin || origin[2] > zmax || origin[2] < zmin) {
        float minT;
        glm::vec3 test;
        bool isFirst = true;

        if (slope[0] != 0) {
            float xtleft = (xmin - origin[0])/slope[0];
            test = origin + slope*xtleft;
            if (test[1] >= ymin && test[1] <= ymax && test[2] >= zmin && test[2] <= zmax && xtleft > 0.001 && (isFirst || xtleft < minT)) {
                minT = xtleft;
                isFirst = false;
            }

            float xtright = (xmax - origin[0])/slope[0];
            test = origin + slope*xtright;
            if (test[1] >= ymin && test[1] <= ymax && test[2] >= zmin && test[2] <= zmax && xtright > 0.001 && (isFirst || xtright < minT)) {
                minT = xtright;
                isFirst = false;
            }
        }

        if (slope[1] != 0) {
            float yttop = (ymax - origin[1]) / slope[1];
            test = origin + slope*yttop;
            if (test[0] >= xmin && test[0] <= xmax && test[2] >= zmin && test[2] <= zmax && yttop > 0.001 && (isFirst || yttop < minT)) {
                minT = yttop;
                isFirst = false;
            }

            float ytbot = (ymin - origin[1]) / slope[1];
            test = origin + slope*ytbot;
            if (test[0] >= xmin && test[0] <= xmax && test[2] >= zmin && test[2] <= zmax && ytbot > 0.001 && (isFirst || ytbot < minT)) {
                minT = ytbot;
                isFirst = false;
            }
        }


        if (slope[2] != 0) {
            float ztback = (zmin - origin[2]) / slope[2];
            test = origin + slope*ztback;
            if (test[0] >= xmin && test[0] <= xmax && test[1] >= zmin && test[1] <= zmax && ztback > 0.001 && (isFirst || ztback < minT)) {
                minT = ztback;
                isFirst = false;
            }

            float ztfront = (zmax - origin[2]) / slope[2];
            test = origin + slope*ztfront;
            if (test[0] >= xmin && test[0] <= xmax && test[1] >= zmin && test[1] <= zmax && ztfront > 0.001 && (isFirst || ztfront < minT)) {
                minT = ztfront;
                
                isFirst = false;
            }
        }

        if (isFirst) {
            return;
        }
        position = origin + slope*minT;
    } else {
        position = origin;
    }

    float numRows = pow(2, totalDepth);

    float distX = (xmax - xmin) / pow(2, totalDepth);
    float distY = (ymax - ymin) / pow(2, totalDepth);
    float distZ = (zmax - zmin) / pow(2, totalDepth);

    float tX = std::numeric_limits<float>::max();
    if (slope[0] != 0) {
        tX = distX / slope[0];
    }
    float tY = std::numeric_limits<float>::max();
    if (slope[1] != 0) {
        tY = distY / slope[1];
    }
    float tZ = std::numeric_limits<float>::max();
    if (slope[2] != 0) {
        tZ = distZ / slope[2];
    }

    float errX;
    int gridi;
    if (slope[0] > 0) {
        gridi = floor((position[0] - xmin)/distX);
        errX = position[0] - (xmin + gridi*distX);
    } else {
        gridi = ceil((position[0] - xmin)/distX) - 1;
        errX = (xmin + (gridi + 1)*distX) - position[0];
        tX = -tX;
    }

    float errY;
    int gridj;
    if (slope[1] > 0) {
        gridj = floor((position[1] - ymin)/distY);
        errY = position[1] - (ymin + gridj*distY);
    } else {
        gridj = ceil((position[1] - ymin)/distY) - 1;
        errY = (ymin + (gridj + 1)*distY) - position[1];
        tY = -tY;
    }

    float errZ;
    int gridk;
    if (slope[2] > 0) {
        gridk = floor((position[2] - zmin)/distZ);
        errZ = position[2] - (zmin + gridk*distZ);
    } else {
        gridk = ceil((position[2] - zmin)/distZ) - 1;
        errZ = (zmin + (gridk + 1)*distZ) - position[2];
        tZ = -tZ;
    }
    
    while (gridi < numRows && gridj < numRows && gridk < numRows && gridi >= 0 && gridj >= 0 && gridk >= 0) {

        checkIntersection(gridi, gridj, gridk, origin, slope, t, normal, rNode, checkedIndices, result);

        float xt = (tX*(distX - errX))/distX;
        float yt = (tY*(distY - errY))/distY;
        float zt = (tZ*(distZ - errZ))/distZ;

        if ((yt < 0.001 || xt < yt) && (zt < 0.001 || xt < zt) && slope[0] != 0 && xt > 0.001) {
            position = position + slope*xt;
        } else if ((xt < 0.001 || yt < xt) && (zt < 0.001 || yt < zt) && slope[1] != 0 && yt > 0.001) {
            position = position + slope*yt;
        } else if (slope[2] != 0 && zt > 0.001) {
            position = position + slope*zt;
        } else {
            position = position + slope*min(tX, min(tY, tZ));
        }

        if (slope[0] > 0) {
            gridi = floor((position[0] - xmin)/distX);
            errX = position[0] - (xmin + gridi*distX);
        } else {
            gridi = ceil((position[0] - xmin)/distX) - 1;
            errX = (xmin + (gridi + 1)*distX) - position[0];
        }

        if (slope[1] > 0) {
            gridj = floor((position[1] - ymin)/distY);
            errY = position[1] - (ymin + gridj*distY);
        } else {
            gridj = ceil((position[1] - ymin)/distY) - 1;
            errY = (ymin + (gridj + 1)*distY) - position[1];
        }

        if (slope[2] > 0) {
            gridk = floor((position[2] - zmin)/distZ);
            errZ = position[2] - (zmin + gridk*distZ);
        } else {
            gridk = ceil((position[2] - zmin)/distZ) - 1;
            errZ = (zmin + (gridk + 1)*distZ) - position[2];
        }
    }
    return;
}

void GridSubdivision::checkIntersection(
    int i, int j, int k, glm::vec3 origin, glm::vec3 slope,
    float &t, glm::vec3 &normal, SceneNode **rNode, std::unordered_set<uint> &checkedIndices, bool &result) {

    for (uint ind : lattice.at(i*pow(4, totalDepth) + j*pow(2, totalDepth) + k)) {
        if (checkedIndices.find(ind) != checkedIndices.end()) {
            continue;
        }
        ConvertedNode *conv = convertedNodes.at(ind);
        SceneNode *node = conv->node;
        if (node->m_nodeType == NodeType::GeometryNode) {
            double temp;
            glm::vec3 tempn;
            GeometryNode *casted = static_cast<GeometryNode*>(node);

            glm::vec3 invO = glm::vec3(conv->invTrans*glm::vec4(origin, 1));
            glm::vec3 invM = glm::vec3(conv->invTrans*glm::vec4(slope, 0));

            if ((*casted).m_primitive->intersects(invO, invM, temp, tempn) && temp > 0.001) {

                if (!result || t > temp) {
                    t = temp;

                    normal = normalize(glm::vec3(glm::transpose(conv->invTrans) * glm::vec4(tempn, 0)));
                    *rNode = conv->node;
                    result = true;
                }
            }
        }

        checkedIndices.insert(ind);
    }
}

void GridSubdivision::subdivide(std::vector<uint> objIndices, int depth, int i, int j, int k, int totalDepth) {
    if (depth == 0) {
        lattice.at(i*pow(4, totalDepth) + j*pow(2, totalDepth) + k) = objIndices;
    } else if (!objIndices.empty()){

        std::vector<uint> i000;
        std::vector<uint> i001;
        std::vector<uint> i010;
        std::vector<uint> i011;
        std::vector<uint> i100;
        std::vector<uint> i101;
        std::vector<uint> i110;
        std::vector<uint> i111;

        float xmid = xmin + (i + pow(2, depth - 1))*((float) (xmax - xmin)) / pow(2, totalDepth);

        float ymid = ymin + (j + pow(2, depth - 1))*((float) (ymax - ymin)) / pow(2, totalDepth);

        float zmid = zmin + (k + pow(2, depth - 1))*((float) (zmax - zmin)) / pow(2, totalDepth);

        for (uint index : objIndices) {
            std::vector<float> bounds = convertedNodes.at(index)->boundaries;

            //boundaries: smallx, bigx, smally, bigy, smallz, bigz
            if (xmid > bounds.at(0)) {
                if (ymid > bounds.at(2)) {
                    if (zmid > bounds.at(4)) {
                        i000.push_back(index);
                    }
                    if (zmid < bounds.at(5)) {
                        i001.push_back(index);
                    }
                }
                if (ymid < bounds.at(3)) {
                    if (zmid > bounds.at(4)) {
                        i010.push_back(index);
                    }
                    if (zmid < bounds.at(5)) {
                        i011.push_back(index);
                    }
                }
            }
            if (xmid < bounds.at(1)) {
                if (ymid > bounds.at(2)) {
                    if (zmid > bounds.at(4)) {
                        i100.push_back(index);
                    }
                    if (zmid < bounds.at(5)) {
                        i101.push_back(index);
                    }
                }
                if (ymid < bounds.at(3)) {
                    if (zmid > bounds.at(4)) {
                        i110.push_back(index);
                    }
                    if (zmid < bounds.at(5)) {
                        i111.push_back(index);
                    }
                }
            }
        }

        subdivide(i000, depth - 1, i, j, k, totalDepth);
        subdivide(i001, depth - 1, i, j, k + pow(2, depth - 1), totalDepth);
        subdivide(i010, depth - 1, i, j + pow(2, depth - 1), k, totalDepth);
        subdivide(i011, depth - 1, i, j + pow(2, depth - 1), k + pow(2, depth - 1), totalDepth);
        subdivide(i100, depth - 1, i + pow(2, depth - 1), j, k, totalDepth);
        subdivide(i101, depth - 1, i + pow(2, depth - 1), j, k + pow(2, depth - 1), totalDepth);
        subdivide(i110, depth - 1, i + pow(2, depth - 1), j + pow(2, depth - 1), k, totalDepth);
        subdivide(i111, depth - 1, i + pow(2, depth - 1), j + pow(2, depth - 1), k + pow(2, depth - 1), totalDepth);
    }
}

void GridSubdivision::convertToNonHier(SceneNode *node, glm::mat4 ptrans, glm::mat4 pinv) {

    glm::mat4 curTrans = ptrans * node->get_transform();
    glm::mat4 curInv = node->get_inverse() * pinv;

    if (node->m_nodeType == NodeType::GeometryNode) {
        convertedNodes.push_back(new ConvertedNode(node, std::vector<float>(6), curTrans, curInv));
    }

    for (SceneNode *child : node->children) {
		GridSubdivision::convertToNonHier(child, curTrans, curInv);
	}
}

void GridSubdivision::findSceneBorder(float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax) {
    bool firstRunz = true;
    bool firstRuny = true;
    bool firstRunx = true;

    for (ConvertedNode *conv : convertedNodes) {
        SceneNode *node = conv->node;
        if (node->m_nodeType == NodeType::GeometryNode) {
		    GeometryNode *casted = static_cast<GeometryNode*>(node);

            NonhierSphere *bound = casted->m_primitive->getBoundingSphere();

            glm::vec4 spos = glm::vec4(bound->m_pos,1);

            //find minmaxz
            glm::vec4 normalxy = conv->invTrans*glm::vec4(0,0,1,0);
            glm::vec4 tz = bound->m_radius * glm::normalize(normalxy);
            float zbig = (conv->trans*(spos + tz))[2];
            float zsmall = (conv->trans*(spos - tz))[2];
            float temp;

            if (zbig < zsmall) {
                temp = zbig;
                zbig = zsmall;
                zsmall = temp;
            }

            if (firstRunz) {
                zmax = zbig;
                zmin = zsmall;
                firstRunz = false;
            } else {
                if (zbig > zmax) {
                    zmax = zbig;
                }
                if (zsmall < zmin) {
                    zmin = zsmall;
                }
            }

            //find minmaxy
            glm::vec4 normalxz = conv->invTrans*glm::vec4(0,1,0,0);
            glm::vec4 ty = bound->m_radius * glm::normalize(normalxz);
            float ybig = (conv->trans*(spos + ty))[1];
            float ysmall = (conv->trans*(spos - ty))[1];

            if (ybig < ysmall) {
                temp = ybig;
                ybig = ysmall;
                ysmall = temp;
            }

            if (firstRuny) {
                ymax = ybig;
                ymin = ysmall;
                firstRuny = false;
            } else {
                if (ybig > ymax) {
                    ymax = ybig;
                }
                if (ysmall < ymin) {
                    ymin = ysmall;
                }
            }

            //find minmax x
            glm::vec4 normalyz = conv->invTrans*glm::vec4(1,0,0,0);
            glm::vec4 tx = bound->m_radius * glm::normalize(normalyz);
            float xbig = (conv->trans*(spos + tx))[0];
            float xsmall = (conv->trans*(spos - tx))[0];

            if (xbig < xsmall) {
                temp = xbig;
                xbig = xsmall;
                xsmall = temp;
            }

            if (firstRunx) {
                xmax = xbig;
                xmin = xsmall;
                firstRunx = false;
            } else {
                if (xbig > xmax) {
                    xmax = xbig;
                }
                if (xsmall < xmin) {
                    xmin = xsmall;
                }
            }

            conv->boundaries.at(0) = (xsmall);
            conv->boundaries.at(1) = (xbig);
            conv->boundaries.at(2) = (ysmall);
            conv->boundaries.at(3) = (ybig);
            conv->boundaries.at(4) = (zsmall);
            conv->boundaries.at(5) = (zbig);
        }
    }
}