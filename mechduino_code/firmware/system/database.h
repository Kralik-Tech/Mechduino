#ifndef DATABASE_H
#define DATABASE_H

typedef enum {
    DATABASE_OK = 0,            // No error
    DATABASE_INVALID_STACK_ID,  // Stack ID is invalid
    DATABASE_FULL,              // Not enough space in database
    DATABASE_NEED_INIT          // Function database_init needs ro be executed first
} DATABASE_ERROR;

typedef uint8_t STACK_ID;        // Cannot be equal to INVALID_STACK_ID
#define INVALID_STACK_ID 0

/*
    Performs basic inicialisation of database
*/

void database_init(void);


/* 
    Sets new data stack with element of size element_size (in bytes) in database and puts id of stack to stack_id (or INVALID_STACK_ID if error occured)
    
    Returns DATABASE_FULL or DATABASE_OK
*/

DATABASE_ERROR database_set_stack(uint8_t element_size, STACK_ID *stack_id);


/*
    Removes data stack specified by stack_id

    Returns DATABASE_INVALID_STACK_ID or DATABASE_OK
*/

DATABASE_ERROR database_remove_stack(STACK_ID stack_id);


/*
    Puts one entry into data stack specified by stack_id

    Returns DATABASE_INVALID_STACK_ID or DATABASE_OK
*/

DATABASE_ERROR database_write(STACK_ID stack_id, uint8_t* entry);


/*
    Reads latest entry in data stack specified by stack_id and puts it in out (out needs to be innitialised and have sufficient size)

    Returns DATABASE_INVALID_STACK_ID, DATABASE_OK
*/

DATABASE_ERROR database_read_latest(STACK_ID stack_id, uint8_t* out);


/*
    Reads all entries in data stack specified by stack_id, puts them in out (out needs to be innitialised and have sufficient size)
    and sets number_of_entries. Entries are ordered from newest.

    Returns DATABASE_INVALID_STACK_ID or DATABASE_OK
*/

DATABASE_ERROR database_read_all(STACK_ID stack_id, uint8_t** out, uint8_t *number_of_entries);

#endif // DATABASE_H