#include <iostream>
#include <vector>
#include <chrono>
#include <cstdio>

using namespace std;
using namespace std::chrono;

struct Node {
    int key;
    Node* next;
};

const int TABLE_SIZE = 26;
Node* table[TABLE_SIZE];

int hashFunction(int key) {
    return key % TABLE_SIZE;
}

void insert(int key) {
    int index = hashFunction(key);
    Node* curr = table[index];
    while (curr != NULL) {
        if (curr->key == key) return;
        curr = curr->next;
    }
    Node* newNode = new Node{key, table[index]};
    table[index] = newNode;
}

bool search(int key) {
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
    for (int i = 0; i < 26; i++) {
        table[i] = NULL;
    }

    FILE* file_pointer;
    int key_from_file;

    file_pointer = fopen("nilai3.txt", "r");
    if (file_pointer == NULL) {
        perror("Error: Tidak dapat membuka file nilai.txt");
        return 1; 
    }
    
    cout << "Membaca data dari nilai.txt dan memasukkan ke hash table (C-style)...\n";
    while (fscanf(file_pointer, "%d", &key_from_file) == 1) {
        insert(key_from_file);
    }

    fclose(file_pointer);
    cout << "Selesai memasukkan data.\n\n";


    cout << "Hash table setelah diisi dari file:\n";
    display();
    cout << "\n------------------------------------------\n";
    
    int worst_case_key = 6;
    cout << "Worst Case: Mencari key " << worst_case_key << " (elemen di akhir chain)\n";
    auto startWorst = high_resolution_clock::now();
    bool foundWorst = search(worst_case_key);
    auto endWorst = high_resolution_clock::now();
    auto durationWorst = duration_cast<microseconds>(endWorst - startWorst);
    cout << "Status: " << (foundWorst ? "Ditemukan" : "Tidak ada") << "\n";
    cout << "Waktu yang dibutuhkan (Worst Case): " << durationWorst.count() << " microseconds\n\n";

    int best_case_key = 999;
    cout << "Best Case: Mencari key " << best_case_key << " (elemen tunggal di chain)\n";
    auto startBest = high_resolution_clock::now();
    bool foundBest = search(best_case_key);
    auto endBest = high_resolution_clock::now();
    auto durationBest = duration_cast<microseconds>(endBest - startBest);
    cout << "Status: " << (foundBest ? "Ditemukan" : "Tidak ada") << "\n";
    cout << "Waktu yang dibutuhkan (Best Case): " << durationBest.count() << " microseconds\n";
    
    cout << "\n------------------------------------------\n";
    
    return 0;
}