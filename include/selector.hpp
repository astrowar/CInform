
#pragma once
#ifndef SELECTORPP_HPP
#define SELECTORPP_HPP 

#include "PreCodeGenerate.hpp"
#include "NSource.hpp"
#include <cctype>
using namespace std;

namespace CInform
{

	namespace CodeParser
	{
		class SelectorItem
		{
		public:
			string target;
			SelectorItem* next;
			SelectorItem( string _target ) : target( _target ), next( nullptr ) {}
			virtual ~SelectorItem() {}
		};

		class SelectorKind : public  SelectorItem
		{
		public:
			string kindRef;
			SelectorKind( string _target, string _kindRef ) : SelectorItem( _target ), kindRef( _kindRef ) {}
		};

		class SelectorInstance :public SelectorItem
		{
		public:
			string instRef;
		};

		class SelectorAdjetive :public SelectorItem
		{
		public:
			string adjetive;
			SelectorAdjetive( string _target, string _adjetive ) : SelectorItem( _target ), adjetive( _adjetive ) {}
		};

		class SelectorOr :public SelectorItem
		{
		public:
			SelectorItem* sel1;
			SelectorItem* sel2;
			SelectorOr( string _target, SelectorItem* _sel1, SelectorItem* _sel2 ) : SelectorItem( _target ), sel1( _sel1 ), sel2( _sel2 ) {}
		};

		class SelectorUnify :public SelectorItem
		{
		public:
			string other;
			SelectorUnify( string _target, string _other ) : SelectorItem( _target ), other( _other ) {}
		};



	}
}

#endif