#include <iostream>
#include <fstream> 
#include <cstdlib>
#include <ctime>
#include <map>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>

using namespace std;


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"


map<int, int> snakes;
map<int, int> ladders;
int numPlayers = 2;
int positions[5];
bool hasShield[5];
int totalRolls[5];
string playerNames[5]; 



void saveHighScore(string name, int rolls) {
    ofstream outFile("highscores.txt", ios::app);
    if (outFile.is_open()) {
        outFile << "Winner: " << left << setw(15) << name << " | Total Rolls: " << rolls << endl;
        outFile.close();
        cout << GREEN << "\n   [System] High score saved to highscores.txt!" << RESET << endl;
    }
    else {
        cout << RED << "\n   [Error] Could not save high score." << RESET << endl;
    }
}


void viewHighScores() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    cout << MAGENTA << "============================================================\n";
    cout << "              " << BOLD << "GLOBAL LEADERBOARD" << RESET << MAGENTA << "           \n";
    cout << "============================================================\n" << RESET;

    ifstream inFile("highscores.txt");
    if (inFile.is_open()) {
        string line;
        int count = 0;
        cout << YELLOW << "\n   Past Winners History:\n" << RESET;
        cout << "   --------------------------------------------------\n";
        while (getline(inFile, line)) {
            cout << "   " << line << endl;
            count++;
        }
        if (count == 0) cout << "   No entries found in the file yet.\n";
        inFile.close();
    }
    else {
        cout << RED << "\n   [Notice] No high score file found yet. Play a game first!" << RESET << endl;
    }

    cout << "\n   Press ENTER to return to menu...";
    string dummy;
    getline(cin, dummy);
}


