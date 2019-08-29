#pragma once
#ifndef GRAMMAR_EN_HPP
#define GRAMMAR_EN_HPP

#include <list>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <string>

using namespace std;
namespace CInform
{
	class GrammarContext
	{
	public:
		virtual bool  isReservedWord(string x) = 0;
		 
	};


	class GrammarContextEnglish : public GrammarContext
	{
	public:
		virtual	bool  isReservedWord(string x);
	   
	};


	class GrammarContextCode : public GrammarContext
	{
	public:
		virtual	bool  isReservedWord( string x );

	};
}


#endif