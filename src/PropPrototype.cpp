// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
// *** END ***
#define BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "PropPrototype.h"

#include "ErrorMacros.h"
#include "Settings.h"
#include "TextureAtlas.h"

namespace gsl
{
namespace rectopia
{

boost::ptr_map<std::string, PropPrototype> PropPrototype::collection;

typedef boost::filesystem::path TypePath;
typedef boost::container::vector<std::string> NameVector;
typedef NameVector::const_iterator NameIterator;

Visibility PropPrototype::get_visibility() const
{
  return visibility;
}

void PropPrototype::initialize()
{
  std::cout << "=== PROP PROTOTYPE STORE ================================"
            << std::endl;

  // Before anything else we create the "anomaly" prop type.
  std::string tag = "anomaly";
  collection.insert(tag, new PropPrototype("anomaly"));

  // Scan through the "./data/props" directory to find all prop type descriptors.
  TypePath typePath
  { "data/props" };

  if (boost::filesystem::exists(typePath))
  {
    if (boost::filesystem::is_directory(typePath))
    {
      NameVector names;

      // Populate the vector with pathnames.
      for (boost::filesystem::directory_iterator iter =
             boost::filesystem::directory_iterator(typePath);
           iter != boost::filesystem::directory_iterator(); ++iter)
      {
        if ((is_regular_file(iter->path())) && (iter->path().filename().extension()
                                                == ".xml"))
        {
          names.push_back(iter->path().filename().stem().generic_string());
          if (Settings::debugShowVerboseInfo)
          {
            std::cout << "Parsing prop type " << iter->path().filename().stem()
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
        for (NameIterator iter
      { names.begin() }; iter != names.end(); ++iter)
        {
          std::string tag = *iter;
          collection.insert(tag, new PropPrototype(tag));
        }
      }
      else
      {
        FATAL_ERROR("\"data/props\" exists, but has no descriptor files in it");
      }
    }
    else
    {
      FATAL_ERROR("\"data/props\" exists but is not a directory");
    }
  }
  else
  {
    FATAL_ERROR("Prop type directory \"data/props\" does not exist");
  }
}

PropPrototype& PropPrototype::get(std::string name)
{
  if (collection.count(name) != 0)
  {
    return collection.at(name);
  }
  else
  {
    MINOR_ERROR("Unable to find prop prototype \"%s\", returning \"anomaly\"",
                name.c_str());
    return collection.at("anomaly");
  }
}

// *** Private Methods ********************************************************

PropPrototype::PropPrototype(std::string _name)
{
  bool is_opaque, is_visible;

  name = _name;

  // Attempt to load the property tree for this prop.
  try
  {
    read_xml("data/props/" + name + ".xml", properties);
  }
  catch (std::exception& e)
  {
    MAJOR_ERROR("Can't parse \"%s\" descriptor file: %s",
                _name.c_str(), e.what());
  }

  // Determine visibility.
  // TODO: replace display.opaque, display.visible with single "display.visibility" attribute.
  is_opaque = properties.get<bool>("display.opaque", true);
  is_visible = properties.get<bool>("display.visible", true);

  if (!is_visible)
  {
    visibility = Visibility::Invisible;
  }
  else if (!is_opaque)
  {
    visibility = Visibility::Translucent;
  }
  else
  {
    visibility = Visibility::Opaque;
  }

  // Get the color associated with this Prop.
  // (If not present the color will always defer to the Substance color.)
  std::string colorString;
  unsigned int _color;
  uint8_t _r, _g, _b, _a;
  colorString = properties.get(
                  "display.color", properties.get("display.color.main", "00000000"));
  _color = strtoul(colorString.c_str(), nullptr, 16);
  _r = (_color >> 24) & 0xFF;
  _g = (_color >> 16) & 0xFF;
  _b = (_color >> 8) & 0xFF;
  _a = _color & 0xFF;

  color = sf::Color(_r, _g, _b, _a);

  cached_xoffset = properties.get<int>("display.oblique.xoffset", 0);
  cached_yoffset = properties.get<int>("display.oblique.yoffset", 0);

  cached_colorize = properties.get<bool>("display.colorize", false);

  // Cache substance "must be" category information.
  try
  {
    std::string catName;
    bool value;

    for (boost::property_tree::ptree::value_type & v : properties.get_child(
           "physical.substance.mustbe"))
    {
      catName = v.first;
      value = properties.get<bool>("physical.substance.mustbe." + catName,
                                   false);
      materialMustBe[name] = value;
    }
  }
  catch (const boost::property_tree::ptree_bad_path& p)
  {
    // If the subtree didn't exist, we don't care.
  }

  // Cache substance "should be" category information.
  try
  {
    std::string catName;
    bool value;

    for (boost::property_tree::ptree::value_type & v : properties.get_child(
           "physical.substance.shouldbe"))
    {
      catName = v.first;
      value = properties.get<bool>("physical.substance.shouldbe." + catName,
                                   false);
      materialShouldBe[name] = value;
    }
  }
  catch (const boost::property_tree::ptree_bad_path& p)
  {
    // If the subtree didn't exist, we don't care.
  }
}

const sf::Font& PropPrototype::getUnicodeFont()
{
  static sf::Font _font;
  static bool loaded = false;

  if (!loaded)
  {
    std::cout << "Loading Unicode font..." << std::endl;
    // TODO: eliminate magic string
    if (!_font.loadFromFile("fonts/DejaVuSans.ttf"))
    {
      MAJOR_ERROR("Can't load the Unicode font");
    }

    loaded = true;
  }

  return _font;
}

void PropPrototype::drawTextOntoTexture(sf::Text& renderText,
                                        sf::RenderTexture& renderTexture,
                                        bool outline,
                                        bool invert)
{
  sf::FloatRect textBounds = renderText.getGlobalBounds();

  sf::Color textColor = (invert ? sf::Color::Transparent : sf::Color::White);
  sf::Color backColor = (invert ? sf::Color::White : sf::Color::Transparent);
  sf::Color outlineColor = sf::Color(0, 0, 0, 128);

  renderTexture.create(textBounds.width, textBounds.height);
  renderTexture.clear(backColor);

  if (outline)
  {
    // Draw an outline by copying silhouettes U/D/L/R.
    renderText.setColor(outlineColor);
    renderText.setPosition(-textBounds.left + 1, -textBounds.top);
    renderTexture.draw(renderText);
    renderText.setPosition(-textBounds.left, -textBounds.top + 1);
    renderTexture.draw(renderText);
    renderText.setPosition(-textBounds.left - 1, -textBounds.top);
    renderTexture.draw(renderText);
    renderText.setPosition(-textBounds.left, -textBounds.top - 1);
    renderTexture.draw(renderText);
  }

  renderText.setColor(textColor);
  renderText.setPosition(-textBounds.left, -textBounds.top);
  renderTexture.draw(renderText);

  renderTexture.display();
}

} // namespace rectopia
} // namespace gsl
