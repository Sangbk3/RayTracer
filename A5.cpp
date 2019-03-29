// Winter 2019

#include <glm/ext.hpp>

#include "A5.hpp"
using namespace std;
using namespace glm;

static float PI = 3.14159265359f;
static int numPixels;
static int progress;

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
) {
	std::vector<std::thread> myThreads;

	// Fill in raytracing code here...  
	int pwidth = image.width();
	int pheight = image.height();
	numPixels = pwidth * pheight;
	progress = 0;

	glm::mat4 pixelToWorldTransform = getPixelToWorldTransform(image, fovy, view, up, eye);

	size_t h = image.height();
	size_t w = image.width();
	vec3 *pixelColors = new vec3[w*h];

	GridSubdivision *gridSubdivision;

	if (VarHolder::useSubdivision) {
		gridSubdivision = new GridSubdivision(root, VarHolder::subdivideDepth, eye, view, fovy, pwidth / pheight);
	}

    auto start = std::chrono::system_clock::now();
	system("setterm -cursor off");


	if (VarHolder::useThread) {
		cout << "Initializing threads... " << endl;
		ThreadPool threadPool(256);

		for (uint x = 0; x < w; ++x) {
			std::thread t(setPixelOfImage, pixelToWorldTransform, x, h, pixelColors, eye, root, lights, ambient, gridSubdivision);
			myThreads.push_back(std::move(t));
		}

		for (int i = 0; i < myThreads.size(); i++)
		{
			std::thread & th = myThreads.at(i);
			if (th.joinable()) {
				th.join();
				myThreads.erase(myThreads.begin() + i);

				i--;
			}
		}
	} else {
		for (uint x = 0; x < w; ++x) {
			setPixelOfImage(pixelToWorldTransform, x, h, pixelColors, eye, root, lights, ambient, gridSubdivision);
		}
	}

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			// Red: 
			image(x, y, 0) = (double)pixelColors[y*h + x][0];
			// Green: 
			image(x, y, 1) = (double)pixelColors[y*h + x][1];
			// Blue: 
			image(x, y, 2) = (double)pixelColors[y*h + x][2];
		}
	}

    auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "\rFinished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";

	system("setterm -cursor on");
}

void setPixelOfImage(
	glm::mat4 pixelToWorldTransform, int x, int h, vec3 *pixelColors, glm::vec3 eye,
	SceneNode *root, std::list<Light *> lights, glm::vec3 ambient, GridSubdivision *gridSubdivision) {
		
	for (int y = 0; y < h; y++) {
		glm::vec4 pixel = glm::vec4(x, y, 0, 1);
		pixel = pixelToWorldTransform * pixel;

		vec4 slope = pixel - glm::vec4(eye, 1);
		glm::vec3 m = glm::normalize(glm::vec3(slope));
		glm::vec3 normal = glm::vec3();
		float t = -1.f;
		PhongMaterial *mat;
		bool result = false;

		if (VarHolder::useSubdivision) {
			SceneNode *rNode;
			getClosestObjectPointUseGrid(root, eye, m, t, normal, result, &rNode, gridSubdivision);

			if (result) {
				GeometryNode *casted = static_cast<GeometryNode*>(rNode);
				mat = static_cast<PhongMaterial*>(casted->m_material);
			}
		} else {
			getClosestObjectPoint(root, eye, m, t, normal, &mat, result, glm::mat4(1.f));
		}

		if (!result) {
			// pixelColors[y*h + x][0] = 1 - std::min(((double) y)/h, 0.5);
			// pixelColors[y*h + x][1] = std::max(((double) y)/h, 0.5);
			// pixelColors[y*h + x][2] = std::max(0.4 - ((double) y)/h, 0.2);

			pixelColors[y*h + x][0] = 0.2;
			pixelColors[y*h + x][1] = 0.7;
			pixelColors[y*h + x][2] = 0.7;

		} else {
			glm::vec3 colorHere = getColorAtPoint(eye, m, t, normal, mat, lights, ambient, root, gridSubdivision, 0, 0);
			if (VarHolder::showNormal) {
				colorHere = normal;
			}
			pixelColors[y*h + x][0] = colorHere[0];
			pixelColors[y*h + x][1] = colorHere[1];
			pixelColors[y*h + x][2] = colorHere[2];
		}

		progress++;
		printf("\rRendering in progress: %.2f%%", progress*100.0 / numPixels);
	}
}

