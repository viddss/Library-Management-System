#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <iomanip>
using namespace std;

// getCurrentTime(): Returns current time in seconds since epoch
long long getCurrentTime() {
    return chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
}

// isValidEmail(): Validates email format using basic checks
bool isValidEmail(const string& email) {
    // Basic email validation
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.');
    return atPos != string::npos && dotPos != string::npos && 
           atPos > 0 && dotPos > atPos + 1 && dotPos < email.length() - 1;
}

// Input validation functions
bool isValidBookInput(int book_id, int year) {
    // Basic validation for book input
    return book_id > 0 && year > 1000 && year <= 2025;
}

bool isValidPhone(const string& phone) {
    // Check if phone number is 10 digits
    if (phone.length() != 10) return false;
    return all_of(phone.begin(), phone.end(), ::isdigit);
}

// Function to check if a string is numeric
bool isNumeric(const string& str) {
    return all_of(str.begin(), str.end(), ::isdigit);
}

// Book Class: Represents a book in the library with its attributes and status
class Book {
public:
    int book_id;
    string title, author, publisher, isbn;
    int year;
    string status;
    int borrower_id;
    long long borrowed_time;
    bool is_reserved;
    int reservation_id;

    Book(int book_id, string title, string author, string publisher, string isbn, int year, string status = "Available", int borrower_id = -1, long long borrowed_time = 0, bool is_reserved = false, int reservation_id = -1) {
        this->book_id = book_id;
        this->title = title;
        this->author = author;
        this->publisher = publisher;
        this->isbn = isbn;
        this->year = year;
        this->status = status;
        this->borrower_id = borrower_id;
        this->borrowed_time = borrowed_time;
        this->is_reserved = is_reserved;
        this->reservation_id = reservation_id;
    }
};

// Forward declarations
class Library;
class User; 
class Student;
class Faculty;
class Librarian;


// Library Class: Central management class that handles all library operations and data
class Library {
private:
    vector<Book> books;
    unordered_map<int, Student*> students;
    unordered_map<int, Faculty*> faculties;
    unordered_map<int, Librarian*> librarians;
public:
    friend class User;
    friend class Student;
    friend class Faculty;
    friend class Librarian;

    // Function to display book details
    static void displayBook(const Book* book, bool show_return_time = false, long long return_time = 0) {
        if (!book) {
            cout << "Book not found" << endl;
            return;
        }
        cout << "----------------------------------------" << endl;
        cout << "Book ID: " << book->book_id << endl;
        cout << "Title: " << book->title << endl;
        cout << "Author: " << book->author << endl;
        cout << "Publisher: " << book->publisher << endl;
        cout << "ISBN: " << book->isbn << endl;
        cout << "Year: " << book->year << endl;
        cout << "Status: " << book->status << endl;
        if (book->status == "Borrowed") {
            cout << "Borrower ID: " << book->borrower_id << endl;
            time_t timestamp = book->borrowed_time;
            cout << "Borrowed on: " << ctime(&timestamp);
        }
        if (book->is_reserved) {
            cout << "Reserved by: " << book->reservation_id << endl;
        }
        if (show_return_time) {
            time_t timestamp = return_time;
            cout << "Returned on: " << ctime(&timestamp);
        }
        cout << "----------------------------------------" << endl;
    }

    // Function to display all books in library
    void displayAllBooks() const {
        if (books.empty()) {
            cout << "No books in library" << endl;
            return;
        }
        cout << "\nAll Books in Library:" << endl;
        for (const auto& book : books) {
            displayBook(&book);
        }
    }

    // Function to clear the library
    void clear() {
        books.clear();
        students.clear();
        faculties.clear();
        librarians.clear();
    }

    // Friend Functions
    friend void addBook(const Book& book);
    friend void addstudent(Student* user);
    friend void addFaculty(Faculty* user);
    friend void addLibrarian(Librarian* user);
    friend void removeBook(int book_id);
    friend void removeStudent(int user_id);
    friend void removeFaculty(int user_id);
    friend void removeLibrarian(int user_id);
    friend void studentreserveBook(int book_id, Student* user);
    friend void studentcancelReservation(int book_id, Student* user);
    friend void facultyreserveBook(int book_id, Faculty* user);
    friend void facultycancelReservation(int book_id, Faculty* user);
    friend void saveBooks();
    friend void loadBooks();
    friend void saveStudents();
    friend void loadStudents();
    friend void saveFaculties();
    friend void loadFaculties();
    friend void saveLibrarians();
    friend void loadLibrarians();
    friend void saveBorrowingHistory();
    friend void loadBorrowingHistory();
    friend void loadcurrentlyborrowed();
    friend void savecurrentlyborrowed();
    friend Book* getBook(int book_id);
    friend Student* getStudent(int user_id);
    friend Faculty* getFaculty(int user_id);
    friend Librarian* getLibrarian(int user_id);
    friend void saveReservedBooks();
    friend void loadReservedBooks();
};

Library library;

// Function to add a book to the library
void addBook(const Book& book) {
    if (find_if(library.books.begin(), library.books.end(), [book](const Book& b) { return b.book_id == book.book_id; }) != library.books.end()) {
        cout << "Book already exists" << endl;
        return;
    }
    library.books.push_back(book);
    cout << "Book added successfully" << endl;
}

// Function to get a book by its id
Book* getBook(int book_id) {
    for (auto& book : library.books) {
        if (book.book_id == book_id) {
            return &book;
        }
    }
    return nullptr;
}

// Function to remove a book from the library
void removeBook(int book_id)
{
    // First find the book to check if it is available
    auto it = find_if(library.books.begin(), library.books.end(), 
                     [book_id](const Book& b) { return b.book_id == book_id; });
    
    if (it == library.books.end()) {
        cout << "Book not found" << endl;
        return;
    }
    
    // Check if the book is currently borrowed
    if (it->status != "Available") {
        cout << "Cannot remove book - it is currently borrowed or reserved by user " << it->borrower_id << endl;
        return;
    }
    
    // If book is available, remove it
    library.books.erase(it);
    cout << "Book removed successfully" << endl;
}

/* 
Account Class: Manages user's borrowed books, history, and fines
*/
class Account {
public:
    int user_id;
    vector<int> borrowed_books;
    unordered_map<int, long long> borrowed_time;
    vector<pair<Book*, long long>> borrowing_history;
    int prev_fine = 0;
    unordered_map<int, long long> reserved_books;


    Account(int user_id) {
        this->user_id = user_id;
    }

    // Function to view books currently borrowed
    void view_books() {
        if (borrowed_books.empty()) {
            cout << "No books currently borrowed" << endl;
            return;
        }
        cout << "\nCurrently Borrowed Books:" << endl;
        for (int book_id : borrowed_books) {
            Book* book = getBook(book_id);
            if (book) {
                Library::displayBook(book);
            }
        }
    }

    // Function to view reserved books
    void view_reserved_books() {
        if (reserved_books.empty()) {
            cout << "No books currently reserved" << endl;
            return;
        }
        cout << "\nCurrently Reserved Books:" << endl;
        for (const auto& pair : reserved_books) {
            int book_id = pair.first;
            Book* book = getBook(book_id);
            if (book) {
                Library::displayBook(book);
            }
        }
    }

