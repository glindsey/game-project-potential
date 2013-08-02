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
    : stage_(stage), seed_(seed), sea_level_(sea_level)
  {
    Reset();
  }

  void Reset()
  {

    begin_ = true;
  }

  /// Carve out a channel.  This method is not perfect.  It is slow, and it
  /// will probably start making some gaps in the ball if the radius gets really
  /// large.
  void CarveChannel(sf::Vector3f center,
                    float radius,
                    int max_z_level,
                    Substance const& substance)
  {
    static StageCoord3 stage_size = stage_.size();

    for (float r = 0.5; r <= radius; r += 0.5)
    {
      for (float theta = 0;
                 theta < PI;
                 theta += (PI / (2 * r * r)))
      {
        // Do the half-shell of water at the bottom of the channel.
        for (float phi = (PI / 2);
                   phi < ((3 * PI) / 2);
                   phi += (PI / (2 * r * r)))
        {
          glm::vec3 offset = SphericalToCubical(r, theta, phi);

          StageCoord3 coord;
          StageCoord3 stage_size = stage_.size();
          coord.x = (int)(center.x + offset.x);
          coord.y = (int)(center.y + offset.y);
          coord.z = (int)(center.z + offset.z);

          if ((coord.x >= 0) && (coord.y >= 0) && (coord.z >= 0) &&
              (coord.x < stage_size.x) &&
              (coord.y < stage_size.y) &&
              (coord.z < stage_size.z))
          {
            StageBlock& get_block = stage_.get_block(coord.x, coord.y, coord.z);
            get_block.set_substance(BlockLayer::Fluid,
                                    substance);
            get_block.set_substance(BlockLayer::Solid,
                                    Substance::get("nothing"));
          }
        }
      }

      // Carve out the rest of the channel.
      for (int z = max_z_level; z >= center.z; --z)
      {
        for (float theta = 0; theta < 2*PI; theta += (PI / (2 * r * r)))
        {
          glm::vec2 offset = PolarToRectangular(r, theta);
          StageCoord3 coord;
          coord.x = (int)(center.x + offset.x);
          coord.y = (int)(center.y + offset.y);
          coord.z = z;

          if ((coord.x >= 0) && (coord.y >= 0) && (coord.z >= 0) &&
              (coord.x < stage_size.x) &&
              (coord.y < stage_size.y) &&
              (coord.z < stage_size.z))
          {
            StageBlock& get_block = stage_.get_block(coord.x, coord.y, coord.z);
            get_block.set_substance(BlockLayer::Solid,
                                    Substance::get("nothing"));
          }

        }
      }
    }
  }

  bool begin_;

  /// State that the builder is currently in.
  BuilderState state_;

  /// Reference to the stage.
  Stage& stage_;

  /// Seed used for the RNG.
  int seed_;

  /// Sea level for the state.
  StageCoord sea_level_;

  /// Points that make up the river.
  boost::ptr_vector<RiverPoint> river_;

  /// Initial river direction.
  double river_direction_;

  /// Initial river width.
  double river_width_;

  /// Number of the next river point.
  int next_river_point_;

  /// River origin -- origin of the river on the map.
  StageCoord2 river_origin_;

  /// River seed point -- indicates where in the Perlin noise we start the river.
  sf::Vector3<double> river_seed_;

  /// How much we update the Perlin coordinates by for each additional river point.
  double river_coarseness_;

  /// How wiggly the river is, in radians. 0 would indicate a completely straight line (no curves),
  /// while PI would indicate the river could turn completely back on itself.
  double river_wiggliness_;

  /// Perlin noise used to make the river flow.
  noise::module::Perlin perlin_noise_;

  /// Scaler module for the Perlin noise.
  noise::module::ScaleBias scale_bias_;

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
  static StageCoord3 stage_size = impl->stage_.size();


  // DEBUG CODE -- temporarily disable this builder
  return true;
  // END DEBUG CODE

  if (impl->begin_)
  {
    std::cout << "Adding rivers..." << std::endl;

    impl->state_ = BuilderState::Begin;
    impl->river_.clear();
    impl->begin_ = false;
  }

  switch (impl->state_)
  {
  case BuilderState::Begin:
  {
    impl->state_ = BuilderState::FlowRivers;
    break;
  }

  case BuilderState::FlowRivers:
  {
    if (impl->river_.size() == 0)
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
        impl->river_origin_.x = x;
        impl->river_origin_.y = 0;
      }
      break;

      case 1:
      {
        // Start on the left.
        boost::random::uniform_int_distribution<> y_selection(
          0, stage_size.y - 1);
        int y = y_selection(App::instance().twister());
        impl->river_origin_.x = 0;
        impl->river_origin_.y = y;
      }
      break;
      case 2:
      {
        // Start at the front.
        boost::random::uniform_int_distribution<> x_selection(
          0, stage_size.x - 1);
        int x = x_selection(App::instance().twister());
        impl->river_origin_.x = x;
        impl->river_origin_.y = stage_size.y - 1;
      }
      break;
      case 3:
      {
        // Start at the right.
        boost::random::uniform_int_distribution<> y_selection(
          0, stage_size.y - 1);
        int y = y_selection(App::instance().twister());
        impl->river_origin_.x = stage_size.x - 1;
        impl->river_origin_.y = y;
      }
      break;
      }

      // Pick a random seed point to get our river's Perlin noise from.
      boost::random::uniform_int_distribution<> seed_selection(-1000, 1000);
      impl->river_seed_.x = seed_selection(App::instance().twister());
      impl->river_seed_.y = seed_selection(App::instance().twister());
      impl->river_seed_.z = seed_selection(App::instance().twister());

      // How fast are we going to change our Perlin noise values?
      impl->river_coarseness_ = 0.005f; // TODO: no magic numbers

      // How wiggly can the river get?
      impl->river_wiggliness_ = PI * 0.66667f;

      // Set up the Perlin noise.
      // TODO: no magic numbers
      impl->perlin_noise_.SetSeed(impl->seed_);
      impl->perlin_noise_.SetNoiseQuality(noise::NoiseQuality::QUALITY_BEST);
      impl->perlin_noise_.SetFrequency(1);
      impl->perlin_noise_.SetOctaveCount(3);
      impl->perlin_noise_.SetPersistence(0.5f);
      impl->perlin_noise_.SetLacunarity(2);

      // Set up the scaler.
      impl->scale_bias_.SetSourceModule(0, impl->perlin_noise_);
      impl->scale_bias_.SetBias(0);
      impl->scale_bias_.SetScale(impl->river_wiggliness_);

      // Figure out river's initial direction and size based on coordinates.
      sf::Vector2f stage_center = sf::Vector2f((float) stage_size.x / 2,
                                  (float) stage_size.y / 2);
      impl->river_direction_ = atan2(
                                 stage_center.y - impl->river_origin_.y,
                                 stage_center.x - impl->river_origin_.x);
      impl->river_width_ = 5; // TODO: no magic numbers

      //std::cout << "DEBUG: First river coord is (" << impl->river_origin_.x << ", " << impl->river_origin_.y << ")" << std::endl;
      impl->river_.push_back(
        new RiverPoint(impl->river_origin_.x, impl->river_origin_.y,
                       impl->river_direction_, impl->river_width_));

      impl->next_river_point_ = 1;
    }
    else
    {
      // Keep the river going.
      sf::Vector3<double> next_perlin_coord;
      next_perlin_coord.x = impl->river_seed_.x
                            + (impl->river_coarseness_ *
                               (double)impl->next_river_point_);
      next_perlin_coord.y = impl->river_seed_.y;
      next_perlin_coord.z = impl->river_seed_.z;

      // Find the next river heading.
      double next_perlin_value = impl->scale_bias_.GetValue(
                                   next_perlin_coord.x,
                                   next_perlin_coord.y,
                                   next_perlin_coord.z);

      double next_direction = impl->river_direction_ + next_perlin_value;

      // Calculate the next river coordinate.
      sf::Vector2<double> next_coord;
      next_coord.x = impl->river_[impl->next_river_point_ - 1].coord.x;
      next_coord.y = impl->river_[impl->next_river_point_ - 1].coord.y;

      next_coord.x += (0.5f * cos(next_direction));
      next_coord.y += (0.5f * sin(next_direction));
      //std::cout << "DEBUG: Next river coord is (" << next_coord.x << ", " << next_coord.y << ")" << std::endl;

      // TODO: vary the size as well
      double next_size = impl->river_[impl->next_river_point_ - 1].size;

      // Check X, Y coordinates against edges.
      if ((next_coord.x < 0) || (next_coord.y < 0)
          || (next_coord.x > stage_size.x - 1)
          || (next_coord.y > stage_size.y - 1))
      {
        // TODO: We're done, now we have to determine river heights.
        impl->state_ = BuilderState::SetHeightsFirstPass;
        impl->next_river_point_ = 0;
      }
      else
      {
        // Not at an edge yet, so push this last point back and increment the number.
        impl->river_.push_back(
          new RiverPoint(next_coord.x, next_coord.y, next_direction,
                         next_size));
        ++(impl->next_river_point_);
      }
    }

    break;
  }

  case BuilderState::SetHeightsFirstPass:
  {
    static int last_height = std::numeric_limits<int>::max();

    if (impl->next_river_point_ < (int)impl->river_.size())
    {
      RiverPoint& point_ = impl->river_[impl->next_river_point_];
      int height_ = impl->stage_.getColumnSolidHeight(point_.coord.x,
                                                      point_.coord.y) - 1;

      if (height_ >= last_height)
      {
        height_ = last_height;
      }
      point_.height = height_;
      last_height = height_;

      ++(impl->next_river_point_);
    }
    else
    {
      impl->state_ = BuilderState::SetHeightsSecondPass;
      impl->next_river_point_ = impl->river_.size() - 1;
    }
    break;
  }

  case BuilderState::SetHeightsSecondPass:
  {
    if (impl->next_river_point_ >= 0)
    {
      //RiverPoint& point_ = impl->river_[impl->next_river_point_];

      // TODO: Backtrack along the river.  If we see the height go up, mark the
      //       first point that it does so and the height it was at.
      //       Once we see the height STOP going up, check the overall slope of
      //       the increase.  If greater than 30 degrees, set all points in
      //       between to the initial height, creating a waterfall.
      //point_.height = height_;

      --(impl->next_river_point_);
    }
    else
    {
      impl->state_ = BuilderState::RasterizeRivers;
      impl->next_river_point_ = 0;
    }
    break;
  }
  case BuilderState::RasterizeRivers:
  {
    for (unsigned int point = 0; point < impl->river_.size(); ++point)
    {
      sf::Vector3i coord(impl->river_[point].coord.x,
                         impl->river_[point].coord.y,
                         impl->river_[point].height);
      int max_height = impl->stage_.getColumnSolidHeight(coord.x, coord.y);

      sf::Vector3f float_coord = sf::Vector3f(coord.x, coord.y, coord.z);

      // TODO: size that changes
      impl->CarveChannel(float_coord, 3.0f,
                         max_height, Substance::get("freshwater"));
    }

    impl->state_ = BuilderState::Done;
    impl->next_river_point_ = 0;
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
