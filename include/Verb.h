// *** ADDED BY HEADER FIXUP ***
#include <string>
// *** END ***
#ifndef VERB_H
#define VERB_H

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

namespace gsl
{
namespace rectopia
{

/// Description of an action that can be performed within the game engine.
class Verb
{
public:
  /// Property tree containing all properties of this verb.
  boost::property_tree::ptree properties;

protected:
private:
  /// Constructor; private because only initialize() can create verbs.
  Verb(std::string _name, const boost::property_tree::ptree& propertyTree);

  /// Verb's name.
  std::string name;

  // === Static members =====================================================
public:
  static void initialize();
  static Verb& get(std::string name);

private:
  /// Static verb dictionary.
  static boost::ptr_map<std::string, Verb> dictionary;
};

} // namespace rectopia
} // namespace gsl

#endif // VERB_H
