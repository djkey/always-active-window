#include <iostream>
#include <string>
#include "WindowPinning.h"
#include "ProcessSelector.h"

using namespace std;

void printHelp() {
    cout << "\nAlways active window\n"
        << "Options:\n"
        << "  PID <id>     Pins the window of the process with the specified PID on top.\n"
        << "  name <name>  Pins the window of the process with the specified name on top.\n"
        << "  help         Displays this message and exits the program.\n"
        << "\nExamples:\n"
        << "  prog.exe PID 1234\n"
        << "  prog.exe name notepad.exe\n";
}

int main(int argc, char* argv[]) {
    string option;

    if (argc > 1) {
        option = argv[1];

        if ((option == "PID" && argc > 3 && string(argv[3]) == "name") ||
            (option == "name" && argc > 3 && string(argv[3]) == "PID")) {
            cerr << "Error: You cannot specify both PID and name at the same time.\n";
            return 1;
        }

        if (option == "PID" && argc > 2) {
            DWORD value = atoi(argv[2]);
            WindowPinning pinningByPid(value);
            pinningByPid.StartPinning();
        }
        else if (option == "name" && argc > 2) {
            string value = argv[2];
            WindowPinning pinningByName(value);
            pinningByName.StartPinning();
        }
        else if (option == "help") {
            printHelp();
        }
        else {
            cerr << "Invalid arguments. Use 'help' for usage instructions.\n";
        }
    }
    else {
        cout << "No arguments provided. Launching window selection mode.\n";

        ProcessSelector selector;
        DWORD selectedPid = selector.run();  
        if (selectedPid != 0) {
            WindowPinning pinningBySelectedPid(selectedPid);
            pinningBySelectedPid.StartPinning();
        }
        else {
            cout << "No window selected.\n";
        }
    }

    return 0;
}
