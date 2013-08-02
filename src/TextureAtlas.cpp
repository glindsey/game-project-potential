#include <iostream>

#include "TextureAtlas.h"

#include "ErrorMacros.h"
#include "Settings.h"

namespace gsl
{
namespace rectopia
{

std::unique_ptr<TextureAtlas> TextureAtlas::instance_;

TextureAtlas* TextureAtlas::instance()
{
  if (instance_.get() == nullptr)
  {
    instance_.reset(new TextureAtlas());
  }

  return instance_.get();
}

TextureAtlas::TextureAtlas()
{
  unsigned int maximumSize = sf::Texture::getMaximumSize();
  std::cout << "Maximum size for textures on this computer is " << maximumSize
            << " pixels." << std::endl;
  std::cout << "Block size for this texture atlas is " << (1 << blockSize)
            << " pixels." << std::endl;

  atlas.create(maximumSize, maximumSize);
  maxBlocks = maximumSize >> blockSize;
  used.resize(maxBlocks * maxBlocks, false);

  // Mark block (0, 0) as used, and create an area of {255,255,255,255} so
  // we have a "blank" texture.
  sf::Image blankImage;
  blankImage.create((1 << blockSize), (1 << blockSize), sf::Color::White);
  save(blankImage);
}

SerialNumber TextureAtlas::save(sf::Image& image)
{
  sf::Vector2u divided
  { image.getSize().x >> blockSize, image.getSize().y >> blockSize };
  sf::Vector2u remaining
  {
    image.getSize().x& ((1 << blockSize) - 1), image.getSize().y
    & ((1 << blockSize) - 1)
  };
  sf::Vector2u imageBlocks
  {
    divided.x + ((remaining.x != 0) ? 1 : 0), divided.y
    + ((remaining.y != 0) ? 1 : 0)
  };

  bool suitable = true;
  sf::Vector2u origin
  { 0, 0 };

  SerialNumber serialNumber = nextSerialNumber;

  suitable = findNextSpot(origin, imageBlocks);

  if (suitable == false)
  {
    FATAL_ERROR("No room left in texture atlas to store an image");
  }

  // At this point, xOrigin/yOrigin is a suitable block to store the image.
  atlas.update(image, origin.x << blockSize, origin.y << blockSize);

  // Save texture information and add it to our set.
  sf::IntRect info;
  info.left = origin.x << blockSize;
  info.top = origin.y << blockSize;
  info.width = image.getSize().x;
  info.height = image.getSize().y;
  texRect[serialNumber] = info;
  ++nextSerialNumber;

  reserveSpot(origin, imageBlocks);

  return serialNumber;
}

bool TextureAtlas::findNextSpot(sf::Vector2u& origin,
                                sf::Vector2u const& imageBlocks)
{
  bool suitable = false;

  // NOTE: This is super-duper inefficient.
  //       I am sure this algorithm's speed could be greatly improved; for now this works.
  for (origin.y = 0; origin.y < maxBlocks; ++origin.y)
  {
    for (origin.x = 0; origin.x < maxBlocks; ++origin.x)
    {
      // Start by assuming the origin block is suitable.
      suitable = true;

      // Now check the spots left of and below the origin to see if there's room.
      for (unsigned int yCheck = origin.y; yCheck < origin.y + imageBlocks.y;
           ++yCheck)
      {
        for (unsigned xCheck = origin.x; xCheck < origin.x + imageBlocks.x;
             ++xCheck)
        {
          if (used[get_block(xCheck, yCheck)])
          {
            suitable = false;
            break;
          }
        }
        if (suitable == false)
        {
          break;
        }
      }
      if (suitable == true)
      {
        break;
      }
    }
    if (suitable == true)
    {
      break;
    }
  }
  return suitable;
}

void TextureAtlas::reserveSpot(const sf::Vector2u& origin,
                               const sf::Vector2u& imageBlocks)
{
  for (unsigned int yBlocks = origin.y; yBlocks < origin.y + imageBlocks.y;
       ++yBlocks)
  {
    for (unsigned int xBlocks = origin.x; xBlocks < origin.x + imageBlocks.x;
         ++xBlocks)
    {
      used[get_block(xBlocks, yBlocks)] = true;
    }
  }
}

const sf::IntRect& TextureAtlas::getRect(SerialNumber texNumber)
{
  return texRect[texNumber];
}

const sf::Texture* TextureAtlas::getAtlasTexture(void)
{
  return &atlas;
}

} // namespace rectopia
} // namespace gsl