void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void sleepDelay(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

void playSound() {
    cout << "\a";
}

string getPlayerColor(int pIndex) {
    if (pIndex == 1) return BLUE;
    if (pIndex == 2) return YELLOW;
    if (pIndex == 3) return CYAN;
    if (pIndex == 4) return MAGENTA;
    return RESET;
}

void setupBoard() {
    snakes[99] = 78; snakes[95] = 56; snakes[88] = 24;
    snakes[62] = 18; snakes[48] = 26; snakes[36] = 6;
    snakes[32] = 10;
    ladders[4] = 14;  ladders[9] = 31;  ladders[21] = 42;
    ladders[28] = 84; ladders[51] = 67; ladders[71] = 91;
    ladders[80] = 100;
}

int rollDice() {
    return rand() % 6 + 1;
}

void drawDice(int value) {
    cout << CYAN << "   Dice Result: " << BOLD << value << RESET << "\n";
    if (value == 1) cout << "   +-----+\n   |     |\n   |  o  |\n   |     |\n   +-----+\n";
    else if (value == 2) cout << "   +-----+\n   | o   |\n   |     |\n   |   o |\n   +-----+\n";
    else if (value == 3) cout << "   +-----+\n   | o   |\n   |  o  |\n   |   o |\n   +-----+\n";
    else if (value == 4) cout << "   +-----+\n   | o o |\n   |     |\n   | o o |\n   +-----+\n";
    else if (value == 5) cout << "   +-----+\n   | o o |\n   |  o  |\n   | o o |\n   +-----+\n";
    else if (value == 6) cout << "   +-----+\n   | o o |\n   | o o |\n   | o o |\n   +-----+\n";
}

void drawHeader() {
    cout << MAGENTA << "============================================================\n";
    cout << "        " << BOLD << "SNAKES & LADDERS : ULTIMATE DELUXE EDITION" << RESET << MAGENTA << "           \n";
    cout << "============================================================\n" << RESET;
}

void drawBoard() {
    cout << "\n";
    int rowStart = 100;
    for (int r = 0; r < 10; r++) {
        cout << "  +";
        for (int k = 0; k < 10; k++) cout << "----+";
        cout << "\n  |";
        int cells[10];
        for (int c = 0; c < 10; c++) {
            if (r % 2 == 0) cells[c] = rowStart - c;
            else cells[c] = (rowStart - 9) + c;
        }
        for (int c = 0; c < 10; c++) {
            int currentNum = cells[c];
            string playerStr = "";
            for (int i = 1; i <= numPlayers; i++) {
                if (positions[i] == currentNum) playerStr += getPlayerColor(i) + "P" + to_string(i) + RESET;
            }
            if (playerStr != "") cout << playerStr << "|";
            else {
                if (snakes.count(currentNum))       cout << RED << " SS " << RESET << "|";
                else if (ladders.count(currentNum)) cout << GREEN << " ## " << RESET << "|";
                else if (currentNum == 50)          cout << CYAN << " 🛡️ " << RESET << "|";
                else if (currentNum == 25 || currentNum == 75) cout << MAGENTA << " ?? " << RESET << "|";
                else cout << setw(4) << currentNum << "|";
            }
        }
        cout << "\n";
        rowStart -= 10;
    }
    cout << "  +";
    for (int k = 0; k < 10; k++) cout << "----+";
    cout << "\n\n   Legend: " << RED << "SS (Snake) " << GREEN << "## (Ladder) " << CYAN << "🛡️ (Shield) " << MAGENTA << "?? (Mystery Box)\n" << RESET;
}

int handleMysteryBox(int pos) {
    cout << MAGENTA << "\n   ❓ MYSTERY BOX ACTIVATED! ❓\n" << RESET;
    playSound(); sleepDelay(1000);
    int luck = rand() % 3;
    if (luck == 0) { cout << "   --> Lucky! Boost forward 5 steps.\n"; return pos + 5; }
    else if (luck == 1) { cout << "   --> Unlucky! Slide back 5 steps.\n"; return pos - 5; }
    else { cout << "   --> Empty box. Nothing happens.\n"; return pos; }
}

int checkSpecialBlocks(int playerIndex, int pos) {
    if (pos == 50) {
        cout << CYAN << "\n   🛡️  " << playerNames[playerIndex] << " EQUIPPED MAGIC SHIELD! 🛡️\n" << RESET;
        hasShield[playerIndex] = true; playSound(); sleepDelay(1000); return pos;
    }
    if (pos == 25 || pos == 75) return handleMysteryBox(pos);
    if (snakes.count(pos)) {
        if (hasShield[playerIndex]) {
            cout << CYAN << "\n   🛡️  SHIELD BLOCKED THE SNAKE! " << playerNames[playerIndex] << " is safe.\n" << RESET;
            hasShield[playerIndex] = false; sleepDelay(1000); return pos;
        }
        else {
            cout << RED << "\n   🐍 SNAKE! " << playerNames[playerIndex] << " sliding down to " << snakes[pos] << "...\n" << RESET;
            playSound(); sleepDelay(1000); return snakes[pos];
        }
    }
    if (ladders.count(pos)) {
        cout << GREEN << "\n   🪜 LADDER! " << playerNames[playerIndex] << " climbing up to " << ladders[pos] << "...\n" << RESET;
        playSound(); sleepDelay(1000); return ladders[pos];
    }
    return pos;
}

void playGame(int mode) {
    for (int i = 0; i < 5; i++) { positions[i] = 1; hasShield[i] = false; totalRolls[i] = 0; }
    clearScreen(); drawHeader();
    if (mode == 1) {
        cout << "   Enter number of players (2-4): "; cin >> numPlayers;
        if (numPlayers < 2) numPlayers = 2; if (numPlayers > 4) numPlayers = 4;
        for (int i = 1; i <= numPlayers; i++) { cout << "   Enter Name for Player " << i << ": "; cin >> playerNames[i]; }
        cin.ignore();
    }
    else if (mode == 2) {
        numPlayers = 2; cout << "   Enter your Name: "; cin >> playerNames[1]; playerNames[2] = "Computer"; cin.ignore();
    }
    else { numPlayers = 2; playerNames[1] = "AI_Alpha"; playerNames[2] = "AI_Beta"; }

    int turn = 1; bool gameOver = false;
    while (!gameOver) {
        bool rollAgain = false;
        do {
            clearScreen(); drawHeader(); drawBoard();
            cout << "   Turn: " << getPlayerColor(turn) << BOLD << playerNames[turn] << RESET;
            if (hasShield[turn]) cout << " [SHIELD ON]";
            cout << "\n   ----------------------------------------\n";
            if (mode == 3 || (mode == 2 && turn == 2)) sleepDelay(800);
            else { cout << "   Press ENTER to Roll Dice..."; string dummy; getline(cin, dummy); }

            int dice = rollDice(); totalRolls[turn]++; drawDice(dice); sleepDelay(1000);
            if (positions[turn] + dice <= 100) {
                positions[turn] += dice;
                positions[turn] = checkSpecialBlocks(turn, positions[turn]);
            }
            if (positions[turn] == 100) {
                clearScreen(); drawHeader(); drawBoard(); playSound();
                cout << GREEN << "\n   🎉 " << playerNames[turn] << " WINS THE GAME! 🎉\n" << RESET;
                saveHighScore(playerNames[turn], totalRolls[turn]); //
                gameOver = true; rollAgain = false;
            }
            else if (dice == 6) { cout << GREEN << "   🌟 ROLLED A 6! FREE TURN! 🌟\n" << RESET; rollAgain = true; sleepDelay(1000); }
            else rollAgain = false;
        } while (rollAgain && !gameOver);
        if (!gameOver) { turn++; if (turn > numPlayers) turn = 1; }
    }
    cout << "\n   Press ENTER to return to menu..."; string dummy; getline(cin, dummy);
}

int main() {
    srand(time(0)); setupBoard();
    while (true) {
        clearScreen(); drawHeader();
        cout << "\n   [1] Human vs Human\n   [2] Human vs Computer\n   [3] Auto-Simulation\n"
             << YELLOW << "   [4] View High Scores (Leaderboard)\n" << RESET
             << "   [5] Exit\n\n   Choice: ";
        int choice;
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        cin.ignore();
        if (choice == 1) playGame(1);
        else if (choice == 2) playGame(2);
        else if (choice == 3) playGame(3);
        else if (choice == 4) viewHighScores(); 
        else if (choice == 5) break;
    }
    return 0;
}