#ifndef IMAGE_H
#define IMAGE_H

#include <memory>

class Image
{
  public:
    Image(unsigned int width_, unsigned int height_);
    virtual ~Image();

    unsigned int* get_data_ptr();

    bool blit(Image& src_image,
              unsigned int src_width,
              unsigned int src_height,
              unsigned int dst_x_start,
              unsigned int dst_y_start,
              unsigned int src_x_start,
              unsigned int src_y_start);
    unsigned int get_pixel(unsigned int x, unsigned int y);
    void set_pixel(unsigned int x, unsigned int y, unsigned int c);

  protected:
  private:
    // Private implementation pointer
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // IMAGE_H
