#include "createCodeTree.hpp"
namespace CInform
{
	namespace CodeTree
	{
		TypeNode * SourceControl::getType( string name )
		{
			 
		}
		void CodeTree::SourceControl::addClass( ClassDeclaration * cc )
		{

		}

		ClassDeclaration * CodeTree::SourceControl::getClass( std::string name )
		{
			return nullptr;
		}

		ClassDeclaration * SourceControl::createClassDeclaration( Identifier * name, HeritageClause * base )
		{
			auto c = new ClassDeclaration();
			c->name = name->originalKeywordKind;
			c->base = base;
			return c;


		}

		Identifier* SourceControl::createIdentifier( string _name )
		{
			return new Identifier(_name);
		}

		HeritageClause * SourceControl::createHeritageClause( std::list<ExpressionWithTypeArguments*> _types )
		{
			auto h = new HeritageClause( _types );
			return h;
		}

		ExpressionWithTypeArguments * SourceControl::createExpressionWithTypeArguments( Expression * _expression, HeritageClause * _parent )
		{
			auto n = new ExpressionWithTypeArguments();
			n->parent = _parent;
			n->expression = _expression;
			return n;
		}

		PropertyDeclaration * SourceControl::createProperty( Identifier * name, TypeNode * type, Expression * initializer )
		{
			auto p = new  PropertyDeclaration();
			p->initializer = initializer;
			p->name = name->originalKeywordKind;
			p->type = type;
			return p;
		}

		TypeReferenceNode * SourceControl::createTypeReferenceNode( Identifier * name )
		{
			auto t = new TypeReferenceNode();
			t->typeName = name->originalKeywordKind;
			return t;
		}

		StringLiteral * SourceControl::createStringLiteral( string value )
		{
			auto s= new  StringLiteral();
			s->str = value;
			return s;
		}

		 
	}
}