#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>

using namespace std;

// Forward declaration
class LibraryUser;
class Book;

// Book Class
class Book {
private:
    string title;
    string author;
    string ISBN;
    int publication_year;
    bool is_available;

public:
    Book(string title, string author, string ISBN, int publication_year)
        : title(title), author(author), ISBN(ISBN), publication_year(publication_year), is_available(true) {}

    void display() const {
        cout << "Title: " << title << endl;
        cout << "Author: " << author << endl;
        cout << "ISBN: " << ISBN << endl;
        cout << "Publication Year: " << publication_year << endl;
        cout << "Availability: " << (is_available ? "Available" : "Not Available") << endl;
    }

    string getISBN() const { return ISBN; }
    string getTitle() const { return title; }
    void setAvailability(bool available) { is_available = available; }
    bool isAvailable() const { return is_available; }

    // Friend function for file I/O
    friend ostream& operator<<(ostream& os, const Book& book) {
        os << book.title << "|" << book.author << "|" << book.ISBN << "|" << book.publication_year << "|" << book.is_available;
        return os;
    }

    friend istream& operator>>(istream& is, Book& book) {
        getline(is, book.title, '|');
        getline(is, book.author, '|');
        getline(is, book.ISBN, '|');
        string year_str, avail_str;
        getline(is, year_str, '|');
        getline(is, avail_str);
        book.publication_year = stoi(year_str);
        book.is_available = (avail_str == "1");
        return is;
    }
};

// LibraryUser Class
class LibraryUser {
private:
    string name;
    int user_id;
    vector<Book*> borrowed_books;

public:
    LibraryUser(string name, int user_id) : name(name), user_id(user_id) {}

    ~LibraryUser() {
        // Note: No need to delete Book pointers here, as the Library class manages them.
    }

    void display() const {
        cout << "Name: " << name << endl;
        cout << "User ID: " << user_id << endl;
        cout << "Borrowed Books: " << endl;
        if (borrowed_books.empty()) {
            cout << "  - None" << endl;
        } else {
            for (const auto& book : borrowed_books) {
                cout << "  - " << book->getTitle() << " (ISBN: " << book->getISBN() << ")" << endl;
            }
        }
    }

    int getID() const { return user_id; }
    string getName() const { return name; }
    vector<Book*> getBorrowedBooks() const { return borrowed_books; }

    void borrowBook(Book* book) {
        borrowed_books.push_back(book);
    }

    void returnBook(Book* book) {
        for (auto it = borrowed_books.begin(); it != borrowed_books.end(); ++it) {
            if (*it == book) {
                borrowed_books.erase(it);
                return;
            }
        }
    }

    // Friend function for file I/O
    friend ostream& operator<<(ostream& os, const LibraryUser& user) {
        os << user.name << "|" << user.user_id;
        for (const auto& book : user.borrowed_books) {
            os << "|" << book->getISBN();
        }
        return os;
    }

    friend istream& operator>>(istream& is, LibraryUser& user) {
        getline(is, user.name, '|');
        string id_str;
        getline(is, id_str, '|');
        user.user_id = stoi(id_str);
        return is;
    }
};

// Library Class
class Library {
private:
    vector<Book*> books;
    vector<LibraryUser*> users;
    const string books_filename = "books.txt";
    const string users_filename = "users.txt";

public:
    Library() {
        loadBooksFromFile();
        loadUsersFromFile();
    }

    ~Library() {
        for (auto& book : books) {
            delete book;
        }
        for (auto& user : users) {
            delete user;
        }
    }

    void addBook(Book* book) {
        books.push_back(book);
        saveBooksToFile();
    }

    void addUser(LibraryUser* user) {
        users.push_back(user);
        saveUsersToFile();
    }

    void displayBooks() const {
        cout << "\n--- All Books ---" << endl;
        for (const auto& book : books) {
            book->display();
            cout << "-----------------" << endl;
        }
    }

    void displayUsers() const {
        cout << "\n--- All Users ---" << endl;
        for (const auto& user : users) {
            user->display();
            cout << "-----------------" << endl;
        }
    }

    void saveBooksToFile() {
        ofstream file(books_filename);
        if (file.is_open()) {
            for (const auto& book : books) {
                file << *book << endl;
            }
            file.close();
            cout << "Books saved to " << books_filename << endl;
        } else {
            cerr << "Unable to open file: " << books_filename << endl;
        }
    }

