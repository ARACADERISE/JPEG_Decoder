# JPEG_Decoder

JPEG decoder written in C.

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
