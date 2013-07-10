#include "StageBlock.h"

#include "BlockTopCorners.h"
#include "ColumnData.h"
#include "MathUtils.h"
#include "Settings.h"
#include "Stage.h"
#include "StageChunk.h"
#include "StageComponentVisitor.h"

namespace gsl
{
namespace rectopia
{

StageBlock::StageBlock(StageCoord x, StageCoord y, StageCoord z)
{
  coord_.x = x;
  coord_.y = y;
  coord_.z = z;

  hidden_faces_dirty_ = false;
  corner_low_count_ = 0;
  known_ = false;
  substance_[(unsigned int) BlockLayer::Solid] = &(Substance::getNothing());
  substance_[(unsigned int) BlockLayer::Fluid] = &(Substance::getAir());
  substance_[(unsigned int) BlockLayer::Cover] = &(Substance::getNothing());
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
    for (StageComponent * object : inventory_.getContents())
    {
      object->accept(visitor);
    }
  }
}

Inventory& StageBlock::inventory()
{
  return inventory_;
}

const Substance& StageBlock::substance(BlockLayer _layer) const
{
  return *(substance_[(unsigned int) _layer]);
}

void StageBlock::setSubstance(BlockLayer layer, const Substance& substance)
{
  bool change = (substance_[(unsigned int) layer] != &substance);
  if (change)
  {
    StageChunk& chunk = Stage::getInstance().getChunkContaining(coord_.x,
                                                                coord_.y,
                                                                coord_.z);
    substance_[(unsigned int) layer] = &substance;
    invalidateNeighborFaces();
    Stage::getInstance().setColumnDirty(coord_.x, coord_.y);
    chunk.setRenderDataDirty(true);
  }
}

void StageBlock::setSubstanceQuickly(BlockLayer layer, const Substance& substance)
{
  StageChunk& chunk = Stage::getInstance().getChunkContaining(coord_.x,
                                                              coord_.y,
                                                              coord_.z);
  substance_[(unsigned int) layer] = &substance;
  hidden_faces_dirty_ = true;
  Stage::getInstance().setColumnDirty(coord_.x, coord_.y);
  chunk.setRenderDataDirty(true);
}

bool StageBlock::isOpaque(void) const
{
  const Substance* solid_substance =
    substance_[(unsigned int) BlockLayer::Solid];
  const Substance* fluid_substance =
    substance_[(unsigned int) BlockLayer::Fluid];

  Visibility solid_visibility = solid_substance->getVisibility();
  Visibility fluid_visibility = fluid_substance->getVisibility();

  return (solid_visibility == Visibility::Opaque
          || fluid_visibility == Visibility::Opaque);
}

bool StageBlock::solid(void) const
{
  return (substance_[(unsigned int) BlockLayer::Solid]->getData().phase
          == Phase::Solid);
}

bool StageBlock::traversable(void) const
{
  return (!solid()) || (corner_low_count_ > 0);
}

bool StageBlock::visible(void) const
{
  return (substance_[(unsigned int) BlockLayer::Solid]->getVisibility()
          != Visibility::Invisible
          || substance_[(unsigned int) BlockLayer::Fluid]->getVisibility() != Visibility::Invisible);
}

bool StageBlock::known(void) const
{
  return known_;
}

void StageBlock::setKnown(bool known)
{
  bool change = (known_ != known);
  if (change)
  {
    StageChunk& chunk = Stage::getInstance().getChunkContaining(coord_.x, coord_.y, coord_.z);
    known_ = known;
    invalidateNeighborFaces();
    Stage::getInstance().setColumnDirty(coord_.x, coord_.y);
    chunk.setRenderDataDirty(true);
  }
}

void StageBlock::setKnownQuickly(bool known)
{
  known_ = known;
  hidden_faces_dirty_ = true;
}

FaceBools StageBlock::getHiddenFaces(BlockLayer _layer)
{
  if (hidden_faces_dirty_)
  {
    calculateHiddenFaces();
  }
  return hidden_faces_[(unsigned int) _layer];
}

void StageBlock::setFaceDataDirty()
{
  hidden_faces_dirty_ = true;
}

bool StageBlock::hasAnyVisibleFaces()
{
  return (!hidden_faces_[(unsigned int) BlockLayer::Solid].allTrue()
          || !hidden_faces_[(unsigned int) BlockLayer::Fluid].allTrue());
}

unsigned int StageBlock::getLowCornerCount(void) const
{
  return corner_low_count_;
}

bool StageBlock::isSameSubstanceAs(StageBlock& other, BlockLayer layer)
{
  const Substance* my_substance = substance_[(unsigned int) layer];
  const Substance* other_substance = &(other.substance(layer));
  return (my_substance == other_substance);
}

void StageBlock::calculateHiddenFaces()
{
  FaceBools solid_hidden(false);
  FaceBools fluid_hidden(false);

  Stage& stage = Stage::getInstance();

  // A face is hidden if the chunk adjacent to it is:
  // 1. Made of the same material.
  // 2. Opaque and has no lowered corners.

  // Bottom face:
  if (!stage.atEdgeBottom(coord_))
  {
    StageBlock& adjacent = stage.getBlock(coord_.x, coord_.y, coord_.z - 1);
    if ((isSameSubstanceAs(adjacent, BlockLayer::Solid) || (adjacent.isOpaque())) && (adjacent.getLowCornerCount()
        == 0))
    {
      solid_hidden.set_bottom(true);
    }
    if (isSameSubstanceAs(adjacent, BlockLayer::Fluid) || (adjacent.isOpaque()))
    {
      fluid_hidden.set_bottom(true);
    }
  }

  // Top face...
  if (!stage.atEdgeTop(coord_))
  {
    StageBlock& adjacent = stage.getBlock(coord_.x, coord_.y, coord_.z + 1);
    if ((isSameSubstanceAs(adjacent, BlockLayer::Solid) || (adjacent.isOpaque())) && (adjacent.getLowCornerCount()
        == 0))
    {
      solid_hidden.set_top(true);
    }
    if (isSameSubstanceAs(adjacent, BlockLayer::Fluid) || (adjacent.isOpaque()))
    {
      fluid_hidden.set_top(true);
    }
  }

  // Back face...
  if (!stage.atEdgeBack(coord_))
  {
    StageBlock& adjacent = stage.getBlock(coord_.x, coord_.y - 1, coord_.z);
    if ((isSameSubstanceAs(adjacent, BlockLayer::Solid) || (adjacent.isOpaque())) && (adjacent.getLowCornerCount()
        == 0))
    {
      solid_hidden.set_back(true);
    }
    if (isSameSubstanceAs(adjacent, BlockLayer::Fluid) || (adjacent.isOpaque()))
    {
      fluid_hidden.set_back(true);
    }
  }

  // Front face...
  if (!stage.atEdgeFront(coord_))
  {
    StageBlock& adjacent = stage.getBlock(coord_.x, coord_.y + 1, coord_.z);
    if ((isSameSubstanceAs(adjacent, BlockLayer::Solid) || (adjacent.isOpaque())) && (adjacent.getLowCornerCount()
        == 0))
    {
      solid_hidden.set_front(true);
    }
    if (isSameSubstanceAs(adjacent, BlockLayer::Fluid) || (adjacent.isOpaque()))
    {
      fluid_hidden.set_front(true);
    }
  }

  // Left face...
  if (!stage.atEdgeLeft(coord_))
  {
    StageBlock& adjacent = stage.getBlock(coord_.x - 1, coord_.y, coord_.z);
    if ((isSameSubstanceAs(adjacent, BlockLayer::Solid) || (adjacent.isOpaque())) && (adjacent.getLowCornerCount()
        == 0))
    {
      solid_hidden.set_left(true);
    }
    if (isSameSubstanceAs(adjacent, BlockLayer::Fluid) || (adjacent.isOpaque()))
    {
      fluid_hidden.set_left(true);
    }
  }

  // Right face...
  if (!stage.atEdgeRight(coord_))
  {
    StageBlock& adjacent = stage.getBlock(coord_.x + 1, coord_.y, coord_.z);
    if ((isSameSubstanceAs(adjacent, BlockLayer::Solid) || (adjacent.isOpaque())) && (adjacent.getLowCornerCount()
        == 0))
    {
      solid_hidden.set_right(true);
    }
    if (isSameSubstanceAs(adjacent, BlockLayer::Fluid) || (adjacent.isOpaque()))
    {
      fluid_hidden.set_right(true);
    }
  }

  // Write the new values back.  This also clears the dirty bit.
  hidden_faces_[(unsigned int) BlockLayer::Solid] = solid_hidden;
  hidden_faces_[(unsigned int) BlockLayer::Fluid] = fluid_hidden;
  hidden_faces_dirty_ = false;
}

BlockTopCorners& StageBlock::top_corners()
{
  return top_corners_;
}

StageCoord3 const& StageBlock::getCoordinates() const
{
  return coord_;
}

// === PRIVATE METHODS ========================================================
void StageBlock::invalidateNeighborFaces()
{
  Stage& stage = Stage::getInstance();

  hidden_faces_dirty_ = true;

  if (!stage.atEdgeLeft(coord_))
  {
    stage.getBlock(coord_.x - 1, coord_.y, coord_.z).setFaceDataDirty();
  }
  if (!stage.atEdgeRight(coord_))
  {
    stage.getBlock(coord_.x + 1, coord_.y, coord_.z).setFaceDataDirty();
  }
  if (!stage.atEdgeBack(coord_))
  {
    stage.getBlock(coord_.x, coord_.y - 1, coord_.z).setFaceDataDirty();
  }
  if (!stage.atEdgeFront(coord_))
  {
    stage.getBlock(coord_.x, coord_.y + 1, coord_.z).setFaceDataDirty();
  }
  if (!stage.atEdgeBottom(coord_))
  {
    stage.getBlock(coord_.x, coord_.y, coord_.z - 1).setFaceDataDirty();
  }
  if (!stage.atEdgeTop(coord_))
  {
    stage.getBlock(coord_.x, coord_.y, coord_.z + 1).setFaceDataDirty();
  }
}

} // namespace rectopia
} // namespace gsl
