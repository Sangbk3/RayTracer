// Winter 2019

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	bool hasSet = false;
    std::string filename = "Assets/" + fname;

	std::ifstream ifs( filename.c_str() );
	while( ifs >> code ) {

		if( code == "v" ) {
			ifs >> vx >> vy >> vz;

			m_vertices.push_back( glm::vec3( vx, vy, vz ) );

			if (!hasSet) {
				maxV = glm::vec3( vx, vy, vz );
				minV = glm::vec3( vx, vy, vz );
				hasSet = true;
			} else {
				if (maxV[0] < vx) {
					maxV[0] = vx;
				} else if (minV[0] > vx) {
					minV[0] = vx;
				}
				if (maxV[1] < vy) {
					maxV[1] = vy;
				} else if (minV[1] > vy) {
					minV[1] = vy;
				}
				if (maxV[2] < vz) {
					maxV[2] = vz;
				} else if (minV[2] > vz) {
					minV[2] = vz;
				}
			}
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

    boundingSphere = new NonhierSphere((maxV + minV)/2, glm::length2(maxV - minV)/2);
}


bool Mesh::intersects(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal) {
	if (VarHolder::showBoundingBox) {
		double temp;
		glm::vec3 tempn;
		if (intersectsBoundingBoxAndGetData(origin, slope, temp, tempn)) {
			if (temp > 0.001) { 
				t = temp;
				normal = tempn;
			}
			return true;
		}
	} else {
		if (intersectsBoundingBox(origin, slope)) {
			bool result = false;
			for (int i = 0; i < m_faces.size(); i++) {
				Triangle curF = m_faces[i];
				glm::vec3 v1 = m_vertices[curF.v1];
				glm::vec3 v2 = m_vertices[curF.v2];
				glm::vec3 v3 = m_vertices[curF.v3];

				glm::vec3 otov = (origin - v1);
				glm::vec3 v12 = (v2 - v1);
				glm::vec3 v23 = (v3 - v1);
				glm::vec3 m = slope;

				glm::mat3 m0 = glm::mat3(v12, v23, -m);

				glm::mat3 m1 = glm::mat3(otov, v23, -m);
				glm::mat3 m2 = glm::mat3(v12, otov, -m);
				glm::mat3 m3 = glm::mat3(v12, v23, otov);

				float det0 = glm::determinant(m0);
				float det1 = glm::determinant(m1);
				float det2 = glm::determinant(m2);
				float det3 = glm::determinant(m3);

				if (det0 != 0) {
					float beta = det1/det0;
					float gamma = det2/det0;
					float tt = det3/det0;

					bool intersectsTri = (beta >= 0 && beta <= 1) && (gamma >= 0) && beta + gamma <= 1;
					// std::cout << tt << std::endl;
					if (intersectsTri && tt > 0.001) {
						if (tt < t || !result) {
							t = tt;
							normal = glm::cross(v12, v23);
							result = true;
						}
					}
				}
			}

			return result;
		}
	}

	return false;
}

bool Mesh::intersectsBoundingBox(glm::vec3 origin, glm::vec3 slope) {
	glm::vec3 cori = origin - minV;
    glm::vec3 clen = maxV - minV;
    bool result = false;

    double t = 0;
    double tz0 = (-cori[2])/slope[2];
    glm::vec3 z0 = getPointAt(cori, slope, tz0);
    bool intFront = z0[0] >= 0 && z0[0] <= clen[0] && z0[1] >= 0 && z0[1] <= clen[1];
    if (intFront && (tz0 < t || !result)) {
        t = tz0;
        result = true;
    }

    double tz1 = (clen[2] - cori[2])/slope[2];
    glm::vec3 z1 = getPointAt(cori, slope, tz1);
    bool intBack = z1[0] >= 0 && z1[0] <= clen[0] && z1[1] >= 0 && z1[1] <= clen[1];
    if (intBack && (tz1 < t || !result)) {
        t = tz1;
        result = true;
    }

    double ty0 = (-cori[1])/slope[1];
    glm::vec3 y0 = getPointAt(cori, slope, ty0);
    bool intBot = y0[0] >= 0 && y0[0] <= clen[0] && y0[2] >= 0 && y0[2] <= clen[2];
    if (intBot && (ty0 < t || !result)) {
        t = ty0;
        result = true;
    }

    double ty1 = (clen[1] - cori[1])/slope[1];
    glm::vec3 y1 = getPointAt(cori, slope, ty1);
    bool intTop = y1[0] >= 0 && y1[0] <= clen[0] && y1[2] >= 0 && y1[2] <= clen[2];
    if (intTop && (ty1 < t || !result)) {
        t = ty1;
        result = true;
    }

    double tx0 = (-cori[0])/slope[0];
    glm::vec3 x0 = getPointAt(cori, slope, tx0);
    bool intLeft = x0[2] >= 0 && x0[2] <= clen[2] && x0[1] >= 0 && x0[1] <= clen[1];
    if (intLeft && (tx0 < t || !result)) {
        t = tx0;
        result = true;
    }

    double tx1 = (clen[0] - cori[0])/slope[0];
    glm::vec3 x1 = getPointAt(cori, slope, tx1);
    bool intRight = x1[2] >= 0 && x1[2] <= clen[2] && x1[1] >= 0 && x1[1] <= clen[1];
    if (intRight && (tx1 < t || !result)) {
        t = tx1;
        result = true;
    }

	return result;
}


bool Mesh::intersectsBoundingBoxAndGetData(glm::vec3 origin, glm::vec3 slope, double &t, glm::vec3 &normal) {
	glm::vec3 cori = origin - minV;
    glm::vec3 clen = maxV - minV;
    bool result = false;

    double tz0 = (-cori[2])/slope[2];
    glm::vec3 z0 = getPointAt(cori, slope, tz0);
    bool intFront = z0[0] >= 0 && z0[0] <= clen[0] && z0[1] >= 0 && z0[1] <= clen[1];
    if (tz0 > 0.001 && intFront && (tz0 < t || !result)) {
        t = tz0;
        normal = glm::vec3(0, 0, -1.f);
        result = true;
    }

    double tz1 = (clen[2] - cori[2])/slope[2];
    glm::vec3 z1 = getPointAt(cori, slope, tz1);
    bool intBack = z1[0] >= 0 && z1[0] <= clen[0] && z1[1] >= 0 && z1[1] <= clen[1];
    if (tz1 > 0.001 && intBack && (tz1 < t || !result)) {
        t = tz1;
        normal = glm::vec3(0, 0, 1.f);
        result = true;
    }

    double ty0 = (-cori[1])/slope[1];
    glm::vec3 y0 = getPointAt(cori, slope, ty0);
    bool intBot = y0[0] >= 0 && y0[0] <= clen[0] && y0[2] >= 0 && y0[2] <= clen[2];
    if (ty0 > 0.001 && intBot && (ty0 < t || !result)) {
        t = ty0;
        normal = glm::vec3(0, -1.f, 0);
        result = true;
    }

    double ty1 = (clen[1] - cori[1])/slope[1];
    glm::vec3 y1 = getPointAt(cori, slope, ty1);
    bool intTop = y1[0] >= 0 && y1[0] <= clen[0] && y1[2] >= 0 && y1[2] <= clen[2];
    if (ty1 > 0.001 && intTop && (ty1 < t || !result)) {
        t = ty1;
        normal = glm::vec3(0, 1.f, 0);
        result = true;
    }

    double tx0 = (-cori[0])/slope[0];
    glm::vec3 x0 = getPointAt(cori, slope, tx0);
    bool intLeft = x0[2] >= 0 && x0[2] <= clen[2] && x0[1] >= 0 && x0[1] <= clen[1];
    if (tx0 > 0.001 && intLeft && (tx0 < t || !result)) {
        t = tx0;
        normal = glm::vec3(-1.f, 0, 0);
        result = true;
    }

    double tx1 = (clen[0] - cori[0])/slope[0];
    glm::vec3 x1 = getPointAt(cori, slope, tx1);
    bool intRight = x1[2] >= 0 && x1[2] <= clen[2] && x1[1] >= 0 && x1[1] <= clen[1];
    if (tx1 > 0.001 && intRight && (tx1 < t || !result)) {
        t = tx1;
        normal = glm::vec3(1.f, 0, 0);
        result = true;
    }

	return result;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
