#ifndef SUBSTANCEDATA_H_INCLUDED
#define SUBSTANCEDATA_H_INCLUDED

#include <string>

#include "common.h"

struct SubstanceData
{
  std::string name;         ///< Material's name.
  SerialNumber texNumber;   ///< Texture (if any) to use.
  bool textured;            ///< Indicates whether texture exists.
  glm::vec4 color;          ///< Base color.
  glm::vec4 colorSecond;    ///< Secondary color.
  glm::vec4 colorRender;    ///< Color to use for rendering.
  glm::vec4 color_specular; ///< Specular color (determines shininess).
  Visibility visibility;    ///< Visibility (invisible/transparent/opaque).
  Phase phase;              ///< Material phase (solid, liquid, etc).
  int hardness;             ///< Approximate hardness on Mohs scale * 10.
  int specificGravity;      ///< Specific gravity of material.
  int tempFreeze;           ///< Degrees Celsius at which material freezes.
  int tempMelt;             ///< Degrees Celsius at which material melts.
  int tempBurn;             ///< Degrees Celsius at which material burns.
  int tempBoil;             ///< Degrees Celsius at which material boils.
  int tempCondense;         ///< Degrees Celsius at which material condenses.
};

#endif // SUBSTANCEDATA_H_INCLUDED
