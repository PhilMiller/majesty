#include <unordered_map>


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
  static unsigned int registration_counter;
  friend class Proxy<ChareType>;

  template <typename Return, typename ...Args>
  static void register_entry_method(Return (ChareType::*method_pointer)(Args...))
  {
    entry_methods[reinterpret_cast<void*>(method_pointer)] = ++registration_counter;
  }


};

template <typename ChareType>
struct Proxy
{
  int collection;
  int element;
  
  template<typename ...Args>
  void operator()(void (ChareType::*entry_method)(Args...), Args... args)
  {
    int ep = CBase<ChareType>::entry_methods[reinterpret_cast<void*>(entry_method)];

  }
  
};


struct MyChare : public CBase<MyChare>
{

  void foo(int i) { }

  static void register_chare_members()
  {
    register_entry_method(&MyChare::foo);
  }

};

int main(int argc, char** argv)
{
  MyChare c;

  Proxy<MyChare> p{10,10};
  p(&MyChare::foo, 1);
  

  return 0;
}
