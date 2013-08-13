#include "ErrorMacros.h"
#include "SimpleMatrixFont.h"
#include "TextureFont.h"

#include <boost/container/map.hpp>
#include <boost/shared_ptr.hpp>

#include "FontCollection.h"

struct FontCollection::Impl
{
  boost::container::map<std::string, boost::shared_ptr<TextureFont> > fontCollection;
};

FontCollection::FontCollection()
  : impl(new Impl())
{
  boost::shared_ptr<TextureFont> defaultFont;

  defaultFont.reset(new SimpleMatrixFont());

  bool success = defaultFont->load("Md_curses_16x16.png");
  if (!success)
  {
    FATAL_ERROR("Could not load default font!");
  }

  add("default", defaultFont);
  //ctor
}

FontCollection::~FontCollection()
{
  //dtor
}

bool FontCollection::add(std::string fontName, boost::shared_ptr<TextureFont> textureFont)
{
  if (impl->fontCollection.count(fontName) == 0)
  {
    impl->fontCollection[fontName] = textureFont;
    return true;
  }
  return false;
}

bool FontCollection::exists(std::string fontName)
{
  return (impl->fontCollection.count(fontName) != 0);
}

TextureFont& FontCollection::get(std::string fontName)
{
  if (impl->fontCollection.count(fontName) != 0)
  {
    return *(impl->fontCollection[fontName].get());
  }
  else
  {
    return *(impl->fontCollection["default"].get());
  }
}

TextureFont& FontCollection::get_default()
{
  return *(impl->fontCollection["default"].get());
}
