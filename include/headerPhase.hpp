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
		SReference get_instance_reference( ParserStore *pstore, string instname, PreCodeGenerate** error );
		SReference get_kind_reference( ParserStore *pstore, string kindname, PreCodeGenerate** error );
		SReference get_verb_reference( ParserStore *pstore, string verb, PreCodeGenerate** error );
		list<SReference> get_adj_instance_reference( ParserStore *pstore, string adj_instname, PreCodeGenerate** error );
		SReference get_instance_reference( ParserStore *pstore, string instname, PreCodeGenerate** error );

		class HeaderPhaseEntry
		{
		public:
			HeaderPhaseEntry(  )  {}
			HeaderPhaseEntry(string _header ):header( _header){}
			string header;
			list<HSelectorItem > args;
			bool empty() { return header.empty() && args.empty(); }
			HeaderPhaseEntry	operator+( const HeaderPhaseEntry  & other );


		};

		class HeaderPhaseEntrySelector
		{
		public:
			string header;
			list<pair<string, string> > var_selector;
		};


		class SufixGenerator
		{
			std::string base;
			int i = 1;
		public:
			SufixGenerator( std::string _base ):base(_base) {}
			SufixGenerator( std::string _base ,int _i) :base( _base ),i(_i) {}
			string next_var() {
				return base + std::to_string( i++ ); 				
			 
			 }
		};
	}
}

#endif