void getClosestObjectPointUseGrid(
	SceneNode *node, glm::vec3 origin, glm::vec3 slope,
	float &t, glm::vec3 &normal, bool &result, SceneNode **rNode,
	GridSubdivision *gridSubdivision) {

	gridSubdivision->do3DDDA(origin, slope, t, normal, rNode, result);
}

void getClosestObjectPoint(
	SceneNode *node, glm::vec3 origin, glm::vec3 slope,
	float &t, glm::vec3 &normal, PhongMaterial **mat, bool &result, glm::mat4 parentInv) {
		
	glm::vec4 invO = node->get_inverse() * glm::vec4(origin, 1);
	glm::vec3 invOrigin = glm::vec3(invO);
	glm::vec3 invSlope = glm::vec3(node->get_inverse() * glm::vec4(slope, 0));
	glm::mat4 newT = node->get_inverse() * parentInv;

	if (node->m_nodeType == NodeType::GeometryNode) {
		double temp;
		glm::vec3 tempn;
		GeometryNode *casted = static_cast<GeometryNode*>(node);

		if ((*casted).m_primitive->intersects(invOrigin, invSlope, temp, tempn) && temp > 0.01) {
			if (!result || t > temp) {
				t = temp;
				normal = glm::normalize(glm::vec3(glm::transpose(newT) * glm::vec4(tempn, 0)));
				*mat = static_cast<PhongMaterial*>(casted->m_material);

				result = true;
			}
		}
	}

	for (SceneNode *child : node->children) {
		getClosestObjectPoint(child, invOrigin, invSlope, t, normal, mat, result, newT);
	}
}

