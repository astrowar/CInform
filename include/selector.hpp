
#pragma once
#ifndef SELECTORPP_HPP
#define SELECTORPP_HPP 

#include "PreCodeGenerate.hpp"
#include "NSource.hpp"
#include <cctype>
#include <memory>

using namespace std;

namespace CInform
{

	namespace CodeParser
	{

		class SelectorItem;
		using HSelectorItem = std::shared_ptr<SelectorItem>;

		class SelectorItem
		{
			HSelectorItem _next;
		public:
			string target;
			SelectorItem* add( HSelectorItem  next );
			HSelectorItem next();
			SelectorItem( string _target ) : target( _target ), _next( nullptr ) {}
			virtual ~SelectorItem() {}
		 
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
			HSelectorItem sel1;
			HSelectorItem sel2;
			SelectorOr( string _target, HSelectorItem _sel1, HSelectorItem _sel2 ) : SelectorItem( _target ), sel1( _sel1 ), sel2( _sel2 ) {}
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