    // Function to cancel a reservation
    bool cancel_reservation(int book_id) {
        if (reserved_books.find(book_id) != reserved_books.end()) {
            reserved_books.erase(book_id);
            return true;
        }
        return false;
    }

    // Function to check fine
    int check_fine() {
        long long current_time = getCurrentTime();
        int curr_fine = 0;
        for (auto& pair : borrowed_time) {
            long long borrow_time = pair.second;
            if ((current_time - borrow_time) / 86400 > 15) {
                curr_fine += 10 * ((current_time - borrow_time) / 86400 - 15);
            }
        }
        return curr_fine;
    }

    // Function to pay fine
    void pay_fine() {
        prev_fine = 0;
        cout << "Fine paid successfully" << endl;
    }

    // Function to check if the user has overdue books
    bool hasOverdue(int limit) {
        long long current_time = getCurrentTime();
        int flag = 0;
        for (auto& p: borrowed_time) {
            int book_id = p.first;
            long long borrow_time = p.second;
            if ((current_time - borrow_time) / 86400 > limit) {
                Book* book = getBook(book_id);
                int amount = (current_time - borrow_time) / 86400 - limit;
                cout<<"Overdue book: "<<book_id<<" "<<book->title<<" by "<<amount<<"days!!"<<endl;
                flag=1;
            }
        }
        if(flag==1){
            return true;
        }
        return false;
    }

    // Function to view borrowing history
    void view_borrowing_history() {
        if (borrowing_history.empty()) {
            cout << "No borrowing history" << endl;
            return;
        }
        cout << "\nBorrowing History:" << endl;
        for (const auto& pair : borrowing_history) {
            Book* book = pair.first;
            long long return_time = pair.second;
            Library::displayBook(book, true, return_time);
        }
    }

    // Function to add a book to borrowing history
    void add_borrowing_history(Book* book, long long return_time) {
        borrowing_history.push_back({book, return_time});
    }

    // Friend Functions
    friend void saveBorrowingHistory();
    friend void loadBorrowingHistory();
    friend void loadcurrentlyborrowed();
    friend void savecurrentlyborrowed();
};


// User Class: Base class for all user types with common functionality
class User : public Library {
private:
    string password = "password"; // Default password
protected:
    int user_id;
    string email, phone, role;
    Account account;

public:
    string name;
    User(int user_id, string name, string email, string phone, string role, string password = "password") 
        : account(user_id) {
        this->user_id = user_id;
        this->name = name;
        this->email = email;
        this->phone = phone;
        this->role = role;
        this->password = password;
    }

    virtual void borrowBook(int book_id) = 0;
    virtual void returnBook(int book_id) = 0;

    // Function to check if the user id and password are correct
    bool check_credentials(int user_id, string password) {
        return this->user_id == user_id && this->password == password;
    }

    // Function to change password
    void changePassword(string new_password) {
        password = new_password;
    }
    
    // Function to check fine
    int check_fine(){
        return account.check_fine();
    }

    // Function to pay fine
    void payFine() {
        account.pay_fine();
    }
    
    //Function to view password
    string view_password(){
        return password;
    }

    // Function to view books currently borrowed
    void view_books(){
        account.view_books();
    }
    
    // Function to view borrowing history
    void view_borrowing_history(){
        account.view_borrowing_history();
    }
    
    // Function to check if the user has overdue books
    bool checkOverdue(int limit){
        if(account.hasOverdue(limit)){
           return true;
        }
        return false;
    }

    // Function to check if user has any borrowed books
    bool hasBorrowedBooks() const {
        return !account.borrowed_books.empty();
    }

    // Friend Functions
    friend void addBook(const Book& book);
    friend void addstudent(Student* user);
    friend void addFaculty(Faculty* user);
    friend void addLibrarian(Librarian* user);
    friend void removeBook(int book_id);
    friend void removeStudent(int user_id);
    friend void removeFaculty(int user_id);
    friend void removeLibrarian(int user_id);
    friend void studentreserveBook(int book_id, Student* user);
    friend void studentcancelReservation(int book_id, Student* user);
    friend void facultyreserveBook(int book_id, Faculty* user);
    friend void facultycancelReservation(int book_id, Faculty* user);
    friend void saveBooks();
    friend void loadBooks();
    friend void saveStudents();
    friend void loadStudents();
    friend void saveFaculties();
    friend void loadFaculties();
    friend void saveLibrarians();
    friend void loadLibrarians();
    friend void saveBorrowingHistory();
    friend void loadBorrowingHistory();
    friend void loadcurrentlyborrowed();
    friend void savecurrentlyborrowed();
    friend Book* getBook(int book_id);
    friend Student* getStudent(int user_id);
    friend Faculty* getFaculty(int user_id);
    friend Librarian* getLibrarian(int user_id);
    friend void saveReservedBooks();
    friend void loadReservedBooks();

    // AVirtual Mwthod Defined to Display User Details
    virtual void displayUserDetails() {
        cout << "\n+-------------------------------------------+" << endl;
        cout << "| User Details                              |" << endl;
        cout << "+-------------------------------------------+" << endl;
        cout << "User ID: " << user_id << endl;
        cout << "Name: " << name << endl;
        cout << "Email: " << email << endl;
        cout << "Phone: " << phone << endl;
        cout << "Role: " << role << endl;
        cout << "+-------------------------------------------+" << endl;
    }
};

// Class to represent a student. It contains a user id, name, email, phone, role, password and an account.
class Student : public User {
public:
    int roll_number;  // Add roll number attribute as specific to this class
    Student(int user_id, string name, string email, string phone, int roll_number, string password = "password") 
        : User(user_id, name, email, phone, "Student", password), roll_number(roll_number) {}

    // Function to borrow a book    
    void borrowBook(int book_id) override {
        Book* book = getBook(book_id);

        // Check if the book exists
        if (!book) {
            cout << "Book not found" << endl;
            return;
        }

        // Check if the student has already borrowed the book
        if (find(account.borrowed_books.begin(), account.borrowed_books.end(), book_id) != account.borrowed_books.end()) {
            cout << "You already have this book." << endl;
            return;
        }

        // Check if the student has borrowed 3 books
        if (account.borrowed_books.size() >= 3) {
            cout << "Limit reached: 3 books" << endl;
            return;
        }

        // Check if the student has overdue books
        if (account.hasOverdue(15)) {
            cout << "Overdue books detected" << endl;
            return;
        }
        
        // Check if the student has pending fine
        if (account.prev_fine > 0) {
            cout << "Pending fine detected" << endl;
            return;
        }

        // Check if the book is available
        if (book->status == "Available") {
            // Check if the book is reserved by the same user or not reserved
            if ((book->is_reserved && book->reservation_id == user_id) || !book->is_reserved) {
                book->status = "Borrowed";
                book->borrower_id = user_id;
                book->borrowed_time = getCurrentTime();
                account.borrowed_books.push_back(book_id);
                account.borrowed_time[book_id] = getCurrentTime();
                cout << "Book borrowed successfully" << endl;
                // Clear reservation
                book->is_reserved = false;
                book->reservation_id = -1;
                account.reserved_books.erase(book_id);
                Library::displayBook(book);                
            } else if (book->is_reserved) {
                cout << "Book is already reserved by another user." << endl;
            } else {
                cout << "Book is available but not reserved by you." << endl;
            }
        } else if (book->status == "Borrowed") {
            if (book->is_reserved) {
                cout << "Book is already reserved by another user." << endl;
            } else {
                cout << "Book is currently borrowed. Would you like to reserve it? (yes/no)" << endl;
                string response;
                cin >> response;
                if (response == "yes") {
                    studentreserveBook(book_id, this);
                }
            }
        } else {
            cout << "Book is not available" << endl;
        }
    }

