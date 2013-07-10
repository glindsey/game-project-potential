#ifndef SUBSTANCE_H_
#define SUBSTANCE_H_

#include <string>
#include <boost/container/vector.hpp>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/scoped_ptr.hpp>
#include <glm/glm.hpp>

#include "common.h"

#include "SubstanceData.h"

namespace gsl
{
namespace rectopia
{

/// Representation of a Substance that a StageChunk or a Prop can be made out of.
class Substance: public boost::noncopyable
{
public:
  virtual ~Substance();                    ///< Destructor

  const boost::property_tree::ptree& getProperties(); ///< Get substance XML properties.
  const boost::property_tree::ptree& getProperties() const; ///< Get substance XML properties.

  Visibility getVisibility();        ///< Get visibility.
  Visibility getVisibility() const;  ///< Get visibility.

  const SubstanceData& getData();         ///< Get substance data.
  const SubstanceData& getData() const;   ///< Get substance data.

  /// Vector indicating all substances that can be found as large deposits within this one.
  boost::container::vector<const Substance*> largeDeposits;

  /// Vector indicating all substances that can be found as small deposits within this one.
  boost::container::vector<const Substance*> smallDeposits;

  /// Vector indicating all substances that can be found as veins within this one.
  boost::container::vector<const Substance*> veinDeposits;

  /// Vector indicating all substances that can be found as single chunk deposits within this one.
  boost::container::vector<const Substance*> singleDeposits;

  /// Vector indicating all substances that can be found as gangue next to this one.
  boost::container::vector<const Substance*> gangueDeposits;

  /// @todo: Replace getTextureRect methods with proper OpenGL methods.
  //const sf::IntRect& getTextureRect(void); ///< Get texture rect for rendering.
  //const sf::IntRect& getTextureRect(void) const; ///< Get texture rect for rendering.

protected:
  Substance(std::string _name);    ///< Constructor (private)

private:
  struct Impl;
  /// Private implementation
  boost::scoped_ptr<Impl> impl;

// === Static members =========================================================
public:
  static void initialize();

  static const Substance& get(std::string name);

  static const Substance& getAir();
  static const Substance& getNothing();

  static bool isSubstance(std::string substance, std::string classtype);

  static boost::container::vector<const Substance*> layerSoil;
  static boost::container::vector<const Substance*> layerSedimentary;
  static boost::container::vector<const Substance*> layerMetamorphic;
  static boost::container::vector<const Substance*> layerIgneousIntrusive;
  static boost::container::vector<const Substance*> layerIgneousExtrusive;

private:
};

} // namespace rectopia
} // namespace gsl

#endif // SUBSTANCE_H_