#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XTest.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
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

// Recording Functions

// Gets the KeySym for all keys and stores it in the keysArray
void SavePressedKeys(Display* display, KeyCode keysArray[50][256], int& counter) {
    char keys[32];
    XQueryKeymap(display, keys);

    int currentIndex = counter % 50;
        int keyIndex = 0;

    for (int i = 0; i < 256; ++i) {
        int byteIndex = i / 8;
        int bitIndex = i % 8;

        if (keys[byteIndex] & (1 << bitIndex)) {
            KeyCode keycode = i;
            if (keyIndex < 256) {
                keysArray[currentIndex][keyIndex] = keycode;
                keyIndex++;
            }
        }
    }

    while (keyIndex < 256) {
        keysArray[currentIndex][keyIndex] = 0;
        keyIndex++;
    }

    counter++;
}

int Recorder(std::string filename, int timeDelay) {

    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cout << "Cannot open display\n";
        return 1;
    }
    Window rootWindow = DefaultRootWindow(display);

    PointerData Pointer[50] = {};
    KeyCode keysArray[50][256] = {};
    int keyCounter = 0;

    int counter = 0;
    while (true) {
        unsigned int mask;
        Window returnedRoot, returnedChild;
        int rootX, rootY, winX, winY;

        // Gets Pointer data
        XQueryPointer(display, rootWindow, &returnedRoot, &returnedChild, &rootX, &rootY, &winX, &winY, &mask);
        Pointer[counter].Xcoord = rootX;
        Pointer[counter].Ycoord = rootY;
        Pointer[counter].LMB = (mask & Button1Mask) != 0;
        Pointer[counter].MMB = (mask & Button2Mask) != 0;
        Pointer[counter].RMB = (mask & Button3Mask) != 0;
        Pointer[counter].scrollUp = (mask & Button4Mask) != 0;
        Pointer[counter].scrollDown = (mask & Button5Mask) != 0;

        // Save keyboard data (pressed keys) to array
        SavePressedKeys(display, keysArray, keyCounter);

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
                for (int j = 0; j < 256 && keysArray[i][j] != NoSymbol; ++j) {
                    outFile << keysArray[i][j];
                }
                outFile << "\n";
            }
            outFile.close();
        }

        usleep(timeDelay);
        counter++;
        counter %= 50;
    }

    XCloseDisplay(display);
    return 0;
}

// Playback Function

int Playback(std::string filename, int timeDelay){
    // Opens display and sets window to root window
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cout << "Cannot open display\n";
        return 1;
    }
    Window rootWindow = DefaultRootWindow(display);

    // Opens file
    std::ifstream recordingFile(filename);
    if (!recordingFile) {
        std::cout << "Error: Unable to open the file " << filename
                  << "\nCheck if file is in a different directory or with a different name\n";
        return 1;
    }

    std::string mouseLine;
    std::string keyboardLine;
    int numberOfKeycodes;

    while (std::getline(recordingFile, mouseLine)) {
        std::stringstream sm(mouseLine);

	    // Grab Mouse data
        int Xcord, Ycord;
        bool LMB, MMB, RMB, scrollUp, scrollDown;
        char comma;

        sm >> Xcord >> comma
           >> Ycord >> comma
           >> LMB >> comma
           >> MMB >> comma
           >> RMB >> comma
           >> scrollUp >> comma
           >> scrollDown;

        // Simulate mouse movements, presses and scrolls respectively
        XTestFakeMotionEvent(display, -1, Xcord, Ycord, CurrentTime);

        if(LMB){XTestFakeButtonEvent(display, 1, True, CurrentTime);}else{XTestFakeButtonEvent(display, 1, False, CurrentTime);}
        if(MMB){XTestFakeButtonEvent(display, 2, True, CurrentTime);}else{XTestFakeButtonEvent(display, 2, False, CurrentTime);}
        if(RMB){XTestFakeButtonEvent(display, 3, True, CurrentTime);}else{XTestFakeButtonEvent(display, 3, False, CurrentTime);}

        if(scrollUp){XTestFakeButtonEvent(display, 4, True, CurrentTime);XTestFakeButtonEvent(display, 4, False, CurrentTime);}
        if(scrollDown){XTestFakeButtonEvent(display, 5, True, CurrentTime);XTestFakeButtonEvent(display, 5, False, CurrentTime);}



	    // Get keyboard data
        std::getline(recordingFile, keyboardLine);
        numberOfKeycodes = keyboardLine.size();
        std::stringstream sk(keyboardLine);

        // Presses keys to be pressed
        for(int i = 0 ; i < numberOfKeycodes; i++){
            XTestFakeKeyEvent(display, keyboardLine[i], True, CurrentTime);
        }

        // Applies all keyboard and mouse events
        XFlush(display);

        // Releases all keys that were pressed
        for(int i = 0 ; i < numberOfKeycodes; i++){
            XTestFakeKeyEvent(display, keyboardLine[i], False, CurrentTime);
        }


        usleep(timeDelay);
    }


    // Cleanup (Releases all mouse and keyboard keys, closes file and closes connection to display)
    for(int i = 1; i <= 5; i++){
        XTestFakeButtonEvent(display, i, False, CurrentTime);
    }
    for(int i = 0 ; i < numberOfKeycodes; i++){
        XTestFakeKeyEvent(display, keyboardLine[i], False, CurrentTime);
    }
    recordingFile.close();
    XCloseDisplay(display);

    return 0;
}

int main() {
    std::cout << "########################### Macro by Dekta92 on Github ###########################\n\n";

    int timeDelay = 80000;  // in microseconds (10^-6 seconds)

    // Main menu
    std::string filename;
    bool programContinue = true;
    int choice = 0;
    while(programContinue) {
        std::cout << "Please select your choice (Press 1/2/3)\n"
                  << "1. Record Macro\n"
                  << "2. Play Macro\n"
                  << "3. Exit\n";
        std::cin >> choice;
        switch (choice) {
            case 1:
                std::cout << "Please input a name for your recording file\n";
                std::cin >> filename;
                Recorder(filename, timeDelay);
                break;
            case 2:
                std::cout << "Please input recording's name with extension name (make sure the file is in the same directory as this file)\n";
                std::cin >> filename;
                Playback(filename, timeDelay);
                std::cout << "Playback Finished!";
                break;
            case 3:
                programContinue = false;
                break;
        }
    }
}
