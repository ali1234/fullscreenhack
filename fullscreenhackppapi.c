// fullscreenhack.c
// Alistair Buxton <a.j.buxton@gmail.com>

#define XLIB_ILLEGAL_ACCESS

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>

typedef Status (*xgg_func)(Display *, Drawable, Window *, 
				int *, int *, unsigned int *, 
				unsigned int *, unsigned int *, 
				unsigned int *);

typedef XineramaScreenInfo * (*xsi_func)(Display*,int*);

typedef int (*xfree_func) (void *);

typedef Bool (*xqp_func) (Display *, Window, Window *,
              Window *, int *, int *,
              int *, int *, unsigned int *);

typedef Display * (*xod_func) (_Xconst char *);


void __attribute__ ((constructor)) load(void);

// Called when the library is loaded and before dlopen() returns
void load(void)
{
    fprintf(stderr, "fullscreen hack loaded...\n");
}

Display *XOpenDisplay(_Xconst char *display_name) {
    void *xlib_handle = dlopen("libX11.so", RTLD_LAZY);
    xod_func xod = dlsym(xlib_handle, "XOpenDisplay");
    Display *d = xod(display_name);
    xfree_func XFree = dlsym(xlib_handle, "XFree");
    void *xin_handle;
    XineramaScreenInfo *screens;
    int n_screens;
    xin_handle = dlopen("libXinerama.so", RTLD_LAZY);
    xsi_func qs = dlsym(xin_handle, "XineramaQueryScreens");
    screens = qs(d, &n_screens);

    d->screens[0].width=screens[0].width;
    d->screens[0].height=screens[0].height;
    XFree(screens);

    fprintf(stderr, "patched XOpenDisplay: %dx%d\n", d->screens[0].width, d->screens[0].height);

    return d;
}


