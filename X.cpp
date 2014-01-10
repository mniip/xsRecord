#include <vector>
#include <stdexcept>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

namespace X
{
	Display *Display;
	Window Root;
	int Width, Height;

	void Init()
	{
		Display = XOpenDisplay(NULL);
		Root = DefaultRootWindow(Display);
		XWindowAttributes attr;
		XGetWindowAttributes(Display, Root, &attr);
		Width = attr.width;
		Height = attr.height;
	}

	XImage *Pull(int x, int y, int w, int h)
	{
		XImage *image = XGetImage(Display, Root, x, y, w, h, AllPlanes, ZPixmap);
		int dummy_int;
		unsigned int dummy_unsigned;
		Window dummy_window;
		int cursorx, cursory;
		if(XQueryPointer(Display, Root, &dummy_window, &dummy_window, &cursorx, &cursory, &dummy_int, &dummy_int, &dummy_unsigned) != True)
			throw std::runtime_error("Could not query pointer");
		cursorx -= x;
		cursory -= y;
		if(cursorx >= 0 && cursory >= 0 && cursorx < w && cursory < h)
			XPutPixel(image, cursorx, cursory, ~XGetPixel(image, cursorx, cursory));
		return image;
	}

	void Destroy(XImage *image)
	{
		XDestroyImage(image);
	}

	void Push(Drawable window, XImage *image)
	{
		GC gc = XCreateGC(Display, window, 0, 0);
		XPutImage(Display, window, gc, image, 0, 0, 0, 0, image->width, image->height);
		XFreeGC(Display, gc);
		Destroy(image);
	}

	std::vector<int> SelectRange()
	{
		Cursor cursor = XCreateFontCursor(Display, XC_tcross);
		if(XGrabPointer(Display, Root, False, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, Root, cursor, CurrentTime) != GrabSuccess)
			throw std::runtime_error("Could not grab pointer");
		int x1, y1, x2, y2;
		XEvent event;
		while(1)
		{
			XNextEvent(Display, &event);
			if(event.type == ButtonPress)
			{
				x1 = event.xbutton.x;
				y1 = event.xbutton.y;
			}
			else if(event.type == ButtonRelease)
			{
				x2 = event.xbutton.x;
				y2 = event.xbutton.y;
				break;
			}
		}
		XUngrabPointer(Display, CurrentTime);
		std::vector<int> result;
		if(x1 > x2)
		{
			int i = x1;
			x1 = x2;
			x2 = i;
		}
		else if(x1 == x2)
			x2++;
		if(y1 > y2)
		{
			int i = y1;
			y1 = y2;
			y2 = i;
		}
		else if(y1 == y2)
			y2++;
		result.push_back(x1);
		result.push_back(y1);
		result.push_back(x2);
		result.push_back(y2);
		return result;
	}
	
	std::vector<int> SelectWindow()
	{
		Cursor cursor = XCreateFontCursor(Display, XC_tcross);
		if(XGrabPointer(Display, Root, False, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, Root, cursor, CurrentTime) != GrabSuccess)
			throw std::runtime_error("Could not grab pointer");
		int x1, y1, x2, y2;
		XEvent event;
		while(1)
		{
			XNextEvent(Display, &event);
			if(event.type == ButtonRelease)
			{
				Window dummy_window, target;
				int dummy_int;
				unsigned int dummy_unsigned;
				if(XQueryPointer(Display, Root, &dummy_window, &target, &dummy_int, &dummy_int, &dummy_int, &dummy_int, &dummy_unsigned) != True)
					throw std::runtime_error("Could not query pointer");
				XWindowAttributes attr;
				XGetWindowAttributes(Display, target, &attr);
				x1 = attr.x;
				y1 = attr.y;
				x2 = attr.x + attr.width;
				y2 = attr.y + attr.height;
				break;
			}
		}
		XUngrabPointer(Display, CurrentTime);
		std::vector<int> result;
		if(x1 == x2)
			x2++;
		if(y1 == y2)
			y2++;
		result.push_back(x1);
		result.push_back(y1);
		result.push_back(x2);
		result.push_back(y2);
		return result;
	}

}
