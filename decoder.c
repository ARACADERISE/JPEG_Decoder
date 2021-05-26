#include <stdio.h>
#include <stdlib.h>

typedef struct ReadImge {
	enum {
		jpg = 1,
		png = 2,
		jpeg = 1 // same as jpg
	} ImageType;

	char* file_info;
	size_t file_size;

	unsigned char** new_image;
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
const int _start_imagae[2] = {0xD8, 0xFF}; // -1 is the signed value of 0xFF, -40 is the signed value of 0xD8

RI* check_image_format(RI* image) {
	
	return image;
}

int main() {
	RI* img = init_image("img.jpg");
	
	check_image_format(img);
}
