// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
// *** END ***
#include "Verb.h"

#include "ErrorMacros.h"
#include "Settings.h"

namespace gsl
{
namespace rectopia
{

boost::ptr_map<std::string, Verb> Verb::dictionary;

void Verb::initialize()
{
  std::cout << "=== VERB DICTIONARY ====================================="
            << std::endl;

  boost::property_tree::ptree properties;

  // Try to open the verb descriptor file and load its property tree.
  try
  {
    read_xml("data/verbs.xml", properties);
  }
  catch (std::exception& e)
  {
    FATAL_ERROR("Can't parse data/verbs.xml: %s", e.what());
  }

  // Try to read each verb underneath the tree.
  try
  {
    for (boost::property_tree::ptree::value_type & v : properties.get_child(""))
    {
      //std::cout << "Found verb \"" + v.first + "\"" << std::endl;
      std::string tag = v.first;
      dictionary.insert(tag, new Verb(tag, v.second));
    }
  }
  catch (const boost::property_tree::ptree_bad_path& p)
  {
    // If the subtree didn't exist, we should probably throw a warning.
    MAJOR_ERROR("data/verbs.xml appears to be empty");
  }

  // TODO: write me
}

Verb& Verb::get(std::string name)
{
  if (dictionary.count(name) != 0)
  {
    return dictionary.at(name);
  }
  else
  {
    MINOR_ERROR("Unable to find verb \"%s\", returning \"idle\"", name.c_str());
    return dictionary.at("idle");
  }
}

Verb::Verb(std::string _name, const boost::property_tree::ptree& propertyTree)
{
  // Copy the name and property tree to this verb.
  name = _name;
  properties = propertyTree;

  // TODO MAYBE: any other data aggregation required?
}

} // namespace rectopia
} // namespace gsl
