
#include "PreCodeGenerate.hpp"
#include <Grammar.hpp>
#include <iostream>

using namespace std;

namespace CInform
{
	using namespace Match;

	namespace CodeParser
	{
		void replaceAll( string &s, const string &search, const string &replace );

		PreCodeGenerateError::PreCodeGenerateError(string _errorMessage) :errorMessage(_errorMessage) {}
		string PreCodeGenerateError::repr() { return "ERRO: " + errorMessage; }
		PreCodeGenerateIL::PreCodeGenerateIL() {}
		PreCodeGenerateIL::PreCodeGenerateIL(string _verb, string _noum, string _x) : verb(_verb), noum(_noum), x(_x) 
		{
			 
		}
		PreCodeGenerateIL::PreCodeGenerateIL(string _verb, string _noum, string _x, string _y) : verb(_verb), noum(_noum), x(_x), y(_y) 
		{
			 
		
		}
		PreCodeGenerateIL::PreCodeGenerateIL(string _verb, string _noum, string _obj, string _prop, string _kind) : verb(_verb), noum(_noum), x(_obj), y(_prop), z(_kind) 
		{
			 
		}

		string PreCodeGenerateIL::repr()
		{
			string  ss = verb + " " + noum;
			if (x.find(' ') != string::npos)
			{
				ss += " (" + x + ")";
			}
			else
			{
				ss += " " + x;
			}

			if (y.find(' ') != string::npos)
			{
				ss += " (" + y + ") ";
			}
			else
			{
				ss += " " + y;
			}


			if (z.find(' ') != string::npos)
			{
				ss += " (" + z + ")";
			}
			else
			{
				ss += " " + z;
			}
			return ss;
		}
		ParserStore::ParserStore(GrammarContext * gtx) :_grammar(gtx)
		{
			this->codeTree = new CodeTree::SourceControl();
			this->push_state();
		}
		GrammarContext * ParserStore::grammar()
		{
			return _grammar;
		}
		void ParserStore::push_state()
		{
			symbol_stack.push_front(ParserStoreSymbolList());
		}
		std::string ParserStore::next_temp()
		{
			int ic = temp_id++;
			return std::to_string( ic );
		}

		bool ParserStore::isKind(string nameOfKind)
		{
			if (isSameNoum( nameOfKind, "kind")) return true;
			for (auto &e : symbol_stack)
			{
				if (e.isKind( nameOfKind )) return true;
			}
			 
			return false;
		}

 

		bool ParserStore::isImplicityVar( std::string name )
		{
			if (name.size() > 0)
			{
				if (name[0] =='$') return true;
				if (name.size() < 3)
				{
					if (toupper( name[0] ) == name[0]) return true;
				}
			}

			return false;
		}

		bool ParserStore::isInstance(string name)
		{
			for (auto &e : symbol_stack)
			{
				if (e.isInstance(name)) return true;
			}
			return false;
		}

		bool ParserStore::isArticle( std::string name )
		{
			if (isSameNoum( name, "a" )) return true;
			if (isSameNoum( name, "an" )) return true;
			if (isSameNoum( name, "the" )) return true;
			if (isSameNoum( name, "some" )) return true;
			return false;

		}


		bool ParserStore::isValue(string name)
		{			if (isSameNoum(name, "value")) return true;
			for (auto &e : symbol_stack)
			{
				if (e.isValue(name)) return true;
			}
			 
			return false;
		}

		bool ParserStore::isComposeType(string mangled_name)
		{
			auto demangle_1 = this->demangleg( mangled_name );
			if (demangle_1.size() > 1) return true;
			return false;
		}


		std::list<std::string> demangleg_compose( std::string x )
		{
			

			size_t n = x.size();
			for (size_t i = 0; i < n; ++i)
			{
				if (x[i] == '<')
				{   std::list<std::string> args;
					string base = x.substr( 0, i );
					int  p = 0;
					size_t pivot = i+1;
					for (size_t j = i + 1; j < n - 1; ++j)
					{
						if (x[j] == '<') p++;
						if (x[j] == '>') p--;
						if (x[j] == ',' && p == 0)
						{
							//achei uma divisao no nivel zero ...
							args.push_back( x.substr( pivot, j - pivot ) );
							pivot = j;
						}
					}
					//adiciona o remainder
					args.push_back( x.substr( pivot, (n - 1) - pivot ) );
					args.push_front( base );
					return args;
				}
			}
			return std::list<std::string>();
		}

