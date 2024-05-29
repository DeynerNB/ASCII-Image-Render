#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <iostream>
#include <windows.h>
#include <filesystem>

const std::string ColorBuffer = "N@#W$9876543210?!abc;:+=-,._";
const short int BufferLength = ColorBuffer.size();

// Define size of the resulted image
const short int Width_px = 64;
const short int Height_px = 64;

int getRangeIndex(int x, int y, stbi_uc *img, int img_Width, int img_Height, int channels) {

	int desire_Width = img_Width / Width_px;
	int desire_Height = img_Height / Height_px;

	int middle_X = (desire_Width / 2) + x;
	int middle_Y = (desire_Height / 2) + y;

	int resultX = 0, resultY = 0, final_Result = 0;

	int R_Value = 0, G_Value = 0, B_Value = 0;

	// Get the median RGB values for the middle horizontal line
	for (int i = 0; i < desire_Width; i++) {
		R_Value = img[ channels * (middle_Y * img_Width + (x + i)) + 0];
		G_Value = img[ channels * (middle_Y * img_Width + (x + i)) + 1];
		B_Value = img[ channels * (middle_Y * img_Width + (x + i)) + 2];

		resultX += (R_Value + G_Value + B_Value);
	}

	// Get the median RGB values for the middle vertical line
	for (int i = 0; i < desire_Height; i++) {
		R_Value = img[ channels * ((y + i) * img_Width + middle_X) + 0];
		G_Value = img[ channels * ((y + i) * img_Width + middle_X) + 1];
		B_Value = img[ channels * ((y + i) * img_Width + middle_X) + 2];

		resultY += (R_Value + G_Value + B_Value);
	}

	// Grab the RGB values from the intersection of the horizontal and vertical line
	R_Value = img[ channels * (middle_Y * img_Width + middle_X) + 0];
	G_Value = img[ channels * (middle_Y * img_Width + middle_X) + 1];
	B_Value = img[ channels * (middle_Y * img_Width + middle_X) + 2];

	// Take out the double sum of the intersection
	final_Result = resultX + resultY - (R_Value + G_Value + B_Value);

	return (final_Result / (BufferLength * (desire_Width + desire_Height - 1)));
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Es necesario indicar el directorio con las imagenes\nUso: <PROGRAMA.exe> <ruta/al/directorio>\n\n", argv[0]);
		return 1;
	}
	std::string path = argv[1];

	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
		printf("Error: La ruta no es un directorio valido");
		return 1;
	}

	int width, height, channels;
	stbi_uc *img;

	std::string result = "";
	result.reserve( ((2 * Width_px) * Height_px) + Height_px );

	// Clear screen
    printf("\x1b[2J");
	for (const auto & entry : std::filesystem::directory_iterator(path)) {
		// Set cursor position 0, 0
        printf("\x1b[H");

		img = stbi_load(entry.path().string().c_str(), &width, &height, &channels, 0);

		if (!img) {
			std::cout << "\n\nERROR: Cannot open file!\n\n";
			Sleep(500);
			exit(1);
		}

		// Set the size of each quadrant of the image
		int quadrantSize_x = width / Width_px;
		int quadrantSize_y = height / Height_px;

		for (int y = 0; y < height - 32; y += quadrantSize_y) {
			for (int x = 0; x < width - 32; x += quadrantSize_x) {
				result += ' ';
				result += ColorBuffer[ getRangeIndex(x, y, img, width, height, channels) ];
			}
			result += '\n';
		}

		std::cout << result;
		Sleep(10);

		stbi_image_free(img);
		result.clear();
	}
	return 0;
}

