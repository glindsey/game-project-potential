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

  // == Static members ======================================================
  static void checkSubstances();

  static SubstanceCollection collection;
  static Categories categories;
  static Verbs verbs;

  // Special pointers for very fast retrieval of important substances.
  // Helps to greatly speed up StageBlock initialization.
  static Substance* substance_nothing_;
  static Substance* substance_air_;
};

SubstanceCollection Substance::Impl::collection;
Categories Substance::Impl::categories;
Verbs Substance::Impl::verbs;
Substance* Substance::Impl::substance_nothing_;
Substance* Substance::Impl::substance_air_;


std::vector<Substance const*> Substance::layerSoil;
std::vector<Substance const*> Substance::layerSedimentary;
std::vector<Substance const*> Substance::layerMetamorphic;
std::vector<Substance const*> Substance::layerIgneousIntrusive;
std::vector<Substance const*> Substance::layerIgneousExtrusive;

Substance::Substance(std::string _name)
  : impl(new Impl())
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
}

Substance::~Substance()
{
}

boost::property_tree::ptree const& Substance::getProperties()
{
  return impl->properties;
}

boost::property_tree::ptree const& Substance::getProperties() const
{
  return impl->properties;
}

SubstanceData const& Substance::getData()
{
  return impl->data;
}

SubstanceData const& Substance::getData() const
{
  return impl->data;
}

Visibility Substance::get_visibility()
{
  return impl->data.visibility;
}

Visibility Substance::get_visibility() const
{
  return impl->data.visibility;
}

/*
sf::IntRect const& Substance::getTextureRect(void)
{
  return TAtlas->getRect(impl->data.texNumber);
}

sf::IntRect const& Substance::getTextureRect(void) const
{
  return TAtlas->getRect(impl->data.texNumber);
}
*/

// === Static methods =========================================================
void Substance::initialize()
{
  std::cout << "Scanning substances directory for descriptor files..."
            << std::endl;

  // Scan through the "./data/substances" directory to find all material descriptors.
  MaterialPath materialPath = "data/substances";

  if (boost::filesystem::exists(materialPath))
  {
    if (boost::filesystem::is_directory(materialPath))
    {
      NameVector names;

      // Populate the vector with pathnames.
      for (boost::filesystem::directory_iterator iter =
             boost::filesystem::directory_iterator(materialPath);
           iter != boost::filesystem::directory_iterator(); ++iter)
      {
        if ((boost::filesystem::is_regular_file(iter->path())) && (iter->path().filename().extension()
            == ".xml"))
        {
          names.push_back(iter->path().filename().stem().generic_string());
          if (Settings::debugShowVerboseInfo)
          {
            std::cout << "Parsing substance " << iter->path().filename().stem()
                      << "." << std::endl;
          }
        }
        else
        {
          if (Settings::debugShowVerboseInfo)
          {
            std::cout << iter->path().filename()
                      << " is not a descriptor file, skipping." << std::endl;
          }
        }
      }

      if (names.size() != 0)
      {
        // Attempt to load data for each file seen.
        for (NameIterator iter = names.begin(); iter != names.end(); ++iter)
        {
          std::string tag = *iter;
          Impl::collection.insert(tag, new Substance(tag));
        }
      }
      else
      {
        FATAL_ERROR("\"data/substances\" exists but has no descriptor files in it");
      }
    }
    else
    {
      FATAL_ERROR("\"data/substances\" exists but is not a directory");
    }
  }
  else
  {
    FATAL_ERROR("Substances directory \"data/substances\" does not exist");
  }

  // If "nothing" and/or "air" do not exist, create them.  These two Materials
  // must exist for world generation to work properly.
  if (Impl::collection.count("nothing") == 0)
  {
    std::string tag = "nothing";
    Impl::collection.insert(tag, new Substance(tag));
  }
  Impl::substance_nothing_ = &(Impl::collection.at("nothing"));

  if (Impl::collection.count("air") == 0)
  {
    std::string tag = "air";
    Impl::collection.insert(tag, new Substance(tag));
  }
  Impl::substance_air_ = &(Impl::collection.at("air"));

  Impl::checkSubstances();
}

