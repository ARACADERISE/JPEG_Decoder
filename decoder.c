#include <stdio.h>
#include <stdlib.h>

#define SOF 0xc0
#define _SOF 0xc1 /*
		   * The SOF can either be 0xc0
		   * or 0xc1. Dependable on the JPEG file.
		   * 
		   * Both values will stand for the SOF. Hence as to why
		   * _SOF is assigned 0xc1. It has the same action as SOF
                   */
#define SOS 0xda

typedef struct ReadImge {
	enum {
		jpg = 1,
		png = 2,
		jpeg = 1 // same as jpg
	} ImageType;

	char* file_info;
	size_t file_size;

	unsigned char* new_image;
} RI;
RI* init_image(char* filename) {
	FILE* file = fopen(filename, "rb");

	if(file) {
		RI* img_info = calloc(1, sizeof(*img_info));

		fseek(file, 1, SEEK_END);
		img_info->file_size = ftell(file);
		fseek(file, 1, SEEK_SET);

		img_info->file_info = calloc(img_info->file_size, sizeof(*img_info->file_info));

		fread(img_info->file_info, img_info->file_size, 1, file);

		fclose(file);

		return img_info;
	}

	fprintf(stderr, "No such file");
	exit(EXIT_FAILURE);
}

const int start_image[2] = {-40, -1};
const int _start_image[2] = {0xD8, 0xFF}; // -1 is the signed value of 0xFF, -40 is the signed value of 0xD
const int marker_id[2] = {0xff, 0xe0};
static int length[2] = {0,0};
static int* values; /*
		     * This will just be the values of 0..length[1]
		     */
static int FIM[5] = {0x4a, 0x46, 0x49, 0x46, 0x00};
static int DQT[5] = {
	0xff, 0xdb, // define DQT
	0, 0, // length
	0
}
static char* QT_values;
static int DHT[5] = {
	0xff, 0xc4,
	0, 0, // length
	0,
	0, 0, 0, 0, 0, 0, 0, 0, /*
	0, 0, 0, 0, 0, 0, 0, 0,  * Number of symbols with lengths 1..16
				 */
};
static char* DHT_TOS;
static int SOS[2] = {0xff, 0xda};

/*
 * TESTING PURPOSES. DO NOT USE.
 *
 * {
 *	0xff, 0xd8, 			// header
 *	0xff, 0xe0, 			// marker id
 *	0, 0, 	    			// length
 *	0x4a, 0x46, 0x49, 0x46, 0x00,	// FIM(File Id Mark)
 *	0xff, 0xdb,			// DQT
 *	0, 0,				// Length of QT
 *	0, 				// QT values(0..3)
 *	0xff, 0xc4, 			// DHT
 *	0, 0, 				// Length of DHT
 *	0,				// HT values(0..3)
 *	0xFF, 0xDA			// Start of scan
 * }
 * }
 */


/*
 * This assigns the header to the new_image variable.
 *
 */
RI* check_image_format(RI* image) {
	
	image->new_image = calloc(image->file_size, sizeof(*image->new_image));

	/*
	 * Make sure the two bytes correspond
	 */
	if(image->file_info[0] == start_image[0]
			&& image->file_info[1] == start_image[1])
	{
		image->new_image[0] = (unsigned char)_start_image[1];
		image->new_image[1] = (unsigned char)_start_image[0];
	}


	return image;
}

int main() {
	RI* img = init_image("img.jpeg");
	
	check_image_format(img);
}
