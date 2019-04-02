// Fall 2018

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <vector>
#include <string>
#include <iostream>

class SceneNode;
class GeometryNode;

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

struct KeyFrame {
    SceneNode* key;
    glm::vec3 translate;
    glm::vec3 scale;
    glm::vec3 rotate;
    glm::vec3 etranslate;
    glm::vec3 escale;
    glm::vec3 erotate;
    float stime;
    float duration;
    double repeat;
    double iMode;
    double interpolate;
};

struct TransformInfo {
    glm::mat4 translation;
    glm::mat4 scales;
    glm::mat4 rotation;

    glm::vec3 transParam;
    glm::vec3 rotParam;
    glm::vec3 scaleParam;

    glm::mat4 trans;
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);

    void add_skybox(GeometryNode* sbox);

    void add_key(SceneNode* key, glm::vec3 translate, glm::vec3 scale, glm::vec3 rotate,
	    glm::vec3 etranslate, glm::vec3 escale, glm::vec3 erotate, float stime, float duration, double repeat, double iMode, double interpolate);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

    void set_rotate(char axis, float angle);
    void set_scale(const glm::vec3& amount);
    void set_translate(const glm::vec3& amount);

    void add_scale(const glm::vec3& amount);
    void add_rotate(const glm::vec3& amount);

	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;

    bool hasSkybox = false;
    GeometryNode* skybox;

    TransformInfo getTransInfo();
    void setTransInfo(TransformInfo info);

    void applyTransInfo();

    TransformInfo transInfo;
    
    glm::mat4 realT;
    glm::mat4 invT;
    
    std::list<SceneNode*> children;
    std::vector<KeyFrame*> keyFrames;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
