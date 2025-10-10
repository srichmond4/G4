/*
  CS1B – G2: Serendipity
  Partner A: Zee Richmond (1244256) — Partner: A
  Partner B: Alexander Jessen (A00186160) — Partner B
  Date: 2025‑09‑23
  Purpose: Creating interactive menus
  Build: g++ -std=c++20 mainmenu.cpp cashier.cpp invmenu.cpp reports.cpp
         booktype.cpp menuutils.cpp -o serendipity
*/

#include "invmenu.h"
#include "booktype.h"
#include "menuutils.h"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static void flushLine() {
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static int getChoiceInRange(int lo, int hi,
                            const string &prompt = "Enter your choice: ") {
  int choice;
  while (true) {
    cout << prompt;
    if (cin >> choice && choice >= lo && choice <= hi) {
      flushLine();
      return choice;
    }
    cout << "Invalid choice. Try again.\n";
    flushLine();
  }
}

namespace {
constexpr size_t kMaxBooks = 20;
// Using std::vector to store bookType objects with a fixed logical capacity of 20.
vector<bookType> inventory;

string formatAmount(bool isSet, float value) {
  ostringstream out;
  out << fixed << setprecision(2) << (isSet ? value : 0.0f);
  return out.str();
}

bool isValidDate(const string &value) {
  if (value.size() != 10)
    return false;
  if (!isdigit(value[0]) || !isdigit(value[1]) || value[2] != '/' ||
      !isdigit(value[3]) || !isdigit(value[4]) || value[5] != '/' ||
      !isdigit(value[6]) || !isdigit(value[7]) || !isdigit(value[8]) ||
      !isdigit(value[9]))
    return false;

  int month = stoi(value.substr(0, 2));
  int day = stoi(value.substr(3, 2));
  int year = stoi(value.substr(6, 4));

  if (month < 1 || month > 12)
    return false;
  if (day < 1 || day > 31)
    return false;
  if (year < 0)
    return false;

  return true;
}

string promptNonEmptyString(const string &prompt) {
  while (true) {
    cout << prompt;
    string input;
    getline(cin, input);
    if (!input.empty())
      return input;
    cout << "Input cannot be blank. Please try again.\n";
  }
}

int promptNonNegativeInt(const string &prompt) {
  while (true) {
    cout << prompt;
    string line;
    getline(cin, line);
    stringstream ss(line);
    int value;
    char leftover;
    if (ss >> value && !(ss >> leftover) && value >= 0)
      return value;
    cout << "Please enter a non-negative whole number.\n";
  }
}

float promptNonNegativeFloat(const string &prompt) {
  while (true) {
    cout << prompt;
    string line;
    getline(cin, line);
    stringstream ss(line);
    float value;
    char leftover;
    if (ss >> value && !(ss >> leftover) && value >= 0.0f)
      return value;
    cout << "Please enter a non-negative value.\n";
  }
}

string promptDate(const string &prompt) {
  while (true) {
    cout << prompt;
    string line;
    getline(cin, line);
    if (isValidDate(line))
      return line;
    cout << "Please enter a date in mm/dd/yyyy format.\n";
  }
}

struct DraftBook {
  string title;
  bool titleSet = false;
  string isbn;
  bool isbnSet = false;
  string author;
  bool authorSet = false;
  string publisher;
  bool publisherSet = false;
  string dateAdded;
  bool dateSet = false;
  int quantity = 0;
  bool quantitySet = false;
  float wholesale = 0.0f;
  bool wholesaleSet = false;
  float retail = 0.0f;
  bool retailSet = false;

  bool anyFieldSet() const {
    return titleSet || isbnSet || authorSet || publisherSet || dateSet ||
           quantitySet || wholesaleSet || retailSet;
  }

