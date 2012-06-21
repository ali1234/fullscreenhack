
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

Display *dis;
Screen *scr;
Window wnd;

int main() {
Window wr;
int x,y;
unsigned int w,h, bw, d;

int num_sizes;

dis = XOpenDisplay(NULL);
wnd = XRootWindow(dis, 0);

XGetGeometry(dis, wnd, &wr, &x, &y, &w, &h, &bw, &d);

printf("XGetGeometry reports width=%d height=%d\n", w, h);

return(0);
}


