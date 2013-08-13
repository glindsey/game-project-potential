#ifndef STAGEBLOCK_H
#define STAGEBLOCK_H

#include "common_includes.h"
#include "common_typedefs.h"

#include "FaceBools.h"
#include "Inventory.h"
#include "Prop.h"
#include "StageComponent.h"
#include "Substance.h"

// Forward declarations
class StageComponentVisitor;
class StageChunk;
class StageNode;

class StageBlock:
  public StageComponent,
  public HasInventory,
  public boost::noncopyable
{
public:
  StageBlock(StageCoord x, StageCoord y, StageCoord z);
  ~StageBlock();

  void accept(StageComponentVisitor& visitor);

  /// Get the block's inventory.
  Inventory& get_inventory();

  /// Get the block's absolute coordinates.
  StageCoord3 const& get_coords() const;

  /// Get the substance a particular block layer is composed of.
  const Substance& get_substance(BlockLayer _layer) const;

  /// Set the substance a particular block layer is composed of.
  void set_substance(BlockLayer layer, const Substance& substance);

  /// Set the substance a particular block layer is composed of, without
  /// invalidating neighboring block hidden face data.  This should speed
  /// up stage generation a LOT.
  void set_substance_quickly(BlockLayer layer, const Substance& substance);

  /// Tells whether a substance is the same as another block's substance.
  bool is_same_substance_as(StageBlock& other, BlockLayer layer);

  /// Calculates hidden face info for the block.
  void calculate_hidden_faces();

  /// Gets hidden face info for a substance layer.
  /// If hidden face info is dirty, recalculates it before returning, but ONLY
  /// if the Stage indicates it is done building.  Otherwise, we're just spinning
  /// our wheels recalculating data that is bound to change over
  /// and over again.
  FaceBools get_hidden_faces(BlockLayer _layer);

  /// Sets the boolean indicating that hidden faces need recalculating.
  void invalidate_face_data();

  /// Returns true if any faces are visible, false otherwise.
  /// Includes both solid AND fluid layers.
  bool has_any_visible_faces();

  bool is_opaque(void) const;
  bool is_solid(void) const;
  bool is_traversable(void) const;
  bool is_visible(void) const;
  bool is_known(void) const;

  void set_known(bool _known);
  void set_known_quickly(bool _known);

private:
  /// @note Normally this class would use a PIMPL idiom like the other classes
  ///       I've implemented.  However, due to the sheer number of StageBlocks
  ///       we deal with, I'm trying to keep complexity down to a minimum.

  void invalidate_neighboring_faces();

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

  /// Boolean indicating whether this block is known to the player.
  bool known_;
};

#endif // STAGEBLOCK_H
