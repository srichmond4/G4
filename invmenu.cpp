/*
  CS1B – G2: Serendipity
  Partner A: Zee Richmond (1244256) — Partner: A
  Partner B: Alexander Jessen (A00186160) — Partner B
  Date: 2025‑09‑23
  Purpose: Creating interactive menus
  Build: g++ -std=c++20 mainmenu.cpp cashier.cpp invmenu.cpp reports.cpp -o
  serendipity
*/

#include "invmenu.h"
#include <iostream>
#include <limits>

using namespace std;

static void clearScreen() { cout << "\033[H\033[2J"; }

static void flushLine() {
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static int getChoiceInRange(int lo, int hi) {
  int choice;
  while (true) {
    cout << "Enter your choice (" << lo << "-" << hi << "): ";
    if (cin >> choice && choice >= lo && choice <= hi) {
      flushLine();
      return choice;
    }
    cout << "Invalid choice. Try again.\n";
    flushLine();
  }
}

void lookUpBook() {
  cout << "\n[lookUpBook stub]\nPress ENTER...";
  cin.get();
}
void addBook() {
  cout << "\n[addBook stub]\nPress ENTER...";
  cin.get();
}
void editBook() {
  cout << "\n[editBook stub]\nPress ENTER...";
  cin.get();
}
void deleteBook() {
  cout << "\n[deleteBook stub]\nPress ENTER...";
  cin.get();
}

void invMenu() {
  int choice = 0;
  do {
    clearScreen();
    cout << "Serendipity Booksellers\n";
    cout << "Inventory Database\n\n";
    cout << "1. Look Up a Book\n";
    cout << "2. Add a Book\n";
    cout << "3. Edit a Book's Record\n";
    cout << "4. Delete a Book\n";
    cout << "5. Return to the Main Menu\n\n";

    choice = getChoiceInRange(1, 5);

    switch (choice) {
    case 1:
      lookUpBook();
      break;
    case 2:
      addBook();
      break;
    case 3:
      editBook();
      break;
    case 4:
      deleteBook();
      break;
    case 5:
      break;
    }
  } while (choice != 5);
}
