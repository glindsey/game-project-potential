#ifndef GUIRENDERER_H
#define GUIRENDERER_H

#include "common.h"

#include <string>

#include "EventListener.h"
#include "GUIElementVisitor.h"

/// Base class implementing a renderer for the GUI.
class GUIRenderer:
  public EventListener,
  public GUIElementVisitor
{
public:
  GUIRenderer();

  GUIRenderer(GUIRenderer const&) = delete;
  GUIRenderer& operator=(GUIRenderer const&) = delete;

  virtual ~GUIRenderer();

  virtual void draw() = 0;

  virtual std::string getName(void);

protected:
  std::string name_;

private:
};
#endif // GUIRENDERER_H
