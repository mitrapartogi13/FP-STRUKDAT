#include <iostream>
#include <string> 
#include <vector>
#include <chrono>
#include <cstdio>
#include <cstring>

using namespace std;
using namespace std::chrono;

struct Node {
    string key;
    Node* next;
};

const int TABLE_SIZE = 26;
Node* table[TABLE_SIZE];

int hashFunction(const string& key) {
    int sum = 0;
    for (char ch : key) {
        sum += ch;
    }
    return sum % TABLE_SIZE;
}

void insert(const string& key) {
    int index = hashFunction(key);
    Node* curr = table[index];
    while (curr != NULL) {
        if (curr->key == key) return;
        curr = curr->next;
    }
    Node* newNode = new Node{key, table[index]};
    table[index] = newNode;
}

bool search(const string& key) {
    int index = hashFunction(key);
    Node* curr = table[index];
    while (curr != NULL) {
        if (curr->key == key) return true;
        curr = curr->next;
    }
    return false;
}

void display() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        cout << i << ": ";
        Node* curr = table[i];
        while (curr != NULL) {
            cout << curr->key << " -> ";
            curr = curr->next;
        }
        cout << "NULL\n";
    }
}

int main() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = NULL;
    }

    FILE* file_pointer = fopen("nilai.txt", "r");
    if (file_pointer == NULL) {
        perror("Error: Tidak dapat membuka file nilai.txt");
        return 1; 
    }
    
    cout << "Membaca nama dari nilai.txt dan memasukkan ke hash table...\n";
    
   
    char temp[256];
    while (fscanf(file_pointer, "%s", temp) == 1) {
        insert(string(temp));  
    }

    fclose(file_pointer);
    cout << "Selesai memasukkan data.\n\n";

    cout << "Hash table setelah diisi dari file:\n";
    display();
    cout << "\n------------------------------------------\n";
    
    string worst_case_key = "Dedi";
    cout << "Worst Case: Mencari nama \"" << worst_case_key << "\" (elemen di akhir chain)\n";
    auto startWorst = high_resolution_clock::now();
    bool foundWorst = search(worst_case_key);
    auto endWorst = high_resolution_clock::now();
    auto durationWorst = duration_cast<microseconds>(endWorst - startWorst);
    cout << "Status: " << (foundWorst ? "Ditemukan\n" : "Tidak ada\n");
    cout << "Waktu yang dibutuhkan (Worst Case): " << durationWorst.count() << " microseconds\n\n";

    string best_case_key = "Budi";
    cout << "Best Case: Mencari nama \"" << best_case_key << "\" (elemen tunggal di chain)\n";
    auto startBest = high_resolution_clock::now();
    bool foundBest = search(best_case_key);
    auto endBest = high_resolution_clock::now();
    auto durationBest = duration_cast<microseconds>(endBest - startBest);
    cout << "Status: " << (foundBest ? "Ditemukan\n" : "Tidak ada\n");
    cout << "Waktu yang dibutuhkan (Best Case): " << durationBest.count() << " microseconds\n";
    
    cout << "\n------------------------------------------\n";
    
    return 0;
}