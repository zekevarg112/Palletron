// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from motor_interfaces:srv/SetMotorRpm.idl
// generated code does not contain a copyright notice
#include "motor_interfaces/srv/detail/set_motor_rpm__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `command`
#include "motor_interfaces/msg/detail/motor_command__functions.h"

bool
motor_interfaces__srv__SetMotorRpm_Request__init(motor_interfaces__srv__SetMotorRpm_Request * msg)
{
  if (!msg) {
    return false;
  }
  // command
  if (!motor_interfaces__msg__MotorCommand__init(&msg->command)) {
    motor_interfaces__srv__SetMotorRpm_Request__fini(msg);
    return false;
  }
  return true;
}

void
motor_interfaces__srv__SetMotorRpm_Request__fini(motor_interfaces__srv__SetMotorRpm_Request * msg)
{
  if (!msg) {
    return;
  }
  // command
  motor_interfaces__msg__MotorCommand__fini(&msg->command);
}

bool
motor_interfaces__srv__SetMotorRpm_Request__are_equal(const motor_interfaces__srv__SetMotorRpm_Request * lhs, const motor_interfaces__srv__SetMotorRpm_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // command
  if (!motor_interfaces__msg__MotorCommand__are_equal(
      &(lhs->command), &(rhs->command)))
  {
    return false;
  }
  return true;
}

bool
motor_interfaces__srv__SetMotorRpm_Request__copy(
  const motor_interfaces__srv__SetMotorRpm_Request * input,
  motor_interfaces__srv__SetMotorRpm_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // command
  if (!motor_interfaces__msg__MotorCommand__copy(
      &(input->command), &(output->command)))
  {
    return false;
  }
  return true;
}

