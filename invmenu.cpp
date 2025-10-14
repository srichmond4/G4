/*
  CS1B – G2: Serendipity
  Partner A: Zee Richmond (1244256) — Partner: A
  Partner B: Alexander Jessen (A00186160) — Partner B
  Date: 2025‑09‑23
  Purpose: Creating interactive menus
  Build: g++ -std=c++20 mainmenu.cpp cashier.cpp invmenu.cpp reports.cpp
         booktype.cpp bookinfo.cpp menuutils.cpp -o serendipity
*/

#include "invmenu.h"
#include "booktype.h"
#include "bookinfo.h"
#include "menuutils.h"
#include <algorithm>
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

string trim(const string &text) {
  size_t start = text.find_first_not_of(" \t\n\r");
  size_t end = text.find_last_not_of(" \t\n\r");
  if (start == string::npos)
    return "";
  return text.substr(start, end - start + 1);
}

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

string toLower(string text) {
  transform(text.begin(), text.end(), text.begin(),
            [](unsigned char c) { return static_cast<char>(tolower(c)); });
  return text;
}

string ellipsize(const string &text, int maxLen) {
  if (maxLen <= 1 || static_cast<int>(text.size()) <= maxLen)
    return text;
  return text.substr(0, maxLen - 1) + "…";
}

// === Render Inv Menu Screen ===
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

// === Render Add Book Screen ===
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
      "9. Save Book to Database",  "0. Return to Inventory Menu"};

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
      string value = fieldSummaries[i].second;
      if (i == 0 && draft.titleSet) {
        value = ellipsize(value, 20);
      }
      rightText = fieldSummaries[i].first + " : " + value;
    }
    menu::printDualColumnLine(menuItems[i], isHighlighted, leftWidth, rightText,
                              rightWidth, innerWidth);
  }

  menu::printEmptyLine(innerWidth);
  menu::printMenuLine(footer, innerWidth);
  menu::drawBorderLine(innerWidth);
}

// === Render LookUp Menu Screen ===
void renderLookUpMenuScreen(int highlight) {
  constexpr int innerWidth = 58;
  menu::clearScreen();
  menu::drawBorderLine(innerWidth);
  menu::printCenteredLine("Serendipity Booksellers", innerWidth);
  menu::printCenteredLine("Look Up Book", innerWidth);
  menu::printEmptyLine(innerWidth);

  const vector<string> options = {"1. Search by Title", "2. Search by ISBN",
                                  "3. Return to Inventory Menu"};

  for (size_t i = 0; i < options.size(); ++i) {
    bool isHighlighted = (static_cast<int>(i) + 1 == highlight);
    menu::printMenuLine(options[i], innerWidth, isHighlighted);
  }

  menu::printEmptyLine(innerWidth);
  menu::printMenuLine("Enter the menu number and press ENTER.", innerWidth);
  menu::drawBorderLine(innerWidth);
}

// === Title Search ===
vector<size_t> searchByTitle(const string &query) {
  vector<size_t> matches;
  string target = toLower(query);
  for (size_t i = 0; i < inventory.size(); ++i) {
    if (toLower(inventory[i].getTitle()).find(target) != string::npos) {
      matches.push_back(i);
    }
  }
  return matches;
}

// === ISBN Search ===
vector<size_t> searchByISBN(const string &query) {
  vector<size_t> matches;
  string target = toLower(query);
  for (size_t i = 0; i < inventory.size(); ++i) {
    if (toLower(inventory[i].getISBN()).find(target) != string::npos) {
      matches.push_back(i);
    }
  }
  return matches;
}

void showNoMatches(const string &query) {
  constexpr int innerWidth = 64;
  menu::clearScreen();
  menu::drawBorderLine(innerWidth);
  menu::printCenteredLine("Serendipity Booksellers", innerWidth);
  menu::printCenteredLine("Look Up Book", innerWidth);
  menu::printEmptyLine(innerWidth);
  string message = "No matches for \"" + query + "\".";
  menu::printMenuLine(ellipsize(message, innerWidth), innerWidth);
  menu::printEmptyLine(innerWidth);
  menu::printMenuLine("Book not in inventory.", innerWidth);
  menu::printEmptyLine(innerWidth);
  menu::printMenuLine("Press ENTER to return...", innerWidth);
  menu::drawBorderLine(innerWidth);
  cin.get();
}

void displaySearchResults(const vector<size_t> &matches, const string &query) {
  vector<string> resultLines;
  resultLines.reserve(matches.size());

  int innerWidth = 70;
  for (size_t i = 0; i < matches.size(); ++i) {
    const bookType &book = inventory[matches[i]];
    string line =
        to_string(i + 1) + ". " + book.getTitle() + "  [ISBN: " + book.getISBN() + "]";
    innerWidth = max(innerWidth, static_cast<int>(line.size()));
    resultLines.push_back(line);
  }

  ostringstream summaryStream;
  summaryStream << matches.size() << " match(es) for \"" << query << "\".";
  string summaryText = summaryStream.str();
  innerWidth = max(innerWidth, static_cast<int>(summaryText.size()));

  const string instructionText = "Enter result number or 0 to cancel.";
  innerWidth = max(innerWidth, static_cast<int>(instructionText.size()));

  while (true) {
    menu::clearScreen();
    menu::drawBorderLine(innerWidth);
    menu::printCenteredLine("Serendipity Booksellers", innerWidth);
    menu::printCenteredLine("Search Results", innerWidth);
    menu::printEmptyLine(innerWidth);

    menu::printMenuLine(summaryText, innerWidth);
    menu::printEmptyLine(innerWidth);

    for (const string &line : resultLines) {
      menu::printMenuLine(line, innerWidth);
    }

    menu::printEmptyLine(innerWidth);
    menu::printMenuLine(instructionText, innerWidth);
    menu::drawBorderLine(innerWidth);
    cout << "\n";

    int selection =
        getChoiceInRange(0, static_cast<int>(matches.size()),
                         matches.empty() ? "Select (0 to cancel): "
                                         : "Select (0 to cancel, 1-"
                                               + to_string(matches.size()) + "): ");
    if (selection == 0) {
      return;
    }

    size_t index = matches[static_cast<size_t>(selection - 1)];
    bookInfo(inventory[index]);
  }
}

