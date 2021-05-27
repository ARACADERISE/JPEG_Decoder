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


# JPEG file format explanation

All JPEG files(unless it's a weird one) starts with _oxFF 0xD8_. This initializes the header.
Then, we run into the JFIF segment marker. Which primarily says, "Hey, this is a JPEG file".

This is defined by the values _0xFF 0xE0_.
Then we get the length of the header.
So, the binary value is, for example, with a length of 16:

`0xFF 0xD8 0xFF 0xE0 0x00 0x10`

Then, we run into the _JFIF_ format. This is equivilant to _0x4a 0x46 0x49 0x46_ in hex.
The _header_ itself, being _JFIF_, is 5 bytes. There is padding at the end.

The sequence is now: `0xFF 0xD8 0xFF 0xE0 0x00 0x10 0x4a 0x46 0x49 0x46 0x00`

But wait. The _entire header length_ is 16. Where is the rest of those bytes? 
We have some other values that give us information about the image. I will be using some values from a image I dumped. The sequence is now:
`0xFF 0xD8 0xFF 0xE0 0x00 0x10 0x4a 0x46 0x49 0x46 0x00 0x00 0x01 0x01 0x00 0x00 0x01 0x00 0x01 0x00`

Ok, but there are two more bytes. Where are they?
Well those next two bytes define what is called a _Quantization Table_, which just gives more information about the pixels in the image.

This action is defined with _0xFF 0xDB_. Lets see:

`0xFF 0xD8 0xFF 0xE0 0x00 0x10 0x4a 0x46 0x49 0x46 0x00 0x00 0x01 0x01 0x00 0x00 0x01 0x00 0x01 0x00 0xFF 0xDB`

The sequence, fully initialized is now:
`0xFF0xD8 0xFF0xE0 0x000x10 0x4a0x46 0x490x46 0x000x00 0x010x01 0x000x00 0x010x00 0x010x00 0xFF0xDB`