motor_interfaces__srv__SetMotorRpm_Request *
motor_interfaces__srv__SetMotorRpm_Request__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_interfaces__srv__SetMotorRpm_Request * msg = (motor_interfaces__srv__SetMotorRpm_Request *)allocator.allocate(sizeof(motor_interfaces__srv__SetMotorRpm_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(motor_interfaces__srv__SetMotorRpm_Request));
  bool success = motor_interfaces__srv__SetMotorRpm_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
motor_interfaces__srv__SetMotorRpm_Request__destroy(motor_interfaces__srv__SetMotorRpm_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    motor_interfaces__srv__SetMotorRpm_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
motor_interfaces__srv__SetMotorRpm_Request__Sequence__init(motor_interfaces__srv__SetMotorRpm_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_interfaces__srv__SetMotorRpm_Request * data = NULL;

  if (size) {
    data = (motor_interfaces__srv__SetMotorRpm_Request *)allocator.zero_allocate(size, sizeof(motor_interfaces__srv__SetMotorRpm_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = motor_interfaces__srv__SetMotorRpm_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        motor_interfaces__srv__SetMotorRpm_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
motor_interfaces__srv__SetMotorRpm_Request__Sequence__fini(motor_interfaces__srv__SetMotorRpm_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      motor_interfaces__srv__SetMotorRpm_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

motor_interfaces__srv__SetMotorRpm_Request__Sequence *
motor_interfaces__srv__SetMotorRpm_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_interfaces__srv__SetMotorRpm_Request__Sequence * array = (motor_interfaces__srv__SetMotorRpm_Request__Sequence *)allocator.allocate(sizeof(motor_interfaces__srv__SetMotorRpm_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = motor_interfaces__srv__SetMotorRpm_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
motor_interfaces__srv__SetMotorRpm_Request__Sequence__destroy(motor_interfaces__srv__SetMotorRpm_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    motor_interfaces__srv__SetMotorRpm_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
motor_interfaces__srv__SetMotorRpm_Request__Sequence__are_equal(const motor_interfaces__srv__SetMotorRpm_Request__Sequence * lhs, const motor_interfaces__srv__SetMotorRpm_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!motor_interfaces__srv__SetMotorRpm_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
motor_interfaces__srv__SetMotorRpm_Request__Sequence__copy(
  const motor_interfaces__srv__SetMotorRpm_Request__Sequence * input,
  motor_interfaces__srv__SetMotorRpm_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(motor_interfaces__srv__SetMotorRpm_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    motor_interfaces__srv__SetMotorRpm_Request * data =
      (motor_interfaces__srv__SetMotorRpm_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!motor_interfaces__srv__SetMotorRpm_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          motor_interfaces__srv__SetMotorRpm_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!motor_interfaces__srv__SetMotorRpm_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


bool
motor_interfaces__srv__SetMotorRpm_Response__init(motor_interfaces__srv__SetMotorRpm_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  return true;
}

void
motor_interfaces__srv__SetMotorRpm_Response__fini(motor_interfaces__srv__SetMotorRpm_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
}

bool
motor_interfaces__srv__SetMotorRpm_Response__are_equal(const motor_interfaces__srv__SetMotorRpm_Response * lhs, const motor_interfaces__srv__SetMotorRpm_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  return true;
}

bool
motor_interfaces__srv__SetMotorRpm_Response__copy(
  const motor_interfaces__srv__SetMotorRpm_Response * input,
  motor_interfaces__srv__SetMotorRpm_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  return true;
}

motor_interfaces__srv__SetMotorRpm_Response *
motor_interfaces__srv__SetMotorRpm_Response__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_interfaces__srv__SetMotorRpm_Response * msg = (motor_interfaces__srv__SetMotorRpm_Response *)allocator.allocate(sizeof(motor_interfaces__srv__SetMotorRpm_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(motor_interfaces__srv__SetMotorRpm_Response));
  bool success = motor_interfaces__srv__SetMotorRpm_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
motor_interfaces__srv__SetMotorRpm_Response__destroy(motor_interfaces__srv__SetMotorRpm_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    motor_interfaces__srv__SetMotorRpm_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
motor_interfaces__srv__SetMotorRpm_Response__Sequence__init(motor_interfaces__srv__SetMotorRpm_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_interfaces__srv__SetMotorRpm_Response * data = NULL;

  if (size) {
    data = (motor_interfaces__srv__SetMotorRpm_Response *)allocator.zero_allocate(size, sizeof(motor_interfaces__srv__SetMotorRpm_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = motor_interfaces__srv__SetMotorRpm_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        motor_interfaces__srv__SetMotorRpm_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
motor_interfaces__srv__SetMotorRpm_Response__Sequence__fini(motor_interfaces__srv__SetMotorRpm_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      motor_interfaces__srv__SetMotorRpm_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

motor_interfaces__srv__SetMotorRpm_Response__Sequence *
motor_interfaces__srv__SetMotorRpm_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  motor_interfaces__srv__SetMotorRpm_Response__Sequence * array = (motor_interfaces__srv__SetMotorRpm_Response__Sequence *)allocator.allocate(sizeof(motor_interfaces__srv__SetMotorRpm_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = motor_interfaces__srv__SetMotorRpm_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
motor_interfaces__srv__SetMotorRpm_Response__Sequence__destroy(motor_interfaces__srv__SetMotorRpm_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    motor_interfaces__srv__SetMotorRpm_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
motor_interfaces__srv__SetMotorRpm_Response__Sequence__are_equal(const motor_interfaces__srv__SetMotorRpm_Response__Sequence * lhs, const motor_interfaces__srv__SetMotorRpm_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!motor_interfaces__srv__SetMotorRpm_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
motor_interfaces__srv__SetMotorRpm_Response__Sequence__copy(
  const motor_interfaces__srv__SetMotorRpm_Response__Sequence * input,
  motor_interfaces__srv__SetMotorRpm_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(motor_interfaces__srv__SetMotorRpm_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    motor_interfaces__srv__SetMotorRpm_Response * data =
      (motor_interfaces__srv__SetMotorRpm_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!motor_interfaces__srv__SetMotorRpm_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          motor_interfaces__srv__SetMotorRpm_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!motor_interfaces__srv__SetMotorRpm_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
