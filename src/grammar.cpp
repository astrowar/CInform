#include "Grammar.hpp"
#include <algorithm>
#include <cctype>
#include <string>


namespace CInform
{
	 

	bool GrammarContextCode::isReservedWord( string x )
	{
		return false;
	}


		bool GrammarContextEnglish::isReservedWord( string x )
	{
		static std::list<string> reserved = { "an","a","the","with","to","as","are","is","be", "in","or" };

		std::transform( x.begin(), x.end(), x.begin(), []( unsigned char c ) { return std::tolower( c ); } );

		return (std::find( reserved.begin(), reserved.end(), x ) != reserved.end());
	}

}