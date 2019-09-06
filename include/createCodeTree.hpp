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

			list< TypeNode* > types;

			TypeNode* getType( string name );
			void addClass( ClassDeclaration *cc );

			ClassDeclaration* getClass( std::string name );						
			ClassDeclaration* createClassDeclaration( Identifier* name, HeritageClause* base );
			Identifier*  createIdentifier( string _name );
			HeritageClause* createHeritageClause( std::list<ExpressionWithTypeArguments*>  _types );
			ExpressionWithTypeArguments* createExpressionWithTypeArguments( Expression* _expression, HeritageClause*  _parent );
			PropertyDeclaration* createProperty( Identifier* name, TypeNode* type, Expression* initializer );
			TypeReferenceNode* createTypeReferenceNode( Identifier* name );
			StringLiteral* createStringLiteral( string value );
		};

	}
}

#endif
