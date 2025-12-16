#include <stdlib.h>
#include <stdint.h>

#include "../../platform.h"

#include "../database.h"

#if DATABASE_SAVED_ENTRIES_PER_STACK > 256
    #error Configured too many entries per stack
#endif

#define database_capacity AVAILABLE_VOLATILE_MEMORY/4

uint32_t unused_database_capacity;

uint32_t next_free_stack_id;

typedef struct entry {
    uint8_t* data;
    struct entry* next;
} ENTRY;

typedef struct stack {
    STACK_ID id;
    uint8_t element_size;
    uint8_t number_of_entries;
    ENTRY* entries;
    struct stack* next;
} STACK;

struct {
    STACK* stacks;
} database;

/** Helper functions **/

STACK* get_stack_by_id (STACK_ID stack_id) {
    STACK* selected_stack = database.stacks;

    while (selected_stack != NULL) {
        if (selected_stack->id == stack_id) {
            return selected_stack;
        }
        selected_stack = selected_stack->next;
    }

    return INVALID_STACK_ID;
}

/** Library functions **/

void database_init(void) {
    database.stacks = NULL;

    unused_database_capacity = database_capacity;

    next_free_stack_id = 1;
}

DATABASE_ERROR database_set_stack(uint8_t element_size, STACK_ID *stack_id) {
    if (unused_database_capacity - element_size * DATABASE_SAVED_ENTRIES_PER_STACK < 0) {
        stack_id = 0;

        return DATABASE_FULL;
    }
    
    STACK* new_stack = malloc(sizeof(STACK));

    new_stack->element_size = element_size;
    new_stack->entries = NULL;
    new_stack->number_of_entries = 0;
    new_stack->next = database.stacks;

    database.stacks = new_stack;

    new_stack->id = next_free_stack_id;
    *stack_id = next_free_stack_id;

    next_free_stack_id++;
    unused_database_capacity -= element_size * DATABASE_SAVED_ENTRIES_PER_STACK;

    return DATABASE_OK;
}

DATABASE_ERROR database_remove_stack(STACK_ID stack_id) {
    STACK* selected_stack = database.stacks;
    STACK* previous_stack = NULL;

    while (selected_stack != NULL) {
        if (selected_stack->id == stack_id) {
            goto VALID_ID;
        }

        previous_stack = selected_stack;
        selected_stack = selected_stack->next;
    }

    return DATABASE_INVALID_STACK_ID;

    VALID_ID:

    if (previous_stack == NULL) {
        database.stacks = selected_stack->next;
    }
    else {
        previous_stack->next = selected_stack->next;
    }

    free(selected_stack);

    return DATABASE_OK;
}

DATABASE_ERROR database_write(STACK_ID stack_id, uint8_t* entry) {

    STACK* write_stack = get_stack_by_id(stack_id);
    
    if (write_stack == INVALID_STACK_ID) {
        return DATABASE_INVALID_STACK_ID;
    };

    if (write_stack->number_of_entries >= DATABASE_SAVED_ENTRIES_PER_STACK) { //Too much saved entries

        ENTRY* selected_entry = write_stack->entries;
        ENTRY* previous_entry = NULL;

        while (selected_entry->next != NULL) {
            previous_entry = selected_entry;
            selected_entry = selected_entry->next;
        }

        previous_entry->next = NULL;

        free(selected_entry);
    }

    ENTRY* new_entry = (ENTRY*) malloc(sizeof(ENTRY));

    new_entry->data = (uint8_t*) malloc(sizeof(uint8_t) * write_stack->element_size);

    for (uint8_t i = 0; i < write_stack->element_size; i++) new_entry->data[i] = entry[i];
    
    new_entry->next = write_stack->entries;

    write_stack->entries = new_entry;

    if (write_stack->number_of_entries < 10) write_stack->number_of_entries++;

    return DATABASE_OK;
}

DATABASE_ERROR database_read_latest(STACK_ID stack_id, uint8_t* out) {

    STACK* selected_stack = get_stack_by_id(stack_id);

    if (selected_stack == INVALID_STACK_ID) return DATABASE_INVALID_STACK_ID;

    for (uint8_t i = 0; i < selected_stack->element_size; i++) out[i] = selected_stack->entries->data[i];

    return DATABASE_OK;
}

DATABASE_ERROR database_read_all(STACK_ID stack_id, uint8_t** out, uint8_t *number_of_entries) {

    STACK* selected_stack = get_stack_by_id(stack_id);

    if (selected_stack == INVALID_STACK_ID) return DATABASE_INVALID_STACK_ID;

    number_of_entries[0] = 0;

    if (selected_stack->number_of_entries == 0) return DATABASE_OK;

    ENTRY* selected_entry = selected_stack->entries;

    for (uint32_t i = 0; i < selected_stack->number_of_entries; i++) {
        for (uint32_t j = 0; j < selected_stack->element_size; j++) {
            out[i][j] = selected_entry->data[j];
        }

        selected_entry = selected_entry->next;

        number_of_entries[0]++;
    }

    return DATABASE_OK;
}

