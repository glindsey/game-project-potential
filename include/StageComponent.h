#ifndef STAGECOMPONENT_H_
#define STAGECOMPONENT_H_

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

class StageComponentVisitor;

/// Superclass for any stage components that may be rendered to the screen.
class StageComponent
{
public:

  /// Accept a visitor to this component.
  virtual void accept(StageComponentVisitor& visitor) = 0;

protected:
private:
};
#endif // STAGECOMPONENT_H_
