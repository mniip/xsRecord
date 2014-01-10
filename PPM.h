#include <string>

#include <X11/Xutil.h>

namespace PPM
{
	void WriteImage(std::string filename, XImage *image);
}
