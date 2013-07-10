#ifndef HASINVENTORY_H_
#define HASINVENTORY_H_

#include "StageComponent.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class Inventory;

class HasInventory: virtual public StageComponent
{
public:

  /// Return this object's inventory.
  virtual Inventory& inventory() = 0;
};

} // namespace rectopia
} // namespace gsl

#endif /* HASINVENTORY_H_ */