    // Function to return a book
    void returnBook(int book_id) override {
        Book* book = getBook(book_id);

        // Check if the book exists and is borrowed by the student
        if (!book || book->borrower_id != user_id) {
            cout << "Invalid return request" << endl;
            return;
        }

        account.add_borrowing_history(book, getCurrentTime());
        book->status = "Available";
        book->borrower_id = -1;
        account.borrowed_books.erase(remove(account.borrowed_books.begin(), account.borrowed_books.end(), book_id), account.borrowed_books.end());

        // Calculate fine if the book is returned after 15 days
        int extra_days = (getCurrentTime() - account.borrowed_time[book_id]) / 86400 - 15;
        if (extra_days > 0) {
            account.prev_fine += 10 * extra_days;
            cout << "Returned with fine: " << 10 * extra_days << endl;
        } else {
            cout << "Book returned successfully" << endl;
        }
        account.borrowed_time.erase(book_id);
    }

    void displayUserDetails() override {
        User::displayUserDetails();
        cout << "Books Currently Borrowed: " << account.borrowed_books.size() << "/3" << endl;
        cout << "Current Fine: $" << check_fine() << endl;
        cout << "+-------------------------------------------+" << endl;
    }

    void cancelReservation() {
        account.view_reserved_books();
        if (account.reserved_books.empty()) {
            return;
        }
        cout << "Enter book ID to cancel reservation: ";
        int book_id;
        cin >> book_id;
        if (account.cancel_reservation(book_id)) {
            Book* book = getBook(book_id);
            if (book && book->reservation_id == user_id) {
                book->is_reserved = false;
                book->reservation_id = -1;
                cout << "Reservation cancelled successfully" << endl;
            } else {
                cout << "Invalid book ID or reservation not found" << endl;
            }
        } else {
            cout << "Invalid book ID or reservation not found" << endl;
        }
    }
};

// Class to represent a faculty. It contains a user id, name, email, phone, role, password and an account.
class Faculty : public User {
public:
    Faculty(int user_id, string name, string email, string phone, string password = "password") 
        : User(user_id, name, email, phone, "Faculty",password) {}

    // Function to borrow a book
    void borrowBook(int book_id) override {
        Book* book = getBook(book_id);

        // Check if the book exists
        if (!book) {
            cout << "Book not found" << endl;
            return;
        }

        // Check if the faculty has already borrowed the book
        if (find(account.borrowed_books.begin(), account.borrowed_books.end(), book_id) != account.borrowed_books.end()) {
            cout << "You already have this book." << endl;
            return;
        }

        // Check if the faculty has borrowed 5 books
        if (account.borrowed_books.size() >= 5) {
            cout << "Limit reached: 5 books" << endl;
            return;
        }

        // Check if the faculty has overdue books
        if (account.hasOverdue(60)) {
            cout << "Overdue books detected" << endl;
            return;
        }

        // Check if the book is available
        if (book->status == "Available") {
            // Check if the book is reserved by the same user
            if ((book->is_reserved && book->reservation_id == user_id) || !book->is_reserved) {
                book->status = "Borrowed";
                book->borrower_id = user_id;
                book->borrowed_time = getCurrentTime();
                account.borrowed_books.push_back(book_id);
                account.borrowed_time[book_id] = getCurrentTime();
                cout << "Book borrowed successfully" << endl;
                // Clear reservation
                book->is_reserved = false;
                book->reservation_id = -1;
                account.reserved_books.erase(book_id);
                Library::displayBook(book);
            } else if (book->is_reserved) {
                cout << "Book is already reserved by another user." << endl;
            } else {
                cout << "Book is available but not reserved by you." << endl;
            }
        } else if (book->status == "Borrowed") {
            if (book->is_reserved) {
                cout << "Book is already reserved by another user." << endl;
            } else {
                cout << "Book is currently borrowed. Would you like to reserve it? (yes/no)" << endl;
                string response;
                cin >> response;
                if (response == "yes") {
                    facultyreserveBook(book_id, this);
                }
            }
        } else {
            cout << "Book is not available" << endl;
        }
    }

    // Function to return a book
    void returnBook(int book_id) override {
        Book* book = getBook(book_id);

        // Check if the book exists and is borrowed by the faculty
        if (!book || book->borrower_id != user_id) {
            cout << "Invalid return request" << endl;
            return;
        }
        
        // Add book to borrowing history
        account.add_borrowing_history(book, getCurrentTime());

        //tO return the book
        book->status = "Available";
        book->borrower_id = -1;
        account.borrowed_books.erase(remove(account.borrowed_books.begin(), account.borrowed_books.end(), book_id), account.borrowed_books.end());
        cout << "Book returned successfully" << endl;
        account.borrowed_time.erase(book_id);
    }

    void displayUserDetails() override {
        User::displayUserDetails();
        cout << "Books Currently Borrowed: " << account.borrowed_books.size() << "/5" << endl;
        cout << "+-------------------------------------------+" << endl;
    }

    void cancelReservation() {
        account.view_reserved_books();
        if (account.reserved_books.empty()) {
            return;
        }
        cout << "Enter book ID to cancel reservation: ";
        int book_id;
        cin >> book_id;
        if (account.cancel_reservation(book_id)) {
            Book* book = getBook(book_id);
            if (book && book->reservation_id == user_id) {
                book->is_reserved = false;
                book->reservation_id = -1;
                cout << "Reservation cancelled successfully" << endl;
            } else {
                cout << "Invalid book ID or reservation not found" << endl;
            }
        } else {
            cout << "Invalid book ID or reservation not found" << endl;
        }
    }
};

//These Function are defined afterwards as they use the classes defined above. (Student, Faculty)
//Forward Declarations
Student* getStudent(int user_id);
Faculty* getFaculty(int user_id);
// Function to add a student to the library
void addstudent(Student* user) {
    if(library.students.find(user->user_id) != library.students.end() || library.faculties.find(user->user_id) != library.faculties.end() || user->user_id==1){
        cout << "User ID already exists. Details of the existing user:" << endl;
        if (library.students.find(user->user_id) != library.students.end()) {
            getStudent(user->user_id)->displayUserDetails();
        } else if(library.faculties.find(user->user_id) != library.faculties.end()){
            getFaculty(user->user_id)->displayUserDetails();
        }
        else{
            cout<<"User ID 1 is reserved for Librarian!!"<<endl;
        }
        return;
    }
    library.students[user->user_id] = user;
    cout << "Student added successfully" << endl;
}

// Function to add a faculty to the library
void addFaculty(Faculty* user) {
    if(library.faculties.find(user->user_id) != library.faculties.end() || library.students.find(user->user_id) != library.students.end() || user->user_id==1){
        cout << "User ID already exists. Details of the existing user:" << endl;
        if (library.faculties.find(user->user_id) != library.faculties.end()) {
            getFaculty(user->user_id)->displayUserDetails();    
        } else if(library.students.find(user->user_id) != library.students.end()){
            getStudent(user->user_id)->displayUserDetails();
        }
        else{
            cout<<"User ID 1 is reserved for Librarian!!"<<endl;
        }
        return;
    }
    library.faculties[user->user_id] = user;
    cout << "Faculty added successfully" << endl;
}

