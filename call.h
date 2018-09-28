#pragma once

#include <experimental/reflect>


template <auto MethodPointer> // auto here is C++17
struct registration
{
  using callable = decltype(MethodPointer);

  static int number = 0;
  int register()
  {
    number = system::get_next_entry_registration();
    return number;
  }
  int get()
  {
    assert(number != 0);
    return number;
  }
};

// Illustrative versions for comparison/discussion of need for symbol->object reification

#define SEND(PROXY_OBJECT, METHOD_NAME, ...)                            \
  do {                                                                  \
    using target_type = decltype(PROXY_OBJECT)::target_type_t;          \
    using call_expression = reflexpr(declval(target_type).METHOD_NAME(__VA_ARGS__)); \
    using callable = std::experimental::reflect::get_callable_t<call_expression>; \
    auto method_ptr = std::experimental::reflect::get_pointer_v<callable>; \
                                                                        \
    if (system.should_call_inline(method_ptr)) {                        \
      target_type *obj_ptr = system.get_object_ptr(PROXY_OBJECT);       \
      obj_ptr->*method_ptr(__VA_ARGS__);                                \
    } else {                                                            \
      auto method_handle = registration<method_ptr>::get();             \
      system.pack_and_send(PROXY_OBJECT, method_handle, __VA_ARGS__);   \
    }                                                                   \
  } while (false)


template <typename Proxy, typename ...Args>
//                     vvvvvv This doesn't actually exist, AFAIK, so we need the macro above
void send(Proxy proxy, Symbol method_name, Args&& ...args) {
  using target_type = Proxy::target_type_t;
  //            Tricky bit is here vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  using call_expression = reflexpr(invoke_member_symbol(method_name, declval(target_type), args...));
  // What reference category do we want for this, value or forwarded?                      ^^^^^^^
  using callable = std::experimental::reflect::get_callable_t<call_expression>;
  auto method_ptr = std::experimental::reflect::get_pointer_v<callable>;

  if (system.should_call_inline(method_ptr)) {
    target_type *obj_ptr = system.get_object_ptr(proxy);
    obj_ptr->*method_ptr(std::forward<Args>(args)...);
  } else {
    auto method_handle = system.get_method_handle(method_ptr);
    system.pack_and_send(PROXY_OBJECT, method_handle, std::forward<Args>(args)...);
  }
}


// More serious implementation versions

#define SEND(PROXY_OBJECT, METHOD_NAME, ...)                          \
  do {                                                                  \
    using target_type = decltype(PROXY_OBJECT)::target_type_t;          \
    using call_expression = reflexpr(declval(target_type).METHOD_NAME(__VA_ARGS__)); \
    using callable = std::experimental::reflect::get_callable_t<call_expression>; \
    auto method_ptr = std::experimental::reflect::get_pointer_v<callable>; \
                                                                        \
    if (system.should_call_inline(method_ptr)) {                        \
      target_type *obj_ptr = system.get_object_ptr(PROXY_OBJECT);       \
      obj_ptr->*method_ptr(__VA_ARGS__); /* return value? */            \
    } else {                                                            \
      auto method_handle = system.get_method_handle(method_ptr);        \
      system.pack_and_send(PROXY_OBJECT, method_handle, __VA_ARGS__);   \
    }                                                                   \
  } while (false)

template <typename Parameter_Tuple, typename ...Args>
void pack_and_send(Args&& ...args)
{
  
}

template <typename Proxy, typename ...Args>
void send(Proxy proxy, Symbol method_name, Args&& ...args)
{
  using target_type = Proxy::target_type_t;
  using call_expression = reflexpr(invoke_member_symbol(method_name, declval(target_type), args...));
  using callable = std::experimental::reflect::get_callable_t<call_expression>;
  auto method_ptr = std::experimental::reflect::get_pointer_v<callable>;

  if (system.should_call_inline(method_ptr)) {
    target_type *obj_ptr = system.get_object_ptr(proxy);
    obj_ptr->*method_ptr(args...); /* return value? */
  } else {
    auto method_handle = system.get_method_handle(method_ptr);

    using parameters_types = get_parameters_t<callable>;
    using parameters_tuple = unpack_sequence_t<std::tuple, parameters>;
    // How to size arguments as converted without performing and
    // hanging onto all of their conversions? Is it even really a big
    // deal to hang onto all the conversions, or should we just make
    // the tuple and then pack from that?

    parameters_tuple converted_args{args...};

    size_t size = (0 + ... + size_arguments<get_type_t<parameter_types>>(args));

    std::index_sequence_for<Args...>

    
    system.pack_and_send(PROXY_OBJECT, method_handle, args...);
  }
}
