/*
  CS1B – G2: Serendipity
  Partner A: Zee Richmond (1244256) — Partner: A
  Partner B: Alexander Jessen (A00186160) — Partner B
  Date: 2025‑09‑23
  Purpose: Creating interactive menus
  Build: g++ -std=c++20 mainmenu.cpp cashier.cpp invmenu.cpp reports.cpp -o
  serendipity
*/

#include "cashier.h"
#include "invmenu.h"
#include "reports.h"
#include <iostream>
#include <limits>

using namespace std;

int main() {
  int choice = 0;
  do {
    // clear screen
    cout << "\033[H\033[2J";

    cout << "Serendipity Booksellers\n";
    cout << "Main Menu\n\n";
    cout << "1. Cashier Module\n";
    cout << "2. Inventory Database\n";
    cout << "3. Reports\n";
    cout << "4. Exit\n" << endl;

    cout << "Enter your choice (1-4): ";
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch (choice) {
    case 1:
      cashier();
      break;
    case 2:
      invMenu();
      break;
    case 3:
      reports();
      break;
    case 4:
      cout << "Exiting..." << endl;
      break;
    default:
      cout << "Invalid Choice." << endl;
    }
  } while (choice != 4);

  return 0;
}
