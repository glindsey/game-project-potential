/*
 * GUIRenderer3D.h
 *
 *  Created on: Jan 16, 2013
 *      Author: Gregory Lindsey
 */

#ifndef GUIRENDERER3D_H_
#define GUIRENDERER3D_H_

#include <boost/scoped_ptr.hpp>

#include "common.h"

#include "GUIRenderer.h"

// Forward declarations
class GUI;
class GUIElement;
class GUIParentElement;
class GUIFrame;
class GUILabel;

namespace gsl
{
namespace rectopia
{

class GUIRenderer3D: public GUIRenderer
{
public:
  GUIRenderer3D();
  virtual ~GUIRenderer3D();

  bool visit(GUI& gui);
  bool visit(GUIElement& element);
  bool visit(GUIParentElement& element);
  bool visit(GUIFrame& frame);
  bool visit(GUILabel& label);

  void prepare();
  void draw();
  void finish();

protected:
private:
  struct Impl;

  /// Private implementation pointer
  boost::scoped_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif /* GUIRENDERER3D_H_ */
