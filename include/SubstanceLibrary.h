#ifndef SUBSTANCELIBRARY_H
#define SUBSTANCELIBRARY_H

#include <memory>
#include <string>
#include <boost/property_tree/ptree.hpp>

#include "common.h"

#include "Substance.h"

/// Class representing the library of all possible substances in the game.
class SubstanceLibrary
{
  public:
    SubstanceLibrary(SubstanceLibrary const&) = delete;
    SubstanceLibrary& operator=(SubstanceLibrary const&) = delete;
    virtual ~SubstanceLibrary();

    /// Get a pointer to the singleton library instance.
    /// If no instance yet exists, create it.
    static std::shared_ptr<SubstanceLibrary> get_instance();

    /// Initialize the substance library.
    void initialize();

    /// Get a pointer to the requested substance.
    /// If the substance does not yet exist, attempt to load it from disk.
    /// If this attempt fails, return "nothing".
    SubstanceConstShPtr get(std::string name);

    /// Get the count of possible substances for a layer.
    unsigned int get_layer_substance_count(std::string name);

    /// Get a random substance choice for the layer.
    std::string get_layer_random_substance(std::string name);

    /// Return whether a substance is a part of a category.
    bool in_category(std::string name, std::string category);

  protected:

  private:
    SubstanceLibrary();    ///< Constructor (private)

    struct Impl;
    /// Private implementation
    std::unique_ptr<Impl> impl;
};

// Using declarations
using SubstanceLibraryShPtr = std::shared_ptr<SubstanceLibrary>;

#define SL SubstanceLibrary::get_instance()

#endif // SUBSTANCELIBRARY_H
