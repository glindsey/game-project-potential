#include "StageBlock.h"

#include "ColumnData.h"
#include "MathUtils.h"
#include "Settings.h"
#include "Stage.h"
#include "StageChunk.h"
#include "StageComponentVisitor.h"
#include "SubstanceLibrary.h"

StageBlock::StageBlock(StageCoord x, StageCoord y, StageCoord z)
{
  coord_.x = x;
  coord_.y = y;
  coord_.z = z;
  hidden_faces_dirty_ = false;
  known_ = false;
  substance_[(unsigned int) BlockLayer::Solid] = "nothing";
  substance_[(unsigned int) BlockLayer::Fluid] = "air";
  substance_[(unsigned int) BlockLayer::Cover] = "nothing";
}

StageBlock::~StageBlock()
{
}

void StageBlock::accept(StageComponentVisitor& visitor)
{
  bool visitChildren = visitor.visit(*this);

  if (visitChildren)
  {
    // Visit this block's contents.
    for (HasLocation* object : inventory_.getContents())
    {
      StageComponent* component = dynamic_cast<StageComponent*>(object);
      if (component != nullptr)
      {
        component->accept(visitor);
      }
    }
  }
}

Inventory& StageBlock::get_inventory()
{
  return inventory_;
}

std::string StageBlock::get_substance(BlockLayer _layer) const
{
  return substance_[(unsigned int) _layer];
}

void StageBlock::set_substance(BlockLayer layer, std::string substance)
{
  bool change = (substance_[(unsigned int) layer] != substance);
  if (change)
  {
    StageChunk& chunk = Stage::get_instance()->get_chunk_containing(coord_.x,
                                                                    coord_.y,
                                                                    coord_.z);
    substance_[(unsigned int) layer] = substance;
    invalidate_neighboring_faces();
    Stage::get_instance()->set_column_dirty(coord_.x, coord_.y);
    chunk.set_render_data_dirty(true);
  }
}

void StageBlock::set_substance_quickly(BlockLayer layer, std::string substance)
{
  StageChunk& chunk = Stage::get_instance()->get_chunk_containing(coord_.x,
                                                                  coord_.y,
                                                                  coord_.z);
  substance_[(unsigned int) layer] = substance;
  hidden_faces_dirty_ = true;
  Stage::get_instance()->set_column_dirty(coord_.x, coord_.y);
  chunk.set_render_data_dirty(true);
}

bool StageBlock::is_opaque(void) const
{
  Visibility solid_visibility =
    SL->get(substance_[(unsigned int) BlockLayer::Solid])->get_visibility();
  Visibility fluid_visibility =
    SL->get(substance_[(unsigned int) BlockLayer::Fluid])->get_visibility();

  return (solid_visibility == Visibility::Opaque
          || fluid_visibility == Visibility::Opaque);
}

bool StageBlock::is_solid(void) const
{
  Phase solid_phase =
    SL->get(substance_[(unsigned int) BlockLayer::Solid])->get_data().phase;
  return (solid_phase == Phase::Solid);
}

bool StageBlock::is_traversable(void) const
{
  return !is_solid();
}

bool StageBlock::is_visible(void) const
{
  Visibility solid_visibility =
    SL->get(substance_[(unsigned int) BlockLayer::Solid])->get_visibility();
  Visibility fluid_visibility =
    SL->get(substance_[(unsigned int) BlockLayer::Fluid])->get_visibility();

  return (solid_visibility != Visibility::Invisible
          || fluid_visibility != Visibility::Invisible);
}

bool StageBlock::is_known(void) const
{
  return known_;
}

void StageBlock::set_known(bool known)
{
  bool change = (known_ != known);
  if (change)
  {
    StageChunk& chunk = Stage::get_instance()->get_chunk_containing(coord_.x,
                                                                    coord_.y,
                                                                    coord_.z);
    known_ = known;
    invalidate_neighboring_faces();
    Stage::get_instance()->set_column_dirty(coord_.x, coord_.y);
    chunk.set_render_data_dirty(true);
  }
}

void StageBlock::set_known_quickly(bool known)
{
  known_ = known;
  hidden_faces_dirty_ = true;
}

FaceBools StageBlock::get_hidden_faces(BlockLayer _layer)
{
  if (hidden_faces_dirty_)
  {
    calculate_hidden_faces();
  }
  return hidden_faces_[(unsigned int) _layer];
}

void StageBlock::invalidate_face_data()
{
  hidden_faces_dirty_ = true;
}

bool StageBlock::has_any_visible_faces()
{
  if (hidden_faces_dirty_)
  {
    calculate_hidden_faces();
  }
  return (!hidden_faces_[(unsigned int) BlockLayer::Solid].allTrue()
          || !hidden_faces_[(unsigned int) BlockLayer::Fluid].allTrue());
}

bool StageBlock::is_same_substance_as(StageBlock& other, BlockLayer layer)
{
  std::string my_substance = substance_[(unsigned int) layer];
  std::string other_substance = other.get_substance(layer);
  return (my_substance == other_substance);
}

