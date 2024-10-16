#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>

struct PointerData {
    short int Xcoord;
    short int Ycoord;
    bool LMB;
    bool MMB;
    bool RMB;
    bool scrollUp;
    bool scrollDown;
};

struct KeyboardData {
    short int counter;
    char keysArray[50][256];
};

void SavePressedKeys(Display* display, KeyboardData& keyboardData) {
    char keys[32]; 
    XQueryKeymap(display, keys);

    int currentIndex = keyboardData.counter % 50;
    int position = 0;

    for (int i = 0; i < 256; ++i) {
        int byteIndex = i / 8;
        int bitIndex = i % 8;

        if (keys[byteIndex] & (1 << bitIndex)) {  // If the key is pressed
            KeyCode keycode = i;
            KeySym keysym = XKeycodeToKeysym(display, keycode, 0);  // Get the KeySym for the keycode

            if (keysym != NoSymbol) {
                char* keyString = XKeysymToString(keysym);  // Convert KeySym to string for easier readability

                // Copy the KeySym string to the array
                if (keyString && position < 255) {
                    std::strncpy(&keyboardData.keysArray[currentIndex][position], keyString, 255 - position);
                    position += std::strlen(keyString);
                    keyboardData.keysArray[currentIndex][position] = ' ';
                    position++;
                }
            }
        }
    }
    keyboardData.keysArray[currentIndex][position] = '\0';
    keyboardData.counter++;
}

int Recorder(std::string filename) {
    Display* display;
    Window rootWindow;
    int rootX, rootY;
    int winX, winY;
    unsigned int mask;
    Window returnedRoot, returnedChild;
    // returnedRoot will always give the root window
    // returnedChild will give the ID of the window under the pointer

    display = XOpenDisplay(nullptr);
    if (!display) {
        std::cout << "Cannot open display\n";
        return 1;
    }
    rootWindow = DefaultRootWindow(display);

    // Initialize PointerData
    PointerData Pointer[50];
    for (int i = 0; i < 50; ++i) {
        Pointer[i] = {0, 0, false, false, false, false, false};
    }

    // Initialize KeyboardData
    KeyboardData keyboardData = {0};

    int timeDelay = 50000;  // in microseconds (10^-6 seconds)
    int counter = 0;

    while (true) {
        // Get pointer and keyboard data
        XQueryPointer(display, rootWindow, &returnedRoot, &returnedChild, &rootX, &rootY, &winX, &winY, &mask);

        // Write pointer data to array
        std::cout << "Pointer is at: (" << rootX << ", " << rootY << ")\n";
        Pointer[counter].Xcoord = rootX;
        Pointer[counter].Ycoord = rootY;
        Pointer[counter].LMB = (mask & Button1Mask) != 0;
        Pointer[counter].MMB = (mask & Button2Mask) != 0;
        Pointer[counter].RMB = (mask & Button3Mask) != 0;
        Pointer[counter].scrollUp = (mask & Button4Mask) != 0;
        Pointer[counter].scrollDown = (mask & Button5Mask) != 0;

        // Save keyboard data (pressed keys) to array
        SavePressedKeys(display, keyboardData);

        // Log data to file every 50 iterations
        if ((counter + 1) % 50 == 0) {
            std::ofstream outFile(filename, std::ios::app);
            if (!outFile) {
                std::cerr << "Error opening file!\n";
                break;
            }
            for (int i = 0; i < 50; ++i) {
                outFile << Pointer[i].Xcoord << ", "
                        << Pointer[i].Ycoord << ", "
                        << Pointer[i].LMB << ", "
                        << Pointer[i].MMB << ", "
                        << Pointer[i].RMB << ", "
                        << Pointer[i].scrollUp << ", "
                        << Pointer[i].scrollDown << "\n";
                outFile << keyboardData.keysArray[i] << "\n";
            }
            outFile.close();
        }

        usleep(timeDelay);
        counter++;
        counter = counter % 50;
    }
    XCloseDisplay(display);
    return 0;
}

int main() {
    std::cout << "########################### Macro by Dekta92 on Github ###########################\n\n";

    std::string filename = "";
    bool programContinue = true;
    int choice = 0;
    while(programContinue){
        std::cout << "Please select your choice (Press 1/2)\n"
                  << "1. Record\n"
                  << "2. Exit\n";
        std::cin >> choice;
        switch (choice)
        {
        case 1:
            std::cout << "Please input a name for your recording file\n";
            std::cin >> filename;
            filename += ".txt";
            Recorder(filename);
            break;

        case 2:
            programContinue = False;
        }

    }
}
