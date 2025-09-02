# Library Management System

This is a C++ application designed to manage library operations with role-based access control and data persistence.

## Features

### For Students
- Borrow up to 3 books at a time
- Return books
- View currently borrowed books
- Check borrowing history
- Pay fines for overdue books (₹10/day after 15 days)
- View book details
- Change password
- View personal details
- Reserve books that are currently borrowed by others

### For Faculty
- Borrow up to 5 books at a time
- Return books
- View currently borrowed books
- Check borrowing history
- View book details
- Change password
- View personal details
- Extended borrowing period (60 days vs 15 days for students)
- Reserve books that are currently borrowed by others

### For Librarians
- Add/remove books
- Add/remove users (students and faculty)
- View all books
- View specific book details
- View all registered students
- View all registered faculty members
- Change password
- View personal details

## Security Features
- Password-protected access for all users
- Role-based permissions
- Input validation for:
  - Email addresses
  - Phone numbers (10 digits)
  - User IDs
  - Book details
  - Empty fields

## Data Management
- Persistent storage using text files
- Automatic data loading and saving
- Maintains separate files for:
  - Books
  - Students
  - Faculty
  - Librarians
  - Borrowing history
  - Currently borrowed books
  - Reserved books
- NOTE => Librarian can delete all the books but when system is restarted it will again load all demo books to maintain functionality of the Library!! same for all the user as well


## Sample Books
The system comes pre-loaded with a curated collection of books, including titles on data structures, algorithms, self-help, and inspiring books.

## Default Users
### Librarian
- ID: 1
- Name: Mr. LibGod
- Email: libgod@example.com
- Phone: 9999999999

### Students
- Gautam Arora (ID: 2, Roll Number: 220405)
- Rahul Yadav (ID: 3, Roll Number: 230756)
- Rohan Raju (ID: 4, Roll Number: 240906)
- Madhav Gupta (ID: 5, Roll Number: 210606)
- Kavya Nair (ID: 6, Roll Number: 191080)

### Faculty
- Prof. Anil Kumar (ID: 7)
- Prof. Meera Iyer (ID: 8)
- Prof. Rajesh Singh (ID: 9)

## Important Notes
1. Default password for all users is "password"
2. Students have a 15-day borrowing period
3. Faculty have a 60-day borrowing period
4. Fine calculation: ₹10 per day after due date only applicable for Students
5. Book IDs and User IDs must be positive numbers
6. All users can view their borrowing history
7. System prevents removal of:
   - Books that are currently borrowed
   - Users who have borrowed books
   - Users with pending fines

## Getting Started

### Prerequisites
- A C++ compiler (e.g., g++, clang++)
- Command line access

### Steps to Run the Application

1. Clone the Repository:
   ```bash
   git clone <repository-url>
   cd <repository-directory>
   ```

2. Compile the Code:
   Use a C++ compiler to compile the `main.cpp` file. For example, using g++:
   ```bash
   g++ main.cpp -o library_management_system
   ```

3. Run the Executable:
   Execute the compiled program:
   ```bash
   ./library_management_system
   ```

4. Login:
   - Choose to login as a Librarian, Student, or Faculty.
   - Use the default password "password" for the first login.

5. Reserve Books:
   - If a book is currently borrowed, you can choose to reserve it.
   - Only the user who reserved the book can borrow it once it becomes available.

6. Logout:
   - Always use the logout option to ensure data is saved.

## Important Reminders
- Pay any pending fines before returning books.
- Keep track of borrowing deadlines to avoid fines.
- Regularly change your password for security.

## Data Storage
All data is stored in text files in the output folder:
- `books.txt`
- `students.txt`
- `faculties.txt`
- `librarians.txt`
- `borrowing_history.txt`
- `currently_borrowed.txt`
- `reserved_books.txt`

## Technical Details
- Written in C++
- Uses file-based persistence
- Input validation for all user inputs
- Memory management for user objects
- Error handling for file operations
- Clean and modular code structure

## Classes and Their Functions

### Book Class
```cpp
class Book {
    int book_id;
    string title, author, publisher, isbn;
    int year;
    string status;
    int borrower_id;
    long long borrowed_time;

    // Constructor
    Book(int book_id, string title, string author, string publisher, 
         string isbn, int year, string status = "Available", 
         int borrower_id = -1, long long borrowed_time = 0);
};
```

### Account Class
```cpp
class Account {
    int user_id;
    vector<int> borrowed_books;
    unordered_map<int, long long> borrowed_time;
    vector<Book*> borrowing_history;
    int prev_fine;

    // Functions
    void view_books();
    int check_fine();
    void pay_fine();
    bool hasOverdue(int limit);
    void view_borrowing_history();
    void add_borrowing_history(Book* book);
};
```

### User Class (Base Class)
```cpp
class User {
    // Functions
    virtual void borrowBook(int book_id) = 0;
    virtual void returnBook(int book_id) = 0;
    bool check_credentials(int user_id, string password);
    void changePassword(string new_password);
    int check_fine();
    void payFine();
    string view_password();
    void view_books();
    void view_borrowing_history();
    bool checkOverdue(int limit);
};
```

