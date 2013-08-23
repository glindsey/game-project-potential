#ifndef FONTCOLLECTION_H
#define FONTCOLLECTION_H

#include <boost/shared_ptr.hpp>
#include <memory>
#include <string>

// Forward declarations
class TextureFont;

class FontCollection
{
  public:
    FontCollection();

    FontCollection(FontCollection const&) = delete;
    FontCollection& operator=(FontCollection const&) = delete;

    virtual ~FontCollection();

    // Add the requested font to the collection.
    bool add(std::string fontName, boost::shared_ptr<TextureFont> textureFont);

    // Return whether the requested font exists.
    bool exists(std::string fontName);

    // Get the requested font.  If the font doesn't exist, return the fallback
    // font.
    TextureFont& get(std::string fontName);

    // Get the default font.  Use for speed when you just want the default.
    TextureFont& get_default();

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
#endif // FONTCOLLECTION_H