		std::list<std::string> ParserStore::demangleg( std::string x )
		{
			auto ls = demangleg_compose( x );
			if (ls.empty())
			{
				return { x };
			}
			else
			{
				return ls;
			}
			return std::list<std::string>();
		}

		bool ParserStore::isNoumReserved( std::string x )
		{
			return  _grammar->isNoumReserved( x );
		}

		bool ParserStore::isPreposition( std::string x )
		{
			return  _grammar->isPreposition( x );

		}

		bool ParserStore::isPronoum( std::string x )
		{
			return  _grammar->isPronoum( x );

		}

		bool ParserStore::isPropertyOf( std::string prop_arroba_obj, SReference & obj, SReference & internal_prop_name )
		{
			auto i = prop_arroba_obj.find( "@" );
			if (i != string::npos)
			{
				obj = getReference( prop_arroba_obj.substr( i + 1, prop_arroba_obj.size() - i - 1 ) );
				if (obj.empty()==false   ) 
				{
					internal_prop_name = getReference( prop_arroba_obj.substr( 0, i ));
					return true;
				}
			}
			return false;
		}


		bool ParserStore::isVerb( std::string x )
		{
			if (isPreposition( x )) return false;
			if (isNoumReserved( x ))return false;
			if (x.find( ' ' ) != string::npos) return false;
			for (auto q : x)
			{
				if (isalpha( q ) == false) return false;
			}
			return true;

		}

		SReference ParserStore::getReference(string name)
		{
			for (auto &e : symbol_stack)
			{
				SReference xx = e.getReference(name);

				if (!xx.empty())
				{
					return xx;
				}
			}

			//eh um tipo composto ?
			if (name.find( "<" ) != string::npos)
			{
				auto ls = demangleg_compose( name );
				if (ls.size() < 2)
				{
					return SReference();
				}
				SReference ref_base;
				SReference ref_sub1;
				SReference ref_sub2;
				
				
				ref_base =   ls.front()  ;
				ls.pop_front();
				ref_sub1 = getReference( ls.front() ); 
				ls.pop_front();	

				if (ls.empty() == false) { ref_sub2 = getReference( ls.front() ); ls.pop_front(); }

				if (ref_sub2.empty())
				{
					return SReference(this->mangleg( ref_base.repr() , ref_sub1.repr() ));
				}				
				
				return 	SReference(this->mangleg( ref_base.repr() , ref_sub1.repr() , ref_sub2.repr() ));

			}



			//eh uma property ?
			if (name.find( "@" ) != string::npos)
			{
				auto i = name.find( "@" );
				//auto hprop = getReference( name.substr( 0, i ) );
				auto hobj = getReference( name.substr( i+1, name.size()-i-1 ) );
				if (hobj.empty() == false)
				{
					auto hprop = getReference( name.substr( 0, i ) );
					return SReference(hprop.repr()+"@"+hobj.repr());
				}
			}

			{
				string tmp = "";
				if (getLocalVar( name, tmp )) return getReference( tmp );
			}

			return SReference();
		}

		SReference ParserStore::getKindOf( SReference ref )
		{
			 
			for (auto &e : symbol_stack)
			{
				auto r = e.getKindOf( ref );
				if (r.empty() ==false ) return r;
			}
			return {};
		}

		SReference ParserStore::getVerbReference( string name )
		{
			

			//if (isSameNoum( name, "is" )) return "is";
			for (auto &e : symbol_stack)
			{
				for (auto &v :  e.ListSymbols())
				{
					if (auto  vb = dynamic_cast<Verb*>(v))
					{
						if (isSameNoum( vb->getName(), name )) return vb->ref;
					}
				}
				
			}

			return SReference();

		}


		bool ParserStore::isSymbol(string name)
		{
			if (isSameNoum(name, "value")) return true;


			for (auto &e : symbol_stack)
			{
				if (e.isSymbol(name)) return true;
			}
			{
				string tmp;
				if (this->getLocalVar( name, tmp )) return isSymbol( tmp );
			}
			return false;
		}

		bool ParserStore::addVerb( string name )
		{
			if (name .empty() )
			{
				throw "unable to set this";
			}
			auto ref = "verb:" + this->mangleg( name );

			CInform::Verb  *vv = new CInform::Verb( ref,  name );
			symbol_stack.front().addSymbol(  vv );
			return true;
		}



