#ifndef STAGE_H_
#define STAGE_H_

#include "common_enums.h"
#include "common_includes.h"
#include "common_typedefs.h"

#include "EventListener.h"
#include "StageComponent.h"

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
  static std::shared_ptr<Stage> get_instance();

  void accept(StageComponentVisitor& visitor);

  void build(StageCoord3 stage_size);
  void build(StageCoord3 stage_size, int _seed);

  /// Returns true if the Stage is ready for use.
  bool is_ready();

  EventResult handle_key_down(sf::Event::KeyEvent key);

  void set_cursor(StageCoord x, StageCoord y, StageCoord z);
  void move_cursor(StageCoord xAdd, StageCoord yAdd, StageCoord zAdd);

  /// Process stage data.
  void process(void);

  /// Gets the initial height of a column.
  StageCoord get_column_initial_height(StageCoord x, StageCoord y);

  /// Gets the solid height of a column.
  StageCoord get_column_solid_height(StageCoord x, StageCoord y);

  /// Gets the outdoor height of a column.
  StageCoord get_column_outdoor_height(StageCoord x, StageCoord y);

  /// Sets the initial height of a column.
  /// @warning Should ONLY be called during the initial height-map generation!
  void set_column_initial_height(StageCoord x, StageCoord y, StageCoord height);

  /// Sets that a column needs recalculating.
  void set_column_dirty(StageCoord x, StageCoord y);

  /// Gets the minimum terrain height of the stage.
  StageCoord min_terrain_height();

  /// Gets the maximum terrain height of the stage.
  StageCoord max_terrain_height();

  bool at_edge_left(StageCoord3 const& coord) const;
  bool at_edge_right(StageCoord3 const& coord) const;
  bool at_edge_back(StageCoord3 const& coord) const;
  bool at_edge_front(StageCoord3 const& coord) const;
  bool at_edge_top(StageCoord3 const& coord) const;
  bool at_edge_bottom(StageCoord3 const& coord) const;

  /// Gets the StageChunk containing a particular block.
  StageChunk& get_chunk_containing(StageCoord x, StageCoord y, StageCoord z);

  /// Gets a particular StageBlock by absolute coordinates.
  StageBlock& get_block(StageCoord x, StageCoord y, StageCoord z);

  /// Gets the stage size.
  StageCoord3 size() const;

  /// Gets the current cursor location.
  StageCoord3 cursor() const;

  /// Checks to see if a set of coordinates is within the stage boundaries.
  bool valid_coordinates(StageCoord x, StageCoord y, StageCoord z) const;

  /// Called to update column heights.
  void update_all_column_heights();

  /// Indicates whether the terrain is ready for rendering.
  bool okay_to_render_map();

protected:

private:
  /// Private constructor since this is a singleton.
  Stage();

  struct Impl;
  /// Private Implementation Pointer
  std::unique_ptr<Impl> impl;

};

// Using declaration
using StageShPtr = std::shared_ptr<Stage>;

#endif // STAGE_H_
