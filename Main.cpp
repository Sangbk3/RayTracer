// Winter 2019

#include <iostream>
#include "scene_lua.hpp"
#include "VarHolder.hpp"

int main(int argc, char** argv)
{
  std::string filename = "simple.lua";
  if (argc > 1) {
    filename = argv[1];
    std::cout << filename << std::endl;
  }
  if (argc > 2) {
    for (int i = 1; i < argc; i++) {
      if (std::string(argv[i]) == "-M") {
        VarHolder::useThread = false;
      }
      if (std::string(argv[i]) == "-R") {
        if (i + 1 < argc) {
          VarHolder::numReflections = std::stoi(std::string(argv[i+1]));
          i++;
        }
      }
      if (std::string(argv[i]) == "-T") {
        if (i + 1 < argc) {
          VarHolder::numRefractions = std::stoi(std::string(argv[i+1]));
          i++;
        }
      }
      if (std::string(argv[i]) == "-B") {
        VarHolder::showBoundingBox = true;
      }
      if (std::string(argv[i]) == "-S") {
        if (i + 1 < argc) {
          VarHolder::useSubdivision = true;
          VarHolder::subdivideDepth = std::stoi(std::string(argv[i+1]));
          i++;
        }
      }

      if (std::string(argv[i]) == "-N") {
        VarHolder::showNormal = true;
      }
      if (std::string(argv[i]) == "-G") {
        VarHolder::showGloss = true;
      }
      if (std::string(argv[i]) == "-Z") {
        VarHolder::softenShadow = true;
      }
      if (std::string(argv[i]) == "-tt") {
        VarHolder::useTexture = true;
      }
      if (std::string(argv[i]) == "-ss") {
        if (i + 1 < argc) {
          VarHolder::supersample = std::stoi(std::string(argv[i+1]));
          i++;
        }
      }
      if (std::string(argv[i]) == "-b") {
        VarHolder::useBumpmap = true;
      }
      if (std::string(argv[i]) == "-d") {
        if (i + 1 < argc) {
          VarHolder::useDepthoffield = true;
          VarHolder::depthoffield = std::stod(std::string(argv[i+1]));
          i++;
        }
      }
      if (std::string(argv[i]) == "-pm") {
        if (i + 1 < argc) {
          VarHolder::usePointMass = true;
          VarHolder::pmCoeff = std::stoi(std::string(argv[i+1]));
          i++;
        }
      }

      if (std::string(argv[i]) == "-a") {
        if (i + 1 < argc) {
          VarHolder::time = std::stoi(std::string(argv[i+1]));
          i++;
        }
      }
    }
  }

  if (!run_lua(filename)) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }
}
