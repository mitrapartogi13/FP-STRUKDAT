#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <limits>
#include <cmath>
#include <iomanip>

// Platform-specific includes
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "bplus.h"

// --- Global State ---
BPlusTree<int> intTree;
BPlusTree<std::string> stringTree;
const std::string DB_FILENAME = "bplus.db";

enum class DataType { NONE, INTEGER, STRING };
DataType activeType = DataType::NONE;

// --- Forward Declarations ---
void handleMenu();
void handleSelectType();
void handleInsertFromFile();
void handleSearch();
void handleUpdate();
void handleDelete();
void handleMemoryAnalysis();
void handleSave();
void handleLoad();
void printPerformance(const std::string& operation, size_t N, size_t node_visits, std::chrono::duration<double, std::milli> duration);
void printComplexityAnalysis(const std::string& operation, size_t N);
void waitForEnter();

// --- Main Function ---
int main() {
    handleMenu();
    return 0;
}

// --- Menu & System Utilities ---
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int getch_portable() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

void displayMenu(int selectedOption) {
    clearScreen();
    std::cout << "--- B+ Tree Interactive Console (Order " << ORDER << ") ---\n";
    std::string currentTypeStr = "None";
    if (activeType == DataType::INTEGER) currentTypeStr = "Integer (" + std::to_string(intTree.size()) + " items)";
    else if (activeType == DataType::STRING) currentTypeStr = "String (" + std::to_string(stringTree.size()) + " items)";
    std::cout << "Active Data Type: " << currentTypeStr << "\n\n";
    
    std::vector<std::string> options = {
        "Select Data Type (Integer/String)", "Insert Data from File", "Search for a Key",
        "Update a Key", "Delete a Key (with Rebalancing)", "Analyze Memory Usage", "Save Tree to Database (bplus.db)",
        "Load Tree from Database (bplus.db)", "Exit"
    };

    for (int i = 0; i < options.size(); ++i) {
        if (i == selectedOption) std::cout << " > " << options[i] << " <\n";
        else std::cout << "   " << options[i] << "\n";
    }
    std::cout << "\nUse Arrow Keys to navigate, Enter to select.\n";
}

void handleMenu() {
    int selectedOption = 0;
    const int numOptions = 9;

    while (true) {
        displayMenu(selectedOption);
        int ch = getch_portable();
        
        if (ch == 224) {
            ch = getch_portable();
            if (ch == 72) selectedOption = (selectedOption - 1 + numOptions) % numOptions;
            if (ch == 80) selectedOption = (selectedOption + 1) % numOptions;
        } else if (ch == 27) {
            if (getch_portable() == '[') {
                ch = getch_portable();
                if (ch == 'A') selectedOption = (selectedOption - 1 + numOptions) % numOptions;
                if (ch == 'B') selectedOption = (selectedOption + 1) % numOptions;
            }
        } else if (ch == 13) {
            clearScreen();
            switch (selectedOption) {
                case 0: handleSelectType(); break;
                case 1: handleInsertFromFile(); break;
                case 2: handleSearch(); break;
                case 3: handleUpdate(); break;
                case 4: handleDelete(); break;
                case 5: handleMemoryAnalysis(); break;
                case 6: handleSave(); break;
                case 7: handleLoad(); break;
                case 8: std::cout << "Exiting program.\n"; return;
            }
            waitForEnter();
        }
    }
}