glm::vec3 getColorAtPoint(
	glm::vec3 origin, glm::vec3 slope, float t,
	glm::vec3 normal, 
	PhongMaterial *mat,
	std::list<Light *> lights,
	glm::vec3 ambient,
	SceneNode * root,
	GridSubdivision *gridSubdivision,
	int numReflected,
	int numTransmitted) {

	glm::vec3 point = origin + slope * t;

	glm::vec3 resultv = glm::vec3(0.f);
	resultv[0] += (mat)->m_kd[0]*ambient[0];
	resultv[1] += (mat)->m_kd[1]*ambient[1];
	resultv[2] += (mat)->m_kd[2]*ambient[2];

	glm::vec3 m = -1*normalize(slope);
	glm::vec3 n = normalize(normal);

	// shadow rays (diffuse component)
	if (glm::length2((mat)->m_kd) != 0) {
		for (Light * l : lights) {
			if (VarHolder::softenShadow && l->radius > 0) {
				int numIter = 15;
				glm::vec3 resC = glm::vec3(0);

				glm::vec3 lv = l->position - point;
				glm::vec3 pp;
				if (lv[1] != 0) {
					pp = glm::vec3(0, (lv[2]/lv[1]), 1);
				} else if (lv[2] != 0) {
					pp = glm::vec3(1, 0, (lv[0]/lv[2]));
				} else if (lv[0] != 0) {
					pp = glm::vec3((lv[1]/lv[0]), 1, 0);
				}
				pp = normalize(pp);

				for (int i = 0; i < numIter; i++) {
					glm::vec3 normall = glm::vec3();
					float tt = -1.f;
					PhongMaterial *dMat;
					
					int lay = std::log2(numIter);
					int curlay = std::log2(i + 1);
					int numL = pow(2, curlay);
					float angle = 2*PI*(((float) (i%(numL)))/numL);
					float r = l->radius*((float) curlay)/lay;
					glm::vec3 randPos = l->position + glm::rotate(pp*r, angle, lv);

					glm::vec3 lin = normalize(randPos - point);

					bool result = false;

					if (VarHolder::useSubdivision) {
						SceneNode *rNode;

						getClosestObjectPointUseGrid(root, point, lin, tt, normall, result, &rNode, gridSubdivision);
					} else {
						getClosestObjectPoint(root, point, lin, tt, normall, &dMat, result, glm::mat4(1.f));
					}
					if (!result) {
						float ld = distance(l->position, origin);
						float coeff = l->falloff[0] + l->falloff[1]*ld + l->falloff[2]*pow(ld, 2);
						glm::vec3 r = normalize(-1*lin + 2*glm::dot(lin, n)*n);

						resC[0] += glm::max(0.f, glm::min(1.f, (float) ((mat)->m_kd[0]*glm::dot(n, lin) + ((mat)->m_ks[0])*pow(glm::dot(r, m), (mat)->m_shininess))*l->colour[0]/coeff));
						resC[1] += glm::max(0.f, glm::min(1.f, (float) ((mat)->m_kd[1]*glm::dot(n, lin) + ((mat)->m_ks[1])*pow(glm::dot(r, m), (mat)->m_shininess))*l->colour[1]/coeff));
						resC[2] += glm::max(0.f, glm::min(1.f, (float) ((mat)->m_kd[2]*glm::dot(n, lin) + ((mat)->m_ks[2])*pow(glm::dot(r, m), (mat)->m_shininess))*l->colour[2]/coeff));
					}
				}

				resC = (1.0/numIter)*resC;
				resultv += resC;

			} else {
				glm::vec3 normall = glm::vec3();
				float tt = -1.f;
				PhongMaterial *dMat;
				glm::vec3 lin = normalize(l->position - point);

				bool result = false;

				if (VarHolder::useSubdivision) {
					SceneNode *rNode;

					getClosestObjectPointUseGrid(root, point, lin, tt, normall, result, &rNode, gridSubdivision);
				} else {
					getClosestObjectPoint(root, point, lin, tt, normall, &dMat, result, glm::mat4(1.f));
				}
				if (!result) {
					float ld = distance(l->position, origin);
					float coeff = l->falloff[0] + l->falloff[1]*ld + l->falloff[2]*pow(ld, 2);
					glm::vec3 r = normalize(-1*lin + 2*glm::dot(lin, n)*n);

					resultv[0] += glm::max(0.f, glm::min(1.f, (float) ((mat)->m_kd[0]*glm::dot(n, lin) + ((mat)->m_ks[0])*pow(glm::dot(r, m), (mat)->m_shininess))*l->colour[0]/coeff));
					resultv[1] += glm::max(0.f, glm::min(1.f, (float) ((mat)->m_kd[1]*glm::dot(n, lin) + ((mat)->m_ks[1])*pow(glm::dot(r, m), (mat)->m_shininess))*l->colour[1]/coeff));
					resultv[2] += glm::max(0.f, glm::min(1.f, (float) ((mat)->m_kd[2]*glm::dot(n, lin) + ((mat)->m_ks[2])*pow(glm::dot(r, m), (mat)->m_shininess))*l->colour[2]/coeff));
				}
			}
		}
	}

	// Reflections
	if ((glm::length2((mat)->m_ks) != 0) && numReflected < VarHolder::numReflections) {
		sangReflect(point, slope, n, (mat)->m_ks, (mat)->perturb, lights, ambient, root, gridSubdivision, numReflected, numTransmitted, resultv);
	}

	// Refractions
	if (glm::length2((mat)->m_kt) != 0 && numTransmitted < VarHolder::numRefractions) {
		sangRefract(point, slope, n, mat, lights, ambient, root, gridSubdivision, numReflected, numTransmitted, resultv);
	}

	return resultv;
}

