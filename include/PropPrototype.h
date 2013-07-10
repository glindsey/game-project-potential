// *** ADDED BY HEADER FIXUP ***
#include <string>
// *** END ***
#ifndef PROPPROTOTYPE_H
#define PROPPROTOTYPE_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

namespace gsl
{
namespace rectopia
{

typedef boost::unordered_map<std::string, bool> MaterialMustBeType;
typedef boost::unordered_map<std::string, bool> MaterialShouldBeType;

enum class PropMapping
{
  Billboard, Top, Sides, All, Count
};

/// Object representing a particular Prop type.  Calling it a "prototype" is a
/// bit of a misnomer, as Props do not clone PropPrototype instances, but rather
/// contain pointers to them.
class PropPrototype
{
public:
  /// Indicates whether this prop is visible.
  Visibility getVisibility() const;

  /// Property tree containing all properties of this prop type.
  boost::property_tree::ptree properties;

  /// Get the X-offset value used when rendering to screen.
  inline int getXOffset(void) const
  {
    return cached_xoffset;
  }

  /// Get the Y-offset value used when rendering to screen.
  inline int getYOffset(void) const
  {
    return cached_yoffset;
  }

  /// Get the color of this prop.
  inline const sf::Color& getColor(void) const
  {
    return color;
  }

  /// Get whether we're doing colorization.
  inline bool getColorize(void) const
  {
    return cached_colorize;
  }

  /// Get type name.
  inline std::string getName(void) const
  {
    return name;
  }

  /// Initialize the prototype collection.
  static void initialize();

  /// Get a particular type from the collection.
  static PropPrototype& get(std::string type);

protected:
private:
  // TODO: PIMPLize this when I'm feeling particularly masochistic.

  /// Construct a PropPrototype object from an XML data file.
  PropPrototype(std::string name);

  /// Static font instance.
  static sf::Font const& getUnicodeFont();

  /// Draw some text onto a texture, complete with black outlines.
  void drawTextOntoTexture(sf::Text& renderText,
                           sf::RenderTexture& renderTexture,
                           bool outline,
                           bool invert);

  /// Prop's name.
  std::string name;

  /// Visibility of this prop type.
  Visibility visibility;

  /// Prop color, if any.  If all zeroes, Material color is used instead.
  sf::Color color;

  int cached_xoffset;
  int cached_yoffset;
  bool cached_colorize;

  /// Permitted/forbidden material categories.
  /// Doesn't technically need to be saved here again, as it is already
  /// available as part of the ptree, but caching it here will probably
  /// be faster than doing ptree lookups every time we need the info.
  MaterialMustBeType materialMustBe;

  /// Recommended material categories.
  /// @see materialMustBe
  MaterialShouldBeType materialShouldBe;

  /// Static collection of all types.
  static boost::ptr_map<std::string, PropPrototype> collection;
};

} // namespace rectopia
} // namespace gsl

#endif // PROPPROTOTYPE_H
