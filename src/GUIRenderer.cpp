// *** ADDED BY HEADER FIXUP ***
#include <iostream>
#include <string>
// *** END ***
#include "GUIRenderer.h"

#include "Settings.h"

GUIRenderer::GUIRenderer()
{
  name_ = "Null Renderer";
}

GUIRenderer::~GUIRenderer()
{
  //dtor
}

std::string GUIRenderer::getName()
{
  return name_;
}
