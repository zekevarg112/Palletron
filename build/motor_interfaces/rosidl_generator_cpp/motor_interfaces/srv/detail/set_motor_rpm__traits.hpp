// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from motor_interfaces:srv/SetMotorRpm.idl
// generated code does not contain a copyright notice

#ifndef MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__TRAITS_HPP_
#define MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "motor_interfaces/srv/detail/set_motor_rpm__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'command'
#include "motor_interfaces/msg/detail/motor_command__traits.hpp"

namespace motor_interfaces
{

namespace srv
{

inline void to_flow_style_yaml(
  const SetMotorRpm_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: command
  {
    out << "command: ";
    to_flow_style_yaml(msg.command, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const SetMotorRpm_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: command
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "command:\n";
    to_block_style_yaml(msg.command, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const SetMotorRpm_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace motor_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use motor_interfaces::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const motor_interfaces::srv::SetMotorRpm_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  motor_interfaces::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use motor_interfaces::srv::to_yaml() instead")]]
inline std::string to_yaml(const motor_interfaces::srv::SetMotorRpm_Request & msg)
{
  return motor_interfaces::srv::to_yaml(msg);
}

template<>
inline const char * data_type<motor_interfaces::srv::SetMotorRpm_Request>()
{
  return "motor_interfaces::srv::SetMotorRpm_Request";
}

template<>
inline const char * name<motor_interfaces::srv::SetMotorRpm_Request>()
{
  return "motor_interfaces/srv/SetMotorRpm_Request";
}

template<>
struct has_fixed_size<motor_interfaces::srv::SetMotorRpm_Request>
  : std::integral_constant<bool, has_fixed_size<motor_interfaces::msg::MotorCommand>::value> {};

template<>
struct has_bounded_size<motor_interfaces::srv::SetMotorRpm_Request>
  : std::integral_constant<bool, has_bounded_size<motor_interfaces::msg::MotorCommand>::value> {};

template<>
struct is_message<motor_interfaces::srv::SetMotorRpm_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace motor_interfaces
{

namespace srv
{

inline void to_flow_style_yaml(
  const SetMotorRpm_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const SetMotorRpm_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const SetMotorRpm_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace motor_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use motor_interfaces::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const motor_interfaces::srv::SetMotorRpm_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  motor_interfaces::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use motor_interfaces::srv::to_yaml() instead")]]
inline std::string to_yaml(const motor_interfaces::srv::SetMotorRpm_Response & msg)
{
  return motor_interfaces::srv::to_yaml(msg);
}

template<>
inline const char * data_type<motor_interfaces::srv::SetMotorRpm_Response>()
{
  return "motor_interfaces::srv::SetMotorRpm_Response";
}

template<>
inline const char * name<motor_interfaces::srv::SetMotorRpm_Response>()
{
  return "motor_interfaces/srv/SetMotorRpm_Response";
}

template<>
struct has_fixed_size<motor_interfaces::srv::SetMotorRpm_Response>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<motor_interfaces::srv::SetMotorRpm_Response>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<motor_interfaces::srv::SetMotorRpm_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<motor_interfaces::srv::SetMotorRpm>()
{
  return "motor_interfaces::srv::SetMotorRpm";
}

template<>
inline const char * name<motor_interfaces::srv::SetMotorRpm>()
{
  return "motor_interfaces/srv/SetMotorRpm";
}

template<>
struct has_fixed_size<motor_interfaces::srv::SetMotorRpm>
  : std::integral_constant<
    bool,
    has_fixed_size<motor_interfaces::srv::SetMotorRpm_Request>::value &&
    has_fixed_size<motor_interfaces::srv::SetMotorRpm_Response>::value
  >
{
};

template<>
struct has_bounded_size<motor_interfaces::srv::SetMotorRpm>
  : std::integral_constant<
    bool,
    has_bounded_size<motor_interfaces::srv::SetMotorRpm_Request>::value &&
    has_bounded_size<motor_interfaces::srv::SetMotorRpm_Response>::value
  >
{
};

template<>
struct is_service<motor_interfaces::srv::SetMotorRpm>
  : std::true_type
{
};

template<>
struct is_service_request<motor_interfaces::srv::SetMotorRpm_Request>
  : std::true_type
{
};

template<>
struct is_service_response<motor_interfaces::srv::SetMotorRpm_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // MOTOR_INTERFACES__SRV__DETAIL__SET_MOTOR_RPM__TRAITS_HPP_
