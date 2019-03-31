// Winter 2019

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"
#include "VarHolder.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
  
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;

	glm::vec3 maxV;
	glm::vec3 minV;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);

    bool intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal, float &u, float &v) override;
	bool intersectsBoundingBox(glm::vec3 origin, glm::vec3 slope);
	bool intersectsBoundingBoxAndGetData(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal);
    void getUV(glm::vec3 &at, float &u, float &v);
};
