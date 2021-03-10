//
//  MemoryManager.cpp
//  MemManange
//
//  Created by Nathan Cherepski on 12/3/20.
//
#include <iostream>
#include "MemoryManager.hpp"

using namespace std;
MemoryManager::MemoryManager(char* p_mem, int s)
{
    cout << "Inside MemoryManager constructor, the sizeof(Node) is: 24 sizeof(Node*) is: 8 size of a bool is: 1" << endl << endl;
    
    p_memory = p_mem;
    size = s;
    
/* sets up head node & associated data members of the struct */
    p_head = (Node*)p_memory;
    p_head->sizeOfNode = size - (2 * sizeof(Node));
    sizeTracker = p_head->sizeOfNode;
    p_head->isFree = true;
    p_head->p_prev = 0;
    p_head->p_next = (Node*)(p_memory + size - sizeof(Node));
    
/* sets up tail node & associated data members of the struct */
    /* represents end block of memoryManager */
    p_tail = (Node*)(p_memory + size - sizeof(Node));
    p_tail->sizeOfNode = 0;
    p_tail->isFree = false;
    p_tail->p_prev = p_head;
    p_tail->p_next = 0;
}

void* MemoryManager::allocate(int size)
{
    /* finds largest free space in memoryManager (p_largest is passed by reference, so is updated */
    Node* p_largest;
    findLargestFreeBlock(p_largest, size);
    
    /* if no block large enough for allocation */
    if(p_largest == 0)
    {
        return NULL;
    }
    else
    {
        Node* p_newNode = (Node*)(((char*)p_largest) + sizeof(Node) + size);
    /* updates original block by making it smaller and assigning to new node */
        p_newNode->p_prev = p_largest;
        p_newNode->p_next = p_largest->p_next;
        p_newNode->isFree = true; //MM- this is a free node
        p_newNode->sizeOfNode = p_largest->sizeOfNode - (size + sizeof(Node)); //MM- size left after allocating

    /* this takes on data and size of desired input, and is set as used */
        p_largest->p_next->p_prev = p_newNode;
        p_largest->p_next = p_newNode;
        p_largest->sizeOfNode = size; //MM
        p_largest->isFree = false;//MM
        sizeTracker = sizeTracker - (sizeof(Node) + size);
    /* returns the starting point (byte) of the node */
        return (void*)(((char*)p_largest) + sizeof(Node));
    }
}

void MemoryManager::deallocate(void* p_delete)
{
    Node* p_remove = (Node*)((char*)p_delete - sizeof(Node));

    /* checks that desired block to be removed is currently used */
    if(canBeDeleted(p_remove))
    {
        /* looks for block to be deallocated */
        Node* p_search = p_head;
        while(p_search != p_tail)
        {
            /* checks if current index of memoryManager matches block to be deallocated */
            if(p_search == p_remove)
            {
                /* updates block from used to free */
                p_search->isFree = true;
                /* updates total available free space */
                sizeTracker = sizeTracker + (sizeof(Node) + size);
                /* makes sure no 2 free blocks exist next to each other */
                coalesceRight(p_search);
                coalesceLeft(p_search);
            }
            // increment
            p_search = p_search->p_next;
        }
    }
    else
    {
        
    }
}

void MemoryManager::dump()
{
    /* p_search used to loop through memoryManager, and count is used for block labelling */
    Node* p_search = p_head;
    int count = 1;
    while(p_search != p_tail)
    {
        cout << "     Block " << count << ": " << p_search->sizeOfNode << " bytes of ";
        /* checks 'isFree' for current node */
        if(p_search->isFree == 1)
        {
            cout << "free";
        }
        else
        {
            cout << "used";
        }
        cout << " space." << endl;
        // increment both the block label (count) and p_search to next block in memoryManager
        count++;
        p_search = p_search->p_next;
    }
    /* place space after dump of blocks */
    cout << endl;
    return;
}


void MemoryManager::findLargestFreeBlock(Node*& p_largest, int minimumSize)
{
    /* check helper variables */
    bool tooBig = false;
    Node* p_search = p_head;
    int largestSoFar = -1;
    
    /* checks if total free space in memoryManager would fit desired allocation (very broad check) */
    if(minimumSize < sizeTracker)
    {
        /* loops thru memoryManager */
        while(p_search != p_tail)
        {
            /* checks if a free block and if it is the largest free block */
            if(p_search->isFree == true && p_search->sizeOfNode > largestSoFar)
            {
                /* checks if both allocation size and size of node will fit in free block */
                if((minimumSize + sizeof(Node)) <= p_search->sizeOfNode)
                {
                    /* holds pointer to largest free block so far, updates size of held largestSoFar */
                    p_largest = p_search;
                    largestSoFar = p_search->sizeOfNode;
                }
                else
                {
                    /* incase data fits, but node does not */
                    tooBig = true;
                }
            }
            // increment
            p_search = p_search->p_next;
        }
    }
    else
    {
        p_largest = 0;
    }
    if(tooBig == true)
    {
        p_largest = 0;
    }
    /* both p_largest = 0 cases are for a size that is not able to be allocated */
}

bool MemoryManager::canBeDeleted(void* p_delete)
{
    /* holds whether or not block is deallocatable */
    bool canCant = false;
    
    /* loop thru memoryManager Nodes */
    Node* p_search = p_head;
    while(p_search != p_tail)
    {
        /* if Node to be deleted is found and used */
        if(p_search == p_delete && p_search->isFree == false)
        {
            /* can be deleted, because was found and is used*/
            canCant = true;
        }
        // increment
        p_search = p_search->p_next;
    }
    return canCant;
}

void MemoryManager::coalesceRight(Node *p_nodeToBeDeleted)
{
    /* if next block exists and is free, then coalesce right */
    if(p_nodeToBeDeleted->p_next != 0 && p_nodeToBeDeleted->p_next->isFree == true)
    {
        // update isFree
        p_nodeToBeDeleted->isFree = true;
        p_nodeToBeDeleted->p_next->isFree = true;
        
        // update sizeOfNode
        p_nodeToBeDeleted->sizeOfNode = p_nodeToBeDeleted->sizeOfNode + p_nodeToBeDeleted->p_next->sizeOfNode + sizeof(Node);
            
        // update pointers
        p_nodeToBeDeleted->p_next = p_nodeToBeDeleted->p_next->p_next;
        p_nodeToBeDeleted->p_next->p_prev = p_nodeToBeDeleted;
    }
}

void MemoryManager::coalesceLeft(Node* p_nodeToBeDeleted)
{
    /* if prev block exists and is free, then coalesce left */
    if(p_nodeToBeDeleted->p_prev != 0 && p_nodeToBeDeleted->p_prev->isFree == true)
    {
        // update isFree
        p_nodeToBeDeleted->isFree = true;
        p_nodeToBeDeleted->p_prev->isFree = true;
        
        // update sizeOfNode
        p_nodeToBeDeleted->p_prev->sizeOfNode = p_nodeToBeDeleted->sizeOfNode + p_nodeToBeDeleted->p_prev->sizeOfNode + sizeof(Node);
        p_nodeToBeDeleted->sizeOfNode = 0;
        
        // update pointers
        p_nodeToBeDeleted->p_prev->p_next = p_nodeToBeDeleted->p_next;
        p_nodeToBeDeleted->p_next->p_prev = p_nodeToBeDeleted->p_prev;
    }
}