void performSearch(bool byTitle) {
  constexpr int innerWidth = 66;
  menu::clearScreen();
  menu::drawBorderLine(innerWidth);
  menu::printCenteredLine("Serendipity Booksellers", innerWidth);
  menu::printCenteredLine(byTitle ? "Search by Title" : "Search by ISBN",
                          innerWidth);
  menu::printEmptyLine(innerWidth);
  menu::printMenuLine("Enter your search text then press ENTER.", innerWidth);
  menu::printEmptyLine(innerWidth);
  menu::drawBorderLine(innerWidth);
  cout << "\n";

  string query = trim(promptNonEmptyString(
      byTitle ? "Enter title keyword(s): " : "Enter ISBN or fragment: "));
  vector<size_t> matches =
      byTitle ? searchByTitle(query) : searchByISBN(query);

  if (matches.empty()) {
    showNoMatches(query);
    return;
  }

  displaySearchResults(matches, query);
}

} // namespace

// === LookUpBook Stuff ===
void lookUpBook() {
  if (inventory.empty()) {
    constexpr int innerWidth = 58;
    menu::clearScreen();
    menu::drawBorderLine(innerWidth);
    menu::printCenteredLine("Serendipity Booksellers", innerWidth);
    menu::printCenteredLine("Look Up Book", innerWidth);
    menu::printEmptyLine(innerWidth);
    menu::printMenuLine("The inventory is currently empty.", innerWidth);
    menu::printEmptyLine(innerWidth);
    menu::printMenuLine("Press ENTER to return...", innerWidth);
    menu::drawBorderLine(innerWidth);
    cin.get();
    return;
  }

  int highlight = 1;
  bool done = false;
  while (!done) {
    renderLookUpMenuScreen(highlight);
    int choice = getChoiceInRange(1, 3, "\nEnter selection (1-3): ");
    highlight = choice;
    switch (choice) {
    case 1:
      performSearch(true);
      break;
    case 2:
      performSearch(false);
      break;
    case 3:
      done = true;
      break;
    }
  }
}

// ==== Add Book Stuff =====
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
    cout << "\nEnter selection (1-10): ";
    string line;
    getline(cin, line);
    stringstream ss(line);
    int choice;
    if (!(ss >> choice) || choice < 1 || choice > 10) {
      footer = "Enter a number between 1 and 10.";
      continue;
    }
    highlight = choice;

    switch (choice) {
    case 1:
      renderAddBookScreen(draft, highlight,
                          "Editing Title. Enter Book Title below.");
      draft.title = promptNonEmptyString("Enter Book Title: ");
      draft.titleSet = true;
      break;
    case 2:
      renderAddBookScreen(draft, highlight,
                          "Editing ISBN. Enter ISBN below.");
      draft.isbn = promptNonEmptyString("Enter ISBN: ");
      draft.isbnSet = true;
      break;
    case 3:
      renderAddBookScreen(draft, highlight,
                          "Editing Author. Enter Author below.");
      draft.author = promptNonEmptyString("Enter Author: ");
      draft.authorSet = true;
      break;
    case 4:
      renderAddBookScreen(draft, highlight,
                          "Editing Publisher. Enter Publisher below.");
      draft.publisher = promptNonEmptyString("Enter Publisher: ");
      draft.publisherSet = true;
      break;
    case 5:
      renderAddBookScreen(
          draft, highlight,
          "Editing Date Added. Enter Date Added (mm/dd/yyyy) below.");
      draft.dateAdded = promptDate("Enter Date Added (mm/dd/yyyy): ");
      draft.dateSet = true;
      break;
    case 6:
      renderAddBookScreen(draft, highlight,
                          "Editing Quantity. Enter Quantity on Hand below.");
      draft.quantity = promptNonNegativeInt("Enter Quantity on Hand: ");
      draft.quantitySet = true;
      break;
    case 7:
      renderAddBookScreen(draft, highlight,
                          "Editing Wholesale Cost. Enter value below.");
      draft.wholesale = promptNonNegativeFloat("Enter Wholesale Cost: ");
      draft.wholesaleSet = true;
      break;
    case 8:
      renderAddBookScreen(draft, highlight,
                          "Editing Retail Price. Enter value below.");
      draft.retail = promptNonNegativeFloat("Enter Retail Price: ");
      draft.retailSet = true;
      break;
    case 9:
      renderAddBookScreen(draft, highlight,
                          "Attempting to save book to database...");
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
    case 0:
      if (!draft.anyFieldSet()) {
        return;
      } else {
        renderAddBookScreen(draft, highlight,
                            "Discard current entry? (y/n) Enter choice below.");
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

	// ==== Inv Menu ====
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
