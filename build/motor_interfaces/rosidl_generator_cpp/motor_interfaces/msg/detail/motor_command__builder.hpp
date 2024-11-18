// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from motor_interfaces:msg/MotorCommand.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_INTERFACES__MSG__DETAIL__MOTOR_COMMAND__BUILDER_HPP_
#define MOTOR_INTERFACES__MSG__DETAIL__MOTOR_COMMAND__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "motor_interfaces/msg/detail/motor_command__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace motor_interfaces
{

namespace msg
{

namespace builder
{

class Init_MotorCommand_left
{
public:
  explicit Init_MotorCommand_left(::motor_interfaces::msg::MotorCommand & msg)
  : msg_(msg)
  {}
  ::motor_interfaces::msg::MotorCommand left(::motor_interfaces::msg::MotorCommand::_left_type arg)
  {
    msg_.left = std::move(arg);
    return std::move(msg_);
  }

private:
  ::motor_interfaces::msg::MotorCommand msg_;
};

class Init_MotorCommand_right
{
public:
  Init_MotorCommand_right()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MotorCommand_left right(::motor_interfaces::msg::MotorCommand::_right_type arg)
  {
    msg_.right = std::move(arg);
    return Init_MotorCommand_left(msg_);
  }

private:
  ::motor_interfaces::msg::MotorCommand msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::motor_interfaces::msg::MotorCommand>()
{
  return motor_interfaces::msg::builder::Init_MotorCommand_right();
}

}  // namespace motor_interfaces

#endif  // MOTOR_INTERFACES__MSG__DETAIL__MOTOR_COMMAND__BUILDER_HPP_
