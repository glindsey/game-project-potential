#ifndef HASINVENTORY_H_
#define HASINVENTORY_H_

#include "StageComponent.h"

// Forward declarations
class Inventory;

class HasInventory
{
public:
  /// Return this object's inventory.
  virtual Inventory& get_inventory() = 0;
};
#endif /* HASINVENTORY_H_ */
