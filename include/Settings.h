#ifndef SETTINGS_H
#define SETTINGS_H

#include "common.h"

/// Totally static class containing all global options read from the
/// configuration file.
class Settings
{
public:
  /// Reads the settings file and caches settings variables.
  static void Initialize();

  /// If debugDieOnMinorError is true, kills the program; otherwise, does nothing.
  static void handleMinorError(char* buf,
                               char const* file,
                               int line,
                               char const* func);

  /// If debugDieOnMajorError is true, kills the program; otherwise, does nothing.
  static void handleMajorError(char* buf,
                               char const* file,
                               int line,
                               char const* func);

  /// Kills the program, unequivocably.
  static void handleFatalError(char* buf,
                               char const* file,
                               int line,
                               char const* func);

  static bool debugMapRevealAll;
  static bool debugDieOnMinorError;
  static bool debugDieOnMajorError;
  static bool debugShowVerboseInfo;

  static StageCoord3 terrainSize;
  static StageCoord terrainStageHeight;
  static StageCoord terrainFeatureHeight;
  static double terrainFrequency;
  static double terrainOctaveCount;
  static double terrainPersistence;
  static double terrainLacunarity;

  static int terrainPlainsThreshold;
  static int terrainForestThreshold;
  static int terrainSeaLevel;

  static int terrainLargeDepositDensity;
  static int terrainSmallDepositDensity;
  static int terrainVeinDensity;
  static int terrainSingleDensity;
  static bool terrainGanguePresent;

  static bool renderLoadTextures;
  static unsigned int renderGeneratedTextureSize;
protected:

private:
  Settings() = delete;  ///< No instantiations!
};
#endif // SETTINGS_H
