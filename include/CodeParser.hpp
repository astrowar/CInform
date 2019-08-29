#pragma once
#ifndef CODEPARSER_HPP
#define CODEPARSER_HPP 
#include "Grammar.hpp"
#include "parseEntry.hpp"
#include  "preprocess.hpp" 
#include "precodegenerate.hpp"
#include <string>
#include <list>


using namespace std;

namespace CInform
{
	namespace CodeParser
	{
		 
		std::list<std::string> emiteInter(SParagraph* code);
		list<string> emiteAsm(list<PreCodeGenerate*> itList);
	}
}


#endif