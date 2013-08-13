/*
 * GUIElementVisitor.h
 *
 *  Created on: Jan 16, 2013
 *      Author: Gregory Lindsey
 */

#ifndef GUIELEMENTVISITOR_H_
#define GUIELEMENTVISITOR_H_

#include "common.h"

// Forward declarations
class GUI;
class GUIElement;
class GUIParentElement;
class GUIFrame;
class GUILabel;

/// Superclass for any visitors to GUI elements.
class GUIElementVisitor
{
public:
  virtual bool visit(GUI& gui) = 0;
  virtual bool visit(GUIElement& element) = 0;
  virtual bool visit(GUIParentElement& element) = 0;
  virtual bool visit(GUIFrame& frame) = 0;
  virtual bool visit(GUILabel& label) = 0;

protected:
private:
};
#endif /* GUIELEMENTVISITOR_H_ */
