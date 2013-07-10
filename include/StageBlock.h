#ifndef STAGEBLOCK_H
#define STAGEBLOCK_H

#include "common_includes.h"
#include "common_typedefs.h"

#include "BlockTopCorners.h"
#include "FaceBools.h"
#include "Inventory.h"
#include "Prop.h"
#include "StageComponent.h"
#include "Substance.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class StageComponentVisitor;
class StageChunk;
class StageNode;

class StageBlock:
  virtual public StageComponent,
  virtual public HasInventory,
  virtual public boost::noncopyable
{
public:
  StageBlock(StageCoord x, StageCoord y, StageCoord z);
  ~StageBlock();

  void accept(StageComponentVisitor& visitor);

  /// Get the block's inventory.
  Inventory& inventory();

  /// Get the block's absolute coordinates.
  StageCoord3 const& getCoordinates() const;

  /// Get the substance a particular block layer is composed of.
  const Substance& substance(BlockLayer _layer) const;

  /// Set the substance a particular block layer is composed of.
  void setSubstance(BlockLayer layer, const Substance& substance);

  /// Set the substance a particular block layer is composed of, without
  /// invalidating neighboring block hidden face data.  This should speed
  /// up stage generation a LOT.
  void setSubstanceQuickly(BlockLayer layer, const Substance& substance);

  /// Tells whether a substance is the same as another block's substance.
  bool isSameSubstanceAs(StageBlock& other, BlockLayer layer);

  /// Calculates hidden face info for the block.
  void calculateHiddenFaces();

  /// Gets hidden face info for a substance layer.
  /// If hidden face info is dirty, recalculates it before returning, but ONLY
  /// if the Stage indicates it is done building.  Otherwise, we're just spinning
  /// our wheels recalculating data that is bound to change over
  /// and over again.
  FaceBools getHiddenFaces(BlockLayer _layer);

  /// Sets the boolean indicating that hidden faces need recalculating.
  void setFaceDataDirty();

  /// Returns true if any faces are visible, false otherwise.
  /// Includes both solid AND fluid layers.
  bool hasAnyVisibleFaces();

  bool isOpaque(void) const;
  bool solid(void) const;
  bool traversable(void) const;
  bool visible(void) const;
  bool known(void) const;

  void setKnown(bool _known);
  void setKnownQuickly(bool _known);

  /// Returns number of lowered corners.
  unsigned int getLowCornerCount(void) const;

  /// Get the top corners of this block.
  BlockTopCorners& top_corners();

private:
  /// @note Normally this class would use a PIMPL idiom like the other classes
  ///       I've implemented.  However, due to the sheer number of StageBlocks
  ///       we deal with, I'm trying to keep complexity down to a minimum.

  void invalidateNeighborFaces();

  /// Absolute coordinates for this block.
  StageCoord3 coord_;

  /// Materials comprising the block.
  const Substance* substance_[(unsigned int) BlockLayer::Count];

  /// Booleans indicating which sides are hidden from view.
  FaceBools hidden_faces_[(unsigned int) BlockLayer::Count];

  /// Boolean indicating whether hidden face data needs updating.
  bool hidden_faces_dirty_;

  /// Info about any fluid flow in this block.
  struct _fluid
  {
    /// Direction of fluid flow (assuming there is fluid and it is flowing),
    /// in radians.  It is a compass direction -- up/down is not represented,
    /// because obviously the fluid's going to flow down whenever possible.
    float direction;

    /// Speed of fluid flow (assuming there is fluid), in... some units which
    /// I haven't decided yet... on the X-Y plane.
    float speed;

  } fluid_;

  /// Inventory of the block.
  Inventory inventory_;

  /// Values indicating relative corner heights from default.
  /// Heights can range from -1 to +1, where -1 is a half-block lower,
  /// and +1 is a half-block higher.
  BlockTopCorners top_corners_;

  /// Number of corners lower than the default.  Automatically updated whenever
  /// setCornerHeight is called.
  unsigned int corner_low_count_;

  /// Boolean indicating whether this block is known to the player.
  bool known_;
};

} // namespace rectopia
} // namespace gsl

#endif // STAGEBLOCK_H
