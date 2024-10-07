#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>

int main() {
    Display* display;
    Window rootWindow;
    int rootX, rootY;
    int winX, winY; 
    unsigned int mask;
    Window returnedRoot, returnedChild; 
    // returnedRoot will always give the root window
    // returnedChild will give the ID of the window under the pointer

    display = XOpenDisplay(nullptr);  // Opens a connection to X server and returns a pointer to Display structure
    if (display == nullptr) {
        std::cout << "Cannot open display\n";
        return 1;
    }

    rootWindow = DefaultRootWindow(display); // Initializes the window variable with the root window.

    while (true) {
        // XQueryPointer will intialize all the parameters it takes except display and rootWindow
        // winX and winY will be same as rootX and rootY because the root window is passed to XQueryPointer()
        XQueryPointer(display, rootWindow, &returnedRoot, &returnedChild, &rootX, &rootY, &winX, &winY, &mask);

        std::cout << "Pointer is at: (" << rootX << ", " << rootY << ")\n";
        usleep(50000);  // Sleep for 50ms
    }
    XCloseDisplay(display);
    return 0;
}