// Function to remove a student from the library
void removeStudent(int user_id) {
    if(library.students.erase(user_id)){
        cout << "Student removed successfully" << endl;
    } else {
        cout << "Student not found" << endl;
    }
}

// Function to remove a faculty from the library
void removeFaculty(int user_id) {
    if(library.faculties.erase(user_id)){
        cout << "Faculty removed successfully" << endl;
    } else {
        cout << "Faculty not found" << endl;
    }
}

// Function to reserve a book
void studentreserveBook(int book_id, Student* user) {
    Book* book = getBook(book_id);
    if (!book) {
        cout << "Book not found" << endl;
        return;
    }
    if (book->is_reserved) {
        cout << "Book is already reserved" << endl;
        return;
    }
    book->is_reserved = true;
    book->reservation_id = user->user_id;
    user->account.reserved_books[book_id] = getCurrentTime();
    cout << "Book reserved successfully" << endl;
}

// Function to cancel a reservation
void studentcancelReservation(int book_id, Student* user) {
    Book* book = getBook(book_id);
    if (!book) {
        cout << "Book not found" << endl;
        return;
    }
    if (!book->is_reserved || book->reservation_id != user->user_id) {
        cout << "Invalid reservation" << endl;
        return;
    }
    book->is_reserved = false;
    book->reservation_id = -1;
    cout << "Reservation cancelled successfully" << endl;
}

// Function to reserve a book
void facultyreserveBook(int book_id, Faculty* user) {
    Book* book = getBook(book_id);
    if (!book) {
        cout << "Book not found" << endl;
        return;
    }
    if (book->is_reserved) {
        cout << "Book is already reserved" << endl;
        return;
    }
    book->is_reserved = true;
    book->reservation_id = user->user_id;
    user->account.reserved_books[book_id] = getCurrentTime();
    cout << "Book reserved successfully" << endl;
}

// Function to cancel a reservation
void facultycancelReservation(int book_id, Faculty* user) {
    Book* book = getBook(book_id);
    if (!book) {
        cout << "Book not found" << endl;
        return;
    }
    if (!book->is_reserved || book->reservation_id != user->user_id) {
        cout << "Invalid reservation" << endl;
        return;
    }
    book->is_reserved = false;
    book->reservation_id = -1;
    cout << "Reservation cancelled successfully" << endl;
}

// Class to represent a librarian. It contains a user id, name, email, phone, role, password and an account.
class Librarian : public User {
public:
    Librarian(int user_id, string name, string email, string phone, string password = "password") 
        : User(user_id, name, email, phone, "Librarian",password) {}

    //Below Two Functions are Dummy Functions as Librarian cannot borrow or return books
    // Function to borrow a book    
    void borrowBook(int book_id) override {
        cout << "Librarians cannot borrow books." <<book_id<< endl;
    }

    // Function to return a book
    void returnBook(int book_id) override {
        cout << "Librarians cannot return books." <<book_id<< endl;
    }

    //Main Functions for Librarian
    // Function to add a book to the library
    void add_Book_to_Lib(const Book& book) {
        addBook(book);
    }

    // Function to remove a book from the library
    void remove_Book_from_Lib(int book_id) {
        removeBook(book_id);
    }

    // Function to add a student to the library
    void add_student_to_Lib(Student*user) {
        addstudent(user);
    }

    // Function to add a faculty to the library
    void add_faculty_to_Lib(Faculty*user) {
        addFaculty(user);
    }

    // Function to remove a student from the library
    void remove_student_from_Lib(int user_id) {
        removeStudent(user_id);
    }

    // Function to remove a faculty from the library
    void remove_faculty_from_Lib(int user_id) {
        removeFaculty(user_id);
    }

    void displayUserDetails() override {
        User::displayUserDetails();
        cout << "+-------------------------------------------+" << endl;
    }

    // Add new methods to view all students and faculty
    void viewAllStudents() {
        cout << "\n+-------------------------------------------+" << endl;
        cout << "| All Registered Students                   |" << endl;
        cout << "+-------------------------------------------+" << endl;
        if (library.students.empty()) {
            cout << "No students registered in the system." << endl;
        } else {
            for (const auto& pair : library.students) {
                Student* student = pair.second;
                student->displayUserDetails();
                cout << endl;
            }
        }
    }

    void viewAllFaculty() {
        cout << "\n+-------------------------------------------+" << endl;
        cout << "| All Registered Faculty Members            |" << endl;
        cout << "+-------------------------------------------+" << endl;
        if (library.faculties.empty()) {
            cout << "No faculty members registered in the system." << endl;
        } else {
            for (const auto& pair : library.faculties) {
                Faculty* faculty = pair.second;
                faculty->displayUserDetails();
                cout << endl;
            }
        }
    }
};


//These could be used further but are not used in the current implementation as I assumed that there will be only one librarian!!
// Function to add a librarian to the library
void addLibrarian(Librarian* user) {
    if(library.librarians.find(user->user_id) != library.librarians.end()){
        cout << "User already exists" << endl;
        return;
    }
    library.librarians[user->user_id] = user;
    cout << "Librarian added successfully" << endl;
}

// Function to remove a librarian from the library
void removeLibrarian(int user_id) {
    if (library.librarians.erase(user_id)) {
        cout << "User removed successfully" << endl;
    } else {
        cout << "User not found" << endl;
    }
}

// Function to get a student by its id
Student* getStudent(int user_id) {
    auto it = library.students.find(user_id);
    if (it != library.students.end()) {
        return it->second;
    }
    return nullptr;
}

// Function to get a faculty by its id
Faculty* getFaculty(int user_id) {
    auto it = library.faculties.find(user_id);
    if (it != library.faculties.end()) {
        return it->second;
    }
    return nullptr;
}

// Function to get a librarian by its id
Librarian* getLibrarian(int user_id) {
    auto it = library.librarians.find(user_id);
    if (it != library.librarians.end()) {
        return it->second;
    }
    return nullptr;
}

// Function to save books to a file
void saveBooks() {
    ofstream file("books.txt");
    for (const auto& book : library.books) {
        file << book.book_id << "|" << book.title << "|" << book.author << "|" << book.publisher << "|" << book.isbn << "|" << book.year << "|" << book.status << "|" << book.borrower_id << "|" << book.borrowed_time << "|" << book.is_reserved << "|" << book.reservation_id << endl;
    }
    file.close();
}

// Function to load books from a file
void loadBooks() {
    ifstream file("books.txt");
    if (!file) return;
    
    library.books.clear();
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        int id, year, borrower_id, reservation_id;
        string title, author, publisher, isbn, status;
        long long borrowed_time;
        bool is_reserved;

        getline(ss, temp, '|'); id = stoi(temp);
        getline(ss, title, '|');
        getline(ss, author, '|');
        getline(ss, publisher, '|');
        getline(ss, isbn, '|');
        getline(ss, temp, '|'); year = stoi(temp);
        getline(ss, status, '|');
        getline(ss, temp, '|'); borrower_id = stoi(temp);
        getline(ss, temp, '|'); borrowed_time = stoll(temp);
        getline(ss, temp, '|'); is_reserved = (temp == "1");
        getline(ss, temp, '|'); reservation_id = stoi(temp);

        Book book(id, title, author, publisher, isbn, year, status, borrower_id, borrowed_time, is_reserved, reservation_id);
        library.books.push_back(book);
    }
    file.close();
}

