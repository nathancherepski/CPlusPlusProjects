//
//  MemoryManager.hpp
//  MemManange
//
//  Created by Nathan Cherepski on 12/3/20.
//

#pragma once

using namespace std;

class MemoryManager{
public:
    
    MemoryManager(char* p_mem, int s);
    void* allocate(int size);
    void deallocate(void* p_delete);
    void dump();
private:
    /* aspects of node */
    struct Node
    {
        Node* p_next;
        Node* p_prev;
        bool isFree;
        int sizeOfNode;
    };
    
    /* important variables for maintaining linked list */
    char* p_memory;
    int size;
    int sizeTracker;
    
    /* initial nodes */
    Node* p_head;
    Node* p_tail;
    
    /* helper functions */
    void findLargestFreeBlock(Node*& p_largest, int minimumSize);
    bool canBeDeleted(void* p_delete);
    void coalesceRight(Node* p_nodeToBeDeleted);
    void coalesceLeft(Node* p_nodeToBeDeleted);
};
