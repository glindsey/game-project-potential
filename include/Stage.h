#ifndef STAGE_H_
#define STAGE_H_

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "EventListener.h"
#include "StageComponent.h"

namespace gsl
{
namespace rectopia
{

// Forward declarations
class ColumnData;
class StageBlock;
class StageChunk;
class StageComponentVisitor;

/// Representation of the game playing field.
class Stage: public EventListener, public StageComponent
{
  enum class ProcessingState
  {
    Idle,
    GenerateTerrain,
    AddDeposits,
    AddLakes,
    AddBeaches,
    AddRivers,
    SmoothTerrain,
    AddFlora,
    AddFauna,
    SetPlayerKnowledge,
    Paused,
    Running,
    Halted
  };

public:
  ~Stage();

  /// Get the single Stage instance.  Create if one doesn't exist yet.
  static Stage& getInstance();

  void accept(StageComponentVisitor& visitor);

  void build(StageCoord3 stage_size);
  void build(StageCoord3 stage_size, int _seed);

  /// Returns true if the Stage is ready for use.
  bool isReady();

  EventResult handleKeyDown(sf::Event::KeyEvent key);

  void setCursor(StageCoord x, StageCoord y, StageCoord z);
  void moveCursor(StageCoord xAdd, StageCoord yAdd, StageCoord zAdd);

  /// Process stage data.
  void process(void);

  /// Gets the initial height of a column.
  StageCoord getColumnInitialHeight(StageCoord x, StageCoord y);

  /// Gets the solid height of a column.
  StageCoord getColumnSolidHeight(StageCoord x, StageCoord y);

  /// Gets the outdoor height of a column.
  StageCoord getColumnOutdoorHeight(StageCoord x, StageCoord y);

  /// Sets the initial height of a column.
  /// @warning Should ONLY be called during the initial height-map generation!
  void setColumnInitialHeight(StageCoord x, StageCoord y, StageCoord height);

  /// Sets that a column needs recalculating.
  void setColumnDirty(StageCoord x, StageCoord y);

  /// Gets the minimum terrain height of the stage.
  StageCoord min_terrain_height();

  /// Gets the maximum terrain height of the stage.
  StageCoord max_terrain_height();

  bool atEdgeLeft(StageCoord3 const& coord) const;
  bool atEdgeRight(StageCoord3 const& coord) const;
  bool atEdgeBack(StageCoord3 const& coord) const;
  bool atEdgeFront(StageCoord3 const& coord) const;
  bool atEdgeTop(StageCoord3 const& coord) const;
  bool atEdgeBottom(StageCoord3 const& coord) const;

  /// Gets the StageChunk containing a particular block.
  StageChunk& getChunkContaining(StageCoord x, StageCoord y, StageCoord z);

  /// Gets a particular StageBlock by absolute coordinates.
  StageBlock& getBlock(StageCoord x, StageCoord y, StageCoord z);

  /// Gets the stage size.
  StageCoord3 size() const;

  /// Gets the current cursor location.
  StageCoord3 cursor() const;

  /// Checks to see if a set of coordinates is within the stage boundaries.
  bool validCoordinates(StageCoord x, StageCoord y, StageCoord z) const;

  /// Called to update column heights.
  void updateAllColumnHeights();

  /// Indicates whether the terrain is ready for rendering.
  bool okay_to_render_map();

protected:

private:
  /// Private constructor since this is a singleton.
  Stage();

  struct Impl;
  /// Private Implementation Pointer
  boost::scoped_ptr<Impl> impl;

};

} // namespace rectopia
} // namespace gsl

#endif // STAGE_H_
