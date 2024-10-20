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

void SavePressedKeys(Display* display, KeySym keysArray[50][256], int& counter) {
    char keys[32]; 
    XQueryKeymap(display, keys);

    int currentIndex = counter % 50;
    int keyIndex = 0;
    
    // Gets the KeySym and store it in the keysArray for all keys
    for (int i = 0; i < 256; ++i) {
        int byteIndex = i / 8;
        int bitIndex = i % 8;

        if (keys[byteIndex] & (1 << bitIndex)) {
            KeyCode keycode = i;
            KeySym keysym = XKeycodeToKeysym(display, keycode, 0);

            if (keysym != NoSymbol) {
                if (keyIndex < 256) {
                    keysArray[currentIndex][keyIndex] = keysym;
                    keyIndex++;
                }
            }
        }
    }

    while (keyIndex < 256) {
        keysArray[currentIndex][keyIndex] = NoSymbol;
        keyIndex++;
    }

    counter++;
}

int Recorder(std::string filename) {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cout << "Cannot open display\n";
        return 1;
    }

    Window rootWindow = DefaultRootWindow(display);
    
    PointerData Pointer[50] = {};
    KeySym keysArray[50][256] = {};
    int keyCounter = 0; 

    int timeDelay = 50000;  // in microseconds (10^-6 seconds)
    int counter = 0;

    std::cout << "Recording...\n";
    while (true) {
        unsigned int mask;
        Window returnedRoot, returnedChild;
        int rootX, rootY, winX, winY;
        
        XQueryPointer(display, rootWindow, &returnedRoot, &returnedChild, &rootX, &rootY, &winX, &winY, &mask);
        Pointer[counter].Xcoord = rootX;
        Pointer[counter].Ycoord = rootY;
        Pointer[counter].LMB = (mask & Button1Mask) != 0;
        Pointer[counter].MMB = (mask & Button2Mask) != 0;
        Pointer[counter].RMB = (mask & Button3Mask) != 0;
        Pointer[counter].scrollUp = (mask & Button4Mask) != 0;
        Pointer[counter].scrollDown = (mask & Button5Mask) != 0;

        SavePressedKeys(display, keysArray, keyCounter);

        if ((counter + 1) % 50 == 0) {
            std::ofstream outFile(filename, std::ios::app);
            if (!outFile) {
                std::cerr << "Error opening file!\n";
                break;
            }

            // Write pointer data
            for (int i = 0; i < 50; ++i) {
                outFile << Pointer[i].Xcoord << ", "
                        << Pointer[i].Ycoord << ", "
                        << Pointer[i].LMB << ", "
                        << Pointer[i].MMB << ", "
                        << Pointer[i].RMB << ", "
                        << Pointer[i].scrollUp << ", "
                        << Pointer[i].scrollDown << "\n";

                for (int j = 0; j < 256 && keysArray[i][j] != NoSymbol; ++j) {
                    outFile << keysArray[i][j] << " ";  // Save as hex
                }
                outFile << "\n";
            }
            outFile.close();
        }

        counter++;
        counter %= 50; // This keeps it within 0-49
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