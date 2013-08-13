// *** ADDED BY HEADER FIXUP ***
#include <iostream>
#include <string>
// *** END ***
#include "BGRenderer.h"

#include "Settings.h"

BGRenderer::BGRenderer()
{
  name_ = "Null Renderer";
}

BGRenderer::~BGRenderer()
{
  //dtor
}

std::string BGRenderer::getName()
{
  return name_;
}
