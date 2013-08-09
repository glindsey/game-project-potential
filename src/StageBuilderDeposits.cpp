// *** ADDED BY HEADER FIXUP ***
#include <algorithm>
#include <cstdlib>
#include <iostream>
// *** END ***
/*
 * StageBuilderDeposits.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: Gregory Lindsey
 */

#include "../include/StageBuilderDeposits.h"

#include "Application.h"
#include "ColumnData.h"
#include "MathUtils.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "Substance.h"

namespace gsl
{
namespace rectopia
{

/// Typedef for a random distribution.
typedef boost::random::uniform_int_distribution<> RandomDistribution;

/// Typedef for a scoped pointer to a random distribution.
typedef std::unique_ptr<RandomDistribution> RDPointer;

enum class PlacerState
{
  Start, LargeDeposits, SmallDeposits, Veins, Solitaires, Gangue, Done
};
struct StageBuilderDeposits::Impl
{
  Impl(Stage& stage, int seed)
    : stage_(stage), seed_(seed)
  {
    Reset();
  }

  void Reset()
  {
    number_remaining_ = 0;  // This gets set later.

    x_distribution_.reset(new RandomDistribution(0, stage_.size().x - 1));
    y_distribution_.reset(new RandomDistribution(0, stage_.size().y - 1));
    z_distribution_.reset(new RandomDistribution(0, stage_.size().z - 1));

    // TODO: Define displacements in Settings instead of hardcoding.
    tiny_displacement_distribution_.reset(new RandomDistribution(-1, 1));
    medium_displacement_distribution_.reset(new RandomDistribution(-2, 2));
    large_displacement_distribution_.reset(new RandomDistribution(-4, 4));
    huge_displacement_distribution_.reset(new RandomDistribution(-8, 8));

    placer_state_ = PlacerState::Start;

    begin_ = true;
  }

  /// Sets a block's substance, after making sure the coordinates are valid.
  void SetSubstance(StageCoord const& x,
                    StageCoord const& y,
                    StageCoord const& z,
                    BlockLayer const& layer,
                    Substance const& substance)
  {
    if (stage_.valid_coordinates(x, y, z))
    {
      stage_.get_block(x, y, z).set_substance(layer, substance);
    }
  }

  void DrawLargeBlob(StageCoord3 const& coord,
                     BlockLayer const& layer,
                     Substance const& substance)
  {
    /// Create a large-deposit blob.
    /// The blob created looks like the following:
    /// <pre>
    ///  ..........  ..[][][]..  ..........
    ///  ....[]....  [][][][][]  ....[]....
    ///  ..[][][]..  [][][][][]  ..[][][]..
    ///  ....[]....  [][][][][]  ....[]....
    ///  ..........  ..[][][]..  ..........
    ///      -1          0           +1
    /// </pre>

    StageCoord3 offsets[] =
    {
      { 0, -1, -1 },
      { -1, 0, -1 },
      { 0, 0, -1 },
      { 1, 0, -1 },
      { 0, 1, -1 },
      { -1, -2, 0 },
      { 0, -2, 0 },
      { 1, -2, 0 },
      { -2, -1, 0 },
      { -1, -1, 0 },
      { 0, -1, 0 },
      { 1, -1, 0 },
      { 2, -1, 0 },
      { -2, 0, 0 },
      { -1, 0, 0 },
      { 0, 0, 0 },
      { 1, 0, 0 },
      { 2, 0, 0 },
      { -2, 1, 0 },
      { -1, 1, 0 },
      { 0, 1, 0 },
      { 1, 1, 0 },
      { 2, 1, 0 },
      { -1, 2, 0 },
      { 0, 2, 0 },
      { 1, 2, 0 },
      { 0, -1, 1 },
      { -1, 0, 1 },
      { 0, 0, 1 },
      { 1, 0, 1 },
      { 0, 1, 1 }
    };

    for (StageCoord3 & offset : offsets)
    {
      SetSubstance(coord.x + offset.x,
                   coord.y + offset.y,
                   coord.z + offset.z,
                   layer, substance);
    }
  }

  void DrawSmallBlob(StageCoord3 const& coord,
                     BlockLayer const& layer,
                     Substance const& substance)
  {
    /// Create a small-deposit blob.
    /// The blob created looks like the following:
    /// <pre>
    ///  ..........  ..........  ..........
    ///  ..........  ....[]....  ..........
    ///  ....[]....  ..[][][]..  ....[]....
    ///  ..........  ....[]....  ..........
    ///  ..........  ..........  ..........
    ///      -1          0           +1
    /// </pre>

    StageCoord3 offsets[] =
    {
      { 0, 0, -1 },
      { 0, -1, 0 },
      { -1, 0, 0 },
      { 0, 0, 0 },
      { 1, 0, 0 },
      { 0, 1, 0 },
      { 0, 0, 1 }
    };

    for (StageCoord3 & offset : offsets)
    {
      SetSubstance(coord.x + offset.x,
                   coord.y + offset.y,
                   coord.z + offset.z,
                   layer, substance);
    }
  }

