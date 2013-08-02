// *** ADDED BY HEADER FIXUP ***
#include <string>
// *** END ***
#ifndef STATUSAREA_H
#define STATUSAREA_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "GUIParentElement.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class Stage;
class GUIFrame;

class StatusArea: public GUIParentElement
{
public:
  StatusArea(GUIElement* parent);
  ~StatusArea();

  virtual void render();

  virtual std::string const getElementType()
  {
    return "StatusArea";
  }

protected:
private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif // STATUSAREA_H
