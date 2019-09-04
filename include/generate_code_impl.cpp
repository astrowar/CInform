
#include "PreCodeGenerate.hpp"
#include "NSource.hpp"
#include <headerPhase.hpp>
#include <emiteIL.hpp>
#include <componentesSelector.hpp>

#include <cctype>

namespace CInform
{
	using namespace Match;
	namespace CodeParser
	{

		bool cleanNoum( string &x )
		{
			while (x.front() == ' ') 
			{
				x = x.substr( 1, x.size() - 1 );
			}
			while (x.back() == ' ')
			{
				x.pop_back();
			}
			if (x.front() == '(' && x.back() == ')')
			{
				x = x.substr( 1, x.size() - 2 );
				return cleanNoum( x );
			}
			return true;
		}

		bool isKindOfValue( ParserStore* pstore, string symbol )
		{
			int k = 0;
			auto ref = pstore->getReference( symbol );
			do{
				k++;
				if (ref.repr() == "value") return true;
				if (ref.repr() == "kind") return false;
				if (ref.empty()) return false;
				ref = pstore->getKindOf( ref );
			} while (k< 20); 
			return false;
		}



		void split_article( ParserStore *pstore, string &a, string &x )
		{
			size_t ipos = x.find( ' ' );
			if (ipos != string::npos)
			{
				if (pstore->isArticle( x.substr( 0, ipos )))
				{
					a = x.substr( 0, ipos );
					x = x.substr( ipos, x.size() - ipos );
				}
			}

			cleanNoum( x );
		}

		PreCodeGenerate*  create_kind( ParserStore *pstore, PreCodeGenerate* prev_generate, string newKindName  )
		{			 
			if (cleanNoum( newKindName ) == false) return nullptr;

			if (pstore->getReference( newKindName ).empty() == false)
			{
				cout << "ja existe" << endl;
				return  new PreCodeGenerateError( "E:Symbolo ja existe: "+ newKindName );;
			}


			PreCodeGenerate* error = nullptr;
			auto hasKindnamed = get_kind_reference( pstore, newKindName, &error );
			if (hasKindnamed.empty() ==false )return new PreCodeGenerateError( "E:Symbolo ja existe:" );
			
			
			
			string  article_str = "";
			if (newKindName.find( ' ' ) != string::npos)
			{
				split_article( pstore, article_str, newKindName );
			}


			{
				if (pstore->getReference( newKindName ).empty() == false)
				{
					cout << "ja existe" << endl;
					return  new PreCodeGenerateError( "E:Symbolo ja existe: " + newKindName );;
				}
			}

			if (pstore->addKind( newKindName, SReference( "kind" ) ))
			{
				auto k_ref = pstore->getReference( newKindName );
				PreCodeGenerate* r = new   PreCodeGenerateIL( "New", "Kind", k_ref.repr() );
				if (k_ref.repr() != newKindName)       r->add( new  PreCodeGenerateIL( "Set", "Name", k_ref.repr(), newKindName ) );
				if (article_str.empty() == false)
				{
					r->add( new  PreCodeGenerateIL( "Set", "article", k_ref.repr(), article_str ) );
				}
				return r;
			}



			////if (pstore->isSymbol(newKindName)) return new PreCodeGenerateError("E:Symbolo ja existe:");
			//pstore->addKind( newKindName, SReference("kind") );
			//return  new  PreCodeGenerateIL( "New", "Kind", newKindName );
		}



		PreCodeGenerate*  create_kind_of( ParserStore *pstore, PreCodeGenerate* prev_generate, string insName,string kindname )
		{
			if (cleanNoum( insName ) == false) return nullptr;


			PreCodeGenerate* error = nullptr;
			auto kind = get_kind_reference( pstore, kindname, &error );
			if (error != nullptr) return error;
			string  article_str = "";
			if (insName.find( ' ' ) != string::npos)
			{
				split_article(pstore, article_str, insName );
			}
			 

			{
				if(pstore->getReference( insName ).empty() ==false )				
				{
					cout << "ja existe" << endl;
					return  new PreCodeGenerateError( "E:Symbolo ja existe: " + insName  );;
				}
			}


			if (pstore->addKind( insName, kind ))
			{
				auto ins_ref = pstore->getReference( insName );
				PreCodeGenerate* r = new   PreCodeGenerateIL( "New", "Kind", ins_ref.repr() , kind.repr()  );
				if (ins_ref.repr() != insName )       r->add( new  PreCodeGenerateIL( "Set", "Name", ins_ref.repr() , insName ) );
				if (article_str .empty() ==false)
				{
					r->add( new  PreCodeGenerateIL( "Set", "article", ins_ref.repr() , article_str ) );
				} 
				return r;
			}
			return nullptr;
		}

