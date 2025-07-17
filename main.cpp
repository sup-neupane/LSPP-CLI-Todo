#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <ctime>
#include <iomanip>

class Task {
private:
    int id;
    std::string description;
    bool isCompleted;
    std::string createdAt;
    std::string completedAt;

public:
    Task(int taskId, const std::string& desc) 
        : id(taskId), description(desc), isCompleted(false), completedAt("") {
        createdAt = Task::getCurrentTimestamp();
    }

    // Getters
    int getId() const { return id; }
    std::string getDescription() const { return description; }
    bool getIsCompleted() const { return isCompleted; }
    std::string getCreatedAt() const { return createdAt; }
    std::string getCompletedAt() const { return completedAt; }

    void markComplete() {
        isCompleted = true;
        completedAt = Task::getCurrentTimestamp();
    }

    std::string toFileString() const {
        return std::to_string(id) + "|" + description + "|" + 
               (isCompleted ? "1" : "0") + "|" + createdAt + "|" + completedAt;
    }

    static std::unique_ptr<Task> fromFileString(const std::string& line) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;

        while (std::getline(ss, token, '|')) {
            parts.push_back(token);
        }

        if (parts.size() < 4) {
            return nullptr;
        }

        try {
            int taskId = std::stoi(parts[0]);
            auto task = std::make_unique<Task>(taskId, parts[1]);
            task->isCompleted = (parts[2] == "1");
            task->createdAt = parts[3];
            if (parts.size() > 4) {
                task->completedAt = parts[4];
            }
            return task;
        } catch (const std::exception&) {
            return nullptr;
        }
    }

private:
    static std::string getCurrentTimestamp() {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

class TodoStorage {
private:
    std::string filename;

public:
    explicit TodoStorage(const std::string& file = "todos.txt") : filename(file) {}

    std::vector<std::unique_ptr<Task>> loadTasks() const {
        std::vector<std::unique_ptr<Task>> tasks;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            return tasks; // Return empty vector if file doesn't exist
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                auto task = Task::fromFileString(line);
                if (task) {
                    tasks.push_back(std::move(task));
                }
            }
        }

        return tasks;
    }

    bool saveTasks(const std::vector<std::unique_ptr<Task>>& tasks) const {
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "Error: Unable to save tasks to file." << std::endl;
            return false;
        }

        for (const auto& task : tasks) {
            file << task->toFileString() << std::endl;
        }

        return true;
    }
};

class TodoManager {
private:
    std::vector<std::unique_ptr<Task>> tasks;
    std::unique_ptr<TodoStorage> storage;

public:
    explicit TodoManager(std::unique_ptr<TodoStorage> stor) 
        : storage(std::move(stor)) {
        tasks = storage->loadTasks();
    }

    bool addTask(const std::string& description) {
        if (isDescriptionEmpty(description)) {
            std::cout << "Error: Task description cannot be empty." << std::endl;
            return false;
        }

        int taskId = generateNextId();
        auto newTask = std::make_unique<Task>(taskId, trimString(description));
        tasks.push_back(std::move(newTask));

        if (storage->saveTasks(tasks)) {
            std::cout << "Task added successfully with ID: " << taskId << std::endl;
            return true;
        }
        return false;
    }

    void listTasks() const {
        if (tasks.empty()) {
            std::cout << "No tasks found. Add a task with 'add <description>'" << std::endl;
            return;
        }

        std::cout << "\n=== Todo List ===" << std::endl;
        for (const auto& task : tasks) {
            std::string status = task->getIsCompleted() ? "✓" : "○";
            std::cout << task->getId() << ". [" << status << "] " 
                      << task->getDescription() << std::endl;
        }
        std::cout << std::endl;
    }

    bool completeTask(int taskId) {
        Task* task = findTaskById(taskId);
        if (!task) {
            std::cout << "Error: Task with ID " << taskId << " not found." << std::endl;
            return false;
        }

        if (task->getIsCompleted()) {
            std::cout << "Task " << taskId << " is already completed." << std::endl;
            return false;
        }

        task->markComplete();
        if (storage->saveTasks(tasks)) {
            std::cout << "Task " << taskId << " marked as completed." << std::endl;
            return true;
        }
        return false;
    }

