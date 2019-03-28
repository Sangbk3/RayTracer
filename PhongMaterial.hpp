// Winter 2019

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& kt, double shininess, double ior, double perturb);
  virtual ~PhongMaterial();
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  glm::vec3 m_kt;

  float refractIndex;
  double m_shininess;
  int perturb;

  // 1 perfect vac
  // water 1.33
  // glass 1.52

};
