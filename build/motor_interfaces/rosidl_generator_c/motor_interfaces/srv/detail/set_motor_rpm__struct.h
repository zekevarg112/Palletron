// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from motor_interfaces:srv/SetMotorRpm.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__STRUCT_H_
#define MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'command'
#include "motor_interfaces/msg/detail/motor_command__struct.h"

/// Struct defined in srv/SetMotorRpm in the package motor_interfaces.
typedef struct motor_interfaces__srv__SetMotorRpm_Request
{
  motor_interfaces__msg__MotorCommand command;
} motor_interfaces__srv__SetMotorRpm_Request;

// Struct for a sequence of motor_interfaces__srv__SetMotorRpm_Request.
typedef struct motor_interfaces__srv__SetMotorRpm_Request__Sequence
{
  motor_interfaces__srv__SetMotorRpm_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} motor_interfaces__srv__SetMotorRpm_Request__Sequence;


// Constants defined in the message

/// Struct defined in srv/SetMotorRpm in the package motor_interfaces.
typedef struct motor_interfaces__srv__SetMotorRpm_Response
{
  bool success;
} motor_interfaces__srv__SetMotorRpm_Response;

// Struct for a sequence of motor_interfaces__srv__SetMotorRpm_Response.
typedef struct motor_interfaces__srv__SetMotorRpm_Response__Sequence
{
  motor_interfaces__srv__SetMotorRpm_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} motor_interfaces__srv__SetMotorRpm_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__STRUCT_H_
