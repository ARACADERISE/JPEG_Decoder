# JPEG_Decoder

JPEG decoder written in C.

### VERSION 0.1.0
#### There will be a later version supporting odd JPEG file formats, such that of a JPEG file without the JFIF header

Command Line args:

- Filename
- Action

Actions include that of the following:

- EVEN
  - Simply brighten each pixe
- ODD
  - Simply dim each pixel
- `_EVEN`
  - Strictly match each bit of the pixel, the brighten it
- `_ODD`
  - Strictly match each bit of the pixel, then dim it
