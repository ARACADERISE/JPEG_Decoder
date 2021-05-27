#include <stdio.h>
#include <stdlib.h>

const int DQT_starter = -36;
const int DHT_starter = -60; /*
		      * Corresponding values:
		      * 	-36: 0xe0
		      * 	-60: 0xc4
		      *
		      * DQT_starter: this is the starting address of DQT in
		      * 	-> signed 2's complememnt
		      * DHT_starter: this is the starting address of DHT in
		      * 	-> signed 2's complement
		      */

const int SOF = 0xc0;
const int _SOF = 0xc1; /*
		   * The SOF can either be 0xc0
		   * or 0xc1. Dependable on the JPEG file.
		   * 
		   * Both values will stand for the SOF. Hence as to why
		   * _SOF is assigned 0xc1. It has the same action as SOF
                   */
const int sos = 0xda;

const int SOF_c = -64;
const int _SOF_c = -63;
const int SOS_c = -38; /*
			* Corresponding values:
			* 	-64: 0xc0
			* 	-63: 0xc1
			* 	-38: 0x38
			*
			* SOF_C is the Start Of Frame. Value has to be 0xc1 or 0xc0.
			* 	->  -64 and -63 corresponds to the 
			* 	-> signed 2's complement
			* 	-> value of each value(being 0xc0 and 0xc1)
			* SOS_C is the Start Of Scan. Value has to be 0xda.
			* 	-> -38 corresponds to the signed 2's complement
			* 	-> of the value 0xda
			*/

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

/*
 * Initialize the RI struct.
 *
 * Version: 0.1.0
 */
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


/* HEADER */
const int start_image[2] = {-1, -40};
const int _start_image[2] = {0xFF, 0xD8}; // -1 is the signed value of 0xFF, -40 is the signed value of 0xD
const int marker_id[2] = {0xff, 0xe0};
static int length[2] = {0,0};
static int* values; /*
		     * This will just be the values of 0..length[1]
		     */
static int FIM[5] = {0x4a, 0x46, 0x49, 0x46, 0x00};

/* DQT Table */
static int DQT[5] = {
	0xff, 0xdb, // define DQT
	0, 0, // length
	0
};
static char* QT_values;

/* DHT TABLE */
static int DHT[5] = {
	0xff, 0xc4,
	0, 0, // length
	0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0 // Number of symbols with lengths 1..16(16 bytes)
};
static char* DHT_TOS;

/* START OF FRAME MARkEr */
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
 * Version: 0.1.0
 */
RI* check_image_format(RI* image) {
	
	image->new_image = calloc(image->file_size, sizeof(*image->new_image));

	/*
	 * Make sure the two bytes correspond
	 */
	if(image->file_info[0] == start_image[1]
			&& image->file_info[1] == start_image[0])
	{
		image->new_image[0] = (unsigned char)_start_image[1];
		image->new_image[1] = (unsigned char)_start_image[0];

		/*
		 * Corresponding values:
		 * 	-> 16 will stand for 0xe0
		 * 	-> 67 will stand for 0xdb
		 * */
		switch(image->file_info[4])
		{
			case 16: {
				int value = (16 * 16) - 2;
				image->file_info[4] = 0xe0;
				printf("HERE: e0");
				break;
			}
			case 67: {
				int value = (67 * 3) + 18;
				image->file_info[4] = 0xdb; /*
							     * This will convert to
							     * -37, because of
							     *  signed 2's
							     *  complement
							     */
				printf("HERE: db, %d", image->file_info[4]);
			}
		}
	}


	return image;
}

int main() {
	RI* img = init_image("img2.jpg");
	
	check_image_format(img);
}
