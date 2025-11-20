#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "../scheduler.h"
#include "../../platform/platform_timer.h"

typedef struct task_handle {
    uint32_t task_id;                       // Identifier of task
    bool delete_flag;                       // Is true when task is due deletion after next execution
    void* task_context;                     // Optional data passed to task on execution
    void (*task_callback) (void * contex);  // Pointer on task function
    TASK_PRIORITY_LEVEL priority;           // Priority over other tasks
    uint32_t execution_frequency;           // Specifies ow many execution windows will pass between executions
    struct task_handle* next_task;          // Pointer to next task in execution_window (NULL if no such task exist)
} TASK_HANDLE;

typedef struct execution_window {
    TASK_HANDLE* task_list;             // List of tasks to execute
    uint32_t number;                    // Number of execution window
    struct execution_window* next;      // Pointer to another execution window
} EXECUTION_WINDOW;

#define MINIMAL_EXECUTION_WINDOW_RUNTIME 5

#define MAXIMAL_TASK_EXECUTION_TIME 200 

EXECUTION_WINDOW* execution_window_list;    // Contains all execution windows, implemented as linked list

uint32_t next_free_task_id;                     
uint32_t number_of_active_tasks;
uint32_t active_execution_window;           // Number of currently running execution window

bool scheduler_needs_init = true;


/** Helper functions **/

bool is_priority_invalid (TASK_PRIORITY_LEVEL priority) {
    switch (priority) {
        case LOW_PRIORITY:
        case ELEVATED_PRIORITY:
        case HIGH_PRIORITY:
            return false;
    }
        
    return true;
}

TASK_HANDLE* get_task_by_id(uint32_t task_id) {
    if (number_of_active_tasks == 0) {
        return NULL;
    }

    if (execution_window_list == NULL) return NULL;

    EXECUTION_WINDOW* window = execution_window_list;
    TASK_HANDLE* task = execution_window_list->task_list;

    while(window != NULL) {        

        while(task != NULL) {

            if (task->task_id == task_id) {
                return task;
            }

            task = task->next_task;
        }

        window = window->next;

        if (window == NULL) break;

        task = window->task_list;
    
    }

    return NULL;

}

/* Inserts task to execution window with number execution_offset higher than active_execution_window */
void insert_task (TASK_HANDLE* task, uint32_t execution_offset) {
    if (execution_window_list == NULL) { // No execution window existed
        execution_window_list = (EXECUTION_WINDOW*) malloc(sizeof(EXECUTION_WINDOW));
        execution_window_list->number = execution_offset;
        execution_window_list->task_list = task;
        execution_window_list->next = NULL;

        return;
    }   
    
    EXECUTION_WINDOW* window = execution_window_list;

    while ((window->next != NULL) && (window->next->number < active_execution_window + execution_offset)) window = window->next;

    if (window->next == NULL) { // Insert in new execution window at the end of execution_window_list
        window->next = (EXECUTION_WINDOW*) malloc(sizeof(EXECUTION_WINDOW));
        window->next->task_list = task;
        window->next->number = active_execution_window + execution_offset;
        window->next->next = NULL;
    } 

    else if (window->next->number == (active_execution_window + execution_offset)) { // Insert into already existing execution window
        TASK_HANDLE* insert = window->next->task_list;

        if (insert->priority <= task->priority) {
            task->next_task = insert;
            window->next->task_list = task;

            return;
        }

        while ((insert->next_task != NULL) && (insert->next_task->priority <= task->priority)) insert = insert->next_task;

        task->next_task = insert->next_task;
        insert->next_task = task;
    }   

    else { // Insert with new execution window in between 2 existing ones
        EXECUTION_WINDOW* new_window = (EXECUTION_WINDOW*) malloc(sizeof(EXECUTION_WINDOW));
        new_window->number = execution_offset;
        new_window->task_list = task;
        new_window->next = window->next;

        window->next = new_window;
    }
}


/** Library functions **/

void scheduler_init() {
    next_free_task_id = 1;

    number_of_active_tasks = 0;

    execution_window_list = NULL;

    active_execution_window = 0;

    scheduler_needs_init = false;
}

