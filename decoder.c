#include "decoder.h"

RI* init_image(char *filename)
{
    RI* img = calloc(1, sizeof(*img));

    img->filename = filename;
    img->index = 0;
    img->_index = 0;

    FILE* file = fopen(img->filename, "rb");

    if(file)
    {
        fseek(file, 1, SEEK_END);
        img->file_size = ftell(file);
        fseek(file, 1, SEEK_SET);

        img->file_info = calloc(img->file_size, sizeof(*img->file_info));
        fread(img->file_info, img->file_size, 1, file);

        img->new_image = calloc(img->file_size, sizeof(*img->new_image));

        fclose(file);

        return img;
    }

    fprintf(stderr, "No such file");
    exit(EXIT_FAILURE);
}

RI* read_data(RI *img)
{
    int ind = img->_index;
    if(img->new_image[ind - 1] < 0)
    {
        for(int i = 0; i < (img->new_image[ind-1] * -1) + 8; i++)
        {
            img->_index++;
            img->index++;
            img->new_image[img->_index] = img->file_info[img->index + 1];
        }
    } else
    {
        for(int i = 0; i <= img->new_image[ind - 1] + 66; i++)
        {
            img->_index++;
            img->index++;
            img->new_image[img->_index] = img->file_info[img->index + 1];
        }
    }

    return img;
}

RI *read_data_dependable(RI *img, int length)
{

    for(int i = 0; i < length; i++)
    {
        img->index++;
        img->_index++;
        img->new_image[img->_index] = img->file_info[img->index + 1];
    }

    return img;
}

/*
 * For 0xFF 0xDB specifically. ONLY if it is at the beginning
 */
RI *special_format(RI *img)
{

    img->new_image[img->_index] = 0x00;
    img->_index++;

    img->new_image[img->_index] = img->file_info[img->index];
    //img->index++;

    img->_index++;
    img->new_image[img->_index] = 0x00;

    redo:
    read_data(img);

    img->index += 2;
    img->_index++;

    
    switch(img->file_info[img->index])
    {
        case -64:
        { // c0, start of frame
            while(!(img->file_info[img->index] == -60) &&
                    !(img->file_info[img->index] == -37)) {
                img->new_image[img->_index] = img->file_info[img->index];
                img->index++;
                img->_index++;
            }

            img->new_image[img->_index] = img->file_info[img->index];
            img->_index++;

            read_data_dependable(img, img->new_image[img->_index] + 132);

            img->_index++;
            img->new_image[img->_index] = 0x00;

            img->_index++;
            img->index += 3;
            img->new_image[img->_index] = img->file_info[img->index];

            img->index -= 1;

            read_data_dependable(img, img->new_image[img->_index] + 71);
            break;
        }
    }

    return img;
}

RI* read_header(RI *img)
{
    if(img->file_info[0] == -40)
    {
        img->new_image[0] = _starter[0];
        img->new_image[1] = _starter[1];

        switch(img->file_info[2])
        {
            case -37:
            { // db
                img->new_image[2] = 0xFF;
                img->new_image[3] = 0xDB;

                img->_index = 4;
                img->index = 4;

                special_format(img);

                break;
            }
            case -32:
            { // eo
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
                img->index = 6;
                img->_index = 10;

                read_data_dependable(img, length);

                // Assign the values
                read_data(img);

                img->index++;
                img->_index++;
                img->new_image[img->_index] = img->file_info[img->index];

                break;
            }
            default:
            {
                fprintf(stderr, "Invalid Format");
                exit(EXIT_FAILURE);
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
