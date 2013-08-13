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

// Forward declarations
class PropPrototype;
class Substance;

/// A movable object in the system.
class Prop:
  public StageComponent,
  public HasInventory,
  public HasLocation
{
public:
  virtual ~Prop();                    ///< Destructor

  void accept(StageComponentVisitor& visitor);

  Inventory& get_inventory();

  /// Return this object's location.
  Inventory& get_location() const;

  /// Try to move this object to a new location.
  bool move_to(Inventory& new_location);

  const PropPrototype& get_prototype() const;

  void set_prototype(std::string typeName);
  void set_prototype(const PropPrototype& type);

  Visibility get_visibility(void) const;
  bool is_solid(void);

  const Substance& get_substance() const;

protected:
  Prop();                             ///< Constructor
  Prop(std::string typeName);      ///< Constructor
  Prop(Prop& _other);                 ///< Constructor

private:
  struct Impl;
  /// Private Implementation Pointer
  std::unique_ptr<Impl> impl;

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
#endif // PROP_H
