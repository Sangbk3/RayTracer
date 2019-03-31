// Winter 2019

#include "Primitive.hpp"
#include <iostream>

float PI = 3.14159265359f;
Primitive::~Primitive()
{

}

void Primitive::setMaterial(Material *pmat) {
    PhongMaterial *mat = static_cast<PhongMaterial *>(pmat);
    material = mat;
}

bool Primitive::intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) {
    return false;
}

NonhierSphere *Primitive::getBoundingSphere() {
    return boundingSphere;
}

Sphere::Sphere() {
    boundingSphere = new NonhierSphere(glm::vec3(0, 0, 0), 1);
}

Sphere::~Sphere()
{
    
}
bool Sphere::intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) {
    double roots[2];
    double aterm = pow(slope[0], 2) + pow(slope[1], 2) + pow(slope[2], 2);
    double bterm = 2 * (slope[0] * origin[0] + slope[1] * origin[1] + slope[2] * origin[2]);
    double cterm = pow(origin[0], 2) + pow(origin[1], 2) + pow(origin[2], 2) - 1;

    size_t numroot = quadraticRoots(aterm, bterm, cterm, roots);
    if (numroot == 1) {
        t = roots[0];
        normal = origin + slope*((float) t);
        getUV(normal, u, v);
        return true;
    } else if (numroot == 2) {
        t = glm::min(roots[0], roots[1]);
        normal = origin + slope*((float) t);
        getUV(normal, u, v);
        return true;
    }
    
    return false;
}

Cube::Cube() {
    boundingSphere = new NonhierSphere(glm::vec3(0.5f, 0.5f, 0.5f), sqrt(.5));
}

Cube::~Cube()
{
}
bool Cube::intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) {
    NonhierBox b = NonhierBox(glm::vec3(0.0, 0.0, 0.0), 1);
    return b.intersects(origin, slope, t, normal, u, v);
}

glm::vec3 getPointAt(glm::vec3 origin, glm::vec3 slope, double t) {
    return glm::vec3(origin[0] + slope[0] * t, origin[1] + slope[1] * t, origin[2] + slope[2] * t);
}

NonhierSphere::NonhierSphere(const glm::vec3& pos, double radius)
      : m_pos(pos), m_radius(radius) {
    boundingSphere = this;
}

NonhierSphere::~NonhierSphere()
{
}
bool NonhierSphere::intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) {
    double roots[2];
    glm::vec3 ori = origin - m_pos;
    double aterm = pow(slope[0], 2)/pow(m_radius, 2) + pow(slope[1], 2)/pow(m_radius, 2) + pow(slope[2], 2)/pow(m_radius, 2);
    double bterm = 2 * (slope[0] * ori[0] + slope[1] * ori[1] + slope[2] * ori[2])/pow(m_radius, 2);
    double cterm = pow(ori[0], 2)/pow(m_radius, 2) + pow(ori[1], 2)/pow(m_radius, 2) + pow(ori[2], 2)/pow(m_radius, 2) - 1;

    size_t numroot = quadraticRoots(aterm, bterm, cterm, roots);
    if (numroot == 1) {
        t = roots[0];
        // std::cout << t << " \n";
        glm::vec3 poi = origin + slope*((float) t);
        normal = glm::normalize(ori + slope*((float) t));
        if (material->hasBump && VarHolder::useBumpmap) {
            getUVNormal(poi, u, v, normal);
        } else {
            getUV(poi, u, v);
        }
        return true;
    } else if (numroot == 2) {
        if (roots[0] < 0.001) {
            t = roots[1];
        } else if (roots[1] < 0.001) {
            t = roots[0];
        } else {
            t = glm::min(roots[0], roots[1]);
        }
        // std::cout << t << " \n";
        glm::vec3 poi = origin + slope*((float) t);
        normal = glm::normalize(ori + slope*((float) t));
        if (material->hasBump && VarHolder::useBumpmap) {
            getUVNormal(poi, u, v, normal);
        } else {
            getUV(poi, u, v);
        }
        return true;
    }
    
    return false;
}

NonhierBox::NonhierBox(const glm::vec3& pos, double size)
      : m_pos(pos), m_size(size) {
    boundingSphere = new NonhierSphere(pos + glm::vec3(size/2.0, size/2.0, size/2.0), glm::distance(pos, glm::vec3(size/2.0, size/2.0, size/2.0)));
}

