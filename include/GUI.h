#ifndef GUI_H
#define GUI_H

#include "common.h"

#include <memory>
#include <string>

#include "EventListener.h"
#include "GUIParentElement.h"

// Forward declarations
class GUIElementVisitor;

// The "trunk" node for all GUI elements.
class GUI: public GUIParentElement
{
public:
  GUI();
  virtual ~GUI();

  void accept(GUIElementVisitor& visitor) override;

  std::string const getElementType()
  {
    return "GUI";
  }

  virtual EventResult handle_window_resize(int w, int h);

protected:
private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};
#endif // GAMEGUI_H