// Function to save students to a file
void saveStudents() {
    ofstream file("students.txt");
    for (const auto& pair: library.students) {
        Student* student = pair.second;
        file << student->user_id << "|" << student->name << "|" << student->email << "|" << student->phone << "|" << student->role << "|" << student->view_password() << endl;
    }
    file.close();
}

// Function to load students from a file
void loadStudents() {
    ifstream file("students.txt");
    if (!file) return;
    
    library.students.clear();
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        int user_id;
        string name, email, phone, role, password;

        getline(ss, temp, '|'); user_id = stoi(temp);
        getline(ss, name, '|');
        getline(ss, email, '|');
        getline(ss, phone, '|');
        getline(ss, role, '|');
        getline(ss, password, '|');

        Student* student = new Student(user_id, name, email, phone, 0, password);
        library.students[user_id] = student;
    }
    file.close();
}

// Function to save faculties to a file
void saveFaculties() {
    ofstream file("faculties.txt");
    for (const auto& pair: library.faculties) {
        Faculty* faculty = pair.second;
        file << faculty->user_id << "|" << faculty->name << "|" << faculty->email << "|" << faculty->phone << "|" << faculty->role << "|" << faculty->view_password() << endl;
    }
    file.close();
}

// Function to load faculties from a file
void loadFaculties() {
    ifstream file("faculties.txt");
    if (!file) return;
    
    library.faculties.clear();
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        int user_id;
        string name, email, phone, role, password;

        getline(ss, temp, '|'); user_id = stoi(temp);
        getline(ss, name, '|');
        getline(ss, email, '|');
        getline(ss, phone, '|');
        getline(ss, role, '|');
        getline(ss, password, '|');

        Faculty* faculty = new Faculty(user_id, name, email, phone, password);
        library.faculties[user_id] = faculty;
    }
    file.close();
}

// Function to save librarians to a file
void saveLibrarians() {
    ofstream file("librarians.txt");
    for (const auto& pair : library.librarians) {
        Librarian* librarian = pair.second;
        file << librarian->user_id << "|" << librarian->name << "|" << librarian->email << "|" << librarian->phone << "|" << librarian->role << "|" << librarian->view_password() << endl;
    }
    file.close();
}

// Function to load librarians from a file
void loadLibrarians() {
    ifstream file("librarians.txt");
    if (!file) return;
    
    library.librarians.clear();
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        int user_id;
        string name, email, phone, role, password;

        getline(ss, temp, '|'); user_id = stoi(temp);
        getline(ss, name, '|');
        getline(ss, email, '|');
        getline(ss, phone, '|');
        getline(ss, role, '|');
        getline(ss, password, '|');

        Librarian* librarian = new Librarian(user_id, name, email, phone, password);
        library.librarians[user_id] = librarian;
    }
    file.close();
}

// Function to save borrowing history to a file
void saveBorrowingHistory() {
    ofstream file("borrowing_history.txt");
    for (const auto& pair: library.students) {
        Student* user = pair.second;
        for (const auto& history_pair : user->account.borrowing_history) {
            Book* book = history_pair.first;
            long long return_time = history_pair.second;
            file << user->user_id << "|" << book->book_id << "|" << return_time << endl;
        }
    }
    for (const auto& pair: library.faculties) {
        Faculty* user = pair.second;
        for (const auto& history_pair : user->account.borrowing_history) {
            Book* book = history_pair.first;
            long long return_time = history_pair.second;
            file << user->user_id << "|" << book->book_id << "|" << return_time << endl;
        }
    }
    file.close();
}

// Function to load borrowing history from a file
void loadBorrowingHistory() {
    ifstream file("borrowing_history.txt");
    if (!file) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        int user_id, book_id;
        long long return_time;

        getline(ss, temp, '|'); user_id = stoi(temp);
        getline(ss, temp, '|'); book_id = stoi(temp);
        getline(ss, temp, '|'); return_time = stoll(temp);

        if (library.students.find(user_id) != library.students.end()) {
            Book* book = getBook(book_id);
            if (book) {
                library.students[user_id]->account.borrowing_history.push_back({book, return_time});
            }
        }
        if (library.faculties.find(user_id) != library.faculties.end()) {
            Book* book = getBook(book_id);
            if (book) {
                library.faculties[user_id]->account.borrowing_history.push_back({book, return_time});
            }
        }
    }
    file.close();
}

// Function to save currently borrowed books to a file
void savecurrentlyborrowed() {
    ofstream file("currently_borrowed.txt");
    for (const auto& pair: library.students) {
        Student* user = pair.second;
        for (int book_id : user->account.borrowed_books) {
            file << user->user_id << "|" << book_id << "|" << user->account.borrowed_time[book_id] << endl;
        }
    }
    for (const auto& pair: library.faculties) {
        Faculty* user = pair.second;
        for (int book_id : user->account.borrowed_books) {
            file << user->user_id << "|" << book_id << "|" << user->account.borrowed_time[book_id] << endl;
        }
    }
    file.close();
}

// Function to load currently borrowed books from a file
void loadcurrentlyborrowed() {
    ifstream file("currently_borrowed.txt");
    if (!file) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        int user_id, book_id;
        long long borrowed_time;

        getline(ss, temp, '|'); user_id = stoi(temp);
        getline(ss, temp, '|'); book_id = stoi(temp);
        getline(ss, temp, '|'); borrowed_time = stoll(temp);

        if (library.students.find(user_id) != library.students.end()) {
            library.students[user_id]->account.borrowed_books.push_back(book_id);
            library.students[user_id]->account.borrowed_time[book_id] = borrowed_time;
        }
        if (library.faculties.find(user_id) != library.faculties.end()) {
            library.faculties[user_id]->account.borrowed_books.push_back(book_id);
            library.faculties[user_id]->account.borrowed_time[book_id] = borrowed_time;
        }
    }
    file.close();
}

bool isValidUserId(int user_id) {
    return user_id > 0;
}

// Function to save reserved books to a file
void saveReservedBooks() {
    ofstream file("reserved_books.txt");
    for (const auto& pair : library.students) {
        Student* student = pair.second;
        for (const auto& reserved : student->account.reserved_books) {
            int book_id = reserved.first;
            long long reserved_time = reserved.second;
            file << student->user_id << "|" << book_id << "|" << reserved_time << endl;
        }
    }
    for (const auto& pair : library.faculties) {
        Faculty* faculty = pair.second;
        for (const auto& reserved : faculty->account.reserved_books) {
            int book_id = reserved.first;
            long long reserved_time = reserved.second;
            file << faculty->user_id << "|" << book_id << "|" << reserved_time << endl;
        }
    }
    file.close();
}

// Function to load reserved books from a file
void loadReservedBooks() {
    ifstream file("reserved_books.txt");
    if (!file) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        int user_id, book_id;
        long long reserved_time;

        getline(ss, temp, '|'); user_id = stoi(temp);
        getline(ss, temp, '|'); book_id = stoi(temp);
        getline(ss, temp, '|'); reserved_time = stoll(temp);

        if (library.students.find(user_id) != library.students.end()) {
            library.students[user_id]->account.reserved_books[book_id] = reserved_time;
        }
        if (library.faculties.find(user_id) != library.faculties.end()) {
            library.faculties[user_id]->account.reserved_books[book_id] = reserved_time;
        }
    }
    file.close();
}