const Substance& Substance::get(std::string name)
{
  if (Impl::collection.count(name) != 0)
  {
    return Impl::collection.at(name);
  }
  else
  {
    if (Settings::debugShowVerboseInfo)
    {
      std::cout << "Unable to find substance \"" << name
                << "\", returning nothing" << std::endl;
    }
    return Impl::collection.at("nothing");
  }
}

const Substance& Substance::getNothing()
{
  return *(Impl::substance_nothing_);
}

const Substance& Substance::getAir()
{
  return *(Impl::substance_air_);
}

bool Substance::isSubstance(std::string substance, std::string classtype)
{
  if (Impl::collection.count(substance) != 0)
  {
    return (Impl::categories[classtype].count(substance) != 0);
  }
  else
  {
    return false;
  }
}

void Substance::Impl::checkSubstances(void)
{
  std::cout << "*** Parsing substance descriptors for consistency..."
            << std::endl;
  int matCount = 0;

  for (SubstanceCollection::iterator substance_iterator = collection.begin();
       substance_iterator != collection.end(); ++substance_iterator)
  {
    ++matCount;

    std::string substance_name = substance_iterator->first;
    Substance& substance = *(substance_iterator->second);

    if ((substance.getData().phase == Phase::Solid) || (substance.getData().phase
        == Phase::Liquid))
    {
      if (substance.getData().specificGravity == 0)
      {
        MINOR_ERROR("%s is solid or liquid but has no specific gravity defined",
                    substance_name.c_str());
      }
    }

    if (substance.getData().specificGravity < 0)
    {
      MINOR_ERROR("%s has a negative specific gravity defined", substance_name.c_str());
    }

    if (substance.getData().hardness < 0)
    {
      MINOR_ERROR("%s has a negative hardness defined", substance_name.c_str());
    }

    // Use attributes to populate substance category information.
    try
    {
      std::string category_name;
      for (boost::property_tree::ptree::value_type & v : substance.impl->properties.get_child(
             "attributes"))
      {
        category_name = v.first;
        Impl::categories[category_name].insert(substance_name);
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Automatic class setting for certain implied classes.
    // TODO: This should be made configurable in XML.
    if (categories["grass"].count(substance_name) != 0)
    {
      categories["plant"].insert(substance_name);
    }
    if (categories["metal"].count(substance_name) != 0)
    {
      categories["conductor"].insert(substance_name);
    }
    if (categories["sand"].count(substance_name) != 0)
    {
      categories["granular"].insert(substance_name);
    }
    if (categories["soil"].count(substance_name) != 0)
    {
      categories["granular"].insert(substance_name);
    }
    if (categories["wood"].count(substance_name) != 0)
    {
      categories["plant"].insert(substance_name);
      categories["fuel"].insert(substance_name);
    }

    // Consistency checks for substance classes: mutually exclusive classes
    if ((categories["flat"].count(substance_name) != 0) && (categories["granular"].count(
          substance_name.c_str())
        != 0))
    {
      MINOR_ERROR("%s is defined as both flat and granular; which are incompatible",
                  substance_name.c_str());
    }

    // Consistency checks for substance classes: solids.
    // TODO: This should be made configurable in XML.
    if (substance.getData().phase != Phase::Solid)
    {
      if (categories["flux"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a flux, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["gem"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a gem, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["rock"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a rock, but is not a solid",
                    substance_name.c_str());
      }
      if ((categories["metal"].count(substance_name) != 0) &&
          (substance.getData().phase != Phase::Liquid))
      {
        MINOR_ERROR("%s is defined as a metal, but is not a solid or liquid",
                    substance_name.c_str());
      }
      if (categories["sand"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a type of sand, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["soil"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a type of soil, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["wood"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a type of wood, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["plant"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a type of plant matter, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["granular"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as granular, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["flat"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as flat, but is not a solid",
                    substance_name.c_str());
      }
      if (substance.getData().hardness != 0)
      {
        MINOR_ERROR("%s has a hardness value defined, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["polished"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is marked as polishable, but is not a solid",
                    substance_name.c_str());
      }
    }
    else // if the substance IS a solid...
    {

    }

    std::string ore_name = substance.impl->properties.get("attributes.ore", "");
    if ((ore_name != "") && (collection.count(ore_name) == 0))
    {
      MINOR_ERROR("%s is defined as an ore of missing substance %s",
                  substance_name.c_str(), ore_name.c_str());
    }
    ore_name = substance.impl->properties.get("attributes.secondaryore", "");
    if ((ore_name != "") && (collection.count(ore_name) == 0))
    {
      MINOR_ERROR("%s is defined as a secondary ore of missing substance %s",
                  substance_name.c_str(), ore_name.c_str());
    }

    // Store verb results.
    // TODO: Probably better that this info is stored in a separate VerbDictionary or something.
    std::string verb_name;
    try
    {
      for (boost::property_tree::ptree::value_type & v : substance.impl->properties.get_child(
             "actions"))
      {
        verb_name = v.first;
        verbs[verb_name].insert(substance_name);
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Check terrain inclusion layers.
    std::string layer_name = substance.impl->properties.get("terrain.layer",
                             "");
    if (layer_name != "")
    {
      if (layer_name == "igneous-extrusive")
      {
        layerIgneousExtrusive.push_back(&substance);
      }
      else if (layer_name == "igneous-intrusive")
      {
        layerIgneousIntrusive.push_back(&substance);
      }
      else if (layer_name == "sedimentary")
      {
        layerSedimentary.push_back(&substance);
      }
      else if (layer_name == "metamorphic")
      {
        layerMetamorphic.push_back(&substance);
      }
      else if (layer_name == "soil")
      {
        layerSoil.push_back(&substance);
      }
      else if (layer_name == "all")
      {
        layerIgneousExtrusive.push_back(&substance);
        layerIgneousIntrusive.push_back(&substance);
        layerSedimentary.push_back(&substance);
        layerMetamorphic.push_back(&substance);
      }
      else
      {
        MINOR_ERROR("%s has the unknown layer designation \"%s\"",
                    substance_name.c_str(), layer_name.c_str());
      }
    } // end if (name != "")

    // Parse "large deposit" designations.
    try
    {
      for (boost::property_tree::ptree::value_type & v : substance.impl->properties.get_child(
             "terrain.largedeposit"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing largedeposit substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection.at(deposit_name).largeDeposits.push_back(&substance);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Parse "small deposit" designations.
    try
    {
      for (boost::property_tree::ptree::value_type & v : substance.impl->properties.get_child(
             "terrain.smalldeposit"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing smalldeposit substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection.at(deposit_name).smallDeposits.push_back(&substance);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Parse "vein" designations.
    try
    {
      for (boost::property_tree::ptree::value_type & v : substance.impl->properties.get_child(
             "terrain.vein"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing vein substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection.at(deposit_name).veinDeposits.push_back(&substance);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Parse "gangue" designations.
    try
    {
      for (boost::property_tree::ptree::value_type & v : substance.impl->properties.get_child(
             "terrain.gangue"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing gangue substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection.at(deposit_name).gangueDeposits.push_back(&substance);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Parse "single" designations.
    try
    {
      for (boost::property_tree::ptree::value_type & v : substance.impl->properties.get_child(
             "terrain.single"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing single substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection.at(deposit_name).singleDeposits.push_back(&substance);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }
  }
  std::cout << "*** Done parsing descriptors:  " << std::endl;
  std::cout << "***   " << collection.size() << " substances parsed"
            << std::endl;
  std::cout << "***   " << categories.size()
            << " distinct substance attributes counted" << std::endl;
}
