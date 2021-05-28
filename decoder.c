#include "decoder.h"

RI* init_image(char *filename)
{
    RI* img = calloc(1, sizeof(*img));

    img->filename = filename;

    FILE* file = fopen(img->filename, "rb");

    if(file)
    {
        fseek(file, 1, SEEK_END);
        img->file_size = ftell(file);
        fseek(file, 1, SEEK_SET);

        img->file_info = calloc(img->file_size, sizeof(*img->file_info));\
        fread(img->file_info, img->file_size, 1, file);

        img->new_image = calloc(img->file_size, sizeof(*img->new_image));

        fclose(file);

        return img;
    }

    fprintf(stderr, "No such file");
    exit(EXIT_FAILURE);
}

RI* read_header(RI* img)
{
    if(img->file_info[0] == -40)
    {
        img->new_image[0] = 0xFF;
        img->new_image[1] = 0xD8;

        switch(img->file_info[2])
        {
            case -32:
            {
                // Marker Id
                img->new_image[2] = 0xFF;
                img->new_image[3] = 0xE0;

                // Length
                img->new_image[4] = 0x00;
                img->new_image[5] = img->file_info[4];

                // JFIF
                img->new_image[6] = 0x4a;
                img->new_image[7] = 0x46;
                img->new_image[8] = 0x49;
                img->new_image[9] = 0x46;
                img->new_image[10] = 0x00;

                int length = img->new_image[5];

                // Create two variables to reference both arrays
                int index = 6;
                int _index = 10;

                for(int i = 0; i < length; i++)
                {
                    index++;
                    _index++;
                    img->new_image[_index] = img->file_info[index];
                }

                // Assign the values
                int ind = _index;
                for(int i = 0; i <= (img->new_image[ind] * -1) + 22; i++)
                {
                    _index++;
                    index++;
                    img->new_image[_index] = img->file_info[index + 3];
                }

                index += 3;
            }
        }
    }

    FILE* file = fopen("NEW", "wb");

    for(int i = 0; i < img->file_size; i++)
    {
        fwrite(&img->new_image[i], sizeof(img->new_image[i]), 1, file);
    }

    fclose(file);

    return img;
}
