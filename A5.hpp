// Winter 2019

#pragma once

#include <glm/glm.hpp>
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>    

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "VarHolder.hpp"


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
	SceneNode *root, std::list<Light *> lights, glm::vec3 ambient);

glm::mat4 getPixelToWorldTransform(Image image, double fovy, glm::vec3 view, glm::vec3 up, glm::vec3 eye);
void getClosestObjectPoint(
	SceneNode *node, glm::vec3 origin, glm::vec3 slope,
	float &t, glm::vec3 &normal, glm::vec3& kd, glm::vec3& ks,
	 double &shininess, bool &result, glm::mat4 parentInv);
glm::vec3 getColorAtPoint(
	glm::vec3 origin, glm::vec3 slope, float t, glm::vec3 normal, glm::vec3 kd, glm::vec3 ks, double shininess, std::list<Light *> lights, glm::vec3 ambient,
	SceneNode * root,
	int numReflected);
float distance(glm::vec3 p1, glm::vec3 p2);
glm::vec3 divide(glm::vec3 v, float weight);