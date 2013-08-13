// *** ADDED BY HEADER FIXUP ***
#include <string>
// *** END ***
#ifndef CHUNKRENDERER_H
#define CHUNKRENDERER_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "EventListener.h"
#include "StageComponentVisitor.h"

// Forward declarations
class FaceBools;
class Stage;
class Prop;
class StageBlock;
class StageNode;

/// Base class implementing a renderer for the GameStage.
class StageRenderer: public EventListener,
  public StageComponentVisitor,
  public boost::noncopyable
{
public:
  StageRenderer();
  virtual ~StageRenderer();

  virtual void draw() = 0;

  virtual std::string getName(void);

protected:
  std::string name_;

private:
};
#endif // CHUNKRENDERER_H
