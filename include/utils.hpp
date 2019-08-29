#ifndef PUTILS_HPP
#define PUTILS_HPP

#include <list>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <string>

using namespace std;
namespace CInform
{

	std::list<string>   split(string x);
	std::list<string>   split(string x, char sep);
	std::string stringList(std::list<std::string> xs);


	//auto  list_transform( list<auto> _in, function< Q(auto) > func ) ;



}

#endif