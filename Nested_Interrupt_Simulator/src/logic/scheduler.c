#include "scheduler.h"

resource_status_t global_cpu_status;
interrupt_task_t task_history_array[MAXIMUM_HISTORY_SIZE];
int history_count = 0;
int current_task_duration = 0;

void initialize_scheduler() {
    global_cpu_status.is_cpu_idle = true;
    history_count = 0;
    current_task_duration = 0;
}

static void save_task_to_history() {
    if (current_task_duration > 0) {
        if (history_count < MAXIMUM_HISTORY_SIZE) {
            task_history_array[history_count] = global_cpu_status.current_task;
            task_history_array[history_count].remaining_running_time = current_task_duration;
            history_count++;
            current_task_duration = 0;
        }
    }
}

void handle_incoming_interrupt(interrupt_task_t new_task, nested_stack_t* system_stack, priority_queue_t* system_queue) {
    if (global_cpu_status.is_cpu_idle == true) {
        global_cpu_status.current_task = new_task;
        global_cpu_status.is_cpu_idle = false;
        current_task_duration = 0;
    }
    else {
        if (new_task.task_priority < global_cpu_status.current_task.task_priority) {
            save_task_to_history();
            system_stack->push_task(system_stack, global_cpu_status.current_task);
            global_cpu_status.current_task = new_task;
        }
        else {
            system_queue->push_task(system_queue, new_task);
        }
    }
}

void execute_system_tick(nested_stack_t* system_stack, priority_queue_t* system_queue) {
    if (global_cpu_status.is_cpu_idle == true) {
        return;
    }

    global_cpu_status.current_task.remaining_running_time--;
    current_task_duration++;

    if (global_cpu_status.current_task.remaining_running_time <= 0) {
        save_task_to_history();
        global_cpu_status.is_cpu_idle = true;

        bool is_stack_empty = system_stack->is_empty(system_stack);
        bool is_queue_empty = system_queue->is_empty(system_queue);

        if (is_stack_empty == false && is_queue_empty == false) {
            int stack_top_priority = system_stack->peek_task(system_stack).task_priority;
            int queue_top_priority = system_queue->peek_task(system_queue).task_priority;

            if (stack_top_priority <= queue_top_priority) {
                global_cpu_status.current_task = system_stack->pop_task(system_stack);
            }
            else {
                global_cpu_status.current_task = system_queue->pop_task(system_queue);
            }
            global_cpu_status.is_cpu_idle = false;
        }
        else if (is_stack_empty == false) {
            global_cpu_status.current_task = system_stack->pop_task(system_stack);
            global_cpu_status.is_cpu_idle = false;
        }
        else if (is_queue_empty == false) {
            global_cpu_status.current_task = system_queue->pop_task(system_queue);
            global_cpu_status.is_cpu_idle = false;
        }
    }
}