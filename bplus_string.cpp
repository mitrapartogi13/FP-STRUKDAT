#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <iomanip>

using namespace std;
using namespace chrono;

const int ORDER = 4;

struct BPlusNode {
    bool isLeaf;
    vector<string> keys;
    vector<BPlusNode*> children;
    BPlusNode* parent;
    BPlusNode* next;

    BPlusNode(bool leaf) : isLeaf(leaf), parent(nullptr), next(nullptr) {}
};

BPlusNode* root = nullptr;

void insertInternal(string, BPlusNode*, BPlusNode*);
void removeInternal(string, BPlusNode*, BPlusNode*);

void insert(const string& key) {
    if (!root) {
        root = new BPlusNode(true);
        root->keys.push_back(key);
        return;
    }

    BPlusNode* cursor = root;
    while (!cursor->isLeaf) {
        bool found = false;
        for (int i = 0; i < cursor->keys.size(); i++) {
            if (key < cursor->keys[i]) {
                cursor = cursor->children[i];
                found = true;
                break;
            }
        }
        if (!found) cursor = cursor->children.back();
    }

    auto it = lower_bound(cursor->keys.begin(), cursor->keys.end(), key);
    if (it != cursor->keys.end() && *it == key) return;
    cursor->keys.insert(it, key);

    if (cursor->keys.size() >= ORDER) {
        BPlusNode* newLeaf = new BPlusNode(true);
        int mid = (ORDER + 1) / 2;
        newLeaf->keys.assign(cursor->keys.begin() + mid, cursor->keys.end());
        cursor->keys.resize(mid);
        newLeaf->next = cursor->next;
        cursor->next = newLeaf;

        if (cursor == root) {
            root = new BPlusNode(false);
            root->keys.push_back(newLeaf->keys[0]);
            root->children.push_back(cursor);
            root->children.push_back(newLeaf);
            cursor->parent = root;
            newLeaf->parent = root;
        } else {
            newLeaf->parent = cursor->parent;
            insertInternal(newLeaf->keys[0], cursor->parent, newLeaf);
        }
    }
}

void insertInternal(string key, BPlusNode* cursor, BPlusNode* child) {
    auto it = upper_bound(cursor->keys.begin(), cursor->keys.end(), key);
    int index = it - cursor->keys.begin();
    cursor->keys.insert(it, key);
    cursor->children.insert(cursor->children.begin() + index + 1, child);

    if (cursor->keys.size() >= ORDER) {
        BPlusNode* newInternal = new BPlusNode(false);
        int mid = ORDER / 2;

        newInternal->keys.assign(cursor->keys.begin() + mid + 1, cursor->keys.end());
        newInternal->children.assign(cursor->children.begin() + mid + 1, cursor->children.end());
        string upKey = cursor->keys[mid];

        cursor->keys.resize(mid);
        cursor->children.resize(mid + 1);

        for (auto c : newInternal->children) c->parent = newInternal;

        if (cursor == root) {
            root = new BPlusNode(false);
            root->keys.push_back(upKey);
            root->children.push_back(cursor);
            root->children.push_back(newInternal);
            cursor->parent = root;
            newInternal->parent = root;
        } else {
            newInternal->parent = cursor->parent;
            insertInternal(upKey, cursor->parent, newInternal);
        }
    }
}

bool search(BPlusNode* cursor, string key, int* count) {
    while (cursor && !cursor->isLeaf) {
        bool found = false;
        for (int i = 0; i < cursor->keys.size(); i++) {
            if (key < cursor->keys[i]) {
                cursor = cursor->children[i];
                found = true;
                break;
            }
        }
        if (!found) cursor = cursor->children.back();
    }
    if (cursor) {
        for (string& k : cursor->keys) {
            (*count)++;
            if (k == key) return true;
        }
    }
    return false;
}

void remove(string key);

