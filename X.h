#include <vector>

#include <X11/Xutil.h>

namespace X
{
	extern int Width, Height;
	void Init();
	XImage *Pull(int x, int y, int w, int h);
	void Destroy(XImage *image);
	void Push(Drawable window, XImage *image);
	std::vector<int> SelectRange();
	std::vector<int> SelectWindow();
}
