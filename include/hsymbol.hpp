
#pragma once

#ifndef  HSYMB_HPP
#define  HSYMB_HPP

#include "sreference.hpp"
#include <string>

namespace CInform
{
	//simbolo é a representação em palavras de um tipo do CInform

	enum  SYMBTYPE { SKIND, SVALUE, SINSTANCE, SVAR ,SRELATION, SVERB , SKINDVAR };

	class Symbol
	{
	public:
		SReference ref;   //nome de referencia mangled 
		Symbol( SReference  _ref ) :ref( _ref ) {}
		virtual  std::string getName() = 0;
		virtual  SYMBTYPE getType() = 0; 
	};

	class Kind : public Symbol
	{
	public:	 
		std::string name;
		SReference baseKind;
		Kind( SReference  _ref, std::string  _name ) :Symbol( _ref ), name(_name) , baseKind(""){}
		Kind( SReference  _ref, SReference _baseKind, std::string  _name ) :Symbol( _ref ), name( _name ) , baseKind(_baseKind ) {}

		
		virtual std::string getName() override;

		
		virtual SYMBTYPE getType() override;
	};

	class KindCompose : public Kind
	{
	public:		 
		SReference innerKind;
		KindCompose( SReference  _ref, std::string  _name  , SReference _baseKind , SReference _innerKind  ) :Kind( _ref  ,_name ),   innerKind( _innerKind ){ }
	};

	class KindComposeDual : public Kind
	{
	public:
		SReference innerKind_A;
		SReference innerKind_B;
		KindComposeDual( SReference  _ref, std::string  _name, SReference _baseKind, SReference _innerKind_A, SReference _innerKind_B ) :Kind( _ref, _name ), innerKind_A( _innerKind_A ), innerKind_B( _innerKind_B ) { }

		
		virtual SYMBTYPE getType() override;
	};

	class Instance :public  Symbol
	{
	public:
		std::string name;
		SReference baseKind;
		Instance( SReference  _ref, SReference _baseKind ,  std::string  _name) :Symbol( _ref  ), baseKind( _baseKind ), name( _name ) {} 
		virtual std::string getName() override; 
		virtual SYMBTYPE getType() override;
	};

	class RelationKind : public KindCompose
	{
	public:
		RelationKind( SReference  _ref, std::string  _name, SReference _from, SReference _to ) :KindCompose( _ref, _name  , _from,_to )  { }

	 
		virtual SYMBTYPE getType() override;
	};


 

	//=======================================================



	class  Value  : public Symbol
	{
	public:
		SReference base;
		Value( SReference  _ref  , SReference _base ) :Symbol( _ref ), base(_base) { }
		virtual SYMBTYPE getType() override;
	};


	class Number : public Value
	{
	public:
	 
		Number( SReference  _ref  ) :Value( _ref, SReference("number") )  { }
	};


 



	class Text : public Value
	{
	public:
		std::string text;
		Text( SReference  _ref , std::string _text ) :Value( _ref , SReference("text") ), text(_text) { }
	};



	class Boolean :public  Value
	{
	public:
		bool value;
		Boolean( SReference  _ref, bool _value ) :Value( _ref , SReference("boolean")), value( _value ) { }
	};


	class Verb  : public Value
	{
	public:
		std::string name;
		Verb( SReference  _ref, std::string  _name ) :Value( _ref, SReference("verb") ),name(_name) { }

		// Herdado por meio de Value
		virtual std::string getName() override;
	};

	//VARIABLE

	class Variable : public Symbol
	{
	public:
		std::string name;
		SReference valuekind;
		Variable( SReference  _ref,SReference _valuekind, std::string _name   ) :Symbol( _ref ), name( _name ), valuekind( _valuekind ) { }
		// Herdado por meio de Symbol
		virtual std::string getName() override;
		virtual SYMBTYPE getType() override;
	};

	
	class KindProperty : public Symbol
	{
	public:
		std::string name;
		SReference valuekind;
		KindProperty( SReference  _ref, SReference _valuekind, std::string _name ) :Symbol( _ref ), name( _name ), valuekind( _valuekind ) { }
		// Herdado por meio de Symbol
		virtual std::string getName() override;
		virtual SYMBTYPE getType() override;
	};

}
#endif