// librarianuser(): Handles librarian login and menu interface
// Provides options for managing books, users, and viewing system information
void librarianuser(){
    string name;
    cout << "Enter your name" << endl;
    cin.ignore();
    getline(cin, name);
    
    cout << "Enter your user id" << endl;
    int user_id;
    cin >> user_id;
    
    Librarian* librarian = getLibrarian(user_id);
    if (!librarian || librarian->name != name) {
        cout << "Invalid name or user id" << endl;
        return;
    }
    
    cout << "Enter Password" << endl;
    string password;
    cin >> password;
    if(!librarian->check_credentials(user_id,password)){
        cout << "Invalid password" << endl;
        return;
    }
    
    cout << "\n+-------------------------------------------+" << endl;
    cout << "|            Welcome " << left << setw(20) << librarian->name << "|" << endl;
    cout << "+-------------------------------------------+" << endl << endl;
    
    while(1){
        cout<<"What would you like to do?"<<endl;
        cout<<"[1] Add Book to Library"<<endl;
        cout<<"[2] Remove Book from Library"<<endl;
        cout<<"[3] Add User to Library"<<endl;
        cout<<"[4] Remove User from Library"<<endl;
        cout<<"[5] Change Password"<<endl;
        cout<<"[6] Display Books"<<endl;
        cout<<"[7] View All Students"<<endl;
        cout<<"[8] View All Faculty"<<endl;
        cout<<"[9] View My Details"<<endl;
        cout<<"[10] Logout"<<endl;
        int choice;
        cin>>choice;
        switch (choice) {
            case 1: {
                cout<<"Enter book details"<<endl;
                string book_id_str;
                int book_id;
                string title, author, publisher, isbn;
                int year;
                cout<<"Enter book id"<<endl;
                cin >> book_id_str;
                if (!isNumeric(book_id_str)) {
                    cout << "Invalid book ID (must be numeric)" << endl;
                    break;
                }
                book_id = stoi(book_id_str);
                cin.ignore();
                cout<<"Enter title"<<endl;
                getline(cin, title);
                if (title.empty()) {
                    cout << "Title cannot be empty" << endl;
                    break;
                }
                cout<<"Enter author"<<endl;
                getline(cin, author);
                if (author.empty()) {
                    cout << "Author cannot be empty" << endl;
                    break;
                }
                cout<<"Enter publisher"<<endl;
                getline(cin, publisher);
                if (publisher.empty()) {
                    cout << "Publisher cannot be empty" << endl;
                    break;
                }
                cout<<"Enter isbn"<<endl;
                getline(cin, isbn);
                if (isbn.empty()) {
                    cout << "ISBN cannot be empty" << endl;
                    break;
                }
                cout<<"Enter year"<<endl;
                cin>>year;
                if (!isValidBookInput(book_id, year)) {
                    cout << "Invalid year" << endl;
                    break;
                }
                Book book(book_id, title, author, publisher, isbn, year);
                librarian->add_Book_to_Lib(book);
                break;
            }
            case 2: {
                cout<<"Enter book id"<<endl;
                string book_id_str;
                int book_id;
                cin >> book_id_str;
                if (!isNumeric(book_id_str)) {
                    cout << "Invalid book ID (must be numeric)" << endl;
                    break;
                }
                book_id = stoi(book_id_str);
                librarian->remove_Book_from_Lib(book_id);
                break;
            }
            case 3: {
                cout<<"Enter user details"<<endl;
                string user_id_str;
                int user_id;
                string name, email, phone, role;
                cout<<"Enter user id"<<endl;
                cin >> user_id_str;
                if (!isNumeric(user_id_str)) {
                    cout << "Invalid user ID (must be numeric)" << endl;
                    break;
                }
                user_id = stoi(user_id_str);
                cin.ignore();
                cout<<"Enter name"<<endl;
                getline(cin, name);
                if (name.empty()) {
                    cout << "Name cannot be empty" << endl;
                    break;
                }
                cout<<"Enter email"<<endl;
                getline(cin, email);
                if (!isValidEmail(email)) {
                    cout << "Invalid email format" << endl;
                    break;
                }
                cout<<"Enter phone"<<endl;
                getline(cin, phone);
                if (!isValidPhone(phone)) {
                    cout << "Invalid phone number (must be 10 digits)" << endl;
                    break;
                }
                cout<<"Enter role (Student/Faculty)"<<endl;
                getline(cin, role);
                if(role=="Student"){
                    int roll_number;
                    cout << "Enter roll number" << endl;
                    cin >> roll_number;
                    cin.ignore(); // To clear the newline character from the input buffer
                    Student* user = new Student(user_id, name, email, phone, roll_number);
                    librarian->add_student_to_Lib(user);
                }
                else if(role=="Faculty"){
                    Faculty* user = new Faculty(user_id, name, email, phone);
                    librarian->add_faculty_to_Lib(user);
                }
                else {
                    cout << "Invalid role (must be Student or Faculty)" << endl;
                }
                break;
            }
            case 4: {
                cout<<"Enter role"<<endl;
                cout<<"[1] Student"<<endl;
                cout<<"[2] Faculty"<<endl;
                int role;
                cin>>role;
                if(role==1){
                    cout<<"Enter user id"<<endl;
                    string user_id_str;
                    int user_id;
                    cin >> user_id_str;
                    if (!isNumeric(user_id_str)) {
                        cout << "Invalid user ID (must be numeric)" << endl;
                        break;
                    }
                    user_id = stoi(user_id_str);
                    Student* user = getStudent(user_id);
                    if(!user){
                        cout<<"Invalid user id"<<endl;
                        break;
                    }
                    
                    if(user->hasBorrowedBooks()) {
                        cout << "Cannot remove user. Student must return all borrowed books first." << endl;
                        user->view_books();  // Show which books need to be returned
                        break;
                    }
                    
                    int fine = user->check_fine();
                    if(fine > 0){
                        cout << "Cannot remove user. Student has a pending fine of Rs." << fine << endl;
                        cout << "Please ensure all fines are paid before removing the user." << endl;
                        break;
                    }
                    
                    string student_name = user->name; 
                    cout << "\nRemoving student:" << endl;
                    cout << "Name: " << student_name << endl;
                    cout << "ID: " << user_id << endl;
                    librarian->remove_student_from_Lib(user_id);
                }
                else if(role==2){
                    cout<<"Enter user id"<<endl;
                    string user_id_str;
                    int user_id;
                    cin >> user_id_str;
                    if (!isNumeric(user_id_str)) {
                        cout << "Invalid user ID (must be numeric)" << endl;
                        break;
                    }
                    user_id = stoi(user_id_str);
                    Faculty* user = getFaculty(user_id);
                    if(!user){
                        cout<<"Invalid user id"<<endl;
                        break;
                    }
                    
                    if(user->hasBorrowedBooks()) {
                        cout << "Cannot remove user. Faculty member must return all borrowed books first." << endl;
                        user->view_books(); 
                        break;
                    }
                    
                    string faculty_name = user->name;
                    cout << "\nRemoving faculty member:" << endl;
                    cout << "Name: " << faculty_name << endl;
                    cout << "ID: " << user_id << endl;
                    librarian->remove_faculty_from_Lib(user_id);
                }
                break;
            }
            case 5: {
                cout<<"Enter new password"<<endl;
                string new_password;
                cin>>new_password;
                librarian->changePassword(new_password);
                cout<<"Password Set!!"<<endl;
                break;
            }
            case 6: {
                cout << "[1] Display All Books" << endl;
                cout << "[2] Display Specific Book" << endl;
                int display_choice;
                cin >> display_choice;
                if (display_choice == 1) {
                    library.displayAllBooks();
                } else if (display_choice == 2) {
                    cout << "Enter book ID: ";
                    string book_id_str;
                    int book_id;
                    cin >> book_id_str;
                    if (!isNumeric(book_id_str)) {
                        cout << "Invalid book ID" << endl;
                        break;
                    }
                    book_id = stoi(book_id_str);
                    Book* book = getBook(book_id);
                    Library::displayBook(book);
                } else {
                    cout << "Invalid choice" << endl;
                }
                break;
            }
            case 7: {
                librarian->viewAllStudents();
                break;
            }
            case 8: {
                librarian->viewAllFaculty();
                break;
            }
            case 9: {
                librarian->displayUserDetails();
                break;
            }
            case 10: {
                cout<<"Logged out successfully"<<endl;
                return;
                break;
            }
            default: {
                cout<<"Invalid choice"<<endl;
                break;
            }
        }
    }
}

