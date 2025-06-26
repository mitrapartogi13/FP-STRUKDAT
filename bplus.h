// filepath: /home/mitrap13/kuliah/StrukturData/StrukturData/FP-STRUKDAT/src/bplustree.h

#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <string>

class BPlusTree {
public:
    BPlusTree(int order);
    void insert(const std::string& key);
    void remove(const std::string& key);
    bool search(const std::string& key) const;
    void display() const; // Optional: for debugging purposes

private:
    struct Node {
        bool isLeaf;
        std::vector<std::string> keys;
        std::vector<Node*> children;
        Node(int order, bool leaf);
    };

    Node* root;
    int order;

    void splitChild(Node* parent, int index, Node* child);
    void insertNonFull(Node* node, const std::string& key);
    void removeFromLeaf(Node* node, int index);
    void removeFromNonLeaf(Node* node, int index);
    std::string getPredecessor(Node* node, int index);
    std::string getSuccessor(Node* node, int index);
    void merge(Node* node, int index);
    bool searchInNode(Node* node, const std::string& key) const;
};

#endif // BPLUSTREE_H