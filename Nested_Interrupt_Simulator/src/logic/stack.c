#include "stack.h"

static void push_task_impl(nested_stack_t* self, interrupt_task_t new_task) {
    if (self->current_data_count < MAXIMUM_STACK_SIZE) {
        self->stack_array[self->current_data_count] = new_task;
        self->current_data_count++;
    }
    else {
        // 스택이 다 찼을때
    }
}

static interrupt_task_t pop_task_impl(nested_stack_t* self) {
    self->current_data_count--;
    return self->stack_array[self->current_data_count];
}

static interrupt_task_t peek_task_impl(nested_stack_t* self) {
    return self->stack_array[self->current_data_count - 1];
}

static bool is_empty_impl(nested_stack_t* self) {
    if (self->current_data_count == 0) {
        return true;
    }
    else {
        return false;
    }
}

void initialize_stack(nested_stack_t* stack_instance) {
    stack_instance->current_data_count = 0;
    stack_instance->push_task = push_task_impl;
    stack_instance->pop_task = pop_task_impl;
    stack_instance->peek_task = peek_task_impl;
    stack_instance->is_empty = is_empty_impl;
}