# JPEG_Decoder

JPEG decoder written in C.

### VERSION 0.1.0
#### There will be a later version supporting odd JPEG file formats, such that of a JPEG file without the JFIF header
#### Note: All new images rendered via this application will support the format:
`0xFF 0xD8 0xFF 0xE0 0x00 0x00 0x4a 0x46 0x49 0x46 0x00 ...`
\t t

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

### The Header
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

### Tables
There are two tables you will run into, the _Quantization Table_ and the _Huffman Table_.
Neither are needed for this project since we primarily are just copying the values of each "_table_" into the new image and continuing on.
The only thing we will be changing are the pixels.

A _Huffman Table_ is defined with _0xFF 0xC4_. Each tables syntax is the definition and the length. Example, a _Huffman Table_ with a length of 16:
`0xFF 0xC4 0x00 0x10`

There are, by default, 16 bytes built into the _Huffman Table_ to give each symbol a code 1..16. The rest of the information, which would be another 16 bytes, will just be information about the image.

A _Quantization Table_ is primarily the same as a _Huffman Table_.
It takes in the definition and the length as its first 4 bytes.

Example of a _Quantization Table_ with a length of 16:
`0xFF 0xDB 0x00 0x10`

1 byte of the 16 is given to the QT information which is just a bit 0...3. 4..7 is the precision, 0 = 8 bit and otherwise it's 16. The rest are assigned to values about the image. Primarily, it gives QT values. `64*(precision + 1)`.
