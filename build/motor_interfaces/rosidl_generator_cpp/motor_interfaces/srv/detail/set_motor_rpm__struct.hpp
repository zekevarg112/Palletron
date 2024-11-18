// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from motor_interfaces:srv/SetMotorRpm.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__STRUCT_HPP_
#define MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'command'
#include "motor_interfaces/msg/detail/motor_command__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__motor_interfaces__srv__SetMotorRpm_Request __attribute__((deprecated))
#else
# define DEPRECATED__motor_interfaces__srv__SetMotorRpm_Request __declspec(deprecated)
#endif

namespace motor_interfaces
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SetMotorRpm_Request_
{
  using Type = SetMotorRpm_Request_<ContainerAllocator>;

  explicit SetMotorRpm_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : command(_init)
  {
    (void)_init;
  }

  explicit SetMotorRpm_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : command(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _command_type =
    motor_interfaces::msg::MotorCommand_<ContainerAllocator>;
  _command_type command;

  // setters for named parameter idiom
  Type & set__command(
    const motor_interfaces::msg::MotorCommand_<ContainerAllocator> & _arg)
  {
    this->command = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__motor_interfaces__srv__SetMotorRpm_Request
    std::shared_ptr<motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__motor_interfaces__srv__SetMotorRpm_Request
    std::shared_ptr<motor_interfaces::srv::SetMotorRpm_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SetMotorRpm_Request_ & other) const
  {
    if (this->command != other.command) {
      return false;
    }
    return true;
  }
  bool operator!=(const SetMotorRpm_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SetMotorRpm_Request_

// alias to use template instance with default allocator
using SetMotorRpm_Request =
  motor_interfaces::srv::SetMotorRpm_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace motor_interfaces


#ifndef _WIN32
# define DEPRECATED__motor_interfaces__srv__SetMotorRpm_Response __attribute__((deprecated))
#else
# define DEPRECATED__motor_interfaces__srv__SetMotorRpm_Response __declspec(deprecated)
#endif

namespace motor_interfaces
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct SetMotorRpm_Response_
{
  using Type = SetMotorRpm_Response_<ContainerAllocator>;

  explicit SetMotorRpm_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
    }
  }

  explicit SetMotorRpm_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
    }
  }

  // field types and members
  using _success_type =
    bool;
  _success_type success;

  // setters for named parameter idiom
  Type & set__success(
    const bool & _arg)
  {
    this->success = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__motor_interfaces__srv__SetMotorRpm_Response
    std::shared_ptr<motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__motor_interfaces__srv__SetMotorRpm_Response
    std::shared_ptr<motor_interfaces::srv::SetMotorRpm_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SetMotorRpm_Response_ & other) const
  {
    if (this->success != other.success) {
      return false;
    }
    return true;
  }
  bool operator!=(const SetMotorRpm_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SetMotorRpm_Response_

// alias to use template instance with default allocator
using SetMotorRpm_Response =
  motor_interfaces::srv::SetMotorRpm_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace motor_interfaces

namespace motor_interfaces
{

namespace srv
{

struct SetMotorRpm
{
  using Request = motor_interfaces::srv::SetMotorRpm_Request;
  using Response = motor_interfaces::srv::SetMotorRpm_Response;
};

}  // namespace srv

}  // namespace motor_interfaces

#endif  // MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__STRUCT_HPP_