// studentuser(): Handles student login and menu interface
void studentuser(){
    string name;
    cout << "Enter your name" << endl;
    cin.ignore();
    getline(cin, name);
    
    cout << "Enter your user id" << endl;
    string user_id_str;
    int user_id;
    cin >> user_id_str;
    if (!isNumeric(user_id_str)) {
        cout << "Invalid user ID (must be numeric)" << endl;
        return;
    }
    user_id = stoi(user_id_str);
    
    Student* student = getStudent(user_id);
    if (!student || student->name != name) {
        cout << "Invalid name or user id" << endl;
        return;
    }
    
    cout << "Enter Password" << endl;
    string password;
    cin >> password;
    if(!student->check_credentials(user_id,password)){
        cout << "Invalid password" << endl;
        return;
    }
    
    cout << "\n+-------------------------------------------+" << endl;
    cout << "|            Welcome " << left << setw(20) << student->name << "|" << endl;
    cout << "+-------------------------------------------+" << endl << endl;
    
    while(1){
        cout<<"What would you like to do?"<<endl;
        cout<<"[1] Borrow Book"<<endl;
        cout<<"[2] Return Book"<<endl;
        cout<<"[3] View Borrowed Books"<<endl;
        cout<<"[4] View Borrowing History"<<endl;
        cout<<"[5] Check Fine"<<endl;
        cout<<"[6] Pay Fine"<<endl;
        cout<<"[7] Check Overdue Books"<<endl;
        cout<<"[8] Change Password"<<endl;
        cout<<"[9] View My Details"<<endl;
        cout<<"[10] View Book Details"<<endl;
        cout<<"[11] Cancel Reservation"<<endl;
        cout<<"[12] Logout"<<endl;
        int choice;
        cin>>choice;
        switch (choice) {
            case 1: {
                cout<<"Enter book id"<<endl;
                string book_id_str;
                int book_id;
                cin >> book_id_str;
                if (!isNumeric(book_id_str)) {
                    cout << "Invalid book ID (must be numeric)" << endl;
                    break;
                }
                book_id = stoi(book_id_str);
                student->borrowBook(book_id);
                break;
            }
            case 2: {
                cout<<"Enter book id"<<endl;
                string book_id_str;
                int book_id;
                cin >> book_id_str;
                if (!isNumeric(book_id_str)) {
                    cout << "Invalid book ID (must be numeric)" << endl;
                    break;
                }
                book_id = stoi(book_id_str);
                student->returnBook(book_id);
                break;
            }
            case 3: {
                student->view_books();
                break;
            }
            case 4: {
                student->view_borrowing_history();
                break;
            }
            case 5: {
                cout<<"Pending Fine : "<<student->check_fine()<<endl;;
                break;
            }
            case 6: {
                student->payFine();
                break;
            }
            case 7: {
                if(!student->checkOverdue(15)){
                    cout<<"No overdue books"<<endl;
                }
                break;
            }
            case 8: {
                cout<<"Enter new password"<<endl;
                string new_password;
                cin>>new_password;
                student->changePassword(new_password);
                cout<<"Password Set!!"<<endl;
                break;
            }
            case 9: {
                student->displayUserDetails();
                break;
            }
            case 10: {
                cout << "Enter book ID: ";
                string book_id_str;
                int book_id;
                cin >> book_id_str;
                if (!isNumeric(book_id_str)) {
                    cout << "Invalid book ID" << endl;
                    break;
                }
                book_id = stoi(book_id_str);
                Book* book = getBook(book_id);
                Library::displayBook(book);
                break;
            }
            case 11: {
                student->cancelReservation();
                break;
            }
            case 12: {
                cout<<"Logged out successfully"<<endl;
                return;
                break;
            }
            default: {
                cout<<"Invalid choice"<<endl;
                break;
            }
        }
    }
}

// Function to display the main menu
void facultyuser(){
    string name;
    cout << "Enter your name" << endl;
    cin.ignore();
    getline(cin, name);
    
    cout << "Enter your user id" << endl;
    string user_id_str;
    int user_id;
    cin >> user_id_str;
    if (!isNumeric(user_id_str)) {
        cout << "Invalid user ID (must be numeric)" << endl;
        return;
    }
    user_id = stoi(user_id_str);
    
    Faculty* faculty = getFaculty(user_id);
    if (!faculty || faculty->name != name) {
        cout << "Invalid name or user id" << endl;
        return;
    }
    
    cout << "Enter Password" << endl;
    string password;
    cin >> password;
    if (!faculty->check_credentials(user_id, password)) {
        cout << "Invalid password" << endl;
        return;
    }
    
    cout << "\n+-------------------------------------------+" << endl;
    cout << "|            Welcome " << left << setw(20) << faculty->name << "|" << endl;
    cout << "+-------------------------------------------+" << endl << endl;
    
    while (true)
    {
        cout<<"What would you like to do?"<<endl;
        cout<<"[1] Borrow Book"<<endl;
        cout<<"[2] Return Book"<<endl;
        cout<<"[3] View Borrowed Books"<<endl;
        cout<<"[4] View Borrowing History"<<endl;
        cout<<"[5] Check Overdue Books"<<endl;
        cout<<"[6] Change Password"<<endl;
        cout<<"[7] View My Details"<<endl;
        cout<<"[8] View Book Details"<<endl;
        cout<<"[9] Cancel Reservation"<<endl;
        cout<<"[10] Logout"<<endl;
        int choice;
        cin>>choice;
        switch (choice) {
            case 1: {
                cout<<"Enter book id"<<endl;
                string book_id_str;
                int book_id;
                cin >> book_id_str;
                if (!isNumeric(book_id_str)) {
                    cout << "Invalid book ID (must be numeric)" << endl;
                    break;
                }
                book_id = stoi(book_id_str);
                faculty->borrowBook(book_id);
                break;
            }
            case 2: {
                cout<<"Enter book id"<<endl;
                string book_id_str;
                int book_id;
                cin >> book_id_str;
                if (!isNumeric(book_id_str)) {
                    cout << "Invalid book ID (must be numeric)" << endl;
                    break;
                }
                book_id = stoi(book_id_str);
                faculty->returnBook(book_id);
                break;
            }
            case 3: {
                faculty->view_books();
                break;
            }
            case 4: {
                faculty->view_borrowing_history();
                break;
            }
            case 5: {
                if(!faculty->checkOverdue(60)){
                    cout<<"No overdue books"<<endl;
                }
                break;
            }
            case 6: {
                cout<<"Enter new password"<<endl;
                string new_password;
                cin>>new_password;
                faculty->changePassword(new_password);
                cout<<"Password Set!!"<<endl;
                break;
            }
            case 7: {
                faculty->displayUserDetails();
                break;
            }
            case 8: {
                cout << "Enter book ID: ";
                string book_id_str;
                int book_id;
                cin >> book_id_str;
                if (!isNumeric(book_id_str)) {
                    cout << "Invalid book ID" << endl;
                    break;
                }
                book_id = stoi(book_id_str);
                Book* book = getBook(book_id);
                Library::displayBook(book);
                break;
            }
            case 9: {
                faculty->cancelReservation();
                break;
            }
            case 10: {
                cout<<"Logged out successfully"<<endl;
                return;
                break;
            }
            default: {
                cout<<"Invalid choice"<<endl;
                break;
            }
        }
    }
    
}

