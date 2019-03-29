#include "VarHolder.hpp"

bool VarHolder::useThread = true;
int VarHolder::numReflections = 0;
int VarHolder::numRefractions = 0;
bool VarHolder::showBoundingBox = false;
bool VarHolder::useSubdivision = false;
int VarHolder::subdivideDepth = 0;
bool VarHolder::showNormal = false;
bool VarHolder::showGloss = false;
bool VarHolder::softenShadow = false;
std::uniform_real_distribution<> VarHolder::dist(0, 1);
std::mt19937 VarHolder::generator;