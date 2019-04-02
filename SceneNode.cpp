// Winter 2019

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;

//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	invtrans(mat4()),
	m_nodeId(nodeInstanceCount++)
{
	transInfo.translation = mat4();
	transInfo.scales = mat4(1);
	transInfo.rotation = mat4();
	transInfo.transParam = vec3();
	transInfo.scaleParam = vec3(1);
	transInfo.rotParam = vec3();
	transInfo.trans = trans;
}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	transInfo = other.transInfo;
	applyTransInfo();
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	transInfo.translation = mat4();
	transInfo.scales = mat4(1);
	transInfo.rotation = mat4();
	transInfo.transParam = vec3();
	transInfo.scaleParam = vec3(1);
	transInfo.rotParam = vec3();
	transInfo.trans = trans;
	applyTransInfo();
}

TransformInfo SceneNode::getTransInfo() {
	return transInfo;
}
void SceneNode::setTransInfo(TransformInfo info) {
	transInfo = info;
	trans = info.trans;
	applyTransInfo();
}
//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return realT;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invT;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	children.push_back(child);
}


void SceneNode::add_skybox(GeometryNode* sbox) {
	skybox = sbox;
	hasSkybox = true;
}

void SceneNode::add_key(
	SceneNode* key, glm::vec3 translate, glm::vec3 scale, glm::vec3 rotate,
	glm::vec3 etranslate, glm::vec3 escale, glm::vec3 erotate, 
	float stime, float duration, double repeat, double iMode, double interpolate) {

	KeyFrame *keyframe = new KeyFrame();
	keyframe->key = key;
	keyframe->translate = translate;
	keyframe->scale = scale;
	keyframe->rotate = rotate;
	keyframe->etranslate = etranslate;
	keyframe->escale = escale;
	keyframe->erotate = erotate;
	keyframe->stime = stime;
	keyframe->duration = duration;
	keyframe->repeat = repeat;
	keyframe->iMode = interpolate;
	keyframe->interpolate = interpolate;
	keyFrames.push_back(keyframe);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {

	switch (axis) {
		case 'x':
			transInfo.rotParam[0] += angle;
			break;
		case 'y':
			transInfo.rotParam[1] += angle;
	        break;
		case 'z':
			transInfo.rotParam[2] += angle;
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(transInfo.rotParam[0]), vec3(1,0,0));
	rot_matrix = glm::rotate(degreesToRadians(transInfo.rotParam[1]), vec3(rot_matrix*vec4(0,1,0,0)))*rot_matrix;
	rot_matrix = glm::rotate(degreesToRadians(transInfo.rotParam[2]), vec3(rot_matrix*vec4(0,0,1,0)))*rot_matrix;
	transInfo.rotation = rot_matrix;
	// set_transform( rot_matrix * trans );
	applyTransInfo();
}

void SceneNode::add_rotate(const glm::vec3& amount) {
	transInfo.rotParam += amount;
	mat4 rot_matrix = glm::rotate(degreesToRadians(transInfo.rotParam[0]), vec3(1,0,0));
	rot_matrix = glm::rotate(degreesToRadians(transInfo.rotParam[1]), vec3(rot_matrix*vec4(0,1,0,0)))*rot_matrix;
	rot_matrix = glm::rotate(degreesToRadians(transInfo.rotParam[2]), vec3(rot_matrix*vec4(0,0,1,0)))*rot_matrix;
	transInfo.rotation = rot_matrix;
	applyTransInfo();
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	transInfo.scaleParam[0] *= amount[0];
	transInfo.scaleParam[1] *= amount[1];
	transInfo.scaleParam[2] *= amount[2];
	transInfo.scales = glm::scale(transInfo.scaleParam);
	// set_transform( glm::scale(amount) * trans );
	applyTransInfo();
}

void SceneNode::add_scale(const glm::vec3& amount) {
	transInfo.scaleParam += amount;
	transInfo.scales = glm::scale(transInfo.scaleParam);
	applyTransInfo();
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	transInfo.transParam += amount;
	transInfo.translation = glm::translate(transInfo.transParam);
	// set_transform( glm::translate(amount) * trans );
	applyTransInfo();
}


void SceneNode::set_rotate(char axis, float angle) {
	transInfo.rotation = mat4();
	transInfo.rotParam = vec3();
	rotate(axis, angle);
}
void SceneNode::set_scale(const glm::vec3& amount) {
	transInfo.scales = mat4(1);
	transInfo.scaleParam = vec3(1);
	scale(amount);
}
void SceneNode::set_translate(const glm::vec3& amount) {
	transInfo.translation = mat4();
	transInfo.transParam = vec3();
	translate(amount);
}

void SceneNode::applyTransInfo() {
	realT = transInfo.translation * transInfo.rotation * transInfo.scales * trans;
	invT = glm::inverse(realT);
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;

	os << "]\n";
	return os;
}
