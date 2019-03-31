
#pragma once

#include <random>

class VarHolder {
	public:
  	static bool useThread;
	static int numReflections;
	static int numRefractions;
    static bool showBoundingBox;
	static bool useSubdivision;
	static int subdivideDepth;
	static bool showNormal;
	static bool showGloss;
	static bool softenShadow;
	static bool useTexture;
	static int time;
	static int supersample;
	static bool useBumpmap;
	static bool useDepthoffield;
	static float depthoffield;
	static bool usePointMass;
	static int pmCoeff;

	static std::uniform_real_distribution<> dist;
	static std::mt19937 generator;
};