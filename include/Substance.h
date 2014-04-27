#ifndef SUBSTANCE_H_
#define SUBSTANCE_H_

#include <memory>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <glm/glm.hpp>

#include "common.h"

#include "SubstanceData.h"

/// Forward declarations
class SubstanceLibrary;

/// Representation of a Substance that a StageChunk or a Prop can be made out of.
class Substance
{
  // Friends
  friend class SubstanceLibrary;

public:
  Substance(Substance const&) = delete;
  Substance& operator=(Substance const&) = delete;

  virtual ~Substance();                    ///< Destructor

  /// Check substance information for consistency.
  void check_consistency();

  /// Get a string property from the substance.  Returns "" if not present.
  std::string get_string_property(std::string property) const;

  /// Get an integer property from the substance.  Returns default_value if not
  /// present.  If default_value is not present, it defaults to zero.
  int get_int_property(std::string property, int default_value) const;

  /// Get a boolean property from the substance.  Returns default_value if not
  /// present.  If default_value is not present, it defaults to false.
  bool get_bool_property(std::string property, bool default_value) const;

  Visibility get_visibility() const;  ///< Get visibility.
  SubstanceData get_data() const;   ///< Get substance data.

  /// Vector indicating all substances that can be found as large deposits within this one.
  std::vector<std::string> large_deposits;

  /// Vector indicating all substances that can be found as small deposits within this one.
  std::vector<std::string> small_deposits;

  /// Vector indicating all substances that can be found as veins within this one.
  std::vector<std::string> vein_deposits;

  /// Vector indicating all substances that can be found as single chunk deposits within this one.
  std::vector<std::string> single_deposits;

  /// Vector indicating all substances that can be found as gangue next to this one.
  std::vector<std::string> gangue_deposits;

  /// @todo: Replace getTextureRect methods with proper OpenGL methods.
  //const sf::IntRect& getTextureRect(void); ///< Get texture rect for rendering.
  //const sf::IntRect& getTextureRect(void) const; ///< Get texture rect for rendering.

protected:
  Substance();    ///< Constructor (private)

  /// Load substance data from disk.
  bool load(std::string _name);

  /// Get substance XML properties.
  boost::property_tree::ptree const& get_properties() const;

private:
  struct Impl;
  /// Private implementation
  std::unique_ptr<Impl> impl;

// === Static members =========================================================
public:

private:
};

// Using declarations
using Properties = boost::property_tree::ptree;
using SubstanceShPtr = std::shared_ptr<Substance>;
using SubstanceConstShPtr = std::shared_ptr<Substance const>;

#endif // SUBSTANCE_H_
