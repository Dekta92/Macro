#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>
#include <ctime>

int main() {
    Display* display;
    Window rootWindow;
    int rootX, rootY;
    int oldrootX, oldrootY;
    int winX, winY; 
    XEvent event;
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

    // Intialize Pointer Array for storing data
    struct PointerData{
        short int Xcoord;
        short int Ycoord;
        Bool LMB;
        Bool MMB;
        Bool RMB;       
        Bool scrollUp; 
        Bool scrollDown;
    };
    PointerData Pointer[50];
    XQueryPointer(display, rootWindow, &returnedRoot, &returnedChild, &rootX, &rootY, &winX, &winY, &mask);
    Pointer[0].Xcoord = rootX;
    Pointer[0].Ycoord = rootY;
    Pointer[0].LMB = False;
    Pointer[0].MMB = False;
    Pointer[0].RMB = False;
    Pointer[0].scrollUp = False;
    Pointer[0].scrollDown = False;

    int timeDelay = 50000; // in microseconds (10^-6 seconds)
    int counter = 1;
    while (true) {
        // XQueryPointer will intialize all the parameters it takes except display and rootWindow
        // winX and winY will be same as rootX and rootY because the root window is passed to XQueryPointer()
        int random = mask & Button1Mask;
        XQueryPointer(display, rootWindow, &returnedRoot, &returnedChild, &rootX, &rootY, &winX, &winY, &mask);
        std::cout << "Pointer is at: (" << rootX << ", " << rootY << ")    " << random << std::endl;
        
        if(mask & Button1Mask){
            Pointer[counter].LMB = True;
        }
        if(mask & Button2Mask){
            Pointer[counter].MMB = True;
        }
        if(mask & Button3Mask){
            Pointer[counter].RMB = True;
        }
        if(mask & Button4Mask){
            Pointer[counter].scrollUp = True;
        }
        if(mask & Button5Mask){
            Pointer[counter].scrollDown = True;
        }

        usleep(timeDelay);
        counter += 1;
    }
    XCloseDisplay(display);
}