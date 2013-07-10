// *** ADDED BY HEADER FIXUP ***
#include <string>
// *** END ***
#ifndef PROP_H
#define PROP_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "HasInventory.h"
#include "HasLocation.h"
#include "StageComponent.h"
#include "StageComponentVisitor.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class PropPrototype;
class Substance;

/// A movable object in the system.
class Prop:
  virtual public HasInventory,
  virtual public HasLocation,
  virtual public StageComponent
{
public:
  virtual ~Prop();                    ///< Destructor

  void accept(StageComponentVisitor& visitor);

  Inventory& inventory();

  /// Return this object's location.
  Inventory& location() const;

  /// Try to move this object to a new location.
  bool MoveTo(Inventory& new_location);

  const PropPrototype& prototype() const;

  void SetPrototype(std::string typeName);
  void SetPrototype(const PropPrototype& type);

  Visibility getVisibility(void) const;
  bool isSolid(void);

  const Substance& substance() const;

protected:
  Prop();                             ///< Constructor
  Prop(std::string typeName);      ///< Constructor
  Prop(Prop& _other);                 ///< Constructor

private:
  struct Impl;
  /// Private Implementation Pointer
  boost::scoped_ptr<Impl> impl;

// === Static Members =========================================================
public:
  static SerialNumber create(std::string name);
  static SerialNumber clone(SerialNumber other);
  static bool destroy(SerialNumber prop);
  static bool exists(SerialNumber prop);
  static Prop& get(SerialNumber prop);

protected:

private:
};

} // namespace rectopia
} // namespace gsl

#endif // PROP_H
