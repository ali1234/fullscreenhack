// fullscreenhack.c
// Alistair Buxton <a.j.buxton@gmail.com>

// options:
// the real height and width of all your monitors
// this is what tester should report without the hack
#define REAL_WIDTH  (3200)
#define REAL_HEIGHT (1200)

// the height and width of the primary monitor where flash
// goes fullscreen
#define FAKE_WIDTH  (1920)
#define FAKE_HEIGHT (1200)

#include <dlfcn.h>
#include <stdio.h>
#include <X11/Xlib.h>

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
    typedef Status (*func)(Display *, Drawable, Window *, 
                int *, int *, unsigned int *, 
                unsigned int *, unsigned int *, 
                unsigned int *);
    void *handle;
    Status s;
    handle = dlopen("libX11.so", RTLD_LAZY);
    func orig = dlsym(handle, "XGetGeometry");
    s = orig(display, d, root_return, 
                x_return, y_return, 
                width_return, height_return, 
                border_width_return, depth_return);

    if(*width_return == REAL_WIDTH) *width_return = FAKE_WIDTH;
    if(*height_return == REAL_HEIGHT) *height_return = FAKE_HEIGHT;
    return s;

}