void StageBlock::calculate_hidden_faces()
{
  FaceBools solid_hidden(false);
  FaceBools fluid_hidden(false);

  StageShPtr stage = Stage::get_instance();

  // A face is hidden if the chunk adjacent to it is:
  // 1. Made of the same material.
  // 2. Opaque and has no lowered corners.

  // Bottom face:
  if (!stage->at_edge_bottom(coord_))
  {
    StageBlock& adjacent = stage->get_block(coord_.x, coord_.y, coord_.z - 1);
    if (is_same_substance_as(adjacent, BlockLayer::Solid) || (adjacent.is_opaque()))
    {
      solid_hidden.set_bottom(true);
    }
    if (is_same_substance_as(adjacent, BlockLayer::Fluid) || (adjacent.is_opaque()))
    {
      fluid_hidden.set_bottom(true);
    }
  }

  // Top face...
  if (!stage->at_edge_top(coord_))
  {
    StageBlock& adjacent = stage->get_block(coord_.x, coord_.y, coord_.z + 1);
    if (is_same_substance_as(adjacent, BlockLayer::Solid) || (adjacent.is_opaque()))
    {
      solid_hidden.set_top(true);
    }
    if (is_same_substance_as(adjacent, BlockLayer::Fluid) || (adjacent.is_opaque()))
    {
      fluid_hidden.set_top(true);
    }
  }

  // Back face...
  if (!stage->at_edge_back(coord_))
  {
    StageBlock& adjacent = stage->get_block(coord_.x, coord_.y - 1, coord_.z);
    if (is_same_substance_as(adjacent, BlockLayer::Solid) || (adjacent.is_opaque()))
    {
      solid_hidden.set_back(true);
    }
    if (is_same_substance_as(adjacent, BlockLayer::Fluid) || (adjacent.is_opaque()))
    {
      fluid_hidden.set_back(true);
    }
  }

  // Front face...
  if (!stage->at_edge_front(coord_))
  {
    StageBlock& adjacent = stage->get_block(coord_.x, coord_.y + 1, coord_.z);
    if (is_same_substance_as(adjacent, BlockLayer::Solid) || (adjacent.is_opaque()))
    {
      solid_hidden.set_front(true);
    }
    if (is_same_substance_as(adjacent, BlockLayer::Fluid) || (adjacent.is_opaque()))
    {
      fluid_hidden.set_front(true);
    }
  }

  // Left face...
  if (!stage->at_edge_left(coord_))
  {
    StageBlock& adjacent = stage->get_block(coord_.x - 1, coord_.y, coord_.z);
    if (is_same_substance_as(adjacent, BlockLayer::Solid) || (adjacent.is_opaque()))
    {
      solid_hidden.set_left(true);
    }
    if (is_same_substance_as(adjacent, BlockLayer::Fluid) || (adjacent.is_opaque()))
    {
      fluid_hidden.set_left(true);
    }
  }

  // Right face...
  if (!stage->at_edge_right(coord_))
  {
    StageBlock& adjacent = stage->get_block(coord_.x + 1, coord_.y, coord_.z);
    if (is_same_substance_as(adjacent, BlockLayer::Solid) || (adjacent.is_opaque()))
    {
      solid_hidden.set_right(true);
    }
    if (is_same_substance_as(adjacent, BlockLayer::Fluid) || (adjacent.is_opaque()))
    {
      fluid_hidden.set_right(true);
    }
  }

  // Write the new values back.  This also clears the dirty bit.
  hidden_faces_[(unsigned int) BlockLayer::Solid] = solid_hidden;
  hidden_faces_[(unsigned int) BlockLayer::Fluid] = fluid_hidden;
  hidden_faces_dirty_ = false;
}

StageCoord3 StageBlock::get_coords() const
{
  return coord_;
}

// === PRIVATE METHODS ========================================================
void StageBlock::invalidate_neighboring_faces()
{
  StageShPtr stage = Stage::get_instance();

  hidden_faces_dirty_ = true;

  if (!stage->at_edge_left(coord_))
  {
    stage->get_block(coord_.x - 1, coord_.y, coord_.z).invalidate_face_data();
  }
  if (!stage->at_edge_right(coord_))
  {
    stage->get_block(coord_.x + 1, coord_.y, coord_.z).invalidate_face_data();
  }
  if (!stage->at_edge_back(coord_))
  {
    stage->get_block(coord_.x, coord_.y - 1, coord_.z).invalidate_face_data();
  }
  if (!stage->at_edge_front(coord_))
  {
    stage->get_block(coord_.x, coord_.y + 1, coord_.z).invalidate_face_data();
  }
  if (!stage->at_edge_bottom(coord_))
  {
    stage->get_block(coord_.x, coord_.y, coord_.z - 1).invalidate_face_data();
  }
  if (!stage->at_edge_top(coord_))
  {
    stage->get_block(coord_.x, coord_.y, coord_.z + 1).invalidate_face_data();
  }
}
