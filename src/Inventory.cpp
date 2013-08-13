// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <set>
// *** END ***
#include "Inventory.h"

#include "Prop.h"
#include "Settings.h"

struct Inventory::Impl
{
  /// Set of HasLocation objects under this one.
  boost::container::set<HasLocation*> contents;

  /// A static "Limbo" PropContainer.
  static Inventory limbo;
};

Inventory Inventory::Impl::limbo;

Inventory::Inventory()
  : impl(new Impl())
{
}

Inventory::~Inventory()
{
}

bool Inventory::add(HasLocation* _object)
{
  if (_object != nullptr)
  {
    impl->contents.insert(_object);
    return true;
  }

  return false;
}

bool Inventory::remove(HasLocation* _object)
{
  if ((_object != nullptr) && (impl->contents.count(_object) != 0))
  {
    impl->contents.erase(_object);
    return true;
  }
  return false;
}

int Inventory::Quantity()
{
  return (impl->contents.size());
}

bool Inventory::IsEmpty()
{
  return (impl->contents.size() == 0);
}

boost::container::set<HasLocation*> const& Inventory::getContents(void)
{
  return impl->contents;
}

Inventory& Inventory::getLimbo()
{
  return Impl::limbo;
}