  bool readyToSave() const {
    return titleSet && isbnSet && authorSet && publisherSet && dateSet &&
           quantitySet && wholesaleSet && retailSet;
  }
};

void waitForEnter() {
  cout << "\nPress ENTER to continue...";
  cin.get();
}

void renderInventoryMenuScreen(int highlight) {
  constexpr int innerWidth = 54;
  menu::clearScreen();
  menu::drawBorderLine(innerWidth);
  menu::printCenteredLine("Serendipity Booksellers", innerWidth);
  menu::printCenteredLine("Inventory Database", innerWidth);
  menu::printEmptyLine(innerWidth);

  const vector<string> options = {
      "1. Look Up a Book",      "2. Add a Book",
      "3. Edit a Book's Record","4. Delete a Book",
      "5. Return to the Main Menu"};

  for (size_t i = 0; i < options.size(); ++i) {
    bool isHighlighted = (static_cast<int>(i) + 1 == highlight);
    menu::printMenuLine(options[i], innerWidth, isHighlighted);
  }

  menu::printEmptyLine(innerWidth);
  menu::printMenuLine("Enter the menu number and press ENTER.", innerWidth);
  menu::drawBorderLine(innerWidth);
}

void renderAddBookScreen(const DraftBook &draft, int highlight,
                         const string &footer) {
  constexpr int innerWidth = 66;
  constexpr int leftWidth = 32;
  constexpr int rightWidth = innerWidth - leftWidth - 3;

  menu::clearScreen();
  menu::drawBorderLine(innerWidth);
  menu::printCenteredLine("Serendipity Booksellers", innerWidth);
  menu::printCenteredLine("Add a Book", innerWidth);
  menu::printEmptyLine(innerWidth);
  ostringstream meta;
  meta << "DATABASE SIZE: " << kMaxBooks
       << "   CURRENT COUNT: " << bookType::getBookCount();
  menu::printMenuLine(meta.str(), innerWidth);
  menu::printEmptyLine(innerWidth);

  const vector<string> menuItems = {
      "1. Enter Book Title",       "2. Enter ISBN",
      "3. Enter Author",           "4. Enter Publisher",
      "5. Enter Date Added (mm/dd/yyyy)", "6. Enter Quantity on Hand",
      "7. Enter Wholesale Cost",   "8. Enter Retail Price",
      "9. Save Book to Database",  "10. Return to Inventory Menu"};

  const vector<pair<string, string>> fieldSummaries = {
      {"Title", draft.titleSet ? draft.title : "UNSET"},
      {"ISBN", draft.isbnSet ? draft.isbn : "UNSET"},
      {"Author", draft.authorSet ? draft.author : "UNSET"},
      {"Publisher", draft.publisherSet ? draft.publisher : "UNSET"},
      {"Date Added", draft.dateSet ? draft.dateAdded : "UNSET"},
      {"QOH", draft.quantitySet ? to_string(draft.quantity) : "0"},
      {"Wholesale Value", formatAmount(draft.wholesaleSet, draft.wholesale)},
      {"Retail Value", formatAmount(draft.retailSet, draft.retail)}};

  for (size_t i = 0; i < menuItems.size(); ++i) {
    bool isHighlighted = (static_cast<int>(i) + 1 == highlight);
    string rightText;
    if (i < fieldSummaries.size()) {
      rightText = fieldSummaries[i].first + " : " + fieldSummaries[i].second;
    }
    menu::printDualColumnLine(menuItems[i], isHighlighted, leftWidth, rightText,
                              rightWidth, innerWidth);
  }

  menu::printEmptyLine(innerWidth);
  menu::printMenuLine(footer, innerWidth);
  menu::drawBorderLine(innerWidth);
}
} // namespace

void lookUpBook() {
  cout << "\n[lookUpBook stub]\nPress ENTER...";
  cin.get();
}

void addBook() {
  if (inventory.size() >= kMaxBooks) {
    DraftBook empty;
    renderAddBookScreen(empty, 0, "Database is full. Press ENTER to return...");
    cout << "\nThe inventory database is full. Remove a book before adding a new "
            "record.\n";
    waitForEnter();
    return;
  }

  DraftBook draft;
  int highlight = 1;
  const string defaultFooter = "Enter a number (1-10), then press ENTER.";
  string footer = defaultFooter;

  while (true) {
    renderAddBookScreen(draft, highlight, footer);
    footer = defaultFooter;
    int choice = getChoiceInRange(1, 10, "\nEnter selection (1-10): ");
    highlight = choice;

    switch (choice) {
    case 1:
      draft.title = promptNonEmptyString("Enter Book Title: ");
      draft.titleSet = true;
      break;
    case 2:
      draft.isbn = promptNonEmptyString("Enter ISBN: ");
      draft.isbnSet = true;
      break;
    case 3:
      draft.author = promptNonEmptyString("Enter Author: ");
      draft.authorSet = true;
      break;
    case 4:
      draft.publisher = promptNonEmptyString("Enter Publisher: ");
      draft.publisherSet = true;
      break;
    case 5:
      draft.dateAdded = promptDate("Enter Date Added (mm/dd/yyyy): ");
      draft.dateSet = true;
      break;
    case 6:
      draft.quantity = promptNonNegativeInt("Enter Quantity on Hand: ");
      draft.quantitySet = true;
      break;
    case 7:
      draft.wholesale = promptNonNegativeFloat("Enter Wholesale Cost: ");
      draft.wholesaleSet = true;
      break;
    case 8:
      draft.retail = promptNonNegativeFloat("Enter Retail Price: ");
      draft.retailSet = true;
      break;
    case 9:
      if (inventory.size() >= kMaxBooks) {
        renderAddBookScreen(draft, highlight,
                            "Unable to save. Database already full.");
        waitForEnter();
        return;
      }
      if (!draft.readyToSave()) {
        footer = "Complete all fields before saving.";
        break;
      }
      inventory.emplace_back(draft.isbn, draft.title, draft.author,
                             draft.publisher, draft.dateAdded, draft.quantity,
                             draft.wholesale, draft.retail);
      renderAddBookScreen(draft, highlight, "Book saved to database.");
      waitForEnter();
      return;
    case 10:
      if (!draft.anyFieldSet()) {
        return;
      } else {
        renderAddBookScreen(draft, highlight, "Discard current entry? (y/n)");
        cout << "\nDiscard current entry? (y/n): ";
        string response;
        getline(cin, response);
        if (!response.empty() &&
            (response[0] == 'y' || response[0] == 'Y')) {
          return;
        }
      }
      break;
    }
  }
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
  int highlight = 1;
  int choice = 0;
  do {
    renderInventoryMenuScreen(highlight);
    choice = getChoiceInRange(1, 5, "\nEnter selection (1-5): ");
    highlight = choice;

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
