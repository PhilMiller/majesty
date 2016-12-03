#include <unordered_map>


template <typename ChareType>
struct Proxy
{
  int collection;
  int element;

  
  template<typename ...Args>
  void operator()(void (ChareType::*entry_method)(Args...), Args... args)
  {
    int ep = ChareType::entry_methods[reinterpret_cast<void*>(entry_method)];

  }
  
};

class Chare
{
  static std::unordered_map<void *, unsigned int> entry_methods;
  friend class Proxy<Chare>;

public:
  void foo(int i) { }


};



int main(int argc, char** argv)
{
  Chare c;

  Proxy<Chare> p{10,10};
  p(&Chare::foo, 1);
  

  return 0;
}
