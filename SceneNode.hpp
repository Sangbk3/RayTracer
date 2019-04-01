// Fall 2018

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

class SceneNode;

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

    void add_key(SceneNode* key, glm::vec3 translate, glm::vec3 scale, glm::vec3 rotate,
	    glm::vec3 etranslate, glm::vec3 escale, glm::vec3 erotate);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;
    std::list<KeyFrame*> keyFrames;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
