// fullscreenhack.c
// Alistair Buxton <a.j.buxton@gmail.com>

// the height and width of the primary monitor where flash
// goes fullscreen
#define FAKE_WIDTH  (1920)
#define FAKE_HEIGHT (1200)

#include <dlfcn.h>
#include <stdio.h>
#include <X11/Xlib.h>

typedef Status (*xgg_func)(Display *, Drawable, Window *, 
				int *, int *, unsigned int *, 
				unsigned int *, unsigned int *, 
				unsigned int *);

void __attribute__ ((constructor)) load(void);

// Called when the library is loaded and before dlopen() returns
void load(void)
{
    fprintf(stderr, "fullscreen hack loaded...\n");
}

Status XGetGeometry(Display *display, Drawable d, Window *root_return, 
            int *x_return, int *y_return, 
            unsigned int *width_return, 
            unsigned int *height_return, 
            unsigned int *border_width_return, 
            unsigned int *depth_return)
{
    void *xlib_handle;
    Status s;
    xlib_handle = dlopen("libX11.so", RTLD_LAZY);
    xgg_func xgg = dlsym(xlib_handle, "XGetGeometry");
    s = xgg(display, d, root_return, 
                x_return, y_return, 
                width_return, height_return, 
                border_width_return, depth_return);

    if (d == RootWindow(display, DefaultScreen(display))) {
        *width_return = FAKE_WIDTH;
        *height_return = FAKE_HEIGHT;
    }
    return s;

}


