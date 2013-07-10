// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <iostream>
#include <string>
// *** END ***
#include "Prop.h"

#include "ErrorMacros.h"
#include "Inventory.h"
#include "PropPrototype.h"
#include "Settings.h"
#include "Substance.h"

namespace gsl
{
namespace rectopia
{

// === Private Implementation =================================================
struct Prop::Impl
{
  /// Prototype of this prop.
  const PropPrototype* prototype_;

  /// Material this prop is made out of.
  /// @todo Actually use this -- right now it is ignored.
  const Substance* substance_;

  struct CachedData
  {
    /// Boolean indicating wither the prop is solid.
    bool solid;

    /// Prop's offset within its location, for drawing purposes.
    sf::Vector3f offset;
  };

  /// Prop data that is cached from the Prototype instance.
  /// Members within can be changed independently from the Prototype.
  CachedData cached_;

  /// Prop's inventory.
  Inventory inventory_;

  /// This Prop's serial number.
  SerialNumber serialNumber;

  /// Location of this Prop.  Must not be null.
  Inventory* location_;

  /// Collection of all Props in the game.
  static boost::ptr_map<SerialNumber, Prop> collection;

  /// Next serial number to be allocated to a Prop.
  static SerialNumber nextSerialNumber;
};

boost::ptr_map<SerialNumber, Prop> Prop::Impl::collection;

SerialNumber Prop::Impl::nextSerialNumber = 1;

Prop::Prop()
  : StageComponent(), impl(new Impl())
{
  SetPrototype("anomaly");
  MoveTo(Inventory::getLimbo());
}

Prop::Prop(std::string typeName)
  : StageComponent(), impl(new Impl())
{
  SetPrototype(typeName);
  MoveTo(Inventory::getLimbo());
}

Prop::Prop(Prop& _other)
  : StageComponent(), impl(new Impl())
{
  SetPrototype(_other.prototype());
  MoveTo(Inventory::getLimbo());
}

Prop::~Prop()
{

}

void Prop::accept(StageComponentVisitor& visitor)
{
  bool visitChildren = visitor.visit(*this);

  if (visitChildren)
  {
    // Visit this block's contents.
    for (StageComponent * object : impl->inventory_.getContents())
    {
      object->accept(visitor);
    }
  }
}

Inventory& Prop::inventory()
{
  return impl->inventory_;
}

void Prop::SetPrototype(std::string typeName)
{
  impl->prototype_ = &(PropPrototype::get(typeName));
  impl->cached_.solid = impl->prototype_->properties.get<bool>("physical.solid",
                        false);
}

void Prop::SetPrototype(const PropPrototype& type)
{
  impl->prototype_ = &type;
  impl->cached_.solid = impl->prototype_->properties.get<bool>("physical.solid",
                        false);
}

Visibility Prop::getVisibility(void) const
{
  return impl->prototype_->getVisibility();
}

bool Prop::isSolid(void)
{
  return impl->cached_.solid;
}

const PropPrototype& Prop::prototype() const
{
  return *(impl->prototype_);
}

const Substance& Prop::substance() const
{
  return *(impl->substance_);
}

Inventory& Prop::location() const
{
  return *(impl->location_);
}

bool Prop::MoveTo(Inventory& new_location)
{
  // If current location == new location, just exit!
  if (impl->location_ == &new_location)
  {
    return true;
  }

  // Make sure current location is not NULL; this happens with newly-
  // created objects.
  if (impl->location_ != nullptr)
  {
    // Try removing it from its current location.
    if (impl->location_->remove(this) == false)
    {
      MINOR_ERROR("Could not remove Prop #%u from its current location",
                  impl->serialNumber);
      return false;
    }
  }

  // Try adding to the new location.
  if (new_location.add(this) == false)
  {
    // Couldn't add to new location, so re-add to old one.
    MINOR_ERROR("Could not add Prop #%u to its new location",
                impl->serialNumber);

    if (impl->location_ != nullptr)
    {
      impl->location_->add(this);
    }
    else
    {
      FATAL_ERROR("Prop #%u's old location was NULL, which is forbidden!",
                  impl->serialNumber);
    }

    return false;
  }

  impl->location_ = &new_location;
  return true;
}

SerialNumber Prop::create(std::string name)
{
  Prop* newProp = new Prop(name);
  SerialNumber propNumber = Impl::nextSerialNumber;
  newProp->impl->serialNumber = propNumber;
  Impl::collection.insert(propNumber, newProp);
  ++(Impl::nextSerialNumber);

  // Return the new prop's number.
  return propNumber;
}

SerialNumber Prop::clone(SerialNumber number)
{
  Prop* newProp = new Prop(Prop::get(number));
  SerialNumber propNumber = Impl::nextSerialNumber;
  newProp->impl->serialNumber = propNumber;
  Impl::collection.insert(propNumber, newProp);
  ++(Impl::nextSerialNumber);

  // Return the new prop's number.
  return propNumber;
}

bool Prop::destroy(SerialNumber number)
{
  if (exists(number))
  {
    // Try to remove the prop from whatever inventory is might be in.
    Prop::get(number).MoveTo(Inventory::getLimbo());

    // Erase the prop from existence.
    Impl::collection.erase(number);
    return true;
  }
  else
  {
    MAJOR_ERROR("Attempt to destroy non-existent Prop #%u", number);
  }

  // If we get here, Prop couldn't be destroyed for some reason.
  return false;
}

bool Prop::exists(SerialNumber number)
{
  return (Impl::collection.count(number) != 0);
}

Prop& Prop::get(SerialNumber number)
{
  if (exists(number))
  {
    return Impl::collection.at(number);
  }
  else
  {
    return get(create("anomaly"));
  }
}

} // namespace rectopia
} // namespace gsl
