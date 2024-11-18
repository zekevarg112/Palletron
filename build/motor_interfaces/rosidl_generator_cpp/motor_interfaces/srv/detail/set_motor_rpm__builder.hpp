// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from motor_interfaces:srv/SetMotorRpm.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__BUILDER_HPP_
#define MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "motor_interfaces/srv/detail/set_motor_rpm__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace motor_interfaces
{

namespace srv
{

namespace builder
{

class Init_SetMotorRpm_Request_command
{
public:
  Init_SetMotorRpm_Request_command()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::motor_interfaces::srv::SetMotorRpm_Request command(::motor_interfaces::srv::SetMotorRpm_Request::_command_type arg)
  {
    msg_.command = std::move(arg);
    return std::move(msg_);
  }

private:
  ::motor_interfaces::srv::SetMotorRpm_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::motor_interfaces::srv::SetMotorRpm_Request>()
{
  return motor_interfaces::srv::builder::Init_SetMotorRpm_Request_command();
}

}  // namespace motor_interfaces


namespace motor_interfaces
{

namespace srv
{

namespace builder
{

class Init_SetMotorRpm_Response_success
{
public:
  Init_SetMotorRpm_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::motor_interfaces::srv::SetMotorRpm_Response success(::motor_interfaces::srv::SetMotorRpm_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return std::move(msg_);
  }

private:
  ::motor_interfaces::srv::SetMotorRpm_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::motor_interfaces::srv::SetMotorRpm_Response>()
{
  return motor_interfaces::srv::builder::Init_SetMotorRpm_Response_success();
}

}  // namespace motor_interfaces

#endif  // MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__BUILDER_HPP_
