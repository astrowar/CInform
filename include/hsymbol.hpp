
#pragma once

#ifndef  HSYMB_HPP
#define  HSYMB_HPP

#include <string>

namespace CInform
{
	//simbolo é a representação em palavras de um tipo do CInform

	enum  SYMBTYPE { SKIND, SVALUE, SINSTANCE, SVAR ,SRELATION, SVERB  };

	class Symbol
	{
	public:
		std::string ref;   //nome de referencia mangled 
		Symbol( std::string  _ref ) :ref( _ref ) {} 
		virtual  std::string getName() = 0;
		virtual  SYMBTYPE getType() = 0; 
	};

	class Kind : public Symbol
	{
	public:	 
		std::string name;
		std::string baseKind;
		Kind( std::string  _ref, std::string  _name ) :Symbol( _ref ), name(_name) , baseKind(nullptr){}
		Kind( std::string  _ref, std::string _baseKind, std::string  _name ) :Symbol( _ref ), name( _name ) , baseKind(_baseKind ) {}

		
		virtual std::string getName() override;

		
		virtual SYMBTYPE getType() override;
	};

	class KindCompose : public Kind
	{
	public:		 
		std::string innerKind;
		KindCompose( std::string  _ref, std::string  _name  , std::string _baseKind , std::string _innerKind  ) :Kind( _ref  ,_name ),   innerKind( _innerKind ){ }
	};

	class KindComposeDual : public Kind
	{
	public:
		std::string innerKind_A;
		std::string innerKind_B;
		KindComposeDual( std::string  _ref, std::string  _name, std::string _baseKind, std::string _innerKind_A, std::string _innerKind_B ) :Kind( _ref, _name ), innerKind_A( _innerKind_A ), innerKind_B( _innerKind_B ) { }

		
		virtual SYMBTYPE getType() override;
	};

	class Instance :public  Symbol
	{
	public:
		std::string name;
		std::string baseKind;
		Instance( std::string  _ref,  std::string _baseKind ,  std::string  _name) :Symbol( _ref  ), baseKind( _baseKind ), name( _name ) {}

		
		virtual std::string getName() override;

		
		virtual SYMBTYPE getType() override;
	};

	class RelationKind : public KindCompose
	{
	public:
		RelationKind( std::string  _ref, std::string  _name, std::string _from, std::string _to ) :KindCompose( _ref, _name  , _from,_to )  { }

	 
		virtual SYMBTYPE getType() override;
	};


 

	//=======================================================



	class  Value  : public Symbol
	{
	public:
		std::string base;
		Value( std::string  _ref  , std::string _base ) :Symbol( _ref ), base(_base) { }
		virtual SYMBTYPE getType() override;
	};


	class Number : public Value
	{
	public:
	 
		Number( std::string  _ref  ) :Value( _ref, "number" )  { }
	};


 



	class Text : public Value
	{
	public:
		std::string text;
		Text( std::string  _ref , std::string _text ) :Value( _ref ,  "text" ), text(_text) { }
	};



	class Boolean :public  Value
	{
	public:
		bool value;
		Boolean( std::string  _ref, bool _value ) :Value( _ref ,"boolean"), value( _value ) { }
	};


	class Verb  : public Value
	{
	public:
		std::string name;
		Verb( std::string  _ref, std::string  _name ) :Value( _ref, "verb" ),name(_name) { } 

		// Herdado por meio de Value
		virtual std::string getName() override;
	};

	//VARIABLE

	class Variable : public Symbol
	{
	public:
		std::string name;
		Value *value;
		Variable( std::string  _ref, std::string _name, Value* _value ) :Symbol( _ref ), name( _name ),value( _value ) { }
	};

	
 

}
#endif