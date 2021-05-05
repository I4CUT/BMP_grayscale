#include <iostream>
#include <fstream>

struct BITMAPHEADER
{
	char name[2];
	unsigned int size;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int offset;
};

struct BITMAPINFOHEADER
{
	unsigned int header_size;
	unsigned int width;
	unsigned int height;
	unsigned short int color_planes;
	unsigned short int bits_per_pixel;
	unsigned int compression;
	unsigned int image_size;
	int info_reserved[4];
};

struct RGB
{
	unsigned char red;
	unsigned char blue;
	unsigned char green;
};

struct Image
{
	unsigned int height;
	unsigned int width;
	RGB** rgb;
};

unsigned char grayscale(RGB rgb)
{
	return (((0.11 * rgb.blue) + (0.3 * rgb.red) + (0.59 * rgb.green)) / 3);
}

void to_gray(Image img)
{
	for (int i = 0; i < img.height; i++)
	{
		for (int j = 0; j < img.width; j++)
		{
			img.rgb[i][j].blue = img.rgb[i][j].green = img.rgb[i][j].red
				= grayscale(img.rgb[i][j]);
		}
	}
}

Image read_image(std::ifstream& file, int height, int width, BITMAPINFOHEADER dib_header)
{
	Image img;
	int row_size, rgb_numb;
	img.height = height;
	img.width = width;
	row_size = ((dib_header.bits_per_pixel * img.width + 31 )/ 32) * 4;
	rgb_numb = row_size / sizeof(RGB) + 1;
	img.rgb = new RGB * [height];

	for (int i = img.height - 1; i >= 0; i--)
	{
		img.rgb[i] = new RGB[rgb_numb];
		file.read((char*)img.rgb[i], row_size);
	}

	return img;
}

void free_image(Image img)
{
	for (int i = img.height - 1; i >= 0; i--)
	{
		delete[] img.rgb[i];
	}
	delete[] img.rgb;
}

void create_image(BITMAPHEADER header, BITMAPINFOHEADER dib_header, Image img)
{
	std::ofstream file;


	file.open("new.bmp", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	if (!file.is_open())
	{
		std::cout << "--------------------------------" << std::endl;
		std::cout << "Error! The file was not created!" << std::endl;
		std::cout << "--------------------------------\n" << std::endl;
	}
	else
	{
		std::cout << "--------------------------------" << std::endl;
		std::cout << "Black and white file created!!!" << std::endl;
		std::cout << "--------------------------------\n" << std::endl;
		to_gray(img);
		file.write((char*)header.name, sizeof(header.name));
		file.write((char*)&header.size, sizeof(header.size) + sizeof(header.reserved1) + sizeof(header.reserved2) + sizeof(header.offset));
		file.write((char*)&dib_header, sizeof(BITMAPINFOHEADER));

		int row_size;
		row_size = ((dib_header.bits_per_pixel * img.width + 31)/ 32) * 4;
		for (int i = img.height - 1; i >= 0; i--)
		{

			file.write((char*)img.rgb[i], row_size);
		}


	}

	file.close();

}

void openfile()
{
	std::ifstream file("24.bmp", std::istream::in | std::istream::binary);
	std::string msg;

	if (!file.is_open())
	{
		std::cout << "--------------------------------" << std::endl;
		std::cout << "Error! The file did not open!" << std::endl;
		std::cout << "--------------------------------\n" << std::endl;
	}
	else
	{
		std::cout << "--------------------------------" << std::endl;
		std::cout << "File is open!" << std::endl;
		std::cout << "--------------------------------\n" << std::endl;
		BITMAPHEADER header;
		BITMAPINFOHEADER dib_header;
		file.read((char*)&header.name, sizeof(header.name));
		file.read((char*)&header.size, sizeof(header.size) + sizeof(header.reserved1) + sizeof(header.reserved2) + sizeof(header.offset));
		std::cout <<"File format: " << header.name[0] << header.name[1] << std::endl;
		std::cout <<"Header offset: "<< header.offset << std::endl;
		std::cout <<"Header size: "<< header.size << std::endl;

		file.read((char*)&dib_header, sizeof(BITMAPINFOHEADER));
		std::cout << "DIB header size: " << dib_header.header_size << std::endl;
		std::cout << "The bitmap width in pixels: " << dib_header.width << std::endl;
		std::cout << "The bitmap height in pixels: " << dib_header.height << std::endl;
		std::cout << "The number of color planes: " << dib_header.color_planes << std::endl;
		std::cout << "The number of bits per pixel: " << dib_header.bits_per_pixel << std::endl;
		std::cout << "compression: " << dib_header.compression << std::endl;
		std::cout << "The image size: " << dib_header.image_size << std::endl;

		file.seekg(header.offset, std::ios::beg);
		Image img = read_image(file, dib_header.height, dib_header.width, dib_header);
		create_image(header, dib_header, img);
		file.close();
		free_image(img);
	}


}

int main()
{
	openfile();
}
