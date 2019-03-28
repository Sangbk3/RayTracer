// Winter 2019

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{
	m_kt = glm::vec3(0, 0, 0);
	refractIndex = 1.0f;
}

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& kt, double shininess, double ior )
	: m_kd(kd)
	, m_ks(ks)
	, m_kt(kt)
	, m_shininess(shininess)
	, refractIndex(ior)
{}

PhongMaterial::~PhongMaterial()
{}