    void loadBooksFromFile() {
        ifstream file(books_filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                // Parse the line
                string title, author, ISBN, year_str, avail_str;
                size_t pos = 0;
                string token;
                
                // Get title
                pos = line.find('|');
                title = line.substr(0, pos);
                line.erase(0, pos + 1);

                // Get author
                pos = line.find('|');
                author = line.substr(0, pos);
                line.erase(0, pos + 1);

                // Get ISBN
                pos = line.find('|');
                ISBN = line.substr(0, pos);
                line.erase(0, pos + 1);

                // Get publication year
                pos = line.find('|');
                year_str = line.substr(0, pos);
                line.erase(0, pos + 1);

                // Get availability
                avail_str = line;

                int year = stoi(year_str);
                bool available = (avail_str == "1");

                Book* newBook = new Book(title, author, ISBN, year);
                newBook->setAvailability(available);
                books.push_back(newBook);
            }
            file.close();
            cout << "Books loaded from " << books_filename << endl;
        } else {
            cout << "No existing books file found. A new one will be created." << endl;
        }
    }

    void saveUsersToFile() {
        ofstream file(users_filename);
        if (file.is_open()) {
            for (const auto& user : users) {
                file << *user << endl;
            }
            file.close();
            cout << "Users saved to " << users_filename << endl;
        } else {
            cerr << "Unable to open file: " << users_filename << endl;
        }
    }

    void loadUsersFromFile() {
        ifstream file(users_filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                // Parse the line
                size_t pos = 0;
                
                // Get name
                pos = line.find('|');
                string name = line.substr(0, pos);
                line.erase(0, pos + 1);

                // Get ID
                pos = line.find('|');
                string id_str = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                int id = stoi(id_str);
                LibraryUser* newUser = new LibraryUser(name, id);
                
                // Load borrowed books
                while ((pos = line.find('|')) != string::npos) {
                    string borrowedISBN = line.substr(0, pos);
                    line.erase(0, pos + 1);
                    Book* borrowedBook = findBook(borrowedISBN);
                    if (borrowedBook) {
                        newUser->borrowBook(borrowedBook);
                    }
                }

                users.push_back(newUser);
            }
            file.close();
            cout << "Users loaded from " << users_filename << endl;
        } else {
            cout << "No existing users file found. A new one will be created." << endl;
        }
    }

    void borrowBook(int userID, const string& bookISBN) {
        LibraryUser* user = findUser(userID);
        Book* book = findBook(bookISBN);
        if (user && book) {
            if (book->isAvailable()) {
                user->borrowBook(book);
                book->setAvailability(false);
                cout << user->getName() << " successfully borrowed " << book->getTitle() << endl;
                saveBooksToFile();
                saveUsersToFile();
            } else {
                cout << "Book is not available." << endl;
            }
        } else {
            cout << "User or book not found." << endl;
        }
    }

    void returnBook(int userID, const string& bookISBN) {
        LibraryUser* user = findUser(userID);
        Book* book = findBook(bookISBN);
        if (user && book) {
            bool found = false;
            for (const auto& borrowedBook : user->getBorrowedBooks()) {
                if (borrowedBook->getISBN() == bookISBN) {
                    found = true;
                    break;
                }
            }
            if (found) {
                user->returnBook(book);
                book->setAvailability(true);
                cout << user->getName() << " successfully returned " << book->getTitle() << endl;
                saveBooksToFile();
                saveUsersToFile();
            } else {
                cout << "This user did not borrow this book." << endl;
            }
        } else {
            cout << "User or book not found." << endl;
        }
    }

    Book* findBook(const string& ISBN) const {
        for (const auto& book : books) {
            if (book->getISBN() == ISBN) {
                return book;
            }
        }
        return nullptr;
    }

    LibraryUser* findUser(int userID) const {
        for (const auto& user : users) {
            if (user->getID() == userID) {
                return user;
            }
        }
        return nullptr;
    }

    void showMenu() {
        cout << "\n--- Library Management System ---" << endl;
        cout << "1. Add Book" << endl;
        cout << "2. Add User" << endl;
        cout << "3. Display All Books" << endl;
        cout << "4. Display All Users" << endl;
        cout << "5. Borrow Book" << endl;
        cout << "6. Return Book" << endl;
        cout << "7. Save and Exit" << endl;
        cout << "---------------------------------" << endl;
        cout << "Enter your choice: ";
    }
};

void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    Library myLibrary;
    int choice;
    string title, author, isbn, name;
    int year, id;

    while (true) {
        myLibrary.showMenu();
        cin >> choice;
        clearInputBuffer(); // Clear buffer after reading int

        switch (choice) {
            case 1:
                cout << "Enter book title: ";
                getline(cin, title);
                cout << "Enter author: ";
                getline(cin, author);
                cout << "Enter ISBN: ";
                getline(cin, isbn);
                cout << "Enter publication year: ";
                cin >> year;
                clearInputBuffer();
                myLibrary.addBook(new Book(title, author, isbn, year));
                break;
            case 2:
                cout << "Enter user name: ";
                getline(cin, name);
                cout << "Enter user ID: ";
                cin >> id;
                clearInputBuffer();
                myLibrary.addUser(new LibraryUser(name, id));
                break;
            case 3:
                myLibrary.displayBooks();
                break;
            case 4:
                myLibrary.displayUsers();
                break;
            case 5:
                cout << "Enter user ID: ";
                cin >> id;
                clearInputBuffer();
                cout << "Enter book ISBN to borrow: ";
                getline(cin, isbn);
                myLibrary.borrowBook(id, isbn);
                break;
            case 6:
                cout << "Enter user ID: ";
                cin >> id;
                clearInputBuffer();
                cout << "Enter book ISBN to return: ";
                getline(cin, isbn);
                myLibrary.returnBook(id, isbn);
                break;
            case 7:
                cout << "Exiting program. Data saved." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }

    return 0;
}