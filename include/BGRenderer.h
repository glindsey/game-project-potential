#ifndef BGRENDERER_H
#define BGRENDERER_H

#include "common.h"

#include <boost/noncopyable.hpp>
#include <string>

/// Base class implementing a renderer for the window background.
class BGRenderer:
  public boost::noncopyable
{
public:
  BGRenderer();
  virtual ~BGRenderer();

  virtual void draw() = 0;

  virtual std::string getName(void);

protected:
  std::string name_;

private:
};

#endif // BGRENDERER_H
