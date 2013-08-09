#ifndef GUIRENDERER_H
#define GUIRENDERER_H

#include "common.h"

#include <boost/noncopyable.hpp>
#include <string>

#include "EventListener.h"
#include "GUIElementVisitor.h"

namespace gsl
{
namespace rectopia
{

/// Base class implementing a renderer for the GUI.
class GUIRenderer:
  public EventListener,
  public GUIElementVisitor,
  public boost::noncopyable
{
public:
  GUIRenderer();
  virtual ~GUIRenderer();

  virtual void draw() = 0;

  virtual std::string getName(void);

protected:
  std::string name_;

private:
};

} // namespace rectopia
} // namespace gsl

#endif // GUIRENDERER_H