		PreCodeGenerate*  create_instance( ParserStore *pstore, PreCodeGenerate* prev_generate, string insName,string kind )
		{
			if (cleanNoum( insName ) == false) return nullptr;

			if(isSameNoum( kind,"kind")) return create_kind(pstore, prev_generate, insName);

			PreCodeGenerate*  prev_generate_local = new  PreCodeGenerateEmpty();
				PreCodeGenerate* error = nullptr;
				SReference kind_reference = get_kind_reference( pstore, kind, &error );
				if (error != nullptr) return error; 
				//if (pstore->isSymbol(kind) == false ) return  new   PreCodeGenerateDependency(kind);
				PreCodeGenerate* article_code = nullptr;
				string article_str = "";
				if (insName.find(' ') != string::npos)
				{
					split_article(pstore, article_str, insName );
				}
				 

				SReference ins_ref = pstore->getReference( insName );
				if (ins_ref.empty() ==false )
				{
					cout << "ja existe" << endl;
					return  new PreCodeGenerateError( "E:Symbolo ja existe: " + insName );;
				}

				if (pstore->addInstance( insName, kind_reference ))
				{
					auto ins_refq = pstore->getReference( insName );
					prev_generate_local->add( new  PreCodeGenerateIL( "New", "Instance", ins_refq.repr() , kind_reference.repr()  ));
					if(ins_refq.repr()!= insName)prev_generate_local->add( new  PreCodeGenerateIL( "Set", "Name", ins_refq.repr() , insName ) );

					if (article_str .empty() ==false)
					{
						prev_generate_local->add( new  PreCodeGenerateIL( "Set", "article", ins_refq.repr() , article_str ) );
					}
					return prev_generate_local;
				}
				return nullptr;
		}

		PreCodeGenerate*  newValueInstance( ParserStore *pstore, PreCodeGenerate* prev_generate, string valuName, string kind )
		{
			if (cleanNoum( valuName ) == false) return nullptr;
			PreCodeGenerate* error = nullptr; 
			SReference kind_reference = get_kind_reference( pstore, kind, &error );
			if (error != nullptr) return error;
			string article_str = "";
			if (valuName.find( ' ' ) != string::npos)
			{
				split_article( pstore, article_str, valuName );
			}
			SReference val_ref = pstore->getReference( valuName );
			if (val_ref.empty() == false)
			{
				cout << "ja existe" << endl;
				return  new PreCodeGenerateError( "E:Symbolo ja existe: " + valuName );;
			}
			if(pstore->addInstance( valuName, kind_reference ) )
			{
				PreCodeGenerate*  prev_generate_local = new  PreCodeGenerateEmpty();
				auto val_ref = pstore->getReference( valuName );
				prev_generate_local->add( new  PreCodeGenerateIL( "New", "Value", val_ref.repr(), kind_reference.repr() ) );
				if (val_ref.repr() != valuName) prev_generate_local->add( new  PreCodeGenerateIL( "Set", "name", val_ref.repr(), valuName ) );
				return  prev_generate_local;
			} 
			return nullptr;
		}




		PreCodeGenerate*  newVariable( ParserStore *pstore, PreCodeGenerate* prev_generate, string varName, string kind )
		{
			if (cleanNoum( varName ) == false) return nullptr;
			PreCodeGenerate* error = nullptr;
			SReference kind_reference = get_kind_reference( pstore, kind, &error );
			if (error != nullptr) return error;
			string article_str = "";
			if (varName.find( ' ' ) != string::npos)
			{
				split_article( pstore, article_str, varName );
			}
			SReference val_ref = pstore->getReference( varName );
			if (val_ref.empty() == false)
			{
				cout << "ja existe" << endl;
				return  new PreCodeGenerateError( "E:Symbolo ja existe: " + varName );;
			}
			if (pstore->addGlobalVariable( varName, kind_reference ))
			{
				PreCodeGenerate*  prev_generate_local = new  PreCodeGenerateEmpty();
				auto val_ref = pstore->getReference( varName );
				prev_generate_local->add( new  PreCodeGenerateIL( "New", "Variable", val_ref.repr(), kind_reference.repr() ) );
				if (val_ref.repr() != varName) prev_generate_local->add( new  PreCodeGenerateIL( "Set", "Name", val_ref.repr(), varName ) );
				return  prev_generate_local;
			}
			return nullptr;
		}


