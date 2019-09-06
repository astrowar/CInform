// https://ts-ast-viewer.com

#pragma once
#ifndef  CODETREE_HPP
#define  CODETREE_HPP

#include <list>
#include <string>
namespace CInform
{
	namespace CodeTree
	{
		
		
		class Statement
		{
		public:
			Statement() {}
		};


		class vType
		{
			//tipo do valor ..nao do node que ele ele simboliza
			// Literals.. sao dericvados do vtype e possuem um valor 
		};

		class 	TypeNode
		{
		public:
		};

		class 	TypeKeyword :TypeNode
		{
		public:
			std::string keyword;
		};

		class 	TypeReference :TypeNode
		{
		public:
			std::string typeName;
		};

		class 	ArrayTypeNode :TypeNode
		{
		public:
			TypeNode* elementType;
		};

	 

		class Declaration
		{

		};
		class NamedDeclaration : public Declaration
		{
		public:
			std::string name;
		};


		class DeclarationStatement :public NamedDeclaration, public Statement
		{
			 
		};
		 

		class TypeParameterDeclaration :public NamedDeclaration
		{
		public:			 
			TypeParameterDeclaration* super; //super type from
		};

		class TypeReferenceNode :public TypeNode
		{
		public:
			std::string typeName;
		};


		class VariableDeclaration : public NamedDeclaration
		{
		public:
		 
			TypeParameterDeclaration *type;
			Expression initializer;
		};

		class ParameterVarDecl
		{
		public:
			std::string paramName;
			TypeParameterDeclaration paramType;
		};
		class VariableStatement :public Statement
		{
		public:
			VariableDeclaration var_decl;
		};


		class Expression
		{

		};


		class PrimaryExpression : public Expression
		{

		};


		 
		class Block  
		{
		std::list<Statement> statements;
		};


		class LiteralTypeNode :public TypeNode
		{

		};
 

		class LiteralExpression : public PrimaryExpression
		{
 
		};


		class StringLiteral : public LiteralExpression
		{
		public:
			std::string str;
		};
	 

		class FunctionDeclaration : public DeclarationStatement
		{
 
		public:
			std::list<ParameterVarDecl> parameters;
			TypeParameterDeclaration returnType;
			Statement * body;
		};

		


		class EnumItem   
		{
		public:
			std::string name;
		};


 


		class ClassElement  :public NamedDeclaration
		{
		public:
			 
			DeclarationStatement* parent;
		};


		class PropertyDeclaration : public ClassElement
		{			 
		public:
			TypeNode* type;
			Expression* initializer;

		};

		class GetAccessorDeclaration : public ClassElement, FunctionDeclaration
		{
		 
		};

		class SetAccessorDeclaration : public ClassElement, FunctionDeclaration
		{

		};
		class MethodDeclaration : public ClassElement, FunctionDeclaration
		{

		};
 

		class ExpressionWithTypeArguments
		{
		public:
			HeritageClause*  parent;
			Expression* expression;
			ExpressionWithTypeArguments( Expression* _expression , HeritageClause*  _parent ):parent(_parent), expression(_expression){}

		};

		class HeritageClause
		{
		public:
			std::list<ExpressionWithTypeArguments*>  types;
			HeritageClause( std::list<ExpressionWithTypeArguments*>  _types ) : types( _types ) {}
		};
		 
	 
		class ClassDeclaration : public NamedDeclaration
		{
		public: 
		 
			std::list<ClassElement*> members;
			HeritageClause* base  ;
		};
		 
 


		//Expressions
		class FunctionBody : public  Block
		{
			 
		};

 
		class FunctionExpression : public  PrimaryExpression
		{
			std::string name;
			FunctionBody body;
		};
	 

		class NewExpression : public PrimaryExpression, Declaration
		{
			Expression expr; //funcao chamadora ....
			std::list<Expression>  arguments;
		};

		class Identifier : public PrimaryExpression, Declaration
		{
		public:
			std::string  originalKeywordKind;
			Identifier( std::string _originalKeywordKind ) :originalKeywordKind( _originalKeywordKind ) {}

		};

		class ExpressionStatement :public Statement
		{
			Expression expr;
		};

	}


}

#endif