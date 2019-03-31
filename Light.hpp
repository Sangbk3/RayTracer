// Winter 2019

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>
#include "Primitive.hpp"

struct PointMass;

struct Light {
  Light();
  
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3];

  double radius;


  bool hasPointMass = false;
  PointMass *pointMass;
  NonhierSphere *sphere;
};

std::ostream& operator<<(std::ostream& out, const Light& l);


struct PointMass {
  PointMass();

  double einsteinR;
  glm::vec3 position;
};