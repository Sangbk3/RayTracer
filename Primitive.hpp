// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "polyroots.hpp"
#include "PhongMaterial.hpp"
#include "VarHolder.hpp"

class NonhierSphere;

class Primitive {
  public:
    virtual ~Primitive();
    virtual bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v);
    void setMaterial(Material *pmat);
    void getUV(glm::vec3 &at, float &u, float &v);
    NonhierSphere *getBoundingSphere();
    NonhierSphere *boundingSphere;
    PhongMaterial *material;
    bool hasMaterial = false;
};

class Sphere : public Primitive {
  public:
    Sphere();
    virtual ~Sphere();
    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) override;
    void getUV(glm::vec3 &at, float &u, float &v);
};

class Cube : public Primitive {
  public:
    Cube();
    virtual ~Cube();
    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) override;
    void getUV(glm::vec3 &at, float &u, float &v);
};

class NonhierSphere : public Primitive {
  public:
    NonhierSphere(const glm::vec3& pos, double radius);
    
    virtual ~NonhierSphere();
    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) override;
    void getUV(glm::vec3 &at, float &u, float &v);
    void getUVNormal(glm::vec3 &at, float &u, float &v, glm::vec3 &normal);
    glm::vec3 m_pos;
    double m_radius;
};

class NonhierBox : public Primitive {
  public:
    NonhierBox(const glm::vec3& pos, double size);
    
    virtual ~NonhierBox();
    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) override;
    void getUV(glm::vec3 &at, float &u, float &v);

  private:
    glm::vec3 m_pos;
    double m_size;
};

glm::vec3 getPointAt(glm::vec3 origin, glm::vec3 slope, double t);