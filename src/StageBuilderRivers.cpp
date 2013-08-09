// *** ADDED BY HEADER FIXUP ***
#include <iostream>
// *** END ***
/*
 * StageBuilderRivers.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: Gregory Lindsey
 */

#include "StageBuilderRivers.h"

#include "Application.h"
#include "ColumnData.h"
#include "ErrorMacros.h"
#include "MathUtils.h"
#include "Settings.h"
#include "Stage.h"
#include "StageBlock.h"
#include "StageChunk.h"
#include "Substance.h"

#include <noise/noise.h>

namespace gsl
{
namespace rectopia
{

enum class BuilderState
{
  Begin,
  FlowRivers,
  SetHeightsFirstPass,
  SetHeightsSecondPass,
  RasterizeRivers,
  Done
};

struct RiverPoint
{
  RiverPoint(double x, double y, double d, double s)
  {
    coord.x = x;
    coord.y = y;
    height = 0;
    direction = d;
    size = s;
  }

  sf::Vector2<double> coord;
  int height;
  double direction;
  double size;
};

struct StageBuilderRivers::Impl
{
  Impl(Stage& stage, int seed, StageCoord sea_level)
    : stage(stage), seed(seed), sea_level(sea_level)
  {
    Reset();
  }

  void Reset()
  {

    begin = true;
  }

  /// Carve out a channel.  In order to save time, we just carve out a box.
  /// The algorithm originally carved out a sphere, but I realized that's a
  /// ton of work for something that will only look marginally better.
  void CarveChannel(sf::Vector3f center,
                    float radius,
                    int max_z_level,
                    Substance const& substance)
  {
    static StageCoord3 stage_size = stage.size();

    float z_min = center.z - radius;

    for (float x_offset = -radius; x_offset <= radius; x_offset += 0.5)
    {
      for (float y_offset = -radius; y_offset <= radius; y_offset += 0.5)
      {
        for (float z_level = max_z_level; z_level >= z_min; z_level -= 1)
        {
          StageCoord3 coord;
          coord.x = (int)(center.x + x_offset);
          coord.y = (int)(center.y + y_offset);
          coord.z = (int)(z_level);

          if ((coord.x >= 0) && (coord.y >= 0) && (coord.z >= 0) &&
              (coord.x < stage_size.x) &&
              (coord.y < stage_size.y) &&
              (coord.z < stage_size.z))
          {
            StageBlock& get_block = stage.get_block(coord.x, coord.y, coord.z);

            if (coord.z <= center.z)
            {
              get_block.set_substance(BlockLayer::Fluid,
                                      substance);
            }
          }
        }
      }
    }
  }

  bool begin;

  /// State that the builder is currently in.
  BuilderState state;

  /// Reference to the stage.
  Stage& stage;

  /// Seed used for the RNG.
  int seed;

  /// Sea level for the state.
  StageCoord sea_level;

  /// Points that make up the river.
  boost::ptr_vector<RiverPoint> river;

  /// Initial river direction.
  double river_direction;

  /// Initial river width.
  double river_width;

  /// Number of the next river point.
  int next_river_point;

  /// River origin -- origin of the river on the map.
  StageCoord2 river_origin;

  /// River seed point -- indicates where in the Perlin noise we start the river.
  sf::Vector3<double> river_seed;

  /// How much we update the Perlin coordinates by for each additional river point.
  double river_coarseness;

  /// How wiggly the river is, in radians. 0 would indicate a completely straight line (no curves),
  /// while PI would indicate the river could turn completely back on itself.
  double river_wiggliness;

  /// Perlin noise used to make the river flow.
  noise::module::Perlin perlin_noise;

