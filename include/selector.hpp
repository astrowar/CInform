
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
			SelectorItem* _next;
		public:
			string target;
			SelectorItem* add( SelectorItem*  next );
			SelectorItem* next();
			SelectorItem( string _target ) : target( _target ), _next( nullptr ) {}
			virtual ~SelectorItem() {}
			SelectorItem*  operator<< ( SelectorItem*  __next );
		};

		class SelectorKind : public  SelectorItem
		{
		public:
			SReference kindRef;
			SelectorKind( string _target, SReference _kindRef ) : SelectorItem( _target ), kindRef( _kindRef ) {}
		};

		class SelectorInstance :public SelectorItem
		{
		public:
			SReference instRef;
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
			SelectorUnify( string _target, SReference _other ) : SelectorItem( _target ), other( _other.repr() ) {}
			SelectorUnify( SReference _target, string _other ) : SelectorItem( _target.repr() ), other( _other  ) {}
		};



	}
}

#endif