#ifndef BGRENDERER3D_H_
#define BGRENDERER3D_H_

#include <memory>

#include "common.h"

#include "BGRenderer.h"

class BGRenderer3D: public BGRenderer
{
public:
  BGRenderer3D();
  virtual ~BGRenderer3D();

  void draw();

protected:
private:
  struct Impl;

  /// Private implementation pointer
  std::unique_ptr<Impl> impl;
};

#endif /* GUIRENDERER3D_H_ */
