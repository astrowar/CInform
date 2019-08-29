#pragma once
#ifndef HEADERPH_HPP
#define HEADERPH_HPP 


#include "PreCodeGenerate.hpp"
#include "NSource.hpp"
#include <cctype>

#include "selector.hpp"

using namespace std;

namespace CInform
{
	using namespace Match;

	namespace CodeParser
	{

		string removeArticle( ParserStore *pstore, string compound );
		string get_instance_reference( ParserStore *pstore, string instname, PreCodeGenerate** error );
		string get_kind_reference( ParserStore *pstore, string kindname, PreCodeGenerate** error );
		string get_verb_reference( ParserStore *pstore, string verb, PreCodeGenerate** error );
		list<string> get_adj_instance_reference( ParserStore *pstore, string adj_instname, PreCodeGenerate** error );
		string get_instance_reference( ParserStore *pstore, string instname, PreCodeGenerate** error );

		class HeaderPhaseEntry
		{
		public:
			string header;
			list<SelectorItem* > args;

		};

		class HeaderPhaseEntrySelector
		{
		public:
			string header;
			list<pair<string, string> > var_selector;
		};
	}
}

#endif