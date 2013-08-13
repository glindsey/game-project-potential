// *** ADDED BY HEADER FIXUP ***
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
// *** END ***
#include "Settings.h"

#include <boost/property_tree/xml_parser.hpp>

bool Settings::debugMapRevealAll;
bool Settings::debugDieOnMinorError;
bool Settings::debugDieOnMajorError;
bool Settings::debugShowVerboseInfo;

StageCoord3 Settings::terrainSize;
StageCoord Settings::terrainStageHeight;
StageCoord Settings::terrainFeatureHeight;
double Settings::terrainFrequency;
double Settings::terrainOctaveCount;
double Settings::terrainPersistence;
double Settings::terrainLacunarity;

int Settings::terrainPlainsThreshold;
int Settings::terrainForestThreshold;
int Settings::terrainSeaLevel;

int Settings::terrainLargeDepositDensity;
int Settings::terrainSmallDepositDensity;
int Settings::terrainVeinDensity;
int Settings::terrainSingleDensity;
bool Settings::terrainGanguePresent;

bool Settings::renderLoadTextures;
unsigned int Settings::renderGeneratedTextureSize;

void Settings::Initialize()
{
  std::cout << "=== GAME SETTINGS ======================================="
            << std::endl;

  boost::property_tree::ptree properties;

  // Attempt to load the debug property tree.
  try
  {
    read_xml("config/settings.xml", properties,
             boost::property_tree::xml_parser::trim_whitespace);
  }
  catch (std::exception& e)
  {
    std::cout << "Can't parse debug settings file config/settings.xml: "
              << e.what() << std::endl;
  }

  std::cout << "Loaded settings.xml." << std::endl;

  // Get cached variables.
  debugMapRevealAll = properties.get<bool>("debug.map.revealall", false);
  debugDieOnMinorError = properties.get<bool>("debug.code.dieonminorerror",
                         false);
  debugDieOnMajorError = properties.get<bool>("debug.code.dieonmajorerror",
                         false);
  debugShowVerboseInfo = properties.get<bool>("debug.code.verbose", false);

  terrainSize.x = properties.get<StageCoord>("terrain.size.x", 128);
  terrainSize.y = properties.get<StageCoord>("terrain.size.y", 128);
  terrainSize.z = properties.get<StageCoord>("terrain.size.z", 64);

  terrainStageHeight = properties.get<StageCoord>("terrain.stageheight", 48);
  terrainFeatureHeight = properties.get<StageCoord>("terrain.featureheight", 10);

  terrainFrequency = properties.get<double>("terrain.frequency", 1.0f);
  terrainOctaveCount = properties.get<int>("terrain.octavecount", 6);
  terrainPersistence = properties.get<double>("terrain.persistence", 0.5f);
  terrainLacunarity = properties.get<double>("terrain.lacunarity", 2.0f);

  terrainPlainsThreshold = properties.get<int>("terrain.plainsthreshold", 50);
  terrainForestThreshold = properties.get<int>("terrain.forestthreshold", 80);
  terrainSeaLevel = properties.get<int>("terrain.sealevel", 5);

  terrainLargeDepositDensity = properties.get<int>(
                                 "terrain.largedepositdensity", 5);
  terrainSmallDepositDensity = properties.get<int>(
                                 "terrain.smalldepositdensity", 5);
  terrainVeinDensity = properties.get<int>("terrain.veindensity", 5);
  terrainSingleDensity = properties.get<int>("terrain.singledensity", 5);
  terrainGanguePresent = properties.get<bool>("terrain.ganguepresent", true);

  renderLoadTextures = properties.get<bool>("render.loadtextures", true);
  renderGeneratedTextureSize = properties.get("render.generatedtexturesize", 64);
}

void Settings::handleMinorError(char* buf,
                                char const* file,
                                int line,
                                char const* func)
{
  printf("%s:%d (%s): MINOR error: %s\n", file, line, func, buf);

  if (debugDieOnMinorError)
  {
    printf("Dying because debug.code.dieonminorerror is true in settings file.\n");
    printf("Press Enter to exit.\n");
    getchar();
    exit(-3);
  }
}

void Settings::handleMajorError(char* buf,
                                char const* file,
                                int line,
                                char const* func)
{
  printf("%s:%d (%s): MAJOR error: %s\n", file, line, func, buf);

  if (debugDieOnMajorError)
  {
    printf("Dying because debug.code.dieonmajorerror is true in settings file.\n");
    printf("Press Enter to exit.\n");
    getchar();
    exit(-2);
  }
}

void Settings::handleFatalError(char* buf,
                                char const* file,
                                int line,
                                char const* func)
{
  printf("%s:%d (%s): FATAL error: %s\n", file, line, func, buf);

  printf("Dying because this is an unrecoverable error.\n");
  printf("Press Enter to exit.\n");
  getchar();
  exit(-1);
}
