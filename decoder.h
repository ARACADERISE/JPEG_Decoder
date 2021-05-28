#ifndef JPEG_DECODER
#define JPEG_DECODER
#include <stdio.h>
#include <stdlib.h>

typedef struct JPEG_Decoder {
  enum {
    JPEG = 2,
    JPG = 1
  } Type; // the decoder will depend on the type.

  char *filename;
  size_t file_size;
  char *file_info;

  char *new_image;
} RI;

// Values to be used throughout the program
const static int starter[2] = {
  -40, -1
};
const static int _starter[2] = {
  0xFF, 0xD8
};
static int MARKER_ID = -32;

RI* init_image(char *filename);
RI* read_header(RI* img);

#endif
