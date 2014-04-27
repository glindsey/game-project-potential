#include "SubstanceLibrary.h"

#include <unordered_map>
#include <set>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "ErrorMacros.h"
#include "MathUtils.h"

// Using declarations
using SubstanceCollection = std::unordered_map<std::string, SubstanceShPtr>;
using Path = boost::filesystem::path;
using StringMapVector = std::unordered_map<std::string, std::vector<std::string> >;
using StringMapSet = std::unordered_map<std::string, std::set<std::string> >;
using StringVector = std::vector<std::string>;
using StringIterator = StringVector::const_iterator;

struct SubstanceLibrary::Impl
{
  /// Given a particular substance, populate category info for the substance.
  void populate_categories(SubstanceShPtr substance);

  /// Given a particular substance, populate layer info for the substance.
  void populate_layers(SubstanceShPtr substance);

  /// Given a particular substance, populate verb info for the substance.
  void populate_verbs(SubstanceShPtr substance);

  /// Check substances overall for consistency.
  void check_substances(void);

  /// Collection of known substances.
  SubstanceCollection collection;

  /// Collection of categories and substances classified into them.
  StringMapSet categories;

  /// Collection of layer types and substances classified into them.
  StringMapVector layers;

  /// Collection of verbs and substances associated with them.
  StringMapSet verbs;

  /// Pointer to the library instance.
  static SubstanceLibraryShPtr instance_;
};

SubstanceLibraryShPtr SubstanceLibrary::Impl::instance_;

void SubstanceLibrary::Impl::populate_categories(SubstanceShPtr substance)
{
  // Use attributes to populate substance category information.
  try
  {
    Properties props = substance->get_properties();
    std::string category_name;
    std::string substance_name = substance->get_data().name;

    for (auto v : props.get_child("attributes"))
    {
      category_name = v.first;
      categories[category_name].insert(substance_name);
    }
  }
  catch (const boost::property_tree::ptree_bad_path& p)
  {
    // If the subtree didn't exist, we don't care.
  }
}

void SubstanceLibrary::Impl::populate_layers(SubstanceShPtr substance)
{
  // Check terrain inclusion layers.
  Properties props = substance->get_properties();

  std::string layer_name = substance->get_string_property("terrain.layer");
  std::string substance_name = substance->get_data().name;
  if (layer_name != "")
  {
    if (layer_name == "all")
    {
      layers["igneous-extrusive"].push_back(substance_name);
      layers["igneous-intrusive"].push_back(substance_name);
      layers["sedimentary"].push_back(substance_name);
      layers["metamorphic"].push_back(substance_name);
    }
    else
    {
       layers[layer_name].push_back(substance->get_data().name);
    }
  }
}

void SubstanceLibrary::Impl::populate_verbs(SubstanceShPtr substance)
{
  // Store verb results.
  // TODO: Probably better that this info is stored in a separate VerbDictionary or something.
  try
  {
    Properties props = substance->get_properties();
    std::string verb_name;
    std::string substance_name = substance->get_data().name;

    for (auto v : props.get_child("actions"))
    {
      verb_name = v.first;
      verbs[verb_name].insert(substance_name);
    }
  }
  catch (const boost::property_tree::ptree_bad_path& p)
  {
    // If the subtree didn't exist, we don't care.
  }
}

