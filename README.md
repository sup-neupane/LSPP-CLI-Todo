# CLI Todo Application (C++)

A simple command-line todo application built with modern C++, following Clean Code principles for maintainability and readability.

## Features

- Add new todo tasks
- List all tasks with completion status
- Mark tasks as complete
- Remove tasks from the list
- Persistent storage using text file
- Timestamps for task creation and completion

## Installation & Setup

### Prerequisites
- C++ compiler with C++14 support (g++, clang++, or Visual Studio)
- Make (optional, for using Makefile)

### Building the Application

#### Option 1: Using g++ directly
```bash
g++ -std=c++14 -Wall -Wextra -O2 -o todo main.cpp
```

#### Option 2: Using the provided Makefile
```bash
make
```

#### Option 3: Debug build
```bash
g++ -std=c++14 -Wall -Wextra -g -DDEBUG -o todo_debug main.cpp
```

## Usage

### Basic Commands

```bash
# Add a new task
./todo add "Buy groceries"
./todo add "Complete project report"

# List all tasks
./todo list

# Mark a task as complete
./todo complete 1

# Remove a task
./todo remove 2

# Show help
./todo help
```

### Example Session

```bash
$ ./todo add "Learn C++"
Task added successfully with ID: 1

$ ./todo add "Build todo app"
Task added successfully with ID: 2

$ ./todo list

=== Todo List ===
1. [○] Learn C++
2. [○] Build todo app

$ ./todo complete 1
Task 1 marked as completed.

$ ./todo list

=== Todo List ===
1. [✓] Learn C++
2. [○] Build todo app

$ ./todo remove 2
Task 2 removed successfully.

$ ./todo list

=== Todo List ===
1. [✓] Learn C++
```

## Architecture

The application follows a clean, modular architecture using modern C++ practices:

```
main.cpp
├── Task (Data Model)
├── TodoStorage (Persistence Layer)
├── TodoManager (Business Logic)
└── TodoCLI (User Interface)
```

### Class Responsibilities

- **Task**: Represents a single todo item with properties and behaviors
- **TodoStorage**: Handles reading/writing tasks to text file
- **TodoManager**: Core business logic for managing tasks
- **TodoCLI**: Command-line interface and argument parsing

## Clean Code Principles Applied

### 1. Single Responsibility Principle (SRP)
- Each class has a single, well-defined purpose
- `Task` handles task data and state management
- `TodoStorage` handles file persistence operations
- `TodoManager` manages business logic
- `TodoCLI` handles user interaction

### 2. Meaningful Names
```cpp
// Good: Descriptive method names
void markComplete();
Task* findTaskById(int taskId) const;
int generateNextId() const;

// Good: Clear variable names
int taskId = generateNextId();
auto newTask = std::make_unique<Task>(taskId, trimString(description));
```

### 3. Small Functions
- Functions are kept short and focused
- Each function has a single responsibility
- Complex operations are broken into smaller helper functions

### 4. Avoid Deep Nesting
```cpp
// Good: Early returns reduce nesting
bool addTask(const std::string& description) {
    if (isDescriptionEmpty(description)) {
        std::cout << "Error: Task description cannot be empty." << std::endl;
        return false;
    }
    
    // Continue with main logic...
}
```

### 5. Error Handling
- Proper exception handling with try-catch blocks
- Input validation for all user commands
- Graceful error messages for invalid operations

### 6. Modern C++ Features
- Smart pointers (`std::unique_ptr`) for automatic memory management
- STL algorithms (`std::find_if`, `std::transform`)
- Range-based for loops
- RAII (Resource Acquisition Is Initialization)

### 7. Const Correctness
- Const member functions for read-only operations
- Const references for parameters
- Immutable data where appropriate

### 8. Encapsulation
- Private member variables with public accessors
- Clear public interfaces
- Implementation details hidden from users

## Data Storage

Tasks are stored in a `todos.txt` file with pipe-separated values:

```
1|Learn C++|1|2025-07-17 10:30:00|2025-07-17 11:00:00
2|Build todo app|0|2025-07-17 10:35:00|
```

Format: `ID|Description|IsCompleted|CreatedAt|CompletedAt`

## Memory Management

The application uses modern C++ memory management:

- **Smart Pointers**: `std::unique_ptr` for automatic cleanup
- **RAII**: Resources are automatically managed
- **No Memory Leaks**: Automatic deallocation when objects go out of scope
- **Move Semantics**: Efficient transfer of resources

## Error Handling

The application handles various error scenarios:

- Empty task descriptions
- Invalid task IDs (non-numeric)
- File read/write errors
- Non-existent tasks
- Malformed data in storage file

## Building and Testing

### Compilation Options

```bash
# Release build (optimized)
g++ -std=c++14 -Wall -Wextra -O2 -o todo main.cpp

# Debug build
g++ -std=c++14 -Wall -Wextra -g -DDEBUG -o todo_debug main.cpp

# With additional warnings
g++ -std=c++14 -Wall -Wextra -Wpedantic -O2 -o todo main.cpp
```

### Testing the Application

Test various scenarios:

```bash
# Test adding tasks
./todo add "Test task 1"
./todo add "Test task 2"

# Test listing
./todo list

# Test completion
./todo complete 1

# Test removal
./todo remove 2

# Test error cases
./todo add ""           # Empty description
./todo complete 999     # Non-existent task
./todo remove abc       # Invalid task ID
```

## Performance Considerations

- **File I/O**: Efficient file operations with minimal overhead
- **Memory Usage**: Smart pointers prevent memory leaks
- **String Operations**: Efficient string handling with move semantics
- **Algorithms**: STL algorithms for optimal performance

## Cross-Platform Compatibility

The application is designed to work across different platforms:

- **Windows**: Compile with Visual Studio or MinGW
- **Linux**: Compile with g++ or clang++
- **macOS**: Compile with clang++ or g++

## Future Enhancements

Potential improvements while maintaining Clean Code principles:

- Add task priority levels
- Implement task categories/tags
- Add due dates and reminders
- Support for task editing
- JSON storage format option
- Configuration file support
- Colored output for better UX

## Build System

### Makefile Example

```makefile
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
TARGET = todo
SOURCE = main.cpp

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET)_debug

.PHONY: clean debug
```

## Contributing

When contributing to this project, please maintain the Clean Code principles:

1. Use meaningful variable and function names
2. Keep functions small and focused
3. Add proper error handling
4. Follow const correctness
5. Use modern C++ features appropriately
6. Maintain consistent formatting
7. Add comments for complex logic

## Coding Standards

- **Naming**: camelCase for functions/variables, PascalCase for classes
- **Indentation**: 4 spaces (no tabs)
- **Braces**: Opening brace on same line
- **Const**: Use const wherever possible
- **Smart Pointers**: Prefer over raw pointers

## License

MIT License - Feel free to use and modify as needed.

## Compiler Support

Tested with:
- GCC 7.0+
- Clang 5.0+
- Visual Studio 2017+
- MinGW-w64