  /// Scaler module for the Perlin noise.
  noise::module::ScaleBias scale_bias;

};

StageBuilderRivers::StageBuilderRivers(Stage& stage, int seed, StageCoord sea_level)
  : impl(new Impl(stage, seed, sea_level))
{
}

StageBuilderRivers::~StageBuilderRivers()
{
}

bool StageBuilderRivers::Build()
{
  static StageCoord3 stage_size = impl->stage.size();


  // DEBUG CODE -- temporarily disable this builder
  return true;
  // END DEBUG CODE

  if (impl->begin)
  {
    std::cout << "Adding rivers..." << std::endl;

    impl->state = BuilderState::Begin;
    impl->river.clear();
    impl->begin = false;
  }

  switch (impl->state)
  {
  case BuilderState::Begin:
  {
    impl->state = BuilderState::FlowRivers;
    break;
  }

  case BuilderState::FlowRivers:
  {
    if (impl->river.size() == 0)
    {
      // Start the river.
      // Figure out which edge we will start at.
      boost::random::uniform_int_distribution<> edge_selection(0, 3);
      int edge_choice = edge_selection(App::instance().twister());
      switch (edge_choice)
      {
      case 0:
      {
        // Start at the back.
        boost::random::uniform_int_distribution<> x_selection(
          0, stage_size.x - 1);
        int x = x_selection(App::instance().twister());
        impl->river_origin.x = x;
        impl->river_origin.y = 0;
      }
      break;

      case 1:
      {
        // Start on the left.
        boost::random::uniform_int_distribution<> y_selection(
          0, stage_size.y - 1);
        int y = y_selection(App::instance().twister());
        impl->river_origin.x = 0;
        impl->river_origin.y = y;
      }
      break;
      case 2:
      {
        // Start at the front.
        boost::random::uniform_int_distribution<> x_selection(
          0, stage_size.x - 1);
        int x = x_selection(App::instance().twister());
        impl->river_origin.x = x;
        impl->river_origin.y = stage_size.y - 1;
      }
      break;
      case 3:
      {
        // Start at the right.
        boost::random::uniform_int_distribution<> y_selection(
          0, stage_size.y - 1);
        int y = y_selection(App::instance().twister());
        impl->river_origin.x = stage_size.x - 1;
        impl->river_origin.y = y;
      }
      break;
      }

      // Pick a random seed point to get our river's Perlin noise from.
      boost::random::uniform_int_distribution<> seed_selection(-1000, 1000);
      impl->river_seed.x = seed_selection(App::instance().twister());
      impl->river_seed.y = seed_selection(App::instance().twister());
      impl->river_seed.z = seed_selection(App::instance().twister());

      // How fast are we going to change our Perlin noise values?
      impl->river_coarseness = 0.005f; // TODO: no magic numbers

      // How wiggly can the river get?
      impl->river_wiggliness = PI * 0.66667f;

      // Set up the Perlin noise.
      // TODO: no magic numbers
      impl->perlin_noise.SetSeed(impl->seed);
      impl->perlin_noise.SetNoiseQuality(noise::NoiseQuality::QUALITY_BEST);
      impl->perlin_noise.SetFrequency(1);
      impl->perlin_noise.SetOctaveCount(3);
      impl->perlin_noise.SetPersistence(0.5f);
      impl->perlin_noise.SetLacunarity(2);

      // Set up the scaler.
      impl->scale_bias.SetSourceModule(0, impl->perlin_noise);
      impl->scale_bias.SetBias(0);
      impl->scale_bias.SetScale(impl->river_wiggliness);

      // Figure out river's initial direction and size based on coordinates.
      sf::Vector2f stage_center = sf::Vector2f((float) stage_size.x / 2,
                                  (float) stage_size.y / 2);
      impl->river_direction = atan2(
                                 stage_center.y - impl->river_origin.y,
                                 stage_center.x - impl->river_origin.x);
      impl->river_width = 5; // TODO: no magic numbers

      DEEP_TRACE("First river coord is (%f, %f)",
                 impl->river_origin.x,
                 impl->river_origin.y);

      impl->river.push_back(
        new RiverPoint(impl->river_origin.x, impl->river_origin.y,
                       impl->river_direction, impl->river_width));

      impl->next_river_point = 1;
    }
    else
    {
      // Keep the river going.
      sf::Vector3<double> next_perlin_coord;
      next_perlin_coord.x = impl->river_seed.x
                            + (impl->river_coarseness *
                               (double)impl->next_river_point);
      next_perlin_coord.y = impl->river_seed.y;
      next_perlin_coord.z = impl->river_seed.z;

      // Find the next river heading.
      double next_perlin_value = impl->scale_bias.GetValue(
                                   next_perlin_coord.x,
                                   next_perlin_coord.y,
                                   next_perlin_coord.z);

      double next_direction = impl->river_direction + next_perlin_value;

      // Calculate the next river coordinate.
      sf::Vector2<double> next_coord;
      next_coord.x = impl->river[impl->next_river_point - 1].coord.x;
      next_coord.y = impl->river[impl->next_river_point - 1].coord.y;

      next_coord.x += (0.5f * cos(next_direction));
      next_coord.y += (0.5f * sin(next_direction));

      DEEP_TRACE("Next river coord is (%f, %f)",
                 next_coord.x, next_coord.y);

      // TODO: vary the size as well
      double next_size = impl->river[impl->next_river_point - 1].size;

      // Check X, Y coordinates against edges.
      if ((next_coord.x < 0) || (next_coord.y < 0)
          || (next_coord.x > stage_size.x - 1)
          || (next_coord.y > stage_size.y - 1))
      {
        // TODO: We're done, now we have to determine river heights.
        impl->state = BuilderState::SetHeightsFirstPass;
        impl->next_river_point = 0;
      }
      else
      {
        // Not at an edge yet, so push this last point back and increment the number.
        impl->river.push_back(
          new RiverPoint(next_coord.x, next_coord.y, next_direction,
                         next_size));
        ++(impl->next_river_point);
      }
    }

    break;
  }

  case BuilderState::SetHeightsFirstPass:
  {
    static int last_height = std::numeric_limits<int>::max();

    if (impl->next_river_point < (int)impl->river.size())
    {
      RiverPoint& point_ = impl->river[impl->next_river_point];
      int height_ = impl->stage.get_column_solid_height(point_.coord.x,
                                                      point_.coord.y) - 1;

      if (height_ >= last_height)
      {
        height_ = last_height;
      }
      point_.height = height_;
      last_height = height_;

      ++(impl->next_river_point);
    }
    else
    {
      impl->state = BuilderState::SetHeightsSecondPass;
      impl->next_river_point = impl->river.size() - 1;
    }
    break;
  }

  case BuilderState::SetHeightsSecondPass:
  {
    if (impl->next_river_point >= 0)
    {
      //RiverPoint& point_ = impl->river[impl->next_river_point];

      // TODO: Backtrack along the river.  If we see the height go up, mark the
      //       first point that it does so and the height it was at.
      //       Once we see the height STOP going up, check the overall slope of
      //       the increase.  If greater than 30 degrees, set all points in
      //       between to the initial height, creating a waterfall.
      //point_.height = height_;

      --(impl->next_river_point);
    }
    else
    {
      impl->state = BuilderState::RasterizeRivers;
      impl->next_river_point = 0;
    }
    break;
  }
  case BuilderState::RasterizeRivers:
  {
    for (unsigned int point = 0; point < impl->river.size(); ++point)
    {
      sf::Vector3i coord(impl->river[point].coord.x,
                         impl->river[point].coord.y,
                         impl->river[point].height);
      int max_height = impl->stage.get_column_solid_height(coord.x, coord.y);

      sf::Vector3f float_coord = sf::Vector3f(coord.x, coord.y, coord.z);

      // TODO: size that changes
      impl->CarveChannel(float_coord, 3.0f,
                         max_height, Substance::get("freshwater"));
    }

    impl->state = BuilderState::Done;
    impl->next_river_point = 0;
    break;
  }

  case BuilderState::Done:
    return true;
  }

  return false;
}

void StageBuilderRivers::Reset()
{
  impl->Reset();
}

} /* namespace rectopia */
} /* namespace gsl */