  /// This function uses a 3-D version of Bresenham's line algorithm to draw
  /// a line of small blobs, creating a vein.
  /// @todo Perhaps make this a series of lines, or a Bezier curve.
  void DrawVein(StageCoord3 const& src,
                StageCoord3 const& dst,
                Substance const& substance)
  {
    StageCoord3 d(dst.x - src.x, dst.y - src.y, dst.z - src.z);
    StageCoord3 a(abs(d.x) * 2, abs(d.y) * 2, abs(d.z) * 2);
    StageCoord3 s(sgn(d.x), sgn(d.y), sgn(d.z));
    StageCoord3 coord(src.x, src.y, src.z);

    if (a.x >= std::max(a.y, a.z))    // X dominant
    {
      StageCoord yd = a.y - (a.x / 2);
      StageCoord zd = a.z - (a.x / 2);

      do
      {
        DrawSmallBlob(coord, BlockLayer::Solid, substance);

        if (yd >= 0)        // Move along Y
        {
          coord.y += s.y;
          yd = yd - a.x;
        }

        if (zd >= 0)    // Move along Z
        {
          coord.z += s.z;
          zd = zd - a.x;
        }

        coord.x += s.x;   // Move along X
        yd = yd + a.y;
        zd = zd + a.z;
      }
      while (coord.x != dst.x);
    }
    else if (a.y >= std::max(a.x, a.z))  // Y dominant
    {
      int xd = a.x - (a.y / 2);
      int zd = a.z - (a.y / 2);

      do
      {
        DrawSmallBlob(coord, BlockLayer::Solid, substance);

        if (xd >= 0)    // Move along X
        {
          coord.x += s.x;
          xd = xd - a.y;
        }

        if (zd >= 0)    // Move along Z
        {
          coord.z += s.z;
          zd = zd - a.y;
        }

        coord.y += s.y;   // Move along y
        xd = xd + a.x;
        zd = zd + a.z;
      }
      while (coord.y != dst.y);
    }
    else if (a.z >= std::max(a.x, a.y))  // Z dominant
    {
      int xd = a.x - (a.z / 2);
      int yd = a.y - (a.z / 2);

      do
      {
        DrawSmallBlob(coord, BlockLayer::Solid, substance);

        if (coord.z == dst.z)
        {
          break;
        }

        if (xd >= 0)    // Move along X
        {
          coord.x += s.x;
          xd = xd - a.z;
        }

        if (yd >= 0)    // Move along Y
        {
          coord.y += s.y;
          yd = yd - a.z;
        }

        coord.z += s.z;   // move along Z
        xd = xd + a.x;
        yd = yd + a.y;
      }
      while (coord.z != dst.z);
    }
  }

  /// Reference to the stage.
  Stage& stage_;

  /// Seed used for the RNG.
  int seed_;

  /// State that the placer state machine is in.
  PlacerState placer_state_;

  /// Returns a random number within a given distribution.
  int get_random(RDPointer& distribution)
  {
    RandomDistribution& dist = *(distribution.get());
    return dist(App::instance().twister());
  }

  /// Number of the current feature left to place.
  int number_remaining_;

  /// Random distribution across X axis.
  std::unique_ptr<RandomDistribution> x_distribution_;

  /// Random distribution across Y axis.
  std::unique_ptr<RandomDistribution> y_distribution_;

  /// Random distribution across Z axis.
  std::unique_ptr<RandomDistribution> z_distribution_;

  /// Tiny displacement distribution.
  std::unique_ptr<RandomDistribution> tiny_displacement_distribution_;

  /// Medium displacement distribution.
  std::unique_ptr<RandomDistribution> medium_displacement_distribution_;

  /// Large displacement distribution.
  std::unique_ptr<RandomDistribution> large_displacement_distribution_;

  /// Huge displacement distribution.
  std::unique_ptr<RandomDistribution> huge_displacement_distribution_;

