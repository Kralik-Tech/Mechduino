#ifndef SHEDULER_H
#define SHEDULER_H

    #include <stdlib.h>
    #include <stdint.h>

    typedef enum {
        LOW_PRIORITY = 1,
        ELEVATED_PRIORITY = 2,
        HIGH_PRIORITY = 3
    } TASK_PRIORITY_LEVEL;

    typedef enum {
        SCHEDULER_OK = 0,                   // No error
        SCHEDULER_INVALID_PRIORITY,         // Task cannot be configured with this priority
        SCHEDULER_INVALID_CALLBACK,         // Callback function can´t be used
        SCHEDULER_INVALID_ID                // No task with this id      
    } SCHEDULER_ERROR;

    /*
        Does initialisation of scheduler
    */

    void scheduler_init(void); 

    /*
        Runs scheduler indefinitely
    */

    void scheduler_run(void);

    /*
        Adds task specified by task_callback, sets optional passing of task_context, configures its priority and execution_frequency and puts id of created task in task_id (if error occures, it is set to 0)

        Returns SCHEDULER_INVALID_PRIORITY, SCHEDULER_INVALID_CALLBACK or SCHEDULER_OK
    */

    SCHEDULER_ERROR add_task(void* task_callback, void* task_context, TASK_PRIORITY_LEVEL priority, uint32_t execution_frequency, uint32_t *task_id);

    /*
        Removes task specified by task_id

        Returns SCHEDULER_INVALID_ID or SCHEDULER_OK
    */

    SCHEDULER_ERROR remove_task(uint32_t task_id);

    /*
        Changes priority of task specified by task_id to priority

        Returns SCHEDULER_INVALID_ID, SCHEDULER_INVALID_PRIORITY or SCHEDULER_OK
    */

    SCHEDULER_ERROR change_task_priority(uint32_t task_id, TASK_PRIORITY_LEVEL priority);

    /*
        Changes execution frequency of task specified by task_id to execution_frequency

        Returns SCHEDULER_INVALID_ID or SCHEDULER_OK
    */

    SCHEDULER_ERROR change_task_execution_frequency(uint32_t task_id, uint32_t execution_frequency);

#endif //SCHEDULER_H
