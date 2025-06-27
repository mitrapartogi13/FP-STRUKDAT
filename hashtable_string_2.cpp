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
    sum += key[0];
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

bool search(const string& key, int& iterations) {
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

bool remove(const string& key, int& iterations) {
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

bool update(const string& oldKey, const string& newKey, int& iterations) {
    iterations = 0;
    int search_iters = 0;

    if (search(newKey, search_iters)) {
        cout << "Gagal update: Nama baru \"" << newKey << "\" sudah ada.\n";
        return false;
    }

    if (remove(oldKey, iterations)) {
        insert(newKey);
        return true;
    } else {
        return false;
    }
}

int main() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = NULL;
    }

    FILE* file_pointer = fopen("FP-STRUKDAT/data/string500hash.txt", "r");
    if (file_pointer == NULL) {
        perror("Error: Tidak dapat membuka file nilai.txt");
        return 1; 
    }
    
   
    char temp[256];
    while (fscanf(file_pointer, "%s", temp) == 1) {
        insert(string(temp));  
    }

    fclose(file_pointer);

    cout << "Hash table awal:\n";
    display();
    cout << "\n------------------------------------------\n";
    
    int iterations = 0;

    // Worst case searching (mencari elemen terakhir di chain terpanjang)
    string worst_case_key = "Dedi";
    cout << "Worst Case: Mencari nama \"" << worst_case_key << "\"\n";
    auto startWorst = high_resolution_clock::now();
    bool foundWorst = search(worst_case_key, iterations);
    auto endWorst = high_resolution_clock::now();
    auto durationWorst = duration_cast<microseconds>(endWorst - startWorst);
    if (foundWorst) cout << "Status: Ditemukan pada iterasi ke " << iterations << endl;
    else cout << "Status: Tidak ditemukan setelah " << iterations << " iterasi" << endl;
    cout << "Waktu yang dibutuhkan: " << durationWorst.count() << " microseconds\n\n";

    // Best case searching (mencari elemen di chain tunggal)
    string best_case_key = "Eka";
    iterations = 0; 
    cout << "Best Case: Mencari nama \"" << best_case_key << "\"\n";
    auto startBest = high_resolution_clock::now();
    bool foundBest = search(best_case_key, iterations);
    auto endBest = high_resolution_clock::now();
    auto durationBest = duration_cast<microseconds>(endBest - startBest);
    if (foundBest) cout << "Status: Ditemukan pada iterasi ke " << iterations << endl;
    else cout << "Status: Tidak ditemukan setelah " << iterations << " iterasi" << endl;
    cout << "Waktu yang dibutuhkan: " << durationBest.count() << " microseconds\n\n";

    // Update (misal: "Bagas" menjadi "Gagas")
    string oldKey = "Bagas", newKey = "Gagas";
    iterations = 0;
    cout << "Update nama \"" << oldKey << "\" menjadi \"" << newKey << "\"\n";
    auto startUp = high_resolution_clock::now();
    bool updated = update(oldKey, newKey, iterations);
    auto endUp = high_resolution_clock::now();
    auto durationUp = duration_cast<microseconds>(endUp - startUp);
    if (updated) cout << "Status: Berhasil update, memerlukan " << iterations << " iterasi untuk menemukan dan menghapus nama lama." << endl;
    else cout << "Status: Gagal update, \"" << oldKey << "\" tidak ditemukan setelah " << iterations << " iterasi." << endl;
    cout << "Waktu yang dibutuhkan: " << durationUp.count() << " microseconds\n\n";

    // Delete
    string val_to_delete = "Budi";
    iterations = 0;
    cout << "Delete nama \"" << val_to_delete << "\"\n";
    auto startDel = high_resolution_clock::now();
    bool deleted = remove(val_to_delete, iterations);
    auto endDel = high_resolution_clock::now();
    auto durationDel = duration_cast<microseconds>(endDel - startDel);
    if (deleted) cout << "Status: Berhasil delete, memerlukan " << iterations << " iterasi." << endl;
    else cout << "Status: Gagal delete, \"" << val_to_delete << "\" tidak ditemukan setelah " << iterations << " iterasi." << endl;
    cout << "Waktu yang dibutuhkan: " << durationDel.count() << " microseconds\n";
    
    cout << "------------------------------------------\n";
    
    return 0;
}