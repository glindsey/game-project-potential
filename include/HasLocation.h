#ifndef HASLOCATION_H_
#define HASLOCATION_H_

#include "StageComponent.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class Inventory;

class HasLocation: virtual public StageComponent
{
public:
  HasLocation()
  {
  }
  virtual ~HasLocation()
  {
  }

  /// Return this object's location.
  virtual Inventory& location() const = 0;

  /// Try to move this object to a new location.
  virtual bool MoveTo(Inventory& new_location) = 0;
};

} // namespace rectopia
} // namespace gsl

#endif /* HASINVENTORY_H_ */