		bool ParserStore::addKind(string name, SReference kindbase)
		{
			if (kindbase.repr()  .empty() )
			{
				throw "unable to set this";
			}

			//auto k = getReference( kindbase );
			//if (k .empty() )
			//{
			//	throw "missing kind definition";
			//}

			auto ref = SReference(  this->mangleg( name ));

			CInform::Kind *kn = new CInform::Kind( ref, kindbase, name );

			symbol_stack.front().addSymbol( kn);
			return true;
		}

		bool ParserStore::addInstance(string name, SReference kindbase)
		{
			auto k =  ( kindbase );
			if (k .empty())
			{
				throw "missing kind definition";
			}
			if (name.find( '(' ) != string::npos)
			{
				throw "noum name error";
			}

			CInform::Instance *inst = new CInform::Instance( this->mangleg( name ), k, name );
			symbol_stack.front().addSymbol( inst);
			return true;
		}

		bool ParserStore::addProperty( SReference objectReceiver, std::string prop_name, SReference prop_kind )
		{
			 
			if (prop_kind.empty())
			{
				throw "missing kind definition";
			}
			if (prop_name.find( '(' ) != string::npos)
			{
				throw "prop name name error";
			}
			 
			auto ref = SReference( this->mangleg( prop_name )  );

			CInform::KindProperty *gvar = new CInform::KindProperty( ref, prop_kind, prop_name );
			symbol_stack.front().addSymbol( gvar );
			return true; 
		}

		bool ParserStore::addGlobalVariable( std::string name, SReference kindbase )
		{
			auto k = (kindbase);
			if (k.empty())
			{
				throw "missing kind definition";
			}
			if (name.find( '(' ) != string::npos)
			{
				throw "noum name error";
			}

			CInform::Variable *gvar = new CInform::Variable( this->mangleg( name ) , kindbase,name  );
			symbol_stack.front().addSymbol( gvar );
			return true;
		}



		bool ParserStore::add( Symbol * s )
		{
			this->symbol_stack.front().addSymbol(s);
			return true;
			
		}

		bool ParserStore::setLocalVar( string var, string value )
		{
			string tmp;
			for (auto &e : symbol_stack)
			{
				if (e.getLocalVar(var,tmp ) )
				{
					e.setLocalVar( var, value );
					return true;
				}
			}
			symbol_stack.front().setLocalVar( var, value );
			return true;
		}

		bool ParserStore::getLocalVar( string var, string & value )
		{
			string tmp;
			for (auto &e : symbol_stack)
			{
				if (e.getLocalVar( var, tmp ))
				{
					value = tmp;
					return true;
				}
			}
			return false;
		}

		std::string ParserStore::mangleg( std::string x )
		{  
			while (x.front() == ' ' || x.front() == '\t') x = x.substr( 1, x.size() - 1 );
			while (x.back() == ' ' || x.front() == '\t') x.pop_back();
			replaceAll( x, " ", "_" );

			if (toupper( x[0] ) == x[0])
			{
				if (x.size() <= 3)
				{
					if (x.back() == 's') x.pop_back();
				}
			  }
			  
			  return x;
		}

		//std::string ParserStore::mangleg( std::string x, std::string sub )
		//{
		//	if (sub .empty() )
		//		throw "error on sub type";
		//	return mangleg( x ) + "<" + mangleg( sub ) +">";
		//}

		//std::string ParserStore::mangleg( std::string x, std::string sub )
		//{
		//	if (sub .empty() )
		//		throw "error on sub type";
		//	return mangleg( x ) + "<" + mangleg( sub ) + ">";
		//}

		std::string ParserStore::mangleg( std::string x, SReference sub )
		{
			if (sub.empty())
				throw "error on sub type";
			return mangleg( x ) + "<" +  ( sub.repr() ) + ">";
		}

		std::string ParserStore::mangleg( std::string x, SReference sub1, SReference sub2 )
		{
			if (sub1.empty())throw "error on sub type 1";
			if (sub2.empty())throw "error on sub type 2";

			return mangleg( x ) + "<" +  ( sub1.repr() ) + "," +  ( sub2.repr() ) + ">";
		}



