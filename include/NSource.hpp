#pragma once
#ifndef NSOURCE_HPP
#define NSOURCE_HPP 


#include "CodeParser.hpp"
#include "cinform.hpp"
#include "Grammar.hpp"
#include "PreCodeGenerate.hpp"
#include "pmatch_code.hpp"

#include <string>
#include <iostream>

using namespace std;

namespace CInform
{
	using namespace Match;

	namespace CodeParser
	{
		class NSource : public  CInform::Match::MNoum
		{
		public:
			CInform::HSourceBlock  item;
			virtual string repr() {
				return item->repr();
			}
			virtual  bool isMatch(const string& s)
			{				
				return CInform::Match::isSameCodeString(s, item);
			}
			NSource(CInform::HSourceBlock  _item) :item(_item) {};
		};
	}
}

#endif