void sangRefract(glm::vec3 point, glm::vec3 slope, glm::vec3 n, PhongMaterial *mat, std::list<Light *> lights, glm::vec3 ambient,
	SceneNode * root,
	GridSubdivision *gridSubdivision,
	int numReflected,
	int numTransmitted,
	glm::vec3 &resultv) {


	float refractRatio = 1;
	float cosi = glm::dot(-1*slope, n);
	float cost;
	bool insidePoly = cosi < 0;
	glm::vec3 tD = slope;
	glm::vec3 tL;
	glm::vec3 tnorm = glm::vec3();
	float transt = -1.f;
	PhongMaterial *tMat;
	bool tres = false;

	if (VarHolder::showGloss && mat->perturb >= 0) {
		int numIter = 4;
		glm::vec3 resC = glm::vec3(0);
		for (int i = 0; i < numIter; i++) {
			transt = -1.f;
			tres = false;
			if (!insidePoly) {
				tL = (1.0 / ((mat)->refractIndex))*((tD + n*cosi)) - n * sqrt(1 - pow((1.0 / ((mat)->refractIndex)), 2)*(1 - pow(cosi, 2)));
				tL = perturbNormal(tL, mat->perturb);
				tL = glm::normalize(tL);
				cost = glm::dot(-1*tL, n);
				refractRatio = 1 - fresnelR(1, (mat)->refractIndex, cosi, cost);

			} else {
				tL = (((mat)->refractIndex) / 1.0)*((tD + n*cosi)) - n * sqrt(1 - pow((((mat)->refractIndex) / 1.0), 2)*(1 - pow(cosi, 2)));
				tL = perturbNormal(tL, mat->perturb);
				tL = glm::normalize(tL);
				cost = glm::dot(-1*tL, n);
				refractRatio = 1 - fresnelR((mat)->refractIndex, 1, cosi, cost);
			}

			if ((1 - refractRatio) > 0 && numReflected < VarHolder::numReflections) {
				sangReflect(point, slope, n, (1 - refractRatio)*((mat)->m_kt), (mat)->perturb, lights, ambient, root, gridSubdivision, numReflected, numTransmitted + 1, resC);
			}

			if (refractRatio > 0) {
				if (VarHolder::useSubdivision) {
					SceneNode *rNode;
					getClosestObjectPointUseGrid(root, point - tL*0.002, tL, transt, tnorm, tres, &rNode, gridSubdivision);
					if (tres) {
						GeometryNode *casted = static_cast<GeometryNode*>(rNode);
						tMat = static_cast<PhongMaterial*>(casted->m_material);
					}
				} else {
					getClosestObjectPoint(root, point - tL*0.002, tL, transt, tnorm, &tMat, tres, glm::mat4(1.f));
				}
				
				if (tres) {
					glm::vec3 transL = getColorAtPoint(point, tL, transt, tnorm, tMat, lights, ambient, root, gridSubdivision, numReflected, numTransmitted + 1);
					resC[0] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[0]*refractRatio*transL[0]));
					resC[1] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[1]*refractRatio*transL[1]));
					resC[2] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[2]*refractRatio*transL[2]));
				} else {
					resC[0] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[0]*refractRatio*0.2f));
					resC[1] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[1]*refractRatio*0.2f));
					resC[2] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[2]*refractRatio*0.2f));
				}
			}
		}

		resC = (1.0/numIter)*(resC);
		resultv += resC;

	} else {
		if (!insidePoly) {
			tL = (1.0 / ((mat)->refractIndex))*((tD + n*cosi)) - n * sqrt(1 - pow((1.0 / ((mat)->refractIndex)), 2)*(1 - pow(cosi, 2)));
			tL = glm::normalize(tL);
			cost = glm::dot(-1*tL, n);
			refractRatio = 1 - fresnelR(1, (mat)->refractIndex, cosi, cost);

		} else {
			tL = (((mat)->refractIndex) / 1.0)*((tD + n*cosi)) - n * sqrt(1 - pow((((mat)->refractIndex) / 1.0), 2)*(1 - pow(cosi, 2)));
			tL = glm::normalize(tL);
			cost = glm::dot(-1*tL, n);
			refractRatio = 1 - fresnelR((mat)->refractIndex, 1, cosi, cost);
		}

		if ((1 - refractRatio) > 0 && numReflected < VarHolder::numReflections) {
			sangReflect(point, slope, n, (1 - refractRatio)*((mat)->m_kt), (mat)->perturb, lights, ambient, root, gridSubdivision, numReflected, numTransmitted + 1, resultv);
		}

		if (refractRatio > 0) {
			if (VarHolder::useSubdivision) {
				SceneNode *rNode;
				getClosestObjectPointUseGrid(root, point - tL*0.002, tL, transt, tnorm, tres, &rNode, gridSubdivision);
				if (tres) {
					GeometryNode *casted = static_cast<GeometryNode*>(rNode);
					tMat = static_cast<PhongMaterial*>(casted->m_material);
				}
			} else {
				getClosestObjectPoint(root, point - tL*0.002, tL, transt, tnorm, &tMat, tres, glm::mat4(1.f));
			}
			
			if (tres) {
				glm::vec3 transL = getColorAtPoint(point, tL, transt, tnorm, tMat, lights, ambient, root, gridSubdivision, numReflected, numTransmitted + 1);
				resultv[0] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[0]*refractRatio*transL[0]));
				resultv[1] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[1]*refractRatio*transL[1]));
				resultv[2] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[2]*refractRatio*transL[2]));
			} else {
				resultv[0] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[0]*refractRatio*0.2f));
				resultv[1] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[1]*refractRatio*0.8f));
				resultv[2] += glm::max(0.f, glm::min(1.f, (float) (mat)->m_kt[2]*refractRatio*0.8f));
			}
		}
	}
}

