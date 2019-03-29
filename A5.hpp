// Winter 2019

#pragma once

#include <glm/glm.hpp>
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>  
#include <random>  

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "VarHolder.hpp"
#include "GridSubdivision.hpp"
#include "ThreadPool.hpp"


void A5_Render(

		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);

void setPixelOfImage(
	glm::mat4 pixelToWorldTransform, int x, int h, glm::vec3 *pixelColors, glm::vec3 eye,
	SceneNode *root, std::list<Light *> lights, glm::vec3 ambient, GridSubdivision *gridSubdivision);

glm::mat4 getPixelToWorldTransform(Image image, double fovy, glm::vec3 view, glm::vec3 up, glm::vec3 eye);
void getClosestObjectPoint(
	SceneNode *node, glm::vec3 origin, glm::vec3 slope,
	float &t, glm::vec3 &normal, PhongMaterial **mat, bool &result, glm::mat4 parentInv);

void getClosestObjectPointUseGrid(
	SceneNode *node, glm::vec3 origin, glm::vec3 slope,
	float &t, glm::vec3 &normal, bool &result, SceneNode **rNode,
	 GridSubdivision *gridSubdivision);

glm::vec3 getColorAtPoint(
	glm::vec3 origin, glm::vec3 slope, float t, glm::vec3 norm, PhongMaterial *mat, std::list<Light *> lights, glm::vec3 ambient,
	SceneNode * root,
	GridSubdivision *gridSubdivision,
	int numReflected,
	int numTransmitted);
float distance(glm::vec3 p1, glm::vec3 p2);
glm::vec3 divide(glm::vec3 v, float weight);

glm::vec3 perturbNormal(glm::vec3 &ray, double perturb);

void sangReflect(glm::vec3 origin, glm::vec3 slope, glm::vec3 normal, glm::vec3 ks, double perturb, std::list<Light *> lights, glm::vec3 ambient,
	SceneNode * root,
	GridSubdivision *gridSubdivision,
	int numReflected,
	int numTransmitted,
	glm::vec3 &resultv);

void sangRefract(glm::vec3 point, glm::vec3 slope, glm::vec3 n, PhongMaterial *mat, std::list<Light *> lights, glm::vec3 ambient,
	SceneNode * root,
	GridSubdivision *gridSubdivision,
	int numReflected,
	int numTransmitted,
	glm::vec3 &resultv);

float fresnelR(float iori, float iort, float cosi, float cost);

glm::vec3 getRandomLightPosition(Light *light);