void removeInternal(string key, BPlusNode* cursor, BPlusNode* child) {
    auto it = find(cursor->keys.begin(), cursor->keys.end(), key);
    int idx = it - cursor->keys.begin();
    cursor->keys.erase(it);
    cursor->children.erase(cursor->children.begin() + idx + 1);

    if (cursor == root && cursor->keys.empty()) {
        root = cursor->children[0];
        delete cursor;
        root->parent = nullptr;
        return;
    }

    if (cursor->keys.size() >= (ORDER - 1) / 2) return;

    BPlusNode* parent = cursor->parent;
    int index = -1;
    for (int i = 0; i < parent->children.size(); i++) {
        if (parent->children[i] == cursor) {
            index = i;
            break;
        }
    }

    BPlusNode* left = (index > 0) ? parent->children[index - 1] : nullptr;
    BPlusNode* right = (index < parent->children.size() - 1) ? parent->children[index + 1] : nullptr;

    if (left && left->keys.size() > (ORDER - 1) / 2) {
        cursor->keys.insert(cursor->keys.begin(), parent->keys[index - 1]);
        parent->keys[index - 1] = left->keys.back();
        cursor->children.insert(cursor->children.begin(), left->children.back());
        left->children.pop_back();
        left->keys.pop_back();
    } else if (right && right->keys.size() > (ORDER - 1) / 2) {
        cursor->keys.push_back(parent->keys[index]);
        parent->keys[index] = right->keys.front();
        cursor->children.push_back(right->children.front());
        right->children.erase(right->children.begin());
        right->keys.erase(right->keys.begin());
    } else if (left) {
        left->keys.push_back(parent->keys[index - 1]);
        left->keys.insert(left->keys.end(), cursor->keys.begin(), cursor->keys.end());
        left->children.insert(left->children.end(), cursor->children.begin(), cursor->children.end());
        removeInternal(parent->keys[index - 1], parent, cursor);
        delete cursor;
    } else if (right) {
        cursor->keys.push_back(parent->keys[index]);
        cursor->keys.insert(cursor->keys.end(), right->keys.begin(), right->keys.end());
        cursor->children.insert(cursor->children.end(), right->children.begin(), right->children.end());
        removeInternal(parent->keys[index], parent, right);
        delete right;
    }
}

void remove(string key) {
    BPlusNode* cursor = root;
    while (!cursor->isLeaf) {
        bool found = false;
        for (int i = 0; i < cursor->keys.size(); i++) {
            if (key < cursor->keys[i]) {
                cursor = cursor->children[i];
                found = true;
                break;
            }
        }
        if (!found) cursor = cursor->children.back();
    }
    auto it = find(cursor->keys.begin(), cursor->keys.end(), key);
    if (it != cursor->keys.end()) {
        cursor->keys.erase(it);
        cout << "Hapus " << key << " berhasil.\n";
        if (cursor == root || cursor->keys.size() >= (ORDER - 1) / 2) return;
        removeInternal("", cursor->parent, cursor);
    } else {
        cout << key << " tidak ditemukan.\n";
    }
}

void rangeQuery(string start, string end) {
    BPlusNode* cursor = root;
    while (cursor && !cursor->isLeaf) cursor = cursor->children[0];
    while (cursor) {
        for (string& k : cursor->keys) {
            if (k >= start && k <= end) cout << k << " ";
        }
        cursor = cursor->next;
    }
    cout << endl;
}

void display(BPlusNode* cursor) {
    while (cursor && !cursor->isLeaf) cursor = cursor->children[0];
    while (cursor) {
        for (string& key : cursor->keys) cout << key << " -> ";
        cout << "NULL\n";
        cursor = cursor->next;
    }
}

void update(string oldKey, string newKey) {
    int dummy = 0;
    if (search(root, oldKey, &dummy)) {
        if (search(root, newKey, &dummy)) {
            cout << "Nilai sudah ada.\n";
            return;
        }
        remove(oldKey);
        insert(newKey);
        cout << oldKey << " di update menjadi " << newKey << endl;
    } else {
        cout << oldKey << " tidak ditemukan.\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename.txt>\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error membuka file " << argv[1] << endl;
        return 1;
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty()) insert(line);
    }
    file.close();

    update("IZUL", "Izul");
    update("Izul", "Budi");
    update("konz", "Budi");
    remove("Budi");
    remove("azril");

    cout << "B+ Tree:\n";
    display(root);

    cout << "\nRange query (A - M): ";
    rangeQuery("A", "M");

    int count = 0;
    string worst = "Zyaire";
    auto start = high_resolution_clock::now();
    bool found = search(root, worst, &count);
    auto end = high_resolution_clock::now();
    cout << "\nWorst Case: " << worst << "\n";
    cout << (found ? "Ditemukan" : "Tidak ditemukan") << " pada iterasi ke " << count << endl;
    cout << "Waktu: " << duration_cast<microseconds>(end - start).count() << " us\n";

    count = 0;
    string best = "Aarya";
    start = high_resolution_clock::now();
    found = search(root, best, &count);
    end = high_resolution_clock::now();
    cout << "\nBest Case: " << best << "\n";
    cout << (found ? "Ditemukan" : "Tidak ditemukan") << " pada iterasi ke " << count << endl;
    cout << "Waktu: " << duration_cast<microseconds>(end - start).count() << " us\n";

    return 0;
}
