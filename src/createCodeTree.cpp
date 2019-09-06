#include "createCodeTree.hpp"
namespace CInform
{
	namespace CodeTree
	{
		NamedDeclaration * SourceControl::getDeclaration( string name )
		{
			for (auto e : name_decl)
			{
				if (e->name == name)
				{
					return e;
				}
			}
			return nullptr;
		}
		void CodeTree::SourceControl::addClass( ClassDeclaration * cc )
		{
			main.push_back( cc );
			name_decl.push_back(   cc  );
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

		VariableDeclaration * SourceControl::createVariableDeclaration( Identifier * name, TypeNode * type, Expression * initializer )
		{
			VariableDeclaration *v = new  VariableDeclaration();
			v->name = name->originalKeywordKind;
			v->initializer = initializer;
			v->type = type;

			return v;
		}

		NewExpression * SourceControl::createNew( Expression * expression, list<Expression*> argumentsArray )
		{
			auto n = new  NewExpression();
			n->expr = expression;
			n->arguments = argumentsArray;
			return n;
		}

		VariableStatement * SourceControl::createVariableStatement( VariableDeclaration * v )
		{
			auto vv = new VariableStatement();
			vv->var_decl = v;
			return vv;
		}

		void SourceControl::addGlobalVariableDeclaration( VariableDeclaration * v )
		{
			name_decl.push_back( v );
			main.push_back( createVariableStatement( v ) );
		}

		 
	}
}