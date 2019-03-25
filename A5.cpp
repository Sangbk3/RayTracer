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

    auto start = std::chrono::system_clock::now();

	system("setterm -cursor off");

	if (VarHolder::useThread) {
		cout << "Initializing threads... " << endl;
	}

	for (uint x = 0; x < w; ++x) {
		//With Threads
		if (VarHolder::useThread) {

			std::thread t(setPixelOfImage, pixelToWorldTransform, x, h, pixelColors, eye, root, lights, ambient);
			myThreads.push_back(std::move(t));
		} else {
			setPixelOfImage(pixelToWorldTransform, x, h, pixelColors, eye, root, lights, ambient);
		}
	}

	if (VarHolder::useThread) {
		for (int i = 0; i < myThreads.size(); i++)
		{
			std::thread & th = myThreads.at(i);
			if (th.joinable()) {
				th.join();
				myThreads.erase(myThreads.begin() + i);

				i--;
			}
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
	SceneNode *root, std::list<Light *> lights, glm::vec3 ambient) {
		
	for (int y = 0; y < h; y++) {
		glm::vec4 pixel = glm::vec4(x, y, 0, 1);
		pixel = pixelToWorldTransform * pixel;

		vec4 slope = pixel - glm::vec4(eye, 1);
		glm::vec3 m = glm::normalize(glm::vec3(slope));
		glm::vec3 normal = glm::vec3();
		float t = -1.f;
		glm::vec3 kd;
		glm::vec3 ks;
		double shininess;
		bool result = false;
		getClosestObjectPoint(root, eye, m, t, normal, kd, ks, shininess, result, glm::mat4(1.f));

		if (!result) {
			pixelColors[y*h + x][0] = 1 - std::min(((double) y)/h, 0.5);
			pixelColors[y*h + x][1] = std::max(((double) y)/h, 0.5);
			pixelColors[y*h + x][2] = std::max(0.4 - ((double) y)/h, 0.2);

		} else {
			glm::vec3 colorHere = getColorAtPoint(eye, m, t, normal, kd, ks, shininess, lights, ambient, root, 0);
			pixelColors[y*h + x][0] = colorHere[0];
			pixelColors[y*h + x][1] = colorHere[1];
			pixelColors[y*h + x][2] = colorHere[2];
		}

		progress++;
		printf("\rRendering in progress: %.2f%%", progress*100.0 / numPixels);
	}
}

void getClosestObjectPoint(
	SceneNode *node, glm::vec3 origin, glm::vec3 slope,
	float &t, glm::vec3 &normal, glm::vec3& kd, glm::vec3& ks,
	double &shininess, bool &result, glm::mat4 parentInv) {
		
	glm::vec4 invO = node->get_inverse() * glm::vec4(origin, 1);
	glm::vec3 invOrigin = glm::vec3(invO);
	glm::vec3 invSlope = glm::vec3(node->get_inverse() * glm::vec4(slope, 0));
	glm::mat4 newT = node->get_inverse() * parentInv;

	if (node->m_nodeType == NodeType::GeometryNode) {
		double temp;
		glm::vec3 tempn;
		GeometryNode *casted = static_cast<GeometryNode*>(node);

		if ((*casted).m_primitive->intersects(invOrigin, invSlope, temp, tempn) && temp > 0.001) {
			if (!result || t > temp) {
				// std::cout << temp << std::endl;
				t = temp;
				normal = normalize(glm::vec3(glm::transpose(newT) * glm::vec4(tempn, 0)));
				PhongMaterial *mat = static_cast<PhongMaterial*>(casted->m_material);
				kd = mat->m_kd;
				ks = mat->m_ks;
				shininess = mat->m_shininess;

				result = true;
			}
		}
	}

	for (SceneNode *child : node->children) {
		getClosestObjectPoint(child, invOrigin, invSlope, t, normal, kd, ks, shininess, result, newT);
	}
}

glm::vec3 getColorAtPoint(
	glm::vec3 origin, glm::vec3 slope, float t,
	glm::vec3 normal, glm::vec3 kd, glm::vec3 ks,
	double shininess, std::list<Light *> lights, glm::vec3 ambient,
	SceneNode * root,
	int numReflected) {

	glm::vec3 point = origin + slope * t;

	glm::vec3 resultv = glm::vec3(0.f);
	resultv[0] += kd[0]*ambient[0];
	resultv[1] += kd[1]*ambient[1];
	resultv[2] += kd[2]*ambient[2];

	glm::vec3 m = -1*normalize(slope);
	glm::vec3 n = normalize(normal);

	// if (glm::length2(kd) != 0) {
	for (Light * l : lights) {
		glm::vec3 normall = glm::vec3();
		float tt = -1.f;
		glm::vec3 kdd;
		glm::vec3 kss;
		double shininesss;

		float ld = distance(l->position, origin);
		float coeff = l->falloff[0] + l->falloff[1]*ld + l->falloff[2]*pow(ld, 2);

		glm::vec3 lin = normalize(l->position - point);
		bool result = false;
		glm::vec3 r = normalize(-1*lin + 2*glm::dot(lin, n)*n);
		// glm::vec3 r = normalize(m - 2*glm::dot(m, n)*n);
		// glm::vec3 r = slope - 2*glm::dot(slope, n)*n;

		getClosestObjectPoint(root, point, lin, tt, normall, kdd, kss, shininesss, result, glm::mat4(1.f));

		if (!result) {
			resultv[0] += glm::max(0.f, glm::min(1.f, (float) (kd[0]*glm::dot(n, lin) + ks[0]*pow(glm::dot(r, m), shininess))*l->colour[0]/coeff));
			resultv[1] += glm::max(0.f, glm::min(1.f, (float) (kd[1]*glm::dot(n, lin) + ks[1]*pow(glm::dot(r, m), shininess))*l->colour[1]/coeff));
			resultv[2] += glm::max(0.f, glm::min(1.f, (float) (kd[2]*glm::dot(n, lin) + ks[2]*pow(glm::dot(r, m), shininess))*l->colour[2]/coeff));

			// resultv[0] += glm::max(0.f, glm::min(1.f, (float) (kd[0]*glm::dot(n, lin))*l->colour[0]));
			// resultv[1] += glm::max(0.f, glm::min(1.f, (float) (kd[1]*glm::dot(n, lin))*l->colour[1]));
			// resultv[2] += glm::max(0.f, glm::min(1.f, (float) (kd[2]*glm::dot(n, lin))*l->colour[2]));
		}
	}
	// }

	if (glm::length2(ks) != 0 && numReflected < VarHolder::numReflections) {
		// glm::vec3 pointR = point + r * t;
		glm::vec3 r = slope - 2*glm::dot(slope, n)*n;
		glm::vec3 rnorm = glm::vec3();
		float rt = -1.f;
		glm::vec3 rd;
		glm::vec3 rs;
		double rshin;
		bool rres = false;

		getClosestObjectPoint(root, point, r, rt, rnorm, rd, rs, rshin, rres, glm::mat4(1.f));

		if (rres) {
			glm::vec3 refL = 0.3*getColorAtPoint(point, r, rt, rnorm, rd, rs, rshin, lights, ambient, root, numReflected + 1);
			resultv[0] += glm::max(0.f, glm::min(1.f, (float) ks[0]*refL[0]));
			resultv[1] += glm::max(0.f, glm::min(1.f, (float) ks[1]*refL[1]));
			resultv[2] += glm::max(0.f, glm::min(1.f, (float) ks[2]*refL[2]));
		}

	}

	return resultv;
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