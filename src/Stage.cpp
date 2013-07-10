// *** ADDED BY HEADER FIXUP ***
#include <vector>
// *** END ***
#include "Stage.h"

#include "Application.h"
#include "ColumnData.h"
#include "CubicBezier.h"
#include "ErrorMacros.h"
#include "FaceBools.h"
#include "MathUtils.h"
#include "NoiseField.h"
#include "Prop.h"
#include "Settings.h"
#include "StageBlock.h"
#include "StageBuilderBeaches.h"
#include "StageBuilderDeposits.h"
#include "StageBuilderFlora.h"
#include "StageBuilderKnownStatus.h"
#include "StageBuilderLakes.h"
#include "StageBuilderRivers.h"
#include "StageBuilderSmoother.h"
#include "StageBuilderTerrain.h"
#include "StageChunk.h"
#include "StageChunkCollection.h"

#include <stddef.h>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <ctime>

namespace gsl
{
namespace rectopia
{

struct Stage::Impl
{
  /// Pointer to the stage instance.
  static boost::scoped_ptr<Stage> instance_;

  ColumnData& getColumnData(StageCoord x, StageCoord y)
  {
#ifndef NDEBUG
    if ((x < 0) || (y < 0) ||
        (x >= size_.x) || (y >= size_.y))
    {
      FATAL_ERROR("Request for column (%d, %d) is out of bounds", x, y);
    }
#endif

    return column_data_[(y * size_.x) + x];
  }

  ColumnData& getColumnData(StageCoord x, StageCoord y, StageCoord z)
  {
    return getColumnData(x, y);
  }

  /// Update a single column's statistics.
  /// @param coord Coordinates of the column to update.
  void UpdateColumnData(StageCoord x, StageCoord y)
  {
    ColumnData& cData = getColumnData(x, y);

    if (cData.dirty)
    {
      bool solid_height_found = false;
      bool render_height_found = false;
      bool outdoor_height_found = false;

      for (StageCoord z = size_.z - 1; z >= 0; z--)
      {
        StageBlock& block = chunks->getBlock(x, y, z);

        // Starting at the top, find the first chunk that is solid, and set height to this + 1.
        if ((solid_height_found == false) && (block.solid()))
        {
          cData.solid_height = z + 1;
          solid_height_found = true;
        }

        // Also look for the first chunk that is visible, and set render height to this.
        if ((render_height_found == false) && (block.visible()))
        {
          cData.render_height = z;
          render_height_found = true;
        }

        // Outdoor height is above the first solid chunk that has all hidden solid surfaces.
        if ((outdoor_height_found == false) && (block.solid())
            && (block.getHiddenFaces(BlockLayer::Solid).allTrue()))
        {
          cData.outdoor_height = z + 1;
          outdoor_height_found = true;
        }
      }

      // Clear the dirty bit.
      cData.dirty = false;
    }
  }

  /// Updates all column data.
  void UpdateAllColumnData()
  {
    StageCoord2 column;

    // First update the column data individually.
    for (column.x = size_.x - 1; column.x >= 0; --column.x)
    {
      for (column.y = size_.y - 1; column.y >= 0; --column.y)
      {
        UpdateColumnData(column.x, column.y);
      }
    }

    // If all updated okay, figure out min/max heights.
    StageCoord max_height = 0;
    StageCoord min_height = size_.z - 1;

    for (column.x = size_.x - 1; column.x >= 0; --column.x)
    {
      for (column.y = size_.y - 1; column.y >= 0; --column.y)
      {
        StageCoord height = getColumnData(column.x, column.y).solid_height;
        if (height > max_height)
        {
          max_height = height;
        }
        if (height < min_height)
        {
          min_height = height;
        }
      }
    }

    max_terrain_height_ = max_height;
    min_terrain_height_ = min_height;
  }

  /// This function returns true if the specified coordinates are in-bounds.
  inline bool validCoordinates(StageCoord x, StageCoord y, StageCoord z) const
  {
    return (x >= 0) && (x < size_.x) && (y >= 0)
           && (y < size_.y) && (z >= 0) && (z < size_.z);
  }

  /// Seed used for RNG.
  int seed_;

  /// Collection of all chunks comprising the stage.
  boost::scoped_ptr<StageChunkCollection> chunks;

  /// A vector of column data for the stage.
  boost::container::vector<ColumnData> column_data_;

  /// Size of the game stage, in three dimensions.
  StageCoord3 size_;

  /// Size of the chunk vector, in three dimensions.
  StageCoord3 chunk_vector_size_;

