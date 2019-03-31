// Winter 2019

#include "PhongMaterial.hpp"
#include "Texture.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{
	m_kt = glm::vec3(0, 0, 0);
	refractIndex = 1.0f;
	perturb = -2;
}

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& kt, double shininess, double ior, double perturb)
	: m_kd(kd)
	, m_ks(ks)
	, m_kt(kt)
	, m_shininess(shininess)
	, refractIndex(ior)
	, perturb(perturb)
{}

void PhongMaterial::setTexture(Texture *t) {
	texture = t;
	hasTexture = true;
}

void PhongMaterial::setBump(Texture *t) {
	bump = t;
	hasBump = true;
}

PhongMaterial::~PhongMaterial()
{}
