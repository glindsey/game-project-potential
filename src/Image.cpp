#include "Image.h"

#include <cstdio>
#include <cstring>
#include <libpng/png.h>

#include "ErrorMacros.h"

struct Image::Impl
{
  unsigned int width;
  unsigned int height;
  std::unique_ptr<unsigned int[]> data;

  /// Return the array index for the specified pixel.
  unsigned int pixel(unsigned int x, unsigned int y)
  {
    return (y * width) + x;
  }
};

Image::Image(unsigned int width_, unsigned int height_) :
  impl(new Impl())
{
  // Set width, height to zero.
  impl->width = width_;
  impl->height = height_;

  unsigned int total_size = width_ * height_;

  // Reset the data array.
  impl->data.reset(new unsigned int[total_size]);
  memset(impl->data.get(), 0xFF, total_size);
}

Image::~Image()
{
}

unsigned int* Image::get_data_ptr()
{
  return impl->data.get();
}

bool Image::load_png(char const* filename)
{
  /*
  char header[8];
  bool is_png;
  FILE *fp = fopen(filename, "rb");
  if (!fp)
  {
    FATAL_ERROR("Could not load \"%s\"", filename);
    return false;
  }
  fread(header, 1, 8, fp);
  is_png = !png_sig_cmp(header, 0, 8);
  if (!is_png)
  {
    FATAL_ERROR("\"%s\" is not a PNG file", filename);
    return false;
  }

  return true;
  */
  /// @todo finish me and make me work!
  return false;
}

bool Image::blit(Image& src_image,
                 unsigned int src_width = 0,
                 unsigned int src_height = 0,
                 unsigned int dst_x_start = 0,
                 unsigned int dst_y_start = 0,
                 unsigned int src_x_start = 0,
                 unsigned int src_y_start = 0)
{
  if (src_width == 0)
  {
    src_width = impl->width;
  }

  if (src_height == 0)
  {
    src_height = impl->height;
  }

  for (unsigned int y_offset = 0; y_offset < src_height; ++y_offset)
  {
    for (unsigned int x_offset = 0; x_offset < src_width; ++x_offset)
    {
      unsigned int dst_index = impl->pixel(dst_x_start + x_offset,
                                           dst_y_start + y_offset);

      impl->data[dst_index] = src_image.get_pixel(src_x_start + src_width,
                                                  src_y_start + src_height);
    }
  }

  return true;
};

unsigned int Image::get_pixel(unsigned int x, unsigned int y)
{
  return impl->data[impl->pixel(x, y)];
}

void Image::set_pixel(unsigned int x, unsigned int y, unsigned int c)
{
  impl->data[impl->pixel(x, y)] = c;
}
