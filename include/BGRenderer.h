#ifndef BGRENDERER_H
#define BGRENDERER_H

#include "common.h"

#include <string>

/// Base class implementing a renderer for the window background.
class BGRenderer
{
public:
  BGRenderer();

  BGRenderer(BGRenderer const&) = delete;
  BGRenderer& operator=(BGRenderer const&) = delete;

  virtual ~BGRenderer();

  virtual void draw() = 0;

  virtual std::string getName(void);

protected:
  std::string name_;

private:
};

#endif // BGRENDERER_H
