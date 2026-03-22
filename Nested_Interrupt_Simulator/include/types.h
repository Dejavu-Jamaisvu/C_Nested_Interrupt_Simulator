#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define MAXIMUM_STACK_SIZE 10
#define MAXIMUM_QUEUE_SIZE 10
#define MAXIMUM_HISTORY_SIZE 100

#pragma pack(push, 1)

typedef struct _interrupt_task {
    char task_name[100];
    int task_priority;
    int remaining_running_time;
} interrupt_task_t;

typedef struct _nested_stack {
    interrupt_task_t stack_array[MAXIMUM_STACK_SIZE];
    int current_data_count;

    void (*push_task)(struct _nested_stack* self, interrupt_task_t new_task);
    interrupt_task_t (*pop_task)(struct _nested_stack* self);
    interrupt_task_t (*peek_task)(struct _nested_stack* self);
    bool (*is_empty)(struct _nested_stack* self);
} nested_stack_t;

typedef struct _priority_queue {
    interrupt_task_t queue_array[MAXIMUM_QUEUE_SIZE];
    int current_size;

    void (*push_task)(struct _priority_queue* self, interrupt_task_t new_task);
    interrupt_task_t (*pop_task)(struct _priority_queue* self);
    interrupt_task_t (*peek_task)(struct _priority_queue* self);
    bool (*is_empty)(struct _priority_queue* self);
} priority_queue_t;

typedef struct _resource_status {
    interrupt_task_t current_task;
    bool is_cpu_idle;
} resource_status_t;

#pragma pop(pop)

#endif