void scheduler_run(void) {
    TIMER_ID_TYPE execution_window_timer = 0;

    setup_timer(MILLISECOND_TIMER, &execution_window_timer);

    goto START;

    while (true) {

        if (reset_timer(execution_window_timer)) {
            setup_timer(MILLISECOND_TIMER, &execution_window_timer);
        }

        EXECUTION_WINDOW* old_window = execution_window_list;

        if (old_window == NULL) goto END;
        execution_window_list = execution_window_list->next;
        
        free(old_window);

        START:

        active_execution_window++;

        if (execution_window_list == NULL) goto END;

        while (execution_window_list->task_list != NULL) {

            TASK_HANDLE* active_task = execution_window_list->task_list;

            execution_window_list->task_list = active_task->next_task;
            active_task->next_task = NULL;

            start_watchdog(MAXIMAL_TASK_EXECUTION_TIME);

            active_task->task_callback(active_task->task_context);

            stop_watchdog();

            if (active_task->delete_flag){
                free(active_task);

                continue;
            }
            else {
                insert_task(active_task, active_task->execution_frequency);
            }

        }

        END:

        while(true) {
            TIMER_PRECISION timer_value = 0;

            get_timer(execution_window_timer, &timer_value);

            if (timer_value > MINIMAL_EXECUTION_WINDOW_RUNTIME) break;

        }
    }
}

SCHEDULER_ERROR add_task(void* task_callback, void* task_context, TASK_PRIORITY_LEVEL priority, uint32_t execution_frequency, uint32_t *task_id) {
    if (task_callback == NULL) return SCHEDULER_INVALID_CALLBACK;

    if (is_priority_invalid(priority)) return SCHEDULER_INVALID_PRIORITY;

    TASK_HANDLE *new_task = (TASK_HANDLE*) malloc(sizeof(TASK_HANDLE));

    new_task->delete_flag = false;
    new_task->task_context = task_context;
    new_task->task_callback = task_callback;
    new_task->priority = priority;
    new_task->execution_frequency = execution_frequency;
    new_task->next_task = NULL;
    new_task->task_id = next_free_task_id;

    insert_task(new_task, 1);

    *task_id = new_task->task_id;
    next_free_task_id++;

    number_of_active_tasks++;

    return SCHEDULER_OK;
}

SCHEDULER_ERROR remove_task(uint32_t task_id) {
    TASK_HANDLE *task = get_task_by_id(task_id);

    if (task == NULL) return SCHEDULER_INVALID_ID;

    EXECUTION_WINDOW *window = execution_window_list;
    TASK_HANDLE *remove = execution_window_list->task_list;
    bool is_remove_list_start = false;

    while (window != NULL) {
        if (remove == task) {
            is_remove_list_start = true;

            goto END;
        }
        if (remove != NULL) {
            while (remove->next_task != NULL) {
                if (remove->next_task == task) {
                    goto END;
                }
                remove = remove->next_task;
            }
        }

        window = window->next;
        remove = window->task_list;
    }

    return SCHEDULER_INVALID_ID;

    END:
    if (window->number == active_execution_window) {
        task->delete_flag = true;

        return SCHEDULER_OK;
    }

    if (is_remove_list_start) {
        window->task_list = task->next_task;
        free(task);
    }
    else {
        remove->next_task = task->next_task;

        free(task);
    }

    return SCHEDULER_OK;
}

SCHEDULER_ERROR change_task_priority(uint32_t task_id, TASK_PRIORITY_LEVEL priority) {
    if (is_priority_invalid(priority)) return SCHEDULER_INVALID_PRIORITY;

    TASK_HANDLE* selected_task = get_task_by_id(task_id);

    if (selected_task == NULL) return SCHEDULER_INVALID_ID;

    selected_task->priority = priority;

    return SCHEDULER_OK;
}

SCHEDULER_ERROR change_task_execution_frequency(uint32_t task_id, uint32_t execution_frequency) {
    TASK_HANDLE* selected_task = get_task_by_id(task_id);

    if (selected_task == NULL) return SCHEDULER_INVALID_ID;

    selected_task->execution_frequency = execution_frequency;

    return SCHEDULER_OK;
}