void SubstanceLibrary::Impl::check_substances(void)
{
  std::cout << "*** Parsing substance descriptors for consistency..."
            << std::endl;
  int matCount = 0;

  for (auto substance_iterator = collection.begin();
       substance_iterator != collection.end();
       ++substance_iterator)
  {
    ++matCount;

    std::string substance_name = substance_iterator->first;
    SubstanceShPtr substance = substance_iterator->second;

    substance->check_consistency();

    // Automatic class setting for certain implied classes.
    // TODO: This should be made configurable in XML.
    if (categories["grass"].count(substance_name) != 0)
    {
      categories["plant"].insert(substance_name);
    }
    if (categories["metal"].count(substance_name) != 0)
    {
      categories["conductor"].insert(substance_name);
    }
    if (categories["sand"].count(substance_name) != 0)
    {
      categories["granular"].insert(substance_name);
    }
    if (categories["soil"].count(substance_name) != 0)
    {
      categories["granular"].insert(substance_name);
    }
    if (categories["wood"].count(substance_name) != 0)
    {
      categories["plant"].insert(substance_name);
      categories["fuel"].insert(substance_name);
    }

    // Consistency checks for substance classes: mutually exclusive classes
    if ((categories["flat"].count(substance_name) != 0) &&
        (categories["granular"].count(substance_name.c_str()) != 0))
    {
      MINOR_ERROR("%s is defined as both flat and granular; which are incompatible",
                  substance_name.c_str());
    }

    // Consistency checks for substance classes: solids.
    // TODO: This should be made configurable in XML.
    if (substance->get_data().phase != Phase::Solid)
    {
      if (categories["flux"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a flux, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["gem"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a gem, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["rock"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a rock, but is not a solid",
                    substance_name.c_str());
      }
      if ((categories["metal"].count(substance_name) != 0) &&
          (substance->get_data().phase != Phase::Liquid))
      {
        MINOR_ERROR("%s is defined as a metal, but is not a solid or liquid",
                    substance_name.c_str());
      }
      if (categories["sand"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a type of sand, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["soil"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a type of soil, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["wood"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a type of wood, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["plant"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as a type of plant matter, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["granular"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as granular, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["flat"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is defined as flat, but is not a solid",
                    substance_name.c_str());
      }
      if (substance->get_data().hardness != 0)
      {
        MINOR_ERROR("%s has a hardness value defined, but is not a solid",
                    substance_name.c_str());
      }
      if (categories["polished"].count(substance_name) != 0)
      {
        MINOR_ERROR("%s is marked as polishable, but is not a solid",
                    substance_name.c_str());
      }
    }
    else // if the substance IS a solid...
    {

    }

    std::string ore_name = substance->get_properties().get("attributes.ore", "");
    if ((ore_name != "") && (collection.count(ore_name) == 0))
    {
      MINOR_ERROR("%s is defined as an ore of missing substance %s",
                  substance_name.c_str(), ore_name.c_str());
    }
    ore_name = substance->get_properties().get("attributes.secondaryore", "");
    if ((ore_name != "") && (collection.count(ore_name) == 0))
    {
      MINOR_ERROR("%s is defined as a secondary ore of missing substance %s",
                  substance_name.c_str(), ore_name.c_str());
    }

    // Parse "large deposit" designations.
    try
    {
      for (auto v :
           substance->get_properties().get_child("terrain.largedeposit"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing largedeposit substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection[deposit_name]->large_deposits.push_back(substance_name);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Parse "small deposit" designations.
    try
    {
      for (auto v :
           substance->get_properties().get_child("terrain.smalldeposit"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing smalldeposit substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection[deposit_name]->small_deposits.push_back(substance_name);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Parse "vein" designations.
    try
    {
      for (auto v :
           substance->get_properties().get_child("terrain.vein"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing vein substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection[deposit_name]->vein_deposits.push_back(substance_name);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Parse "gangue" designations.
    try
    {
      for (auto v :
           substance->get_properties().get_child("terrain.gangue"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing gangue substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection[deposit_name]->gangue_deposits.push_back(substance_name);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }

    // Parse "single" designations.
    try
    {
      for (auto v :
           substance->get_properties().get_child("terrain.single"))
      {
        std::string deposit_name = v.first;
        if (collection.count(deposit_name) == 0)
        {
          MINOR_ERROR("%s has the missing single substance \"%s\"",
                      substance_name.c_str(), deposit_name.c_str());
        }
        else
        {
          collection[deposit_name]->single_deposits.push_back(substance_name);
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path& p)
    {
      // If the subtree didn't exist, we don't care.
    }
  }
  std::cout << "*** Done parsing descriptors:  " << std::endl;
  std::cout << "***   " << collection.size() << " substances parsed"
            << std::endl;
  std::cout << "***   " << categories.size()
            << " distinct substance attributes counted" << std::endl;
}

SubstanceLibrary::SubstanceLibrary() :
      impl(new Impl())
{
  //ctor
}

SubstanceLibrary::~SubstanceLibrary()
{
  //dtor
}

std::shared_ptr<SubstanceLibrary> SubstanceLibrary::get_instance()
{
  // Instantiate and initialize library if it hasn't been yet.
  if (Impl::instance_.get() == nullptr)
  {
    Impl::instance_.reset(new SubstanceLibrary());
  }

  return Impl::instance_;
}

void SubstanceLibrary::initialize()
{
  std::cout << "Scanning substances directory for descriptor files..."
            << std::endl;

  // Scan through the "./data/substances" directory to find all material descriptors.
  Path material_path = "data/substances";

  if (boost::filesystem::exists(material_path))
  {
    if (boost::filesystem::is_directory(material_path))
    {
      StringVector names;

      // Populate the vector with pathnames.
      for (boost::filesystem::directory_iterator iter =
             boost::filesystem::directory_iterator(material_path);
           iter != boost::filesystem::directory_iterator(); ++iter)
      {
        if ((boost::filesystem::is_regular_file(iter->path())) &&
            (iter->path().filename().extension() == ".xml"))
        {
          names.push_back(iter->path().filename().stem().generic_string());
        }
        else
        {
          if (Settings::debugShowVerboseInfo)
          {
            std::cout << iter->path().filename()
                      << " is not a descriptor file, skipping." << std::endl;
          }
        }
      }

      if (names.size() != 0)
      {
        // Attempt to load data for each file seen.
        for (StringIterator iter = names.begin(); iter != names.end(); ++iter)
        {
          std::string tag = *iter;
          if (Settings::debugShowVerboseInfo)
          {
            std::cout << "Parsing substance " << tag << "." << std::endl;
          }
          impl->collection[tag].reset(new Substance());
          impl->collection[tag]->load(tag);
          impl->populate_layers(impl->collection[tag]);
          impl->populate_categories(impl->collection[tag]);
          impl->populate_verbs(impl->collection[tag]);
        }
      }
      else
      {
        FATAL_ERROR("\"data/substances\" exists but has no descriptor files in it");
      }
    }
    else
    {
      FATAL_ERROR("\"data/substances\" exists but is not a directory");
    }
  }
  else
  {
    FATAL_ERROR("Substances directory \"data/substances\" does not exist");
  }

  // If "nothing" and/or "air" do not exist, create them.  These two Materials
  // must exist for world generation to work properly.
  if (impl->collection.count("nothing") == 0)
  {
    impl->collection["nothing"].reset(new Substance());
    impl->collection["nothing"]->load("nothing");
    impl->populate_layers(impl->collection["nothing"]);
    impl->populate_categories(impl->collection["nothing"]);
    impl->populate_verbs(impl->collection["nothing"]);
  }

  if (impl->collection.count("air") == 0)
  {
    impl->collection["air"].reset(new Substance());
    impl->collection["air"]->load("air");
    impl->populate_layers(impl->collection["air"]);
    impl->populate_categories(impl->collection["air"]);
    impl->populate_verbs(impl->collection["air"]);
  }

  impl->check_substances();
}

SubstanceConstShPtr SubstanceLibrary::get(std::string name)
{
  if (impl->collection.count(name) != 0)
  {
    return impl->collection.at(name);
  }
  else
  {
    if (Settings::debugShowVerboseInfo)
    {
      std::cout << "Unable to find substance \"" << name
                << "\", returning nothing" << std::endl;
    }
    return impl->collection.at("nothing");
  }
}

unsigned int SubstanceLibrary::get_layer_substance_count(std::string name)
{
  return (impl->layers[name]).size();
}

std::string SubstanceLibrary::get_layer_random_substance(std::string name)
{
  unsigned int rnum = randint((impl->layers[name]).size());
  return (impl->layers[name])[rnum];
}

bool SubstanceLibrary::in_category(std::string name, std::string category)
{
  if (impl->collection.count(name) != 0)
  {
    return (impl->categories[category].count(name) != 0);
  }
  else
  {
    return false;
  }
}
