#include "Substance.h"

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <set>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/unordered_map.hpp>
#include <glm/glm.hpp>
#include <noise/noise.h>

#include "ErrorMacros.h"
#include "Settings.h"
#include "TextureAtlas.h"

typedef boost::ptr_unordered_map<std::string, Substance> SubstanceCollection;
typedef std::unordered_map<std::string, std::set<std::string> > Categories;
typedef std::unordered_map<std::string, std::set<std::string> > Verbs;
typedef boost::filesystem::path MaterialPath;
typedef std::vector<std::string> NameVector;
typedef NameVector::const_iterator NameIterator;

struct Substance::Impl
{
  /// Property tree containing all properties of this chunk type.
  boost::property_tree::ptree properties;

  /// Data corresponding to material.
  SubstanceData data;
};

Substance::Substance()
  : impl(new Impl())
{
}

Substance::~Substance()
{
}

std::string Substance::get_string_property(std::string property) const
{
  return impl->properties.get(property, "");
}

int Substance::get_int_property(std::string property,
                                int default_value = 0) const
{
  return impl->properties.get<int>(property, default_value);
}

bool Substance::get_bool_property(std::string property,
                                  bool default_value = false) const
{
  return impl->properties.get<bool>(property, default_value);
}

SubstanceData Substance::get_data() const
{
  return impl->data;
}

Visibility Substance::get_visibility() const
{
  return impl->data.visibility;
}

void Substance::check_consistency(void)
{
  SubstanceData& data = impl->data;

  if ((data.phase == Phase::Solid) || (data.phase == Phase::Liquid))
  {
    if (data.specificGravity == 0)
    {
      MINOR_ERROR("%s is solid or liquid but has no specific gravity defined",
                  data.name.c_str());
    }
  }

  if (data.specificGravity < 0)
  {
    MINOR_ERROR("%s has a negative specific gravity defined",
                data.name.c_str());
  }

  if (data.hardness < 0)
  {
    MINOR_ERROR("%s has a negative hardness defined", data.name.c_str());
  }
}

bool Substance::load(std::string _name)
{
  bool is_opaque, is_visible;

  impl->data.name = _name;

  // Attempt to load the property tree for this substance.
  try
  {
    read_xml("data/substances/" + impl->data.name + ".xml", impl->properties,
             boost::property_tree::xml_parser::trim_whitespace);
  }
  catch (std::exception& e)
  {
    MINOR_ERROR("Can't parse \"%s\" descriptor file: %s",
                impl->data.name.c_str(), e.what());
    return false;
  }

  // Get substance visibility.
  // TODO: replace display.opaque, display.visible with single "display.visibility" attribute in XML.
  is_opaque = impl->properties.get<bool>("display.opaque", false);
  is_visible = impl->properties.get<bool>("display.visible", true);

  if (!is_visible)
  {
    impl->data.visibility = Visibility::Invisible;
  }
  else if (!is_opaque)
  {
    impl->data.visibility = Visibility::Translucent;
  }
  else
  {
    impl->data.visibility = Visibility::Opaque;
  }

  // Get substance phase.
  std::string strPhase = impl->properties.get("physical.phase", "unknown");
  if (strPhase == "solid")
  {
    impl->data.phase = Phase::Solid;
  }
  else if (strPhase == "viscous")
  {
    impl->data.phase = Phase::Viscous;
  }
  else if (strPhase == "liquid")
  {
    impl->data.phase = Phase::Liquid;
  }
  else if (strPhase == "gas")
  {
    impl->data.phase = Phase::Gas;
  }
  else if (strPhase == "ethereal")
  {
    impl->data.phase = Phase::Ethereal;
  }
  else if (strPhase == "none")
  {
    impl->data.phase = Phase::None;
  }
  else if (strPhase == "unknown")
  {
    impl->data.phase = Phase::Unknown;
  }
  else
  {
    MAJOR_ERROR("Substance \"%s\" has unknown phase \"%s\"",
                impl->data.name.c_str(), strPhase.c_str());
    impl->data.phase = Phase::Unknown;
  }

  // Get physical constants.
  impl->data.hardness = impl->properties.get<int>("physical.hardness", 0);
  impl->data.specificGravity = impl->properties.get<int>(
                                 "physical.specificgravity", 0);
  impl->data.tempFreeze = impl->properties.get<int>(
                            "physical.freezetemp", std::numeric_limits<int>::min());
  impl->data.tempMelt = impl->properties.get<int>(
                          "physical.melttemp", std::numeric_limits<int>::max());
  impl->data.tempBurn = impl->properties.get<int>(
                          "physical.burntemp", std::numeric_limits<int>::max());
  impl->data.tempBoil = impl->properties.get<int>(
                          "physical.boiltemp", std::numeric_limits<int>::max());
  impl->data.tempCondense = impl->properties.get<int>(
                              "physical.condensetemp", std::numeric_limits<int>::min());

  // Get the colors associated with this texture.
  std::string colorString;
  unsigned int _color;
  float _r, _g, _b, _a;
  colorString = impl->properties.get(
                  "display.color.main", impl->properties.get("display.color", "00000000"));
  _color = strtoul(colorString.c_str(), nullptr, 16);
  _r = static_cast<float>((_color >> 24) % 256) / 255;
  _g = static_cast<float>((_color >> 16) % 256) / 255;
  _b = static_cast<float>((_color >> 8) % 256) / 255;
  _a = static_cast<float>(_color % 256) / 255;
  impl->data.color = glm::vec4(_r, _g, _b, _a);

  // Get second color, if any.
  float _r2, _g2, _b2, _a2;
  colorString = impl->properties.get("display.color.secondary", "00000000");
  _color = strtoul(colorString.c_str(), nullptr, 16);
  _r2 = static_cast<float>((_color >> 24) % 256) / 255;
  _g2 = static_cast<float>((_color >> 16) % 256) / 255;
  _b2 = static_cast<float>((_color >> 8) % 256) / 255;
  _a2 = static_cast<float>(_color % 256) / 255;
  impl->data.colorSecond = glm::vec4(_r2, _g2, _b2, _a2);

  // Get specular color, if any.
  // Defaults to 0.3, 0.3, 0.3, 1.0 (4D4D4DFF).
  float _rs, _gs, _bs, _as;
  colorString = impl->properties.get("display.color.specular", "4D4D4DFF");
  _color = strtoul(colorString.c_str(), nullptr, 16);
  _rs = static_cast<float>((_color >> 24) % 256) / 255;
  _gs = static_cast<float>((_color >> 16) % 256) / 255;
  _bs = static_cast<float>((_color >> 8) % 256) / 255;
  _as = static_cast<float>(_color % 256) / 255;
  impl->data.color_specular = glm::vec4(_rs, _gs, _bs, _as);

  // Try to load the graphic associated with the substance, which should be
  // substance-XXXX where XXXX is the material name.
  std::string imageName = "sprites/substance-" + impl->data.name + ".tga";

  sf::Image tempImage;

  bool success = false;

  if ((Settings::renderLoadTextures) && (boost::filesystem::exists(imageName)))
  {
    success = tempImage.loadFromFile(imageName);
  }

  if (success)
  {
    impl->data.texNumber = TAtlas->save(tempImage);
    impl->data.colorRender = glm::vec4(1.0f);
    impl->data.textured = true;
  }
  else
  {
    impl->data.colorRender = impl->data.color;
    impl->data.textured = false;
  }

  return true;
}

boost::property_tree::ptree const& Substance::get_properties() const
{
  return impl->properties;
}