void waitForEnter() {
    std::cout << "\nPress Enter to return to the menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// --- Handler Functions ---

void handleSelectType() {
    std::cout << "--- Select Data Type ---\n1. Integer\n2. String\nChoose an option: ";
    int choice;
    std::cin >> choice;
    if (std::cin.fail() || (choice != 1 && choice != 2)) {
        std::cout << "Invalid choice.\n"; std::cin.clear(); return;
    }
    
    DataType newType = (choice == 1) ? DataType::INTEGER : DataType::STRING;
    if (activeType != newType) {
        std::cout << "Warning: Changing data type will clear the current tree. Are you sure? (y/n): ";
        char confirm; std::cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') {
            if (newType == DataType::INTEGER) { intTree = BPlusTree<int>(); activeType = DataType::INTEGER; std::cout << "Data type set to Integer.\n"; } 
            else { stringTree = BPlusTree<std::string>(); activeType = DataType::STRING; std::cout << "Data type set to String.\n"; }
        } else { std::cout << "Operation cancelled.\n"; }
    } else { std::cout << "Data type is already selected.\n"; }
}

void handleInsertFromFile() {
    if (activeType == DataType::NONE) { std::cout << "Please select a data type first.\n"; return; }
    std::cout << "Enter path to the data file (e.g., data/int100bplus.txt): ";
    std::string filename; std::cin >> filename;
    std::ifstream file(filename);
    if (!file.is_open()) { std::cout << "Error: Could not open file " << filename << std::endl; return; }

    size_t total_node_visits = 0, items_inserted = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    if (activeType == DataType::INTEGER) {
        int value; while (file >> value) { size_t op_visits = 0; intTree.insert(value, op_visits); total_node_visits += op_visits; items_inserted++; }
    } else {
        std::string line; while (std::getline(file, line)) { if (!line.empty()) { size_t op_visits = 0; stringTree.insert(line, op_visits); total_node_visits += op_visits; items_inserted++; } }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    size_t total_items = (activeType == DataType::INTEGER) ? intTree.size() : stringTree.size();

    std::cout << "\n--- Insertion Report ---\nSuccessfully inserted " << items_inserted << " items.\n";
    printPerformance("Total Insertion", total_items, total_node_visits, end_time - start_time);
    if (items_inserted > 0) std::cout << "Average Node Visits per Insert: " << std::fixed << std::setprecision(2) << static_cast<double>(total_node_visits) / items_inserted << "\n";
    printComplexityAnalysis("Insert", total_items);
}

template<typename T> void doSearch(BPlusTree<T>& tree) {
    std::cout << "Enter key to search for: ";
    T key; std::cin >> key;
    size_t node_visits = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    bool found = tree.search(key, node_visits);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "\n--- Search Report ---\n";
    if (found) std::cout << "Key '" << key << "' found.\n"; else std::cout << "Key '" << key << "' not found.\n";
    printPerformance("Search", tree.size(), node_visits, end_time - start_time);
    printComplexityAnalysis("Search", tree.size());
}

void handleSearch() {
    if (activeType == DataType::NONE || ((activeType == DataType::INTEGER && intTree.size() == 0) || (activeType == DataType::STRING && stringTree.size() == 0))) {
        std::cout << "Tree is empty or no data type selected.\n"; return;
    }
    if (activeType == DataType::INTEGER) doSearch(intTree); else doSearch(stringTree);
}

template<typename T> void doUpdate(BPlusTree<T>& tree) {
    T old_key, new_key;
    std::cout << "Enter the key to update: "; std::cin >> old_key;
    std::cout << "Enter the new key value: "; std::cin >> new_key;
    size_t search_visits, remove_visits, insert_visits;
    auto start_time = std::chrono::high_resolution_clock::now();
    tree.update(old_key, new_key, search_visits, remove_visits, insert_visits);
    auto end_time = std::chrono::high_resolution_clock::now();

    std::cout << "\n--- Update Report ---\n";
    if (search_visits == -1) {
        std::cout << "Key '" << old_key << "' not found for update.\n";
    } else {
        std::cout << "Updated '" << old_key << "' to '" << new_key << "'.\n";
        printPerformance("Update", tree.size(), search_visits + remove_visits + insert_visits, end_time - start_time);
        printComplexityAnalysis("Update", tree.size());
    }
}

void handleUpdate() {
    if (activeType == DataType::NONE || ((activeType == DataType::INTEGER && intTree.size() == 0) || (activeType == DataType::STRING && stringTree.size() == 0))) {
        std::cout << "Tree is empty or no data type selected.\n"; return;
    }
    if (activeType == DataType::INTEGER) doUpdate(intTree); else doUpdate(stringTree);
}

template<typename T> void doDelete(BPlusTree<T>& tree) {
    T key; std::cout << "Enter key to delete: "; std::cin >> key;
    size_t search_visits = 0;
    if (!tree.search(key, search_visits)) { std::cout << "Key '" << key << "' not found.\n"; return; }
    size_t remove_visits = 0;
    auto start_time = std::chrono::high_resolution_clock::now();
    tree.remove(key, remove_visits);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "\n--- Deletion Report ---\nKey '" << key << "' removed. Tree has been rebalanced.\n";
    printPerformance("Delete", tree.size(), remove_visits, end_time - start_time);
    printComplexityAnalysis("Delete", tree.size());
}

void handleDelete() {
    if (activeType == DataType::NONE || ((activeType == DataType::INTEGER && intTree.size() == 0) || (activeType == DataType::STRING && stringTree.size() == 0))) {
        std::cout << "Tree is empty or no data type selected.\n"; return;
    }
    if (activeType == DataType::INTEGER) doDelete(intTree); else doDelete(stringTree);
}

template<typename T>
void doMemoryAnalysis(BPlusTree<T>& tree) {
    struct Node {
        bool isLeaf;
        std::vector<T> keys;
        std::vector<Node*> children;
        Node* parent; Node* next_leaf; Node* prev_leaf;
    };

    size_t node_count = tree.getNodeCount();
    if (node_count == 0) {
        std::cout << "Tree is empty, no memory allocated for nodes.\n";
        return;
    }
    
    long long static_node_size = sizeof(Node);
    long long simple_total = node_count * static_node_size;
    long long deep_total = tree.calculateTotalMemory();

    std::cout << "--- Memory Usage Analysis ---\n\n";
    std::cout << "Basic Component Sizes:\n";
    std::cout << " - Size of a pointer (Node*): " << sizeof(Node*) << " bytes\n";
    std::cout << " - Size of a key (" << (activeType == DataType::INTEGER ? "int" : "string") << "): " << sizeof(T) << " bytes (Note: for strings, this is the size of the string object, not its character data)\n";
    std::cout << " - Static size of one Node object: " << static_node_size << " bytes\n\n";

    std::cout << "Total Structure Analysis:\n";
    std::cout << " - Total nodes created: " << node_count << "\n";
    std::cout << " - Memory for tree object (static): " << sizeof(tree) << " bytes\n";

    std::cout << "\n[Method 1: Simple Estimation (static size * node count)]\n";
    std::cout << " - Total memory for all Nodes: " << simple_total << " bytes (" << std::fixed << std::setprecision(2) << simple_total / 1024.0 << " KB)\n";
    std::cout << "   (This method is inaccurate as it doesn't account for data stored in vectors.)\n";

    std::cout << "\n[Method 2: Deep Calculation (traversing tree)]\n";
    std::cout << " - Total memory for data structure: " << deep_total << " bytes (" << std::fixed << std::setprecision(2) << deep_total / 1024.0 << " KB)\n";
    std::cout << "   (This method is more accurate, summing the capacity of all vectors.)\n";
}


void handleMemoryAnalysis() {
    if (activeType == DataType::NONE) {
        std::cout << "Please select a data type first.\n";
        return;
    }
    if (activeType == DataType::INTEGER) {
        doMemoryAnalysis(intTree);
    } else {
        doMemoryAnalysis(stringTree);
    }
}


void handleSave() {
    if (activeType == DataType::NONE || ((activeType == DataType::INTEGER && intTree.size() == 0) || (activeType == DataType::STRING && stringTree.size() == 0))) {
        std::cout << "Tree is empty or no data type selected. Nothing to save.\n"; return;
    }
    std::ofstream db_file(DB_FILENAME);
    if (!db_file) { std::cout << "Error opening " << DB_FILENAME << " for writing.\n"; return; }
    size_t items_saved = 0;
    if (activeType == DataType::INTEGER) {
        db_file << "integer\n";
        auto* node = intTree.getFirstLeaf();
        while (node) { for (const auto& key : node->keys) { db_file << key << "\n"; items_saved++; } node = node->next_leaf; }
    } else {
        db_file << "string\n";
        auto* node = stringTree.getFirstLeaf();
        while (node) { for (const auto& key : node->keys) { db_file << key << "\n"; items_saved++; } node = node->next_leaf; }
    }
    std::cout << "Successfully saved " << items_saved << " items to " << DB_FILENAME << ".\n";
}

void handleLoad() {
    std::ifstream db_file(DB_FILENAME);
    if (!db_file) { std::cout << DB_FILENAME << " not found.\n"; return; }
    std::string type_header; std::getline(db_file, type_header);
    DataType loaded_type = (type_header == "integer") ? DataType::INTEGER : (type_header == "string") ? DataType::STRING : DataType::NONE;
    if (loaded_type == DataType::NONE) { std::cout << "Error: Invalid database file format.\n"; return; }
    std::cout << "Found db for '" << type_header << "'. Loading will overwrite current tree. Continue? (y/n): ";
    char confirm; std::cin >> confirm;
    if (confirm != 'y' && confirm != 'Y') { std::cout << "Load cancelled.\n"; return; }

    activeType = loaded_type;
    if (activeType == DataType::INTEGER) intTree = BPlusTree<int>(); else stringTree = BPlusTree<std::string>();
    
    // Use the same logic as InsertFromFile to show performance
    db_file.seekg(0, std::ios::beg);
    std::getline(db_file, type_header); // Skip header line again
    
    size_t items_loaded = 0;
    if (activeType == DataType::INTEGER) {
        int value; while (db_file >> value) { size_t op_visits; intTree.insert(value, op_visits); items_loaded++; }
    } else {
        std::string line; while (std::getline(db_file, line)) { if (!line.empty()) { size_t op_visits; stringTree.insert(line, op_visits); items_loaded++; } }
    }
    std::cout << "\nSuccessfully loaded " << items_loaded << " items from " << DB_FILENAME << ".\n";
}

// --- Performance Reporting & Analysis ---

void printPerformance(const std::string& operation, size_t N, size_t node_visits, std::chrono::duration<double, std::milli> duration) {
    std::cout << "\n--- Performance Metrics ---\n";
    std::cout << std::left << std::setw(20) << "Operation:" << operation << "\n";
    std::cout << std::left << std::setw(20) << "Items in Tree (N):" << N << "\n";
    std::cout << std::left << std::setw(20) << "Time Taken:" << std::fixed << std::setprecision(4) << duration.count() << " ms\n";
    std::cout << std::left << std::setw(20) << "Node Visits:" << node_visits << "\n";
    if (N > 1) std::cout << std::left << std::setw(20) << "log" << ORDER << "(N) (est. height):" << std::fixed << std::setprecision(2) << log(N) / log(ORDER) << "\n";
}

void printComplexityAnalysis(const std::string& operation, size_t N) {
    std::cout << "\n--- Complexity Analysis (B+ Tree, Order=" << ORDER << ") ---\n";
    std::cout << "Theoretical Complexity: O(log_b(N)), where b is the tree order and N is the number of keys.\n\n";
    if (operation.find("Delete") != std::string::npos) {
        std::cout << "Best Case: The key is removed from a leaf that remains at least half-full. No rebalancing needed. O(log_b(N)).\n";
        std::cout << "Average Case: Removing a key causes a leaf to underflow, requiring a borrow or merge with a sibling. O(log_b(N)).\n";
        std::cout << "Worst Case: A merge operation propagates up to the root, reducing the tree's height. Still O(log_b(N)).\n";
    }
}