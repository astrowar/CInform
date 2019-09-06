#pragma once
#ifndef  CODECONSTTREE_HPP
#define  CODECONSTTREE_HPP

#include "CodeTree.hpp"
#include <list>
#include <string>
using namespace std;

namespace CInform
{
	namespace CodeTree
	{
		class SourceControl
		{
		public:

			SourceControl *parentScope;

			list< Statement* > main;
			list< std::pair<std::string, TypeNode* > > name_types;
			list<  NamedDeclaration* > name_decl;
			

			
			NamedDeclaration *  getDeclaration( string name );
			void addClass( ClassDeclaration *cc );

			ClassDeclaration* getClass( std::string name );						
			ClassDeclaration* createClassDeclaration( Identifier* name, HeritageClause* base );
			Identifier*  createIdentifier( string _name );
			HeritageClause* createHeritageClause( std::list<ExpressionWithTypeArguments*>  _types );
			ExpressionWithTypeArguments* createExpressionWithTypeArguments( Expression* _expression, HeritageClause*  _parent );
			PropertyDeclaration* createProperty( Identifier* name, TypeNode* type, Expression* initializer );
			TypeReferenceNode* createTypeReferenceNode( Identifier* name );
			StringLiteral* createStringLiteral( string value );

			VariableDeclaration* createVariableDeclaration( Identifier* name, TypeNode* type, Expression* initializer );
			NewExpression* createNew( Expression* expression, list< Expression*> argumentsArray );
			VariableStatement* createVariableStatement( VariableDeclaration  *v );
			void addGlobalVariableDeclaration( VariableDeclaration  *v );
		};

	}
}

#endif