### Student Class (Derived from User)
```cpp
class Student : public User {
    // Functions
    void borrowBook(int book_id) override;  // Limit: 3 books, 15 days
    void returnBook(int book_id) override;
};
```

### Faculty Class (Derived from User)
```cpp
class Faculty : public User {
    // Functions
    void borrowBook(int book_id) override;  // Limit: 5 books, 60 days
    void returnBook(int book_id) override;
};
```

### Librarian Class (Derived from User)
```cpp
class Librarian : public User {
    // Functions
    void borrowBook(int book_id) override;
    void returnBook(int book_id) override;
    void add_Book_to_Lib(const Book& book);
    void remove_Book_from_Lib(int book_id);
    void add_student_to_Lib(Student* user);
    void add_faculty_to_Lib(Faculty* user);
    void remove_student_from_Lib(int user_id);
    void remove_faculty_from_Lib(int user_id);
};
```

### Library Class
```cpp
class Library {
    // Friend Functions
    void addBook(const Book& book);
    void addstudent(Student* user);
    void addFaculty(Faculty* user);
    void addLibrarian(Librarian* user);
    void removeBook(int book_id);
    void removeStudent(int user_id);
    void removeFaculty(int user_id);
    void removeLibrarian(int user_id);
    void saveBooks();
    void loadBooks();
    void saveStudents();
    void loadStudents();
    void saveFaculties();
    void loadFaculties();
    void saveLibrarians();
    void loadLibrarians();
    void saveBorrowingHistory();
    void loadBorrowingHistory();
    void loadcurrentlyborrowed();
    void savecurrentlyborrowed();
    Book* getBook(int book_id);
    Student* getStudent(int user_id);
    Faculty* getFaculty(int user_id);
    Librarian* getLibrarian(int user_id);
};
```

## Compilation
Use the provided Makefile to compile the project:
```bash
make
```

To clean the build:
```bash
make clean
```

## Usage
1. Run the compiled program
2. Choose user type (Librarian/Student/Faculty)
3. Enter user ID and password
4. Access available functions based on user role

## Features

- User Management (Students, Faculty, Librarians)
- Book Management (Add, Remove, Borrow, Return)
- Fine Calculation for Overdue Books
- Borrowing History Tracking
- Persistent Data Storage
- Email Validation
- Role-based Access Control

## Project Structure

```
.            # Source files
|── main.cpp
├── output/         # Data storage files
├── Makefile
└── README.md
```

## Code Overview

The system is built with the following key components:

### Classes

1. **Book Class**
   - Stores book information (ID, title, author, publisher, ISBN, year)
   - Tracks borrowing status and borrower details

2. **Account Class**
   - Manages user's borrowed books and borrowing history
   - Handles fine calculations and overdue book tracking

3. **User Class (Abstract)**
   - Base class for all user types
   - Handles authentication and basic user operations
   - Implements email validation

4. **Derived User Classes**
   - **Student**: Can borrow up to 3 books for 15 days
   - **Faculty**: Can borrow up to 5 books for 60 days
   - **Librarian**: Manages library operations (add/remove books and users)

5. **Library Class**
   - Central class managing all library operations
   - Maintains collections of books and users
   - Implements file operations for data persistence

### Key Features Implementation

1. **File Operations**
   - Saves and loads data for books, users, and borrowing history
   - Uses pipe-separated format for data storage
   - Automatically creates demo data if files are empty

2. **User Interface**
   - Role-specific menus for different user types
   - Input validation and error handling
   - Clear feedback messages for all operations

3. **Fine System**
   - Calculates fines based on overdue days
   - Different limits for students (15 days) 
   - Prevents borrowing when fines are pending

## Prerequisites

- C++11 or later
- GNU Make
- g++ compiler

## Building and Running

### Windows (Using MinGW)

1. Install MinGW:
   - Download and install MSYS2 from https://www.msys2.org/
   - Open MSYS2 terminal and run: `pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make`

2. Build and run:
```bash
# Open Command Prompt or PowerShell
cd path/to/project
mkdir database
g++ src/main.cpp src/ui.cpp -o library_system
./library_system
```

### macOS

1. Install prerequisites:
```bash
brew install gcc make
```

2. Build and run:
```bash
cd path/to/project
mkdir database
g++-11 src/main.cpp src/ui.cpp -o library_system
./library_system
```

### Linux

1. Install prerequisites:
```bash
sudo apt-get update
sudo apt-get install build-essential
```

2. Build and run:
```bash
cd path/to/project
g++ src/main.cpp src/ui.cpp -o library_system
./library_system
```

## Default Login Credentials

### Librarian
- ID: 1
- Password: password

### Students
- IDs: 2-6
- Password: password

### Faculty
- IDs: 7-9
- Password: password

## Error Handling

The system includes validation for:
- Email format
- Duplicate email addresses
- Overdue books
- Maximum borrowing limits
- Invalid user credentials

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request 