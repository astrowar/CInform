
#include "utils.hpp"
#include <string>

using namespace std;
namespace CInform
{
    
 
  string stringList(list<string> xs)
            {
                string s = string("");
                bool fr = true;
                for(string &i : xs) 
                {  
                     if (fr ==false )   s=s+" | ";
                     fr= false;
                     s =s + i; 
                }
                return "[" + s + "]";
            }

  //template <typename T,typename Q>  std::list<Q>   list_transform(std::list<T> &_in, function< Q(T)> func ) 
  // {
  //       std::list<Q>  x;
  //       std::transform(_in.begin(), _in.end(), x.begin(),
  //            [](T i){ return func(i) } );
  // }

 

}