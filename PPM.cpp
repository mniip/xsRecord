#include <fstream>
#include <stdexcept>
#include <string>

#include "X.h"

namespace PPM
{
	void WriteImage(std::string filename, XImage *image)
	{
		printf("Writing %s\n", filename.c_str());
		std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary);
		int width = image->width;
		int height = image->height;
		out << "P6\n" << width << " " << height << "\n255\n";
		if(image->red_mask == 0xFF && image->green_mask == 0xFF00 && image->blue_mask == 0xFF0000)
			for(int y = 0; y < height; y++)
				for(int x = 0; x < width; x++)
				{
					unsigned long int pixel = XGetPixel(image, x, y);
					out.write((const char *)&pixel, 3);
				}
		else if(image->red_mask == 0xFF0000 && image->green_mask == 0xFF00 && image->blue_mask == 0xFF)
			for(int y = 0; y < height; y++)
				for(int x = 0; x < width; x++)
				{
					unsigned long int pixel = XGetPixel(image, x, y);
					unsigned long int p = pixel >> 16 | pixel & 0xFF00 | (pixel & 0xFF) << 16; 
					out.write((const char *)&p, 3);
				}
		else
			throw std::runtime_error("Unsupported image format");
		out.close();
		X::Destroy(image);
	}
}
