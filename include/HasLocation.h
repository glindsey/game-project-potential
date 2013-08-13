#ifndef HASLOCATION_H_
#define HASLOCATION_H_

#include "StageComponent.h"

// Forward declarations
class Inventory;

class HasLocation
{
public:
  HasLocation()
  {
  }
  virtual ~HasLocation()
  {
  }

  /// Return this object's location.
  virtual Inventory& get_location() const = 0;

  /// Try to move this object to a new location.
  virtual bool move_to(Inventory& new_location) = 0;
};
#endif /* HASINVENTORY_H_ */
