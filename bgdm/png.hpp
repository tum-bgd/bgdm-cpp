#ifndef PNG_HPP_INC
#define PNG_HPP_INC
#include<string>
#include<stdio.h>
#include<png.h>

void writepng(std::string filename,  unsigned char *pixels, int width, int height) {
  

  FILE *fp = fopen(filename.c_str(), "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    width, height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);
  unsigned char **row_pointers = new unsigned char*[height];
  for (int i=0; i < height; i++)
  {
      row_pointers[i] = &pixels[i*width*4];
  }
  

  if (!row_pointers) abort();

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  free(row_pointers);

  fclose(fp);
  if (png && info)
      png_destroy_write_struct(&png, &info);
}

#endif
