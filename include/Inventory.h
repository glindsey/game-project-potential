// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <set>
// *** END ***
#ifndef INVENTORY_H_
#define INVENTORY_H_

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class HasLocation;

/// Declaration of an inventory of objects derived from HasLocation.
class Inventory
{
public:
  Inventory();                                    ///< Constructor
  virtual ~Inventory();                           ///< Destructor

  /// Attempt to add a HasLocation object to this container.
  /// Called by HasLocation::moveTo().  Does not change the added object's
  /// location -- this is up to the caller.
  bool add(HasLocation* _prop);

  /// Attempt to remove a HasLocation object from this container.
  /// Called by HasLocation::moveTo().  Does not change the removed object's
  /// location -- this is up to the caller.
  bool remove(HasLocation* _prop);

  /// Returns the number of items in an inventory.
  int Quantity();

  /// Returns true if an inventory is empty, false otherwise.
  bool IsEmpty();

  /// Return a const reference to the contents for enumeration.
  boost::container::set<HasLocation*> const& getContents(void);

  /// Get the "limbo" inventory.
  static Inventory& getLimbo();

private:
  struct Impl;
  /// Private implementation pointer
  std::unique_ptr<Impl> impl;

};

} // namespace rectopia
} // namespace gsl

#endif /* INVENTORY_H_ */