    bool removeTask(int taskId) {
        auto it = std::find_if(tasks.begin(), tasks.end(),
            [taskId](const std::unique_ptr<Task>& task) {
                return task->getId() == taskId;
            });

        if (it == tasks.end()) {
            std::cout << "Error: Task with ID " << taskId << " not found." << std::endl;
            return false;
        }

        tasks.erase(it);
        if (storage->saveTasks(tasks)) {
            std::cout << "Task " << taskId << " removed successfully." << std::endl;
            return true;
        }
        return false;
    }

private:
    Task* findTaskById(int taskId) const {
        auto it = std::find_if(tasks.begin(), tasks.end(),
            [taskId](const std::unique_ptr<Task>& task) {
                return task->getId() == taskId;
            });
        return (it != tasks.end()) ? it->get() : nullptr;
    }

    int generateNextId() const {
        int maxId = 0;
        for (const auto& task : tasks) {
            maxId = std::max(maxId, task->getId());
        }
        return maxId + 1;
    }

    bool isDescriptionEmpty(const std::string& description) const {
        return trimString(description).empty();
    }

    std::string trimString(const std::string& str) const {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            return "";
        }
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
};

class TodoCLI {
private:
    std::unique_ptr<TodoManager> manager;

public:
    TodoCLI() {
        auto storage = std::make_unique<TodoStorage>();
        manager = std::make_unique<TodoManager>(std::move(storage));
    }

    void run(int argc, char* argv[]) {
        if (argc < 2) {
            showHelp();
            return;
        }

        std::string command = toLowerCase(argv[1]);

        if (command == "add") {
            handleAddCommand(argc, argv);
        } else if (command == "list") {
            manager->listTasks();
        } else if (command == "complete") {
            handleCompleteCommand(argc, argv);
        } else if (command == "remove") {
            handleRemoveCommand(argc, argv);
        } else if (command == "help" || command == "--help" || command == "-h") {
            showHelp();
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            showHelp();
        }
    }

private:
    void handleAddCommand(int argc, char* argv[]) {
        if (argc < 3) {
            std::cout << "Error: Please provide a task description." << std::endl;
            std::cout << "Usage: ./todo add <task description>" << std::endl;
            return;
        }

        std::string description = buildDescriptionFromArgs(argc, argv, 2);
        manager->addTask(description);
    }

    void handleCompleteCommand(int argc, char* argv[]) {
        if (argc < 3) {
            std::cout << "Error: Please provide a task ID." << std::endl;
            std::cout << "Usage: ./todo complete <task_id>" << std::endl;
            return;
        }

        try {
            int taskId = std::stoi(argv[2]);
            manager->completeTask(taskId);
        } catch (const std::exception&) {
            std::cout << "Error: Task ID must be a number." << std::endl;
        }
    }

    void handleRemoveCommand(int argc, char* argv[]) {
        if (argc < 3) {
            std::cout << "Error: Please provide a task ID." << std::endl;
            std::cout << "Usage: ./todo remove <task_id>" << std::endl;
            return;
        }

        try {
            int taskId = std::stoi(argv[2]);
            manager->removeTask(taskId);
        } catch (const std::exception&) {
            std::cout << "Error: Task ID must be a number." << std::endl;
        }
    }

    void showHelp() const {
        std::cout << R"(
CLI Todo Application - Help

Usage: ./todo <command> [arguments]

Commands:
  add <description>    Add a new todo task
  list                 Display all current tasks with their status
  complete <task_id>   Mark a task as complete
  remove <task_id>     Remove a task from the list
  help                 Show this help message

Examples:
  ./todo add "Buy groceries"
  ./todo list
  ./todo complete 1
  ./todo remove 2
)" << std::endl;
    }

    std::string toLowerCase(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    std::string buildDescriptionFromArgs(int argc, char* argv[], int startIndex) const {
        std::string description;
        for (int i = startIndex; i < argc; ++i) {
            if (i > startIndex) {
                description += " ";
            }
            description += argv[i];
        }
        return description;
    }
};

int main(int argc, char* argv[]) {
    try {
        TodoCLI cli;
        cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
