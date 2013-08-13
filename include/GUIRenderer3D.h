/*
 * GUIRenderer3D.h
 *
 *  Created on: Jan 16, 2013
 *      Author: Gregory Lindsey
 */

#ifndef GUIRENDERER3D_H_
#define GUIRENDERER3D_H_

#include <memory>

#include "common.h"

#include "GUIRenderer.h"

// Forward declarations
class GUI;
class GUIElement;
class GUIParentElement;
class GUIFrame;
class GUILabel;

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

  void draw();

protected:
private:
  struct Impl;

  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};
#endif /* GUIRENDERER3D_H_ */
