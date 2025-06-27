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

bool search(const string& key, int *count) {
    int index = hashFunction(key);
    Node* curr = table[index];
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

void remove(string key) {
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

void update(string oldKey, string newKey) {
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
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = NULL;
    }

    FILE* file_pointer = fopen("nilai.txt", "r");
    if (file_pointer == NULL) {
        perror("Error: Tidak dapat membuka file nilai.txt");
        return 1; 
    }
    
   
    char temp[256];
    while (fscanf(file_pointer, "%s", temp) == 1) {
        insert(string(temp));  
    }

    fclose(file_pointer);

    update("IZUL", "Izul");
    update("Izul", "Budi");
    update("konz", "Budi");
    remove("Budi");
    remove("azril");
    cout << "Hash table:\n";
    display();
    cout << "\n------------------------------------------\n";
    
    int count = 0;
    string worst_case_key = "Dedi";
    cout << "Worst Case: Mencari nama \"" << worst_case_key << "\"\n";
    auto startWorst = high_resolution_clock::now();
    bool foundWorst = search(worst_case_key, &count);
    auto endWorst = high_resolution_clock::now();
    auto durationWorst = duration_cast<microseconds>(endWorst - startWorst);

    if (foundWorst) cout<<"Status: Ditemukan pada iterasi ke "<<count<<endl;
    else cout<<"Tidak ditemukan"<<endl;
    cout << "Waktu yang dibutuhkan: " << durationWorst.count() << " microseconds\n\n";

    count = 0;
    string best_case_key = "Budi";
    cout << "Best Case: Mencari nama \"" << best_case_key << "\" \n";
    auto startBest = high_resolution_clock::now();
    bool foundBest = search(best_case_key, &count);
    auto endBest = high_resolution_clock::now();
    auto durationBest = duration_cast<microseconds>(endBest - startBest);

    if (foundBest) cout<<"Status: Ditemukan pada iterasi ke "<<count<<endl;
    else cout<<"Tidak ditemukan"<<endl;
    cout << "Waktu yang dibutuhkan: " << durationBest.count() << " microseconds\n";
    
    cout << "------------------------------------------\n";
    
    return 0;
}