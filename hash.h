// filepath: /home/mitrap13/kuliah/StrukturData/StrukturData/FP-STRUKDAT/src/hash.cpp

#include "hash.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class HashTable {
private:
    vector<string> table;
    int size;

    int hashFunction(const string& key) {
        int hash = 0;
        for (char ch : key) {
            hash += ch;
        }
        return hash % size;
    }

public:
    HashTable(int s) : size(s) {
        table.resize(size);
    }

    bool insert(const string& key) {
        int index = hashFunction(key);
        if (table[index].empty()) {
            table[index] = key;
            return true;
        }
        return false; // Key already exists
    }

    bool remove(const string& key) {
        int index = hashFunction(key);
        if (table[index] == key) {
            table[index].clear();
            return true;
        }
        return false; // Key not found
    }

    bool search(const string& key) {
        int index = hashFunction(key);
        return table[index] == key;
    }

    void display() {
        for (const auto& entry : table) {
            if (!entry.empty()) {
                cout << entry << endl;
            }
        }
    }
};