		bool ParserStoreSymbolList::isValue(string name)
		{
			for (auto &s : symbols)
			{
				if (auto  v = dynamic_cast<Value*>(s))
				{
					if (isSameNoum( v->getName(), name ))
					{	
							return true;
					}
				}
			}
			return false;
		}

		bool ParserStoreSymbolList::isInstance(string name)
		{
			for (auto &s : symbols)
			{
				if (auto  v = dynamic_cast<Instance*>(s))
				{
					if (isSameNoum( v->getName(), name ))
					{
						return true;
					}
				}
			}
			return false;
		}

		SReference ParserStoreSymbolList::getReference( std::string name )
		{
			for (auto q : symbols)
			{
				auto sname = q->getName();
				if (isSameString( sname, name )) return q->ref;	 
			}
			for (auto q : symbols)
			{ 
				if (isSameString( q->ref.repr(), name )) return q->ref;
			}
			return {};
		}


		bool ParserStoreSymbolList::isKind(string nameOfKind )
		{
			for (auto &s : symbols)
			{				
				if (dynamic_cast<Kind*>(s) != nullptr)
				{
					if (isSameNoum( s->getName(), nameOfKind )) return true;
				
				}
			}
			return false;
		}


		SReference ParserStoreSymbolList::getKindOf(string name)
		{
			for (auto &s : symbols)
			{
				if (auto k =  dynamic_cast<Kind*>(s))
				{
					if (isSameNoum( k->getName(), name ))
					{
						if (k->baseKind.empty() ==false)	return k->baseKind;
						else return SReference("kind");
					}
				}
			}
			return {};
		}
 
		SReference ParserStoreSymbolList::getKindOf( SReference aref )
		{
			for (auto &s : symbols)
			{
				if (auto k = dynamic_cast<Kind*>(s))
				{
					if( k->ref.repr() == aref.repr() )
					{
						if (k->baseKind.empty() == false)	return k->baseKind;
						else return SReference( "kind" );
					}
				}
			}
			return {};
		}

		bool ParserStoreSymbolList::setLocalVar( std::string name, std::string value )
		{
			for (auto &v : locals)
			{
				if (isSameNoum( v.first, name ))
				{
					v.second = value;
					return true;
				}
			}
			locals.push_back( make_pair( name,value ) );
			return true;
		}

		bool ParserStoreSymbolList::getLocalVar( std::string name, std::string & value )
		{
			for (auto &v : locals)
			{
				if (isSameNoum( v.first, name ))				 
				{
					value = v.second;
					return true;
				}
			}
			return false;
		}

		void ParserStoreSymbolList::addSymbol( Symbol * s )
		{
			cout << "add Symbol " << s->getName() << endl;
			symbols.push_back( s );
		}

		std::list<Symbol*> ParserStoreSymbolList::ListSymbols()
		{
			return this->symbols;
		}

		bool ParserStoreSymbolList::isSymbol(string name)
		{
			for (auto &s : symbols)
			{
				 
				if (isSameNoum(s->getName(), name )) return true;
			}

			for (auto &s : symbols)
			{
				if (isSameNoum( s->ref.repr(), name )) return true;				
			}


			return false;
		}
		PreCodeGenerateDependency::PreCodeGenerateDependency(string _requiredSymbol) : requiredSymbol(_requiredSymbol)
		{
			requiredSymbol+"";
		}
		string PreCodeGenerateDependency::repr()
		{
			return "Req:" + requiredSymbol;
		}
		PreCodeGenerateEmpty::PreCodeGenerateEmpty()
		{
		}
		std::string PreCodeGenerateEmpty::repr()
		{
			return std::string();
		}
		PreCodeGenerate*  PreCodeGenerate::add(PreCodeGenerate * atail)
		{
			if (atail == nullptr) return this;

			if (tail == nullptr)	  tail = atail;
			else { tail->add(atail); };
			return this;

		}
		std::string PreCodeGenerateSession::repr()
		{
			return session;
		}
		 PreCodeGenerateSession::PreCodeGenerateSession(string _name) :session(_name), inner(nullptr) 
		{
			 inner = createPreCodeGenerateEmpty();
		}

		   std::string PreCodeGenerateBlock::repr() { return ""; }

		 PreCodeGenerateBlock::PreCodeGenerateBlock(PreCodeGenerate *_header)
		 {
			 header = _header;
			 inner = createPreCodeGenerateEmpty();
		 }


}
}