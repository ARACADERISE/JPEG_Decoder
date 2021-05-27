#include <stdio.h>
#include <stdlib.h>

/* DEFAULT BIT OFFSET  */
#define DEFAULT_BIT_OFFSET	2
static int _DBO =		DEFAULT_BIT_OFFSET; // this could change later on

/* HELPER FUNCTION TO OFFSET THE CURRENT PIXEL 
 *
 * The normal diffference between both is 1 pixel when doing a normal offset.
 * With a reverse offset, the difference is 6 pixels(normally).
 */
#define OFFSET(pixel) ((pixel | _DBO) >> _DBO) 		// brightens the pixel
#define REVERESE_OFFSET(pixel) ((pixel ^ _DBO) << _DBO) // dims the pixel
#define _OFFSET(pixel) ((pixel & _DBO) >> _DBO) 	// Strictly matches the pixel to the 2 bits, then brightens it
#define _REVERSE_OFFSET(pixel) ((pixel ^ _DBO) << _DBO) // Strictly matches the pixel to the 2 bits, then dims it


/* UNSUPPORTED VALUES */
const int U_SOF2  = 0xc2;
const int U_SOF3  = 0xc3;
const int U_SOF4  = 0xc4;
const int U_SOF5  = 0xc5;
const int U_SOF6  = 0xc6;
const int U_SOF7  = 0xc7;
const int U_SOF10 = 0xcA;
const int U_SOF11 = 0xCB;

/* UNSUPPORTED SIGNED 2's COMPLEMENT VALUES */
const int _U_SOF2  = -62;
const int _U_SOF3  = -61;
const int _U_SOF4  = -60;
const int _U_SOF5  = -59;
const int _U_SOF6  = -58;
const int _U_SOF7  = -57;
const int _U_SOF10 = -54;
const int _U_SOF11 = -53;

/* Corresponding 2's COMPLEMENT OF EACH VALUE */
const int DQT_starter = -37;
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

	// File to write to
	FILE* _WRITE;

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

		img_info->_WRITE = fopen(filename, "wb");

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
 * LAYOUT OF FILE. THIS WILL BE CHECKED AT THE END TO MAKE SURE THE
 * FILE FORMAT IS OKAY.
 */
static int FORMAT[15];

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

		unsigned char t = 0xc2;
		printf("%d", t);

		int CV = 0;
		switch(image->file_info[4])
		{
			case 16: { // 0xe0
				CV = (16 * 16) - 32;

				FORMAT[0] = 0xe0;

				// Initalize the Segment Marker for new_image(JFIF)
				image->new_image[3] = 0xff;
				image->new_image[4] = 0xe0; /*
							    * This will convert to -37, because of signed 2's
							    * complement
							    */
				image->new_image[5] = image->file_info[5];

				int index = 5;
				for(int i = 0; i < image->new_image[5]; i++) 
				{ // this should assign the values of JFIF etc
					index++;
					image->new_image[index] = image->file_info[index];
				}

				FORMAT[1] = 0xdb;

				image->new_image[index + 1] = 0xff;
				image->new_image[index + 2] = 0xdb;
				

				printf("HERE: e0");
				break;
			}
			case 67: { // 0xdb
				CV = (67 * 3) + 18;

				FORMAT[0] = 0xe0;

				// The new image supports the same format: 0xff, 0xe0 -> Segment Marker(for JFIF)
				image->new_image[3] = 0xff;
				image->new_image[4] = 0xe0;
				image->new_image[5] = 00;
				image->new_image[6] = 0xA;
				
				// JFIF
				image->new_image[7] = 0x4a;
				image->new_image[8] = 0x46;
				image->new_image[9] = 0x49;
				image->new_image[10] = 0x46;
				image->new_image[11] = 0x00;

				// Cover rest of bytes with default values
				image->new_image[12] = 0x01;
				image->new_image[13] = 0x00;
				image->new_image[14] = 0x00;
				image->new_image[15] = 0x01;
				image->new_image[16] = 0x00;
				image->new_image[17] = 0x01;
				image->new_image[18] = 0x00;
				image->new_image[19] = 0x00;

				FORMAT[1] = 0xdb;

				// Initalize the DQT for new_image
				image->new_image[5] = 0xff;
				image->new_image[6] = 0xdb; /*
							     * This will convert to -37, because of signed 2's
							     * complement
							     */
				printf("HERE: db, %d", image->file_info[4]);
				break;
			}
			case 0xc2:
			case 0xc3:
			case 0xc4:
			case 0xc5:
			case 0xc6:
			case 0xc7: {
				fprintf(stderr, "Error: Invalid Format. Expected DB or E0, found %d.", image->file_info[4]);
				exit(EXIT_FAILURE);
				break;
			}
			default: {
				fprintf(stderr, "Error: Invalid Format.");
				exit(EXIT_FAILURE);
				break;
			}
		}
	}


	return image;
}

int main() {
	RI* img = init_image("img2.jpg");
	
	check_image_format(img);
}