void sangReflect(glm::vec3 origin, glm::vec3 slope, glm::vec3 normal, glm::vec3 ks, double perturb, std::list<Light *> lights, glm::vec3 ambient,
	SceneNode * root,
	GridSubdivision *gridSubdivision,
	int numReflected,
	int numTransmitted,
	glm::vec3 &resultv) {

	glm::vec3 r = glm::normalize(slope - 2*glm::dot(slope, normal)*normal);
	glm::vec3 rnorm = glm::vec3();
	float rt = -1.f;
	PhongMaterial *rMat;
	bool rres = false;

	if (VarHolder::showGloss && (perturb) >= 0) {
		int numIter = 4;
		glm::vec3 resC = glm::vec3(0);
		for (int i = 0; i < numIter; i++) {
			rt = -1.f;
			rres = false;
			vec3 newr = perturbNormal(r, (perturb));

			if (VarHolder::useSubdivision) {
				SceneNode *rNode;
				getClosestObjectPointUseGrid(root, origin, newr, rt, rnorm, rres, &rNode, gridSubdivision);
				if (rres) {
					GeometryNode *casted = static_cast<GeometryNode*>(rNode);
					rMat = static_cast<PhongMaterial*>(casted->m_material);
				}
			} else {
				getClosestObjectPoint(root, origin, newr, rt, rnorm, &rMat, rres, glm::mat4(1.f));
			}
			
			if (rres) {
				glm::vec3 refL = getColorAtPoint(origin, newr, rt, rnorm, rMat, lights, ambient, root, gridSubdivision, numReflected + 1, numTransmitted);
				resC[0] += glm::max(0.f, glm::min(1.f, (float) (ks[0])*refL[0]));
				resC[1] += glm::max(0.f, glm::min(1.f, (float) (ks[1])*refL[1]));
				resC[2] += glm::max(0.f, glm::min(1.f, (float) (ks[2])*refL[2]));
			}
		}

		resC = (1.0/numIter)*resC;
		resultv += resC;
	} else {
		if (VarHolder::useSubdivision) {
			SceneNode *rNode;
			getClosestObjectPointUseGrid(root, origin, r, rt, rnorm, rres, &rNode, gridSubdivision);
			if (rres) {
				GeometryNode *casted = static_cast<GeometryNode*>(rNode);
				rMat = static_cast<PhongMaterial*>(casted->m_material);
			}
		} else {
			getClosestObjectPoint(root, origin, r, rt, rnorm, &rMat, rres, glm::mat4(1.f));
		}
		
		if (rres) {
			glm::vec3 refL = getColorAtPoint(origin, r, rt, rnorm, rMat, lights, ambient, root, gridSubdivision, numReflected + 1, numTransmitted);
			resultv[0] += glm::max(0.f, glm::min(1.f, (float) (ks[0])*refL[0]));
			resultv[1] += glm::max(0.f, glm::min(1.f, (float) (ks[1])*refL[1]));
			resultv[2] += glm::max(0.f, glm::min(1.f, (float) (ks[2])*refL[2]));
		}
	}
}

