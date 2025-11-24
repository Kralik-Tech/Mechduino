#ifndef DATABASE_H
#define DATABASE_H

typedef enum {
    DATABASE_OK = 0,
    DATABASE_INVALID_STACK_ID,
    DATABASE_FULL
} DATABASE_ERROR;

typedef STACK_ID uint8_t;        // Cannot be equal to INVALID_STACK_ID
#define INVALID_STACK_ID 0;

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

    Returns DATABASE_INVALID_STACK_ID, DATABASE_WRITE_ERROR or DATABASE_OK
*/

DATABASE_ERROR database_write(STACK_ID stack_id, char* entry);


/*
    Reads latest entry in data stack specified by stack_id and puts it in data

    Returns DATABASE_INVALID_STACK_ID, DATABASE_READ_ERROR, DATABASE_OK
*/

DATABASE_ERROR database_read_latest(STACK_ID stack_id, char* data);


/*
    Reads all entries in data stack specified by stack_id, puts them in out (entries are ordered from newest) and sets number_of_entries

    Returns DATABASE_INVALID_STACK_ID, DATABASE_READ_ERROR or DATABASE_OK
*/

DATABASE_ERROR database_read_all(STACK_ID stack_id, char* out, uint8_t number_of_entries);

#endif // DATABASE_H