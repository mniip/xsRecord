#include <X11/Xutil.h>

namespace Queue
{
	void Start(std::string path);
	void Stop();
	void Push(XImage *image);
}