NonhierBox::~NonhierBox()
{
}
bool NonhierBox::intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) {
    glm::vec3 cori = origin - m_pos;
    double clen = m_size;
    bool result = false;

    t = 0;

    double tz0 = (-cori[2])/slope[2];
    glm::vec3 z0 = getPointAt(cori, slope, tz0);
    bool intFront = z0[0] >= 0 && z0[0] <= clen && z0[1] >= 0 && z0[1] <= clen;
    if ((tz0 > 0.001) && intFront && (tz0 < t || !result)) {
        t = tz0;
        normal = glm::vec3(0, 0, -1.f);
        result = true;
    }

    double tz1 = (clen - cori[2])/slope[2];
    glm::vec3 z1 = getPointAt(cori, slope, tz1);
    bool intBack = z1[0] >= 0 && z1[0] <= clen && z1[1] >= 0 && z1[1] <= clen;
    if ((tz1 > 0.001) && intBack && (tz1 < t || !result)) {
        t = tz1;
        normal = glm::vec3(0, 0, 1.f);
        result = true;
    }

    double ty0 = (-cori[1])/slope[1];
    glm::vec3 y0 = getPointAt(cori, slope, ty0);
    bool intBot = y0[0] >= 0 && y0[0] <= clen && y0[2] >= 0 && y0[2] <= clen;
    if ((ty0 > 0.001) && intBot && (ty0 < t || !result)) {
        t = ty0;
        normal = glm::vec3(0, -1.f, 0);
        result = true;
    }

    double ty1 = (clen - cori[1])/slope[1];
    glm::vec3 y1 = getPointAt(cori, slope, ty1);
    bool intTop = y1[0] >= 0 && y1[0] <= clen && y1[2] >= 0 && y1[2] <= clen;
    if ((ty1 > 0.001) && intTop && (ty1 < t || !result)) {
        t = ty1;
        normal = glm::vec3(0, 1.f, 0);
        result = true;
    }

    double tx0 = (-cori[0])/slope[0];
    glm::vec3 x0 = getPointAt(cori, slope, tx0);
    bool intLeft = x0[2] >= 0 && x0[2] <= clen && x0[1] >= 0 && x0[1] <= clen;
    if ((tx0 > 0.001) && intLeft && (tx0 < t || !result)) {
        t = tx0;
        normal = glm::vec3(-1.f, 0, 0);
        result = true;
    }

    double tx1 = (clen - cori[0])/slope[0];
    glm::vec3 x1 = getPointAt(cori, slope, tx1);
    bool intRight = x1[2] >= 0 && x1[2] <= clen && x1[1] >= 0 && x1[1] <= clen;
    if ((tx1 > 0.001) && intRight && (tx1 < t || !result)) {
        t = tx1;
        normal = glm::vec3(1.f, 0, 0);
        result = true;
    }

    if (result) {
        glm::vec3 ip = getPointAt(origin, slope, t);
        getUV(ip, u, v);
    }
    return result;
}

void Cube::getUV(glm::vec3 &at, float &u, float &v) {
    return boundingSphere->getUV(at, u, v);   
}

void NonhierBox::getUV(glm::vec3 &at, float &u, float &v) {
    return boundingSphere->getUV(at, u, v);   
}

void Sphere::getUV(glm::vec3 &at, float &u, float &v) {
    float theta;
    glm::vec3 diff = at;
    if (diff[2] == 0 && diff[0] == 0) {
        theta = PI / 2;
    } else {
        theta = atan2(-(diff[2]), diff[0]);
    }
    float phi = acos(std::min(1.f, std::max(-1.f, -(diff[1]))));
    u = (theta + PI) / (2*PI);
    v = phi/PI;
}

void NonhierSphere::getUV(glm::vec3 &at, float &u, float &v) {
    float theta;
    glm::vec3 diff = at - m_pos;
    if (diff[2] == 0 && diff[0] == 0) {
        theta = PI / 2;
    } else {
        theta = atan2(-(diff[2]), diff[0]);
    }
    float phi = acos(std::min(1.0, std::max(-1.0, -(diff[1])/m_radius)));
    u = (theta + PI) / (2*PI);
    v = phi/PI;
}

void NonhierSphere::getUVNormal(glm::vec3 &at, float &u, float &v, glm::vec3 &normal) {
    float epsilon = 0.005;

    float theta;
    glm::vec3 diff = at - m_pos;
    if (diff[2] == 0 && diff[0] == 0) {
        theta = PI / 2;
    } else {
        theta = atan2(-(diff[2]), diff[0]);
    }
    float phi = acos(std::min(1.0, std::max(-1.0, -(diff[1])/m_radius)));
    u = (theta + PI) / (2*PI);
    v = phi/PI;
    
    float scale = (2048.f/material->bump->texture->width());
    u = u*scale;
    v = v*scale;

    glm::vec3 pu = glm::vec3(-sin(theta), 0, cos(theta));
    glm::vec3 pv = glm::cross(normal, pu);

    float uep = u + epsilon*scale;
    float vep = v + epsilon*scale;

    float du = glm::length2(material->bump->getColorAtUV(u, v)) - glm::length2(material->bump->getColorAtUV(uep, v));
    float dv = glm::length2(material->bump->getColorAtUV(u, v)) - glm::length2(material->bump->getColorAtUV(u, vep));

    glm::vec3 result = normal + (du * glm::cross(normal, pv) - dv * glm::cross(normal, pu))/glm::length2(normal);
    normal = result;
}