glm::vec3 perturbNormal(glm::vec3 &ray, double perturb) {
	float alpha = acos(pow((1 - VarHolder::dist(VarHolder::generator)), (1.0/perturb)));
	float beta = VarHolder::dist(VarHolder::generator)*2*PI;

	glm::vec3 zaxis = glm::vec3(0,0,1);
	glm::vec3 result = ray;
	if (glm::dot(ray, zaxis) < 1) {
		glm::vec3 tangent = glm::cross(zaxis, ray);
		result = glm::rotate(result, alpha, tangent);
		result = glm::rotate(result, beta, ray);
	} else {
		glm::vec3 tangent = glm::cross(glm::vec3(0,1,0), ray);
		result = glm::rotate(result, alpha, tangent);
		result = glm::rotate(result, beta, ray);
	}

	return result;
}

float fresnelR(float iori, float iort, float cosi, float cost) {
	if (cost <= 0) {
		return 1;
	}
	float rs = pow(((iort*cosi) - (iori*cost)) / ((iort*cosi) + (iori*cost)), 2); 
    float rp = pow(((iori*cosi) - (iort*cost)) / ((iori*cosi) + (iort*cost)), 2);
	return (rs + rp) / 2;
}

glm::mat4 getPixelToWorldTransform(Image image, double fovy, glm::vec3 view, glm::vec3 up, glm::vec3 eye) {
	int pwidth = image.width();
	int pheight = image.height();

	glm::mat4 pixelToWorldTransform = glm::mat4(1.f);
	float idist = 10;

	pixelToWorldTransform = glm::translate(glm::vec3(-pwidth/2.f, -pheight/2.f, idist)) * pixelToWorldTransform;

	float iheight = 2*idist*tanf(PI * fovy/360);
	float iwidth = iheight*pwidth/((float) pheight);

	pixelToWorldTransform = glm::scale(glm::vec3(-iheight/pheight, -iheight/pheight, 1)) * pixelToWorldTransform;

	glm::mat4 rotateM = glm::mat4(1.f);
	glm::vec3 wval = divide(view - eye, distance(view, eye));
	glm::vec3 mval = glm::cross(up, wval);
	mval = divide(mval, distance(mval, glm::vec3(0,0,0)));
	glm::vec3 vval = glm::cross(wval, mval);

	rotateM[0] = glm::vec4(mval, 0);
	rotateM[1] = glm::vec4(vval, 0);
	rotateM[2] = glm::vec4(wval, 0);
	rotateM[3] = glm::vec4(0,0,0,1);

	pixelToWorldTransform = rotateM * pixelToWorldTransform;

	pixelToWorldTransform = glm::translate(eye) * pixelToWorldTransform;

	return pixelToWorldTransform;
}

float distance(glm::vec3 p1, glm::vec3 p2) {
	return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2) + pow(p1[2] - p2[2], 2));
}

glm::vec3 divide(glm::vec3 v, float weight) {
	return glm::vec3(v[0] / weight, v[1] / weight, v[2] / weight);
}

glm::vec3 getLightPosition(Light *light, int count, int total, glm::vec3 &point) {

	// float a = VarHolder::dist(VarHolder::generator) * 2 * PI;
	// float b = VarHolder::dist(VarHolder::generator) * 2 * PI;
	// glm::vec3 deltaV = glm::vec3(r, 0, 0);
	// deltaV = glm::rotateZ(deltaV, a);
	// deltaV = glm::rotateY(deltaV, b);
	// return deltaV + light->position;
}