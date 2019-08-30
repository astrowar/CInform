
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

		PreCodeGenerate*  codeGenerate( ParserStore *pstore, PreCodeGenerate* prev_generate, string entryName, TBlockGroupItemNoum vx, TBlockGroupItemNoum vy, TBlockGroupItemNoum vz )
		{
			PreCodeGenerate*  prev_generate_local = new  PreCodeGenerateEmpty();
			pstore->push_state(); //adiciona um novo ponto de controle

			if (entryName == "assertKind")
			{
				string newKindName = (vx.repr());
				PreCodeGenerate* error = nullptr;
				string hasKindnamed = get_kind_reference( pstore, newKindName, &error );
				if (hasKindnamed != "")
				{
					return new PreCodeGenerateError( "E:  "+newKindName +" already exist" );
				}


				//if (pstore->isSymbol(newKindName)) return new PreCodeGenerateError("E:Symbolo ja existe");
				pstore->addKind( newKindName, "kind" );
				return  new  PreCodeGenerateIL( "New", "Kind", (vx.repr()) );
			}

			if (entryName == "assertKindOf")
			{
				string insName = (vx.repr());
				string kindname = (vy.repr());
				//if (pstore->isSymbol(kind) == false)return  new   PreCodeGenerateDependency(kind);	
				PreCodeGenerate* error = nullptr;
				string kind = get_kind_reference( pstore, kindname, &error );
				if (error != nullptr) return error;


				PreCodeGenerate* article_code = nullptr;
				if (vx.size() > 1)
				{
					auto gs = codeGenerateArticle( pstore, prev_generate, vx );
					insName = gs.entryName;
					article_code = gs.IL;
				}

				pstore->addKind( insName, kind );
				return prev_generate_local->add( (new  PreCodeGenerateIL( "New", "Kind", insName, kind ))->add( article_code ) );
			}


			if (entryName == "assertInstance")
			{
				string insName = (vx.repr());
				string kind = (vy.repr());

				PreCodeGenerate* error = nullptr;
				string kind_reference = get_kind_reference( pstore, kind, &error );
				if (error != nullptr) return error;
				if (kind_reference == "kind")  return nullptr;


				//if (pstore->isSymbol(kind) == false ) return  new   PreCodeGenerateDependency(kind);
				PreCodeGenerate* article_code = nullptr;
				if (vx.size() > 1)
				{
					auto gs = codeGenerateArticle( pstore, prev_generate, vx );
					insName = gs.entryName;
					article_code = gs.IL;
				}
				pstore->addInstance( insName, kind_reference );
				return  prev_generate_local->add( (new  PreCodeGenerateIL( "New", "Instance", insName, kind_reference ))->add( article_code ) );

			}



			if (entryName == "assertKindValue")
			{
				string insName = (vx.repr());
				string kind = (vy.repr());


				PreCodeGenerate* article_code = nullptr;
				if (vx.size() > 1)
				{
					auto gs = codeGenerateArticle( pstore, prev_generate, vx );
					insName = gs.entryName;
					article_code = gs.IL;
				}
				pstore->addKind( insName, "value" );
				return (new   PreCodeGenerateIL( "New", "Kind", insName, "Value" ))->add( article_code );
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

				if (pstore->isSymbol( symbol ) == false)return  new   PreCodeGenerateDependency( symbol );



				list<string> o_values = get_list_or( pstore, values );
				string temp_name = "_pp" + pstore->next_temp() + "_" + symbol;
				pstore->addKind( temp_name, "value" );
				prev_generate_local->add( new   PreCodeGenerateIL( "New", "Kind", temp_name, "Value" ) );


				for (auto ov : o_values)
				{
					prev_generate_local->add( new   PreCodeGenerateIL( "New", "Value", ov, temp_name ) );
					pstore->addInstance( ov, temp_name );
				}

				return prev_generate_local->add( new   PreCodeGenerateIL( "Add", "Property", symbol, temp_name, temp_name ) );
			}

			//assertMemberVariable
			if (entryName == "assertMemberVariable")
			{
				string symbol = (vx.repr());

				if (vx.size() > 1) symbol = removeArticle( pstore, symbol );


				if (pstore->isSymbol( symbol ) == false)return  new   PreCodeGenerateDependency( symbol );

				string kind = (vy.repr());
				//if (pstore->isSymbol(kind) == false)return  new   PreCodeGenerateDependency(kind);

				PreCodeGenerate* error = nullptr;
				kind = get_kind_reference( pstore, kind, &error );
				if (error != nullptr) return error;



				string named = (vz.repr());
				return prev_generate_local->add( new   PreCodeGenerateIL( "Add", "Property", symbol, named, kind ) );
			}

			if (entryName == "assertAdjetive")
			{
				PreCodeGenerate* error = nullptr;

				string symbol = (vx.repr());
				string value = (vy.repr());
				
				 

				if (vx.size() > 1) symbol = removeArticle( pstore, symbol );

				if (pstore->isInstance( symbol ))
				{
					//fazendo um assert em um tipo
					for (auto o : get_list_and( pstore, value ))
					{
						prev_generate_local->add( new   PreCodeGenerateIL( "Set", symbol, o ) );
					}
					return prev_generate_local;
				}

				{

					auto kindref = get_kind_reference( pstore, value, &error );
					if (kindref != "") return nullptr;
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

				if (pstore->isValue( symbol ))
				{
					//enum de values ?
					list<string> o_values = get_list_and( pstore, value );
					for (auto o : o_values)
					{
						prev_generate_local->add( new   PreCodeGenerateIL( "New", "Value", o, symbol ) );
						//New Value    guttering brightnesses
					}

					return prev_generate_local;
				}
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
				HeaderPhaseEntry aY = listToVerbPredicate( pstore, vy.literals, "B", &error );
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
				PreCodeGenerate* error = nullptr;
				string aX = (vx.repr());
				HeaderPhaseEntry aY = listToComposePredicate_2( pstore, vy.literals, "A" ,&error );
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
				HeaderPhaseEntry aY = listToVerbPredicate( pstore, vy.literals, "B", &error ); //NP obriatoriamente
				if (aY.header.empty())
				{
					return  error; //next
				}


				HeaderPhaseEntry aX = listToVerbEntry( pstore, vx.literals, "A", &error );
				if (aX.header.empty())
				{
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
				HeaderPhaseEntry aX = listToVerbPredicate( pstore, vx.literals, "A", &error );
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