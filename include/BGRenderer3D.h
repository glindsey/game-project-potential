#ifndef BGRENDERER3D_H_
#define BGRENDERER3D_H_

#include <memory>

#include "common.h"

#include "BGRenderer.h"

namespace gsl
{
namespace rectopia
{

class BGRenderer3D: public BGRenderer
{
public:
  BGRenderer3D();
  virtual ~BGRenderer3D();

  void prepare();
  void draw();
  void finish();

protected:
private:
  struct Impl;

  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};

} // namespace rectopia
} // namespace gsl

#endif /* GUIRENDERER3D_H_ */