		PreCodeGenerate* 	createAssertadjetive( ParserStore *pstore, PreCodeGenerate* prev_generate, string symbol  , string value  )
		{
			PreCodeGenerate* error = nullptr;
			PreCodeGenerate*  prev_generate_local = new  PreCodeGenerateEmpty();
	 

			if (symbol.find( " and " ) != string::npos)
			{
				auto olist = get_list_and( pstore, symbol );
				for (auto o : olist)
				{
					PreCodeGenerate*  r = create_instance( pstore, prev_generate, o, value );
					prev_generate_local->add( r );
				}
				return prev_generate_local;

			}

			if (pstore->isInstance( symbol ))
			{
				//fazendo um assert em um tipo
				for (auto o : get_list_and( pstore, value ))
				{
					prev_generate_local->add( new   PreCodeGenerateIL( "Set", symbol, o ) );
				}
				return prev_generate_local;
			}

			if (isKindOfValue( pstore, symbol ))
			{
				//kind of value ??? 
				//enum de values ?
				list<string> o_values = get_list_and( pstore, value );
				for (auto o : o_values)
				{
					prev_generate_local->add( newValueInstance( pstore, prev_generate, o, symbol ) );
					//prev_generate_local->add( new   PreCodeGenerateIL( "New", "Value", o, symbol ) );
					//New Value    guttering brightnesses
				}
				return prev_generate_local;
			}

			if (pstore->isKind( symbol ))

			{
				PreCodeGenerateSession *psess = new   PreCodeGenerateSession( "@init " + symbol );
				//fazendo um assert em um tipo
				for (auto o : get_list_and( pstore, value ))
				{
					psess->inner->add( new   PreCodeGenerateIL( "Set", "this", o ) );
				}
				prev_generate_local->add( psess );
				return prev_generate_local;
			}

			return  new PreCodeGenerateDependency( symbol );


		
		}


			
		PreCodeGenerate*  codeGenerate( ParserStore *pstore, PreCodeGenerate* prev_generate, string entryName, TBlockGroupItemNoum vx, TBlockGroupItemNoum vy, TBlockGroupItemNoum vz )
		{
			PreCodeGenerate*  prev_generate_local = new  PreCodeGenerateEmpty();
			pstore->push_state(); //adiciona um novo ponto de controle

			if (entryName == "assertKind")
			{
				string newKindName = (vx.repr());
				return create_kind( pstore, prev_generate, newKindName );
			}

			if (entryName == "assertKindOf")
			{
				string insName = (vx.repr());
				string kindname = (vy.repr());
				return create_kind_of( pstore, prev_generate, insName, kindname ); 
			}


			if (entryName == "assertInstance")
			{
				string insName = (vx.repr());
				string kind = (vy.repr());
				return create_instance( pstore, prev_generate, insName, kind );
			 
			}



			if (entryName == "assertKindValue")
			{
				string insName = (vx.repr());
				string kind = (vy.repr()); 
				return create_kind_of( pstore, prev_generate, insName,  ( "value" ) ); 
			}


			//assertAdjetiveEnum
			if (entryName == "assertAdjetiveEnum")
			{
				string symbol = (vx.repr());
				string values = (vy.repr());

				if (vx.size() > 1)
				{
					symbol = removeArticle( pstore, symbol );
				}

				if (pstore->isSymbol( symbol ) == false) return  new   PreCodeGenerateDependency( symbol ); 

				auto s_ref = pstore->getReference( symbol );


				list<string> o_values = get_list_or( pstore, values );
				string temp_name = "_pp" + pstore->next_temp() + "_" + s_ref.repr();
				pstore->addKind( temp_name, SReference("value") );
				prev_generate_local->add( new   PreCodeGenerateIL( "New", "Kind", temp_name, "Value" ) ); 
				for (auto ov : o_values)
				{
					prev_generate_local->add( newValueInstance( pstore, prev_generate, ov, temp_name ) ); 
				}
				return prev_generate_local->add( new   PreCodeGenerateIL( "Add", "Property", s_ref.repr(), temp_name, temp_name ) );
			}

			//assertMemberVariable
			if (entryName == "assertMemberVariable")
			{
				string symbol = (vx.repr());

				if (vx.size() > 1) symbol = removeArticle( pstore, symbol );


				if (pstore->isSymbol( symbol ) == false)return  new   PreCodeGenerateDependency( symbol );

				string kind = (vy.repr());
				if (pstore->isSymbol( symbol ) == false)return  new   PreCodeGenerateDependency(kind);

				PreCodeGenerate* error = nullptr;
				SReference   rkind = get_kind_reference( pstore, kind, &error );
				if (error != nullptr) return error;
							   
				string named = (vz.repr());
				string internal_name = pstore->mangleg( named );

				auto s_ref = pstore->getReference( symbol );
				if (s_ref.empty() == false)
				{
					auto h = new   PreCodeGenerateIL( "Add", "Property", s_ref.repr(), internal_name, rkind.repr() );
					if (internal_name != named)
					{
						h->add( new   PreCodeGenerateIL( "Set", "Name", internal_name, named ) );
					}
					return h;
				}
				return    new   PreCodeGenerateDependency( symbol ); ;
			}

			if (entryName == "assertAdjetive")
			{
				string symbol = (vx.repr());
				string value = (vy.repr());

				return createAssertadjetive( pstore, prev_generate, symbol, value );

 

				return  new PreCodeGenerateDependency( symbol );
			}


			if (entryName == "assertVariable") 
			{
			
				string symbol = (vx.repr());
				string value = (vy.repr());
				if (vx.size() > 1) symbol = removeArticle( pstore, symbol );

 
				return newVariable( pstore, prev_generate, symbol, value );
			
			}



			// A object  is usually portable 
			if (entryName == "assertUsually")
			{
				string symbol = (vx.repr());
				if (vx.size() > 1) symbol = removeArticle( pstore, symbol );
				if (pstore->isSymbol( symbol ) == false)return  new   PreCodeGenerateDependency( symbol );

				string value = (vy.repr());

				//if (pstore->isValue(symbol))
				{
					for (auto o : get_list_and( pstore, value ))
					{
						//if (pstore->isValue(symbol))
						{
							PreCodeGenerateSession *psess = new   PreCodeGenerateSession( "@init " + symbol );
							psess->inner->add( new   PreCodeGenerateIL( "Set", "this", o ) );
							prev_generate_local->add( psess );
						}
					}
					return prev_generate_local;
				}
			}

			//to decide IF X is Y
			if (entryName == "phaseDeclDecide")
			{
				PreCodeGenerate* error = nullptr;
				HeaderPhaseEntry aY = listToVerbPreposition( pstore, vy.literals, "B", &error );
				if (aY.header.empty())
				{
					aY = listToArgumentsEntryPatthen_variableSelector( pstore, vy.literals, "B", &error );
				}
				if (aY.header.empty()) return error;

				HeaderPhaseEntry aX = listToArgumentsEntryPatthen_variableSelector( pstore, vx.literals, "A", &error );

				if (aX.header.empty()) return error;

				//to decide IF X is verb Y

				//HeaderPhaseEntry aY = listToArgumentHeader(pstore, vy.literals,"B");
				PreCodeGenerateBlock *pblock = new   PreCodeGenerateBlock( new   PreCodeGenerateIL( "BEGIN", "PHASE4", aX.header + " is " + aY.header, "BOOL" ) );

				auto args = aX.args;
				args.insert( args.end(), aY.args.begin(), aY.args.end() );


				for (auto arg : args)
				{
					for (auto il : emiteILCode( pstore, arg ))pblock->inner->add( il );
					//pblock->inner->add(new   PreCodeGenerateIL("UNIFY", arg.x, arg.y)); 
				}



				prev_generate_local->add( pblock );
				prev_generate_local->add( new   PreCodeGenerateIL( "END", "PHASE", "" ) );
				return prev_generate_local;
			}

			if (entryName == "phaseDeclwhich")
			{
				string aX = (vx.repr());
				HeaderPhaseEntry aY = listToComposePreposition_2( pstore, vy.literals, "A" );
				if (aY.header.empty()) return nullptr;
				PreCodeGenerateBlock *pblock = new   PreCodeGenerateBlock( new   PreCodeGenerateIL( "BEGIN", "PHASE3", aY.header , aX ) );

				for (auto arg : aY.args)
				{
					for (auto il : emiteILCode( pstore, arg ))pblock->inner->add( il );
					//pblock->inner->add( new   PreCodeGenerateIL( "UNIFY", arg.x, arg.y ) );
				}
				prev_generate_local->add( pblock );
				prev_generate_local->add( new   PreCodeGenerateIL( "END", "PHASE", "" ) );
				return prev_generate_local;
			}

			// To X Y 
			// assume que a phase esta na forma VCV , Verbo + complemento Verbal  (VP = V + (VP || NP)  VP = V + (P + NP) )  
			if (entryName == "phaseDeclVerb2")
			{
				PreCodeGenerate* error = nullptr;
				HeaderPhaseEntry aY = listToNP( pstore, vy.literals, "B", &error ); //NP obriatoriamente
				if (aY.header.empty())
				{
					return  error; //next
				}


				HeaderPhaseEntry aX = listToVerbEntry( pstore, vx.literals, "A", &error );
				if (aX.header.empty())
				{
					aX = listToVerbOnly( pstore, vx.literals, "A", &error );
					if (aX.empty())
					return    error; //next
				}
				error = nullptr;

				PreCodeGenerateBlock *pblock = new   PreCodeGenerateBlock( new   PreCodeGenerateIL( "BEGIN", "PHASE2", aX.header+" "+ aY.header ) );


				for (auto arg : aX.args)
				{
					for (auto il : emiteILCode( pstore, arg ))pblock->inner->add( il );
					//pblock->inner->add( new   PreCodeGenerateIL( "UNIFY", arg.x, arg.y ) );
				}
				for (auto arg : aY.args)
				{
					for (auto il : emiteILCode( pstore, arg ))pblock->inner->add( il );
					//pblock->inner->add( new   PreCodeGenerateIL( "UNIFY", arg.x, arg.y ) );
				}
				error = nullptr;
				prev_generate_local->add( pblock );
				prev_generate_local->add( new   PreCodeGenerateIL( "END", "PHASE", "" ) );
				return prev_generate_local;

			}

			// To X 
			if (entryName == "phaseDeclVerb")
			{
				PreCodeGenerate* error = nullptr;
				HeaderPhaseEntry aX = listToComposePreposition_2( pstore, vx.literals, "A"  );
				if (aX.header.empty())
				{
					if (error) return error;
					return new  PreCodeGenerateEmpty();
				}

				PreCodeGenerateBlock *pblock = new   PreCodeGenerateBlock( new   PreCodeGenerateIL( "BEGIN", "PHASE1", aX.header ) );

				for (auto arg : aX.args)
				{
					for (auto il : emiteILCode( pstore, arg ))pblock->inner->add( il );
					//pblock->inner->add( new   PreCodeGenerateIL( "UNIFY", arg.x, arg.y ) );
				}
				prev_generate_local->add( pblock );
				prev_generate_local->add( new   PreCodeGenerateIL( "END", "PHASE", "" ) );
				return prev_generate_local;
			}

			if (entryName == "phaseInstead")
			{
				HeaderPhaseEntry aX = listToActionSelector( pstore, vx.literals, "A" );
				PreCodeGenerateBlock *pblock = new   PreCodeGenerateBlock( new   PreCodeGenerateIL( "BEGIN", "INSTEAD", aX.header ) );
				for (auto arg : aX.args)
				{
					for (auto il : emiteILCode( pstore, arg ))pblock->inner->add( il );
					//pblock->inner->add( new   PreCodeGenerateIL( "UNIFY", arg.x, arg.y ) );
				}


				prev_generate_local->add( pblock );
				prev_generate_local->add( new   PreCodeGenerateIL( "END", "INSTEAD", "" ) );
				return prev_generate_local;

			}


			return  new   PreCodeGenerateIL( entryName, "?", (vx.repr()), (vy.repr()) );
			//return new PreCodeGenerateError("Error");
		}
	}
}