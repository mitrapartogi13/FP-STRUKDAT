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

bool search(int key, int *count) {
    int index = hashFunction(key);
    Node* curr = table[index];
    *count = 0;
    while (curr != NULL) {
        (*count)++;
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

void remove(int key) {
    int index = hashFunction(key);
    Node* curr = table[index];
    Node* prev = NULL;

    while (curr != NULL) {
        if (curr->key == key) {
            if (prev == NULL) { 
                table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            delete curr;
            cout<<"Hapus " << key << " berhasil." <<endl;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    cout<< key << " tidak ditemukan." <<endl;
}

void update(int oldKey, int newKey) {
    int temp = 0;
    if (search(oldKey, &temp)) {
        if(search(newKey, &temp)){
            cout << "Nilai sudah ada.\n";
            return;
        }
        remove(oldKey);
        insert(newKey);
        cout <<oldKey << " di update menjadi " << newKey <<endl;
    } else {
        cout << oldKey << " tidak ditemukan."<<endl;
    }
}

int main() {
    for (int i = 0; i < 26; i++) {
        table[i] = NULL;
    }

    FILE* file_pointer = fopen("nilai3.txt", "r");
    if (file_pointer == NULL) {
        perror("Error: Tidak dapat membuka file nilai2.txt");
        return 1; 
    }

    int key_from_file;
    while (fscanf(file_pointer, "%d", &key_from_file) == 1) {
        insert(key_from_file);
    }
    fclose(file_pointer);

    update(99,100);
    update(99,1001);
    cout << "Hash table:\n";
    display();
    cout << "\n------------------------------------------\n";

    int count;
    int worst_case_key = 6;
    cout << "Worst Case: Mencari key " << worst_case_key << "\n";
    auto startWorst = high_resolution_clock::now();
    bool foundWorst = search(worst_case_key, &count);
    auto endWorst = high_resolution_clock::now();
    auto durationWorst = duration_cast<microseconds>(endWorst - startWorst);

    if (foundWorst) cout<<"Status: Ditemukan pada iterasi ke "<<count<<endl;
    else cout<<"Status: Tidak ditemukan"<<endl;
    cout << "Waktu yang dibutuhkan: " << durationWorst.count() << " microseconds\n\n";

    int best_case_key = 499;
    cout << "Best Case: Mencari key " << best_case_key << "\n";
    auto startBest = high_resolution_clock::now();
    bool foundBest = search(best_case_key, &count);
    auto endBest = high_resolution_clock::now();
    auto durationBest = duration_cast<microseconds>(endBest - startBest);

    if (foundBest) cout<<"Status: Ditemukan pada iterasi ke "<<count<<endl;
    else cout<<"Status: Tidak ditemukan"<<endl;
    cout << "Waktu yang dibutuhkan: " << durationBest.count() << " microseconds\n";
    
    cout << "------------------------------------------\n";
    
    return 0;
}