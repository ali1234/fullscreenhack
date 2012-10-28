// fullscreenhack.c
// Alistair Buxton <a.j.buxton@gmail.com>

// Common functions for npapi and ppapi.

// "running_under_flash" detection code from libvdpau "smurf bug" workaround.

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>

int _running_under_flash = 0;

void fshack_init_running_under_flash(void)
{
 FILE *fp;
 char buffer[1024];
 int ret, i;

 fp = fopen("/proc/self/cmdline", "r");
 if (!fp) {
 return;
 }
 ret = fread(buffer, 1, sizeof(buffer) - 1, fp);
 fclose(fp);
 if (ret < 0) {
 return;
 }
 /*
 * Sometimes the file contains null between arguments. Wipe these out so
 * strstr doesn't stop early.
 */
 for (i = 0; i < ret; i++) {
 if (buffer[i] == '\0') {
 buffer[i] = ' ';
 }
 }
 buffer[ret] = '\0';
 fprintf(stderr, "--- %s ---\n", buffer);
 if (strstr(buffer, "libflashplayer") != NULL) {
 _running_under_flash = 1;
 }
}

// event mask filter prevents flash exiting fullscreen on focus switch.
// by Steve Purchase <steve@t220.com>

typedef int (*xselectinput_func)(Display*, Window, long);

int XSelectInput(Display* display, Window window, long event_mask)
{
    void *xlib_handle;
    xlib_handle = dlopen("libX11.so", RTLD_LAZY);
    xselectinput_func fn = dlsym(xlib_handle, "XSelectInput");

    // Mask out PropertyChange events so Flash doesn't realise
    // the pointer/focus has left the window.
    if(_running_under_flash) {
        event_mask &= ~PropertyChangeMask;
    }

    return fn(display, window, event_mask);
}
