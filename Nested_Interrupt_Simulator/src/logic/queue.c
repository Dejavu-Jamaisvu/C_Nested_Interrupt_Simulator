#include "queue.h"

static void heap_push_impl(priority_queue_t* self, interrupt_task_t new_task) {
    if (self->current_size < MAXIMUM_QUEUE_SIZE) {
        int current_index = self->current_size;
        self->current_size++;
        self->queue_array[current_index] = new_task;

        while (current_index > 0) {
            int parent_index = (current_index - 1) / 2;
            
            if (self->queue_array[parent_index].task_priority > self->queue_array[current_index].task_priority) {
                interrupt_task_t temporary_task = self->queue_array[parent_index];
                self->queue_array[parent_index] = self->queue_array[current_index];
                self->queue_array[current_index] = temporary_task;
                current_index = parent_index;
            }
            else {
                break;
            }
        }
    }
    else {
        // queue full
    }
}

static interrupt_task_t heap_pop_impl(priority_queue_t* self) {
    interrupt_task_t top_priority_task = self->queue_array[0];
    
    self->current_size--;
    self->queue_array[0] = self->queue_array[self->current_size];
    
    int current_index = 0;
    
    while ((current_index * 2) + 1 < self->current_size) {
        int left_child_index = (current_index * 2) + 1;
        int right_child_index = (current_index * 2) + 2;
        int minimum_child_index = left_child_index;

        if (right_child_index < self->current_size) {
            if (self->queue_array[right_child_index].task_priority < self->queue_array[left_child_index].task_priority) {
                minimum_child_index = right_child_index;
            }
        }

        if (self->queue_array[current_index].task_priority > self->queue_array[minimum_child_index].task_priority) {
            interrupt_task_t temporary_task = self->queue_array[current_index];
            self->queue_array[current_index] = self->queue_array[minimum_child_index];
            self->queue_array[minimum_child_index] = temporary_task;
            current_index = minimum_child_index;
        }
        else {
            break;
        }
    }
    
    return top_priority_task;
}

static interrupt_task_t heap_peek_impl(priority_queue_t* self) {
    return self->queue_array[0];
}

static bool heap_is_empty_impl(priority_queue_t* self) {
    if (self->current_size == 0) {
        return true;
    }
    else {
        return false;
    }
}

void initialize_queue(priority_queue_t* queue_instance) {
    queue_instance->current_size = 0;
    queue_instance->push_task = heap_push_impl;
    queue_instance->pop_task = heap_pop_impl;
    queue_instance->peek_task = heap_peek_impl;
    queue_instance->is_empty = heap_is_empty_impl;
}