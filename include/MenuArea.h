// *** ADDED BY HEADER FIXUP ***
#include <string>
// *** END ***
#ifndef MENUAREA_H
#define MENUAREA_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "GUIParentElement.h"

class MenuArea: public GUIParentElement
{
public:
  MenuArea(GUIElement* parent);

  virtual void render();
  virtual const std::string getElementType()
  {
    return "MenuArea";
  }

protected:

private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;

  // TODO: make these variable, via Settings() or otherwise
  static const unsigned int menuWidth = 250;
  static const unsigned int menuMargin = 10;
  static const unsigned int statusBarHeight = 100;
};
#endif // MENUAREA_H
