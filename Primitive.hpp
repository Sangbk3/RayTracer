// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include "polyroots.hpp"

class NonhierSphere;

class Primitive {
  public:
    virtual ~Primitive();
    virtual bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal);
    NonhierSphere *getBoundingSphere();
    NonhierSphere *boundingSphere;
};

class Sphere : public Primitive {
  public:
    Sphere();
    virtual ~Sphere();
    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal) override;
};

class Cube : public Primitive {
  public:
    Cube();
    virtual ~Cube();
    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal) override;
};

class NonhierSphere : public Primitive {
  public:
    NonhierSphere(const glm::vec3& pos, double radius);
    
    virtual ~NonhierSphere();
    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal) override;
    glm::vec3 m_pos;
    double m_radius;
};

class NonhierBox : public Primitive {
  public:
    NonhierBox(const glm::vec3& pos, double size);
    
    virtual ~NonhierBox();
    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal) override;

  private:
    glm::vec3 m_pos;
    double m_size;
};

glm::vec3 getPointAt(glm::vec3 origin, glm::vec3 slope, double t);