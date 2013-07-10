// *** ADDED BY HEADER FIXUP ***
#include <iostream>
#include <string>
// *** END ***
#include "StageRenderer.h"

#include "Settings.h"
#include "Stage.h"

namespace gsl
{
namespace rectopia
{

StageRenderer::StageRenderer()
{
  name_ = "Null Renderer";
}

StageRenderer::~StageRenderer()
{
  //dtor
}

std::string StageRenderer::getName()
{
  return name_;
}

} // namespace rectopia
} // namespace gsl