// Main program flow:
// 1. Initialize library
// 2. Load existing data or create demo data
// 3. Display main menu
// 4. Handle user interactions
// 5. Save all data before exit
int main(){ 
    // Display welcome message in a decorative box
    cout << "\n+-------------------------------------------+" << endl;
    cout << "|                                           |" << endl;
    cout << "|       Library Management System           |" << endl;
    cout << "|                                           |" << endl;
    cout << "+-------------------------------------------+" << endl << endl;

    // Clear the library data
    library.clear();

    // Load Students if file exists and is not empty; otherwise, use demo data.
    ifstream studentsFile("students.txt");
    if (studentsFile && studentsFile.peek() != ifstream::traits_type::eof()){
        loadStudents();
    } else {
        vector<Student*> studentlist;
        studentlist.push_back(new Student(2, "Gautam Arora", "gautam@example.com", "1234567891", 220405));
        studentlist.push_back(new Student(3, "Rahul Yadav", "rahul@example.com", "1234567892", 230756));
        studentlist.push_back(new Student(4, "Rohan Raju", "rohan@example.com", "1234567893", 240906));
        studentlist.push_back(new Student(5, "Madhav Gupta", "madhav@example.com", "1234567894", 210606));
        studentlist.push_back(new Student(6, "Kavya Nair", "kavya@example.com", "1234567895", 191080));
        for (auto& student : studentlist) {
            addstudent(student);
        }
    }
    studentsFile.close();

    // Load Faculties if file exists and is not empty; otherwise, use demo data.
    ifstream facultiesFile("faculties.txt");
    if (facultiesFile && facultiesFile.peek() != ifstream::traits_type::eof()){
        loadFaculties();
    } else {
        vector<Faculty*> Facultylist;
        Facultylist.push_back(new Faculty(7, "Prof. Anil Kumar", "anil@example.com", "9876543211"));
        Facultylist.push_back(new Faculty(8, "Prof. Meera Iyer", "meera@example.com", "9876543212"));
        Facultylist.push_back(new Faculty(9, "Prof. Rajesh Singh", "rajesh@example.com", "9876543213"));
        for (auto& faculty : Facultylist) {
            addFaculty(faculty);
        }
    }
    facultiesFile.close();

    // Load Books if file exists and is not empty; otherwise, use demo data.
    ifstream booksFile("books.txt");
    if (booksFile && booksFile.peek() != ifstream::traits_type::eof()){
        loadBooks();
    } else {
        vector<Book> bookList = {
            {1, "Introduction to Algorithms", "Thomas H. Cormen", "MIT Press", "9780262046305", 2009},
            {2, "Cracking the Coding Interview", "Gayle Laakmann McDowell", "CareerCup", "9780984782857", 2015},
            {3, "Data Structures and Algorithms Made Easy", "Narasimha Karumanchi", "CareerMonk", "9788193245279", 2017},
            {4, "The 7 Habits of Highly Effective People", "Stephen Covey", "Free Press", "9780743269513", 2004},
            {5, "Atomic Habits", "James Clear", "Avery", "9780735211292", 2018},
            {6, "Think and Grow Rich", "Napoleon Hill", "Penguin Books", "9780141189681", 2005},
            {7, "The Power of Now", "Eckhart Tolle", "New World Library", "9781577314806", 2004},
            {8, "Man's Search for Meaning", "Viktor E. Frankl", "Beacon Press", "9780807014295", 2006},
            {9, "Deep Work", "Cal Newport", "Grand Central", "9781455586691", 2016},
            {10, "Mindset: The New Psychology of Success", "Carol S. Dweck", "Ballantine Books", "9780345472328", 2007}
        };
        for (const auto& book : bookList) {
            addBook(book);
        }
    }
    booksFile.close();

    
    // Load Librarians if file exists and is not empty; otherwise, use demo data.
    ifstream librariansFile("librarians.txt");
    if (librariansFile && librariansFile.peek() != ifstream::traits_type::eof()){
        loadLibrarians();
    } else {
        Librarian* libra = new Librarian(1, "Mr. LibGod", "libgod@example.com", "9999999999");
        addLibrarian(libra);
    }
    librariansFile.close();

    // Load currently borrowed books if file exists and is not empty
    ifstream currentlyborrowedFile("currently_borrowed.txt");
    if (currentlyborrowedFile && currentlyborrowedFile.peek() != ifstream::traits_type::eof()) {
        loadcurrentlyborrowed();
    }
    currentlyborrowedFile.close();

    // Load borrowing history if file exists and is not empty
    ifstream historyFile("borrowing_history.txt");
    if (historyFile && historyFile.peek() != ifstream::traits_type::eof()) {
        loadBorrowingHistory();
    }
    historyFile.close();

    // Load reserved books if file exists and is not empty
    ifstream reservedBooksFile("reserved_books.txt");
    if (reservedBooksFile && reservedBooksFile.peek() != ifstream::traits_type::eof()) {
        loadReservedBooks();
    }
    reservedBooksFile.close();

    // Main menu
    cout << "Welcome to the Library Management System" << endl;
    bool running = true;
    while (running) {
        cout << "[1] Login as Librarian" << endl;
        cout << "[2] Login as Student" << endl;
        cout << "[3] Login as Faculty" << endl;
        cout << "[4] Exit" << endl;
        int choice;
        cin >> choice;
        switch (choice) {
            case 1: {
                librarianuser();
                break;
            }
            case 2: {
                studentuser();
                break;
            }
            case 3: {
                facultyuser();
                break;
            }
            case 4: {
                cout << "Thank you for using the Library Management System. Have a Good Day!!" << endl;
                running = false;
                break;
            }
            default: {
                cout << "Invalid choice" << endl;
                break;
            }
        }
    }

    // Save data to files
    saveBooks();
    saveStudents();
    saveFaculties();
    saveLibrarians();
    savecurrentlyborrowed();
    saveBorrowingHistory();
    saveReservedBooks();
    return 0;
}

