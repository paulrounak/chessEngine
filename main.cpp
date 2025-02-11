#include <iostream>
#include <string>

using namespace std;

void uci_loop() {
    string command;
    
    while (getline(cin, command)) {
        if (command == "uci") {
            cout << "id name chessEngine" << endl;
            cout << "id author Rounak Paul" << endl;
            cout << "uciok" << endl;
        } 
        else if (command == "isready") {
            cout << "readyok" << endl;
        } 
        else if (command.rfind("position", 0) == 0) {
            // TODO: Parse FEN or move list
            cout << "Position set" << endl;
        } 
        else if (command.rfind("go", 0) == 0) {
            // TODO: Implement move search logic
            cout << "bestmove h2h4" << endl;  // Dummy move
        } 
        else if (command == "quit") {
            break;
        }
    }
}

int main() {
    uci_loop();
    return 0;
}
