#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

void initialize_scheduler();
void handle_incoming_interrupt(interrupt_task_t new_task, nested_stack_t* system_stack, priority_queue_t* system_queue);
void execute_system_tick(nested_stack_t* system_stack, priority_queue_t* system_queue);

#endif