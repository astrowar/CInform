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

	bool GrammarContextCode::isPreposition( string x )
	{
		return false;
	}

	bool GrammarContextCode::isNoumReserved( string x )
	{
		return false;
	}


		bool GrammarContextEnglish::isReservedWord( string x )
	{
		static std::list<string> reserved = { "an","a","the","with","to","as","are","is","be", "in","or" };

		std::transform( x.begin(), x.end(), x.begin(), []( unsigned char c ) { return std::tolower( c ); } );

		return (std::find( reserved.begin(), reserved.end(), x ) != reserved.end());
	}

		bool GrammarContextEnglish::isPreposition( string x )
		{
			static std::list<string> pp = {"of", "with", "at", "from", "into", "during", "including", "until", "against", "among", "throughout", "despite", "towards", 
                                          "upon", "concerning", "to", "in", "for", "on", "by", "about", "like", "through", "over", "before", "between", "after", "since", "without", 
                                           "under", "within", "along", "following", "across", "behind", "beyond", "plus", "except", "but", "up", "out", "around", "down", "off", "above", "near" };

			std::transform( x.begin(), x.end(), x.begin(), []( unsigned char c ) { return std::tolower( c ); } );
			return (std::find( pp.begin(), pp.end(), x ) != pp.end());
		
		}

		bool GrammarContextEnglish::isNoumReserved( string x )
		{
			static std::list<string> reserved = { "name","list","kind","value" };
			std::transform( x.begin(), x.end(), x.begin(), []( unsigned char c ) { return std::tolower( c ); } );
			return (std::find( reserved.begin(), reserved.end(), x ) != reserved.end());
		}

}