  bool begin_;
};

StageBuilderDeposits::StageBuilderDeposits(Stage& stage, int seed)
  : impl(new Impl(stage, seed))
{
  // TODO Auto-generated constructor stub

}

StageBuilderDeposits::~StageBuilderDeposits()
{
  // TODO Auto-generated destructor stub
}

bool StageBuilderDeposits::Build()
{
  static StageCoord3 stage_size = impl->stage_.size();
  static StageCoord3 max_vector = StageCoord3(stage_size.x - 1,
                                              stage_size.y - 1,
                                              stage_size.z - 1);
  static StageCoord3 zero_vector = StageCoord3(0);
  static int total_block_count = (int)stage_size.x *
                                 (int)stage_size.y *
                                 (int)stage_size.z;

  if (impl->begin_)
  {
    std::cout << "Adding mineral deposits..." << std::endl;
    impl->begin_ = false;
  }

  switch (impl->placer_state_)
  {
  case PlacerState::Start:
    std::cout << "-- Placing large deposits..." << std::endl;
    impl->placer_state_ = PlacerState::LargeDeposits;
    impl->number_remaining_ = (total_block_count
                               * Settings::terrainLargeDepositDensity)
                              >> 18;
    break;

  case PlacerState::LargeDeposits:
    if (impl->number_remaining_ != 0)
    {
      StageCoord3 random = StageCoord3(
                             impl->get_random(impl->x_distribution_),
                             impl->get_random(impl->y_distribution_),
                             impl->get_random(impl->z_distribution_));
      StageBlock& chosen_block = impl->stage_.get_block(random.x,
                                                       random.y,
                                                       random.z);
      const Substance& substance = chosen_block.get_substance(BlockLayer::Solid);

      // Make sure the randomly selected block's substance can contain large deposits.
      if (substance.largeDeposits.size() != 0)
      {

        // Create a distribution to choose one of the substances included.
        boost::random::uniform_int_distribution<> substance_distribution(
          0, substance.largeDeposits.size() - 1);
        const Substance& deposit_substance =
          *(substance.largeDeposits[substance_distribution(
                                      App::instance().twister())]);

        // Create the deposit out of 8 slightly-displaced large blobs.
        // TODO: make the size of a deposit customizable?
        for (int blob_pieces = 0; blob_pieces < 8; ++blob_pieces)
        {
          StageCoord3 coord = random;
          StageCoord3 try_coord = coord;
          do
          {
            StageCoord3 offset = StageCoord3(
                                   impl->get_random(impl->medium_displacement_distribution_),
                                   impl->get_random(impl->medium_displacement_distribution_),
                                   impl->get_random(impl->tiny_displacement_distribution_));
            try_coord.x = coord.x + offset.x;
            try_coord.y = coord.y + offset.y;
            try_coord.z = coord.z + offset.z;
            try_coord = constrain_to_box(zero_vector, try_coord, max_vector);
          }
          while (!impl->stage_.get_block(try_coord.x,
                                        try_coord.y,
                                        try_coord.z).is_solid());

          impl->DrawLargeBlob(coord, BlockLayer::Solid, deposit_substance);
        }

        --(impl->number_remaining_);
      }
      else
      {
        // TODO: substance has no large deposit types, increment "give up" counter
      }

    }
    else
    {
      std::cout << "-- Placing small deposits..." << std::endl;
      impl->placer_state_ = PlacerState::SmallDeposits;
      impl->number_remaining_ = (total_block_count
                                 * Settings::terrainSmallDepositDensity)
                                >> 18;
    }
    break;

  case PlacerState::SmallDeposits:
    if (impl->number_remaining_ != 0)
    {
      StageCoord3 random = StageCoord3(
                             impl->get_random(impl->x_distribution_),
                             impl->get_random(impl->y_distribution_),
                             impl->get_random(impl->z_distribution_));
      StageBlock& chosen_block = impl->stage_.get_block(random.x, random.y, random.z);
      const Substance& substance = chosen_block.get_substance(BlockLayer::Solid);

      // Make sure the randomly selected block's substance can contain small deposits.
      if (substance.smallDeposits.size() != 0)
      {

        // Create a distribution to choose one of the substances included.
        boost::random::uniform_int_distribution<> substance_distribution(
          0, substance.smallDeposits.size() - 1);
        const Substance& deposit_substance =
          *(substance.smallDeposits[substance_distribution(
                                      App::instance().twister())]);

        // Create the deposit out of 4 slightly-displaced small blobs.
        // TODO: make the size of a deposit customizable?
        for (int blob_pieces = 0; blob_pieces < 8; ++blob_pieces)
        {
          StageCoord3 coord = random;
          StageCoord3 try_coord = coord;
          do
          {
            StageCoord3 offset = StageCoord3(
                                   impl->get_random(impl->tiny_displacement_distribution_),
                                   impl->get_random(impl->tiny_displacement_distribution_),
                                   impl->get_random(impl->tiny_displacement_distribution_));
            StageCoord3 try_coord;
            try_coord.x = coord.x + offset.x;
            try_coord.y = coord.y + offset.y;
            try_coord.z = coord.z + offset.z;
            try_coord = constrain_to_box(zero_vector, try_coord, max_vector);
          }
          while (!impl->stage_.get_block(try_coord.x,
                                        try_coord.y,
                                        try_coord.z).is_solid());

          impl->DrawSmallBlob(coord, BlockLayer::Solid, deposit_substance);
        }

        --(impl->number_remaining_);
      }
      else
      {
        // TODO: substance has no small deposit types, increment "give up" counter
      }
    }
    else
    {
      std::cout << "-- Placing veins..." << std::endl;
      impl->placer_state_ = PlacerState::Veins;
      impl->number_remaining_ = (total_block_count
                                 * Settings::terrainVeinDensity)
                                >> 18;

    }
    break;

  case PlacerState::Veins:
    if (impl->number_remaining_ != 0)
    {
      StageCoord3 random = StageCoord3(
                             impl->get_random(impl->x_distribution_),
                             impl->get_random(impl->y_distribution_),
                             impl->get_random(impl->z_distribution_));
      StageBlock& chosen_block = impl->stage_.get_block(random.x,
                                                       random.y,
                                                       random.z);
      const Substance& substance = chosen_block.get_substance(BlockLayer::Solid);

      // Make sure the randomly selected block's substance can contain veins.
      if (substance.veinDeposits.size() != 0)
      {

        // Create a distribution to choose one of the substances included.
        boost::random::uniform_int_distribution<> substance_distribution(
          0, substance.veinDeposits.size() - 1);
        const Substance& deposit_substance =
          *(substance.veinDeposits[substance_distribution(
                                     App::instance().twister())]);

        // Figure out the end of the vein.  We don't do any checking for
        // the endpoint right now except to make sure it is solid.
        StageCoord3 dest = random;
        StageCoord3 try_dest = dest;
        do
        {
          StageCoord3 offset = StageCoord3(
                                 impl->get_random(impl->huge_displacement_distribution_),
                                 impl->get_random(impl->huge_displacement_distribution_),
                                 impl->get_random(impl->huge_displacement_distribution_));

          try_dest.x = dest.x + offset.x;
          try_dest.y = dest.y + offset.y;
          try_dest.z = dest.z + offset.z;
          try_dest = constrain_to_box(zero_vector, try_dest, max_vector);
        }
        while (!impl->stage_.get_block(try_dest.x, try_dest.y, try_dest.z).is_solid());

        impl->DrawVein(random, try_dest, deposit_substance);

        --(impl->number_remaining_);
      }
      else
      {
        // TODO: substance has no vein deposit types, increment "give up" counter
      }
    }
    else
    {
      std::cout << "-- Placing solitaires..." << std::endl;
      impl->placer_state_ = PlacerState::Solitaires;
      impl->number_remaining_ = (total_block_count
                                 * Settings::terrainSingleDensity)
                                >> 18;

    }
    break;

  case PlacerState::Solitaires:
    if (impl->number_remaining_ != 0)
    {
      StageCoord3 random = StageCoord3(
                             impl->get_random(impl->x_distribution_),
                             impl->get_random(impl->y_distribution_),
                             impl->get_random(impl->z_distribution_));
      StageBlock& chosen_block = impl->stage_.get_block(random.x,
                                                       random.y,
                                                       random.z);
      const Substance& substance = chosen_block.get_substance(BlockLayer::Solid);

      // Make sure the randomly selected block's substance can contain solitaires.
      if (substance.singleDeposits.size() != 0)
      {

        // Create a distribution to choose one of the substances included.
        boost::random::uniform_int_distribution<> substance_distribution(
          0, substance.singleDeposits.size() - 1);
        const Substance& deposit_substance =
          *(substance.singleDeposits[substance_distribution(
                                       App::instance().twister())]);

        impl->SetSubstance(random.x, random.y, random.z,
                           BlockLayer::Solid, deposit_substance);

        --(impl->number_remaining_);
      }
      else
      {
        // TODO: substance has no solitaire deposit types, increment "give up" counter
      }
    }
    else
    {
      // TODO: implement gangue materials
      impl->number_remaining_ = 0;
      std::cout << "-- (Placing gangue -- not yet implemented)..." << std::endl;
      impl->placer_state_ = PlacerState::Gangue;
      return false;
    }
    break;

  case PlacerState::Gangue:
    if (impl->number_remaining_ != 0)
    {
      // TODO: place gangue
      --(impl->number_remaining_);
    }
    else
    {
      std::cout << "-- Done!" << std::endl;
      impl->placer_state_ = PlacerState::Done;
      return true;
    }
    break;

  default:
    return true;
  }

  return false;
}

void StageBuilderDeposits::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
