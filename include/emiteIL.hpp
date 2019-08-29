#pragma once
#ifndef HEMITEIL_HPP
#define HEMITEIL_HPP 

#include <precodegenerate.hpp>
#include <selector.hpp>
#include <list>

using namespace std;

namespace CInform
{

	namespace CodeParser
	{
		std::list<PreCodeGenerateIL*> emiteILCode_i( ParserStore *pstore, SelectorItem  *sel );
		std::list<PreCodeGenerateIL*> emiteILCode( ParserStore *pstore, SelectorItem  *sel );

	}
}

#endif