  /// Minimum column height (upon terrain generation).
  StageCoord min_terrain_height_;

  /// Maximum column height (upon terrain generation).
  StageCoord max_terrain_height_;

  /// Location of the cursor (in X/Y/Z space).
  StageCoord3 cursor_;

  /// If true, the map framework has been generated.
  bool ready_;

  /// If true, map can be rendered to the screen safely.
  bool okay_to_render_map_;

  /// State of the processing state machine.
  ProcessingState processing_state_;
}
;

boost::scoped_ptr<Stage> Stage::Impl::instance_;

Stage::Stage()
  : impl(new Impl())
{
}

Stage::~Stage()
{
  std::cout << "Destroying Stage..." << std::endl;
}

Stage& Stage::getInstance()
{
  // Instantiate stage if it hasn't been yet.
  if (Impl::instance_.get() == nullptr)
  {
    Impl::instance_.reset(new Stage());
  }

  return *(Impl::instance_.get());
}


void Stage::accept(StageComponentVisitor& visitor)
{
  bool visitChildren = visitor.visit(*this);

  if (visitChildren)
  {
    impl->chunks->accept(visitor);
  }
}

void Stage::build(StageCoord3 stage_size)
{
  this->build(stage_size, (int) std::time(nullptr));
}

void Stage::build(StageCoord3 stage_size, int seed)
{
  StageCoord3 coord;

  impl->size_.x = stage_size.x + (stage_size.x % StageChunk::ChunkSideLength);
  impl->size_.y = stage_size.y + (stage_size.y % StageChunk::ChunkSideLength);
  impl->size_.z = stage_size.z;

  impl->chunk_vector_size_.x = (stage_size.x / StageChunk::ChunkSideLength);
  impl->chunk_vector_size_.y = (stage_size.y / StageChunk::ChunkSideLength);
  impl->chunk_vector_size_.z = stage_size.z;

  impl->min_terrain_height_ = 0;
  impl->max_terrain_height_ = 0;
  impl->okay_to_render_map_ = false;

  impl->processing_state_ = Stage::ProcessingState::Idle;

  // Start the cursor 3/4 of the way up, in the middle.  (For now.)
  impl->cursor_.x = impl->size_.x / 2;
  impl->cursor_.y = impl->size_.y / 2;
  impl->cursor_.z = (impl->size_.z) * 3 / 4;

  std::cout << "Creating stage data structures..." << std::endl;
  impl->chunks.reset(new StageChunkCollection(impl->chunk_vector_size_));
  impl->column_data_.resize(impl->size_.x * impl->size_.y);

  impl->ready_ = true;

  std::cout << "Size of Stage: " << sizeof(Stage) << " bytes" << std::endl;
  std::cout << "Size of StageChunk: " << sizeof(StageChunk) << " bytes" << std::endl;
  std::cout << "Size of StageBlock: " << sizeof(StageBlock) << " bytes" << std::endl;

  // Seed the Mersenne Twister (used for random number generation).
  impl->seed_ = seed;
  App::instance().twister().seed(seed);

  // Tell the processing thread to fill the stage.
  // (Terrain is awfully rough right now!)
  std::cout << "Creating terrain..." << std::endl;
  impl->processing_state_ = Stage::ProcessingState::GenerateTerrain;
}

bool Stage::isReady()
{
  return impl->ready_;
}

EventResult Stage::handleKeyDown(sf::Event::KeyEvent key)
{
  // Shift+Arrows moves the camera, not the cursor.
  // Control+Arrows does... I don't know, but not this.
  // Ditto with Alt+Arrows.
  if (key.shift == true || key.control == true || key.alt == true)
  {
    return EventResult::Ignored;
  }

  switch (key.code)
  {
  case sf::Keyboard::Left:
    this->moveCursor(-1, 0, 0);
    return EventResult::Handled;

  case sf::Keyboard::Home:
    this->moveCursor(-1, -1, 0);
    return EventResult::Handled;

  case sf::Keyboard::Up:
    this->moveCursor(0, -1, 0);
    return EventResult::Handled;

  case sf::Keyboard::PageUp:
    this->moveCursor(1, -1, 0);
    return EventResult::Handled;

  case sf::Keyboard::Right:
    this->moveCursor(1, 0, 0);
    return EventResult::Handled;

  case sf::Keyboard::PageDown:
    this->moveCursor(1, 1, 0);
    return EventResult::Handled;

  case sf::Keyboard::Down:
    this->moveCursor(0, 1, 0);
    return EventResult::Handled;

  case sf::Keyboard::End:
    this->moveCursor(-1, 1, 0);
    return EventResult::Handled;

  case sf::Keyboard::Period:
    this->moveCursor(0, 0, -1);
    return EventResult::Handled;

  case sf::Keyboard::Comma:
    this->moveCursor(0, 0, 1);
    return EventResult::Handled;

  default:
    /* do nothing */
    return EventResult::Ignored;
  }
}

void Stage::setCursor(StageCoord x, StageCoord y, StageCoord z)
{
  impl->cursor_ = constrainToBox(StageCoord3(0, 0, 0),
                                 StageCoord3(x, y, z),
                                 StageCoord3(impl->size_.x - 1,
                                             impl->size_.y - 1,
                                             impl->size_.z - 1));
}

void Stage::moveCursor(StageCoord xAdd, StageCoord yAdd, StageCoord zAdd)
{
  setCursor(impl->cursor_.x + xAdd,
            impl->cursor_.y + yAdd,
            impl->cursor_.z + zAdd);
}

StageCoord3 Stage::size() const
{
  return impl->size_;
}

StageCoord3 Stage::cursor() const
{
  return impl->cursor_;
}

void Stage::updateAllColumnHeights()
{
  impl->UpdateAllColumnData();
}

bool Stage::atEdgeLeft(StageCoord3 const& coord) const
{
  return (coord.x == 0);
}

bool Stage::atEdgeRight(StageCoord3 const& coord) const
{
  return (coord.x == impl->size_.x - 1);
}

bool Stage::atEdgeBack(StageCoord3 const& coord) const
{
  return (coord.y == 0);
}

bool Stage::atEdgeFront(StageCoord3 const& coord) const
{
  return (coord.y == impl->size_.y - 1);
}

bool Stage::atEdgeBottom(StageCoord3 const& coord) const
{
  return (coord.z == 0);
}

bool Stage::atEdgeTop(StageCoord3 const& coord) const
{
  return (coord.z == impl->size_.z - 1);
}

StageChunk& Stage::getChunkContaining(StageCoord x, StageCoord y, StageCoord z)
{
#ifndef NDEBUG
  if ((x < 0) || (y < 0) || (z < 0) ||
      (x >= impl->size_.x) ||
      (y >= impl->size_.y) ||
      (z >= impl->size_.z))
  {
    FATAL_ERROR("Request for chunk containing (%d, %d, %d) is out of bounds",
                x, y, z);  }
#endif

    return impl->chunks->getChunkContaining(x, y, z);
}

StageBlock& Stage::getBlock(StageCoord x, StageCoord y, StageCoord z)
{
#ifndef NDEBUG
  if ((x < 0) || (y < 0) || (z < 0) ||
      (x >= impl->size_.x) ||
      (y >= impl->size_.y) ||
      (z >= impl->size_.z))
  {
    FATAL_ERROR("Request for block (%d, %d, %d) is out of bounds", x, y, z);
  }
#endif

  return impl->chunks->getBlock(x, y, z);
}

void Stage::process(void)
{
  static ProcessingState last_processing_state = ProcessingState::Idle;
  static boost::scoped_ptr<StageBuilder> builder;
  bool done = false;

  // Have we changed states?
  if (impl->processing_state_ != last_processing_state)
  {
    // Clean up the last state if needed.
    switch (last_processing_state)
    {
    case ProcessingState::SmoothTerrain:
      impl->UpdateAllColumnData();
      break;

    default:
      break;
    }

    // Update the last processing state.
    last_processing_state = impl->processing_state_;

    // Initialize the new state if needed.
    switch (impl->processing_state_)
    {
    case ProcessingState::GenerateTerrain:
      builder.reset(
        new StageBuilderTerrain(*this, impl->seed_,
                                Settings::terrainStageHeight,
                                Settings::terrainFeatureHeight,
                                Settings::terrainFrequency,
                                Settings::terrainOctaveCount,
                                Settings::terrainPersistence,
                                Settings::terrainLacunarity));
      break;

    case ProcessingState::AddDeposits:
      builder.reset(new StageBuilderDeposits(*this, impl->seed_));
      break;

    case ProcessingState::AddLakes:
      builder.reset(
        new StageBuilderLakes(*this, impl->seed_,
                              Settings::terrainSeaLevel));
      break;

    case ProcessingState::AddBeaches:
      builder.reset(
        new StageBuilderBeaches(*this, impl->seed_,
                                Settings::terrainSeaLevel));
      break;

    case ProcessingState::AddRivers:
      builder.reset(
        new StageBuilderRivers(*this, impl->seed_,
                               Settings::terrainSeaLevel));
      break;

    case ProcessingState::SmoothTerrain:
      builder.reset(new StageBuilderSmoother(*this, impl->seed_));
      break;

    case ProcessingState::AddFlora:
      builder.reset(
        new StageBuilderFlora(*this, impl->seed_,
                              Settings::terrainPlainsThreshold,
                              Settings::terrainForestThreshold));
      break;

    case ProcessingState::AddFauna:
      // TODO: create builder for this!
      builder.reset();
      break;

    case ProcessingState::SetPlayerKnowledge:
      builder.reset(new StageBuilderKnownStatus(*this, impl->seed_));
      break;

    default:
      builder.reset();
      break;
    }
  }

  if (builder.get() != nullptr)
  {
    done = builder->Build();
  }
  else
  {
    done = true;
  }

  // Other stuff to do while in the state.
  switch (impl->processing_state_)
  {
  case ProcessingState::Idle:
    // This is the state prior to world generation.
    break;

  case ProcessingState::GenerateTerrain:
    if (done)
    {
      // At this point it's okay to do map rendering.
      impl->okay_to_render_map_ = true;
      impl->processing_state_ = ProcessingState::AddDeposits;
    }
    break;

  case ProcessingState::AddDeposits:
    if (done)
    {
      impl->UpdateAllColumnData();
      impl->processing_state_ = ProcessingState::AddLakes;
    }
    break;

  case ProcessingState::AddLakes:
    if (done)
    {
      impl->processing_state_ = ProcessingState::AddBeaches;
    }
    break;

  case ProcessingState::AddBeaches:
    if (done)
    {
      impl->processing_state_ = ProcessingState::AddRivers;
    }
    break;

  case ProcessingState::AddRivers:
    if (done)
    {
      impl->UpdateAllColumnData();
      impl->processing_state_ = ProcessingState::SmoothTerrain;
    }
    break;

  case ProcessingState::SmoothTerrain:
    if (done)
    {
      impl->processing_state_ = ProcessingState::AddFlora;
    }
    break;

  case ProcessingState::AddFlora:
    if (done)
    {
      impl->processing_state_ = ProcessingState::AddFauna;
    }
    break;

  case ProcessingState::AddFauna:
    impl->processing_state_ = ProcessingState::SetPlayerKnowledge;
    break;

  case ProcessingState::SetPlayerKnowledge:
    if (done)
    {
      std::cout << "Terrain generation complete.  Moving to PAUSED state."
                << std::endl;
      impl->processing_state_ = ProcessingState::Paused;
    }
    break;

  case ProcessingState::Paused:
    // This is the state when world processing is paused.
    break;

  case ProcessingState::Running:
    // This is the state when world processing is running.
    impl->UpdateAllColumnData();
    break;

  case ProcessingState::Halted:
    // This is the state when world processing is halted.
    break;

  default:
    break;
  }
}

bool Stage::validCoordinates(StageCoord x, StageCoord y, StageCoord z) const
{
  return impl->validCoordinates(x, y, z);
}

StageCoord Stage::min_terrain_height()
{
  return impl->min_terrain_height_;
}

StageCoord Stage::max_terrain_height()
{
  return impl->max_terrain_height_;
}

StageCoord Stage::getColumnInitialHeight(StageCoord x, StageCoord y)
{
  return impl->getColumnData(x, y).initial_height;
}

StageCoord Stage::getColumnSolidHeight(StageCoord x, StageCoord y)
{
  impl->UpdateColumnData(x, y);
  return impl->getColumnData(x, y).solid_height;
}

StageCoord Stage::getColumnOutdoorHeight(StageCoord x, StageCoord y)
{
  impl->UpdateColumnData(x, y);
  return impl->getColumnData(x, y).outdoor_height;
}

void Stage::setColumnInitialHeight(StageCoord x,
                                   StageCoord y,
                                   StageCoord height)
{
  ColumnData& column = impl->getColumnData(x, y);

  column.initial_height = height;
  column.dirty = true;
}

void Stage::setColumnDirty(StageCoord x, StageCoord y)
{
  ColumnData& column = impl->getColumnData(x, y);
  if (!column.dirty)
  {
    column.dirty = true;
  }
}

bool Stage::okay_to_render_map()
{
  return impl->okay_to_render_map_;
}

} // namespace rectopia
} // namespace gsl
