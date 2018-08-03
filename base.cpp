#include <unordered_map>
#include <cassert>

struct Chare
{
  static unsigned int type_registration_counter;

  template<typename ChareType>
  static void register_chare_type()
  {
    ++type_registration_counter;
    ChareType::register_chare_members();
  }
  
};

unsigned int Chare::type_registration_counter = 0;

template <typename ChareType>
struct Proxy;

template <typename ChareType>
struct CBase : public Chare
{
  static std::unordered_map<void *, unsigned int> entry_methods;
  static std::unordered_map<unsigned int, void *> entry_table;
  static unsigned int registration_counter;
  friend class Proxy<ChareType>;

  template <typename Return, typename ...Args>
  static void register_entry_method(Return (ChareType::*method_pointer)(Args...))
  {
    entry_methods[reinterpret_cast<void*>(method_pointer)] = ++registration_counter;


    using StaticCaller = void (*)(ChareType&, Args...);
    using CallStruct = Caller<Return, decltype(method_pointer)>;
    
    StaticCaller static_caller = &CallStruct::call<method_pointer>;
    //    StaticCaller static_caller = &Caller<Return, Args...>::call<method_pointer>;
    entry_table[registration_counter] = reinterpret_cast<void*>(static_caller);

      //reinterpret_cast<void*>(static_cast<Return (*)(ChareType&, void*, Args...)>(caller));
  }

  static void* get_entry_method(unsigned int ep_index)
  {
    return entry_table[ep_index];
  }

  //template <typename Return, typename ...Args>
  template <typename Return, Return (ChareType::*method_ptr)(Args...)>
  struct Caller
  {
    static void call(ChareType &c, Args... args)
    {
      
      //return (c.*method_ptr)(args...);
    }
  };

  
  template <typename ...Args, typename Return, Return (ChareType::*method)(Args...)>
  static Return caller(ChareType &chare, void *method_ptr, Args... args)
  {
    return (chare.*method)(args...);
  }
};

template <typename ChareType>
struct Proxy
{
  int collection;
  int element;
  
  template <typename ...MethodArgs, typename ...CallArgs>
  void operator()(void (ChareType::*entry_method)(MethodArgs...), CallArgs... args)
  {
    int ep = CBase<ChareType>::entry_methods[reinterpret_cast<void*>(entry_method)];
#if 0
    decltype(entry_method) method_ptr =
      reinterpret_cast<decltype(entry_method)>(CBase<ChareType>::get_entry_method(ep));
    assert(entry_method == method_ptr);
#endif
    // Make sure we could actually call the damn thing if/when we pack and then unpack on the other side
    ChareType c;
    (c.*entry_method)(args...);
  }

  template <typename ...Args>
  struct Caller
  {
    unsigned int ep;
    void operator()(Args... args)
    {
    }
  };

  template <typename ...Args>
  Caller<Args...> operator()(void (ChareType::*entry_method)(Args...))
  {
    return Caller<Args...>{CBase<ChareType>::entry_methods[reinterpret_cast<void*>(entry_method)]};
  }
  
};


struct MyChare : public CBase<MyChare>
{

  void foo(int i) { }
  void bar(double d) { }

  static void register_chare_members()
  {
    register_entry_method(&MyChare::foo);
    //register_entry_method(&MyChare::bar);
  }

};

int main(int argc, char** argv)
{
  MyChare c;

  Proxy<MyChare> p{10,10};
  p(&MyChare::foo, 1);
  

  return 0;
}
