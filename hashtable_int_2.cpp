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

bool search(int key, int& iterations) {
    int index = hashFunction(key);
    Node* curr = table[index];
    iterations = 0; 
    while (curr != NULL) {
        iterations++;
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

bool remove(int key, int& iterations) {
    int index = hashFunction(key);
    Node* curr = table[index];
    Node* prev = NULL;
    iterations = 0; 
    
    while (curr != NULL) {
        iterations++;
        if (curr->key == key) {
            if (prev == NULL) { 
                table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

bool update(int oldKey, int newKey, int& iterations) {
    iterations = 0;
    int search_iters = 0;

    if (search(newKey, search_iters)) {
        cout << "Gagal update: Nilai baru " << newKey << " sudah ada.\n";
        return false;
    }

    if (remove(oldKey, iterations)) {
        insert(newKey);
        iterations += search_iters;
        return true;
    } else {
        cout << "Gagal update: Nilai lama " << oldKey << " tidak ada.\n";
        return false;
    }
}

int main() {
    for (int i = 0; i < 26; i++) {
        table[i] = NULL;
    }

    FILE* file_pointer = fopen("data/int500hash.txt", "r");
    if (file_pointer == NULL) {
        perror("Error: Tidak dapat membuka file txt");
        return 1; 
    }

    int key;
    while (fscanf(file_pointer, "%d", &key) == 1) {
        insert(key);
    }
    fclose(file_pointer);

    cout << "Hash table:\n";
    display();
    cout << "\n------------------------------------------\n";
    
    int iterations = 0;

    // Worst case searching (mencari elemen terakhir)
    int worst_case_key = 1;
    cout << "Worst Case: Mencari key " << worst_case_key << "\n";
    auto startWorst = high_resolution_clock::now();
    bool foundWorst = search(worst_case_key, iterations);
    auto endWorst = high_resolution_clock::now();
    auto durationWorst = duration_cast<microseconds>(endWorst - startWorst);
    if (foundWorst) cout << "Status: Ditemukan pada iterasi ke " << iterations << endl;
    else cout << "Status: Tidak ditemukan setelah " << iterations << " iterasi" << endl;
    cout << "Waktu yang dibutuhkan: " << durationWorst.count() << " microseconds\n\n";

    // Best case searching (mencari elemen pertama)
    int best_case_key = 499;
    iterations = 0;
    cout << "Best Case: Mencari key " << best_case_key << "\n";
    auto startBest = high_resolution_clock::now();
    bool foundBest = search(best_case_key, iterations);
    auto endBest = high_resolution_clock::now();
    auto durationBest = duration_cast<microseconds>(endBest - startBest);
    if (foundBest) cout << "Status: Ditemukan pada iterasi ke " << iterations << endl;
    else cout << "Status: Tidak ditemukan setelah " << iterations << " iterasi" << endl;
    cout << "Waktu yang dibutuhkan: " << durationBest.count() << " microseconds\n\n";

    // Update
    int oldkey = 488, newkey = 601;
    iterations = 0;
    cout << "Update key value " << oldkey << " menjadi " << newkey << endl;
    auto startUp = high_resolution_clock::now();
    bool updated = update(oldkey, newkey, iterations);
    auto endUp = high_resolution_clock::now();
    auto durationUp = duration_cast<microseconds>(endUp - startUp);
    if (updated) cout << "Status: Berhasil update, memerlukan " << iterations << " iterasi untuk menemukan dan menghapus nilai lama." << endl;
    else cout << "Status: Gagal update, " << oldkey << " tidak ditemukan setelah " << iterations << " iterasi." << endl;
    cout << "Waktu yang dibutuhkan: " << durationUp.count() << " microseconds\n\n";

    // Delete
    int val = 77;
    iterations = 0;
    cout << "Delete key value " << val << endl;
    auto startDel = high_resolution_clock::now();
    bool deleted = remove(val, iterations);
    auto endDel = high_resolution_clock::now();
    auto durationDel = duration_cast<microseconds>(endDel - startDel);
    if (deleted) cout << "Status: Berhasil delete, memerlukan " << iterations << " iterasi." << endl;
    else cout << "Status: Gagal delete, " << val << " tidak ditemukan setelah " << iterations << " iterasi." << endl;
    cout << "Waktu yang dibutuhkan: " << durationDel.count() << " microseconds\n";

    cout << "------------------------------------------\n";
    
    return 0;
}