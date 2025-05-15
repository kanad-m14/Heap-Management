#include <stdio.h>
#include <stdlib.h>

// Structure to represent a block of memory (either free or allocated)
typedef struct Block 
{
    size_t size;      // Size of the block
    int free;       // 1 if free, 0 if allocated
    struct Block *next; // Pointer to the next block in the list

} Block;

// Global pointer to the head of the free list
Block *head = NULL;

// Function to allocate memory
void *allocate(size_t size) 
{
    Block *current = head;

    // Traverse the free list to find a suitable block
    while (current != NULL) 
    {
        if(current->free && (int)(current->size - size) >= (int)sizeof(Block)) // Found a free block large enough
        { 
            if ((current->size - size) == sizeof(Block)) // Exact fit
            { 
                current->free = 0;
                return (void*)current;
            } 
            else if ((current->size - size) > sizeof(Block)) // Enough space for split
            { 
                Block *newBlock = (Block *)((char *)current + size);
                newBlock->size = current->size - size - sizeof(Block);
                newBlock->free = 1;
                newBlock->next = current->next;
                current->size = size;
                current->free = 0;
                current->next = newBlock;
                return (void*)current;
            }
        }

        current = current->next;
    }

    printf("Cannot allocate memory! Not enough space\n");
    return NULL; // No suitable block found
}

// Function to free memory
void free_block(void *ptr) 
{
    if (ptr == NULL) return; //Handle null pointer

    Block *block = (Block *)ptr;
    block->free = 1;

    // Merge with the previous block if it's free
    Block *current = head;
    Block *previous = NULL;

    while (current != NULL && current != block) 
    {
        previous = current;
        current = current->next;
    }

    if (previous != NULL && previous->free) 
    {
        previous->size += block->size + sizeof(Block);
        previous->next = block->next;
        block = previous;  // Update block to the merged block
        
    }

    // Merge with the next block if it's free
    if (block->next != NULL && block->next->free) 
    {
        block->size += block->next->size + sizeof(Block);
        block->next = block->next->next;
    }

}

void print_heap_status() // To print the current heap status 
{
    Block* current = head;
    printf("Heap Status:\n");

    while (current != NULL) 
    {
        printf("Address: %p, Size: %zu, Free: %d\n", current, current->size, current->free);
        current = current->next;
    }
}

// Function to initialize the heap
void initialize_heap(size_t heap_size) 
{
    head = (Block *)malloc(heap_size);

    if (head == NULL) 
    {
        perror("malloc failed");
        exit(1);
    }

    head->size = heap_size - sizeof(Block);
    head->free = 1;
    head->next = NULL;
}

int main() 
{
    size_t heap_size = 1024; // Heap size
    initialize_heap(heap_size);

    void *ptr1 = allocate(sizeof(int) * 4);
    print_heap_status();
    void *ptr2 = allocate(200);
    print_heap_status();
    void *ptr3 = allocate(50);
    print_heap_status();

    free_block(ptr2);
    print_heap_status();
    void *ptr4 = allocate(150);
    print_heap_status();

    free_block(ptr3);
    print_heap_status();

    void *ptr5 = allocate(100);
    print_heap_status();

    free_block(ptr1);
    free_block(ptr4);
    print_heap_status();

    free_block(ptr5);
    void *ptr6 = allocate(2000);
    void *ptr7 = allocate(1012);
    print_heap_status();

    return 0;
}