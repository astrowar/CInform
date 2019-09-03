 	

/* Basic Rules for sentence formation


S = NP VP
NP = (det)(adj) N (PP)
VP = V (NP) (PP)(Adv)
PP = p NP


*/





#include "PreCodeGenerate.hpp"
#include "NSource.hpp"
#include <headerPhase.hpp>

#include <cctype>
#include "..\include\componentesSelector.hpp"

using namespace std; 

namespace CInform
{	using namespace Match;

void replaceAll( string &s, const string &search, const string &replace );

std::list<string>   split( string x );

bool isMuteVariable(string x)
{
	if (x.size() > 3) return false; 
	if (x.size() ==0 ) return false;
	if (toupper( x[0] ) == x[0]) return true;
	return false;
}

	string listToString(list<string> x)
	{
		if (x.size() == 1) return x.front();
		string s = "";
		bool isFront = true;
		for (auto xi : x)
		{
			if (isFront == false) s = s + " ";
			isFront = false;
			s = s + xi;
		}
		return s + "";
	}


	vector<string> decompose(string x)
	{
		auto ls = split( x );
		return vector<string>( ls.begin(), ls.end() );

	}


	namespace CodeParser
	{
	 
		std::list<MatchResultStr> getMatchs( ParserStore *pstore, string txt, string match )
		{
			HMatchExpended ah = getPMatchExpended( match, pstore->grammar() );
			return  MatchThis( txt, ah );
		}


		list< MatchResultStr  > 	sorteReverseBy( list< MatchResultStr  >  lst, string var )
		{ 
			lst.sort( [&]( MatchResultStr &a, MatchResultStr &b ) {return a.getResult( var ).literals.size() > b.getResult( var ).literals.size(); } );
			return lst;
		}
		

		list< MatchResult   > 	sorteReverseBy( list< MatchResult   >  lst, string var )
		{
			lst.sort( [&]( MatchResult  &a, MatchResult  &b ) {return a.getResult( var ).literals.size() > b.getResult( var ).literals.size(); } );
			return lst;
		}

		string nlistToString(list<HNoum> x)
		{
			string s = "";
			bool isFront = true;
			for (auto xi : x)
			{
				if (isFront == false) s = s + " ";
				isFront = false;
				s = s + xi->repr();
			}
			return s;
		}
 

		bool isAuxWord( ParserStore *pstore, string x )
		{
			if( pstore->isArticle( x )) return true;
			if (pstore->isPreposition( x )) return true;
			if (pstore->isPronoum( x )) return true;
			if (pstore->isNoumReserved( x )) return true;

			return false;

		}

		class ArgumentUnify
		{
		public:
			string x, y;

			ArgumentUnify( string _x, string _y ) :x( _x ), y( _y ) 
			{
				if (x == "" || y == "")
				{
					throw "error ?";
				}
			};
		};




		string  expand(list<HNoum> xj)
		{
			string xs;
			bool front = true;
			while (xj.empty() == false)
			{
				
				auto hs = split(xj.front()->repr());
				
				for (string &s : hs)
				{ 
					if (front == false) xs += " ";
					xs += s;
					front = false;
				}
				xj.pop_front();
			}
			return xs;

		}
		HeaderPhaseEntry listToArgumentsEntryPatthen(ParserStore * pstore, string x, string sufix);
		HeaderPhaseEntry listToArgumentsEntryPatthen_variableSelector( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error );
		HeaderPhaseEntry listToVerbPreposition( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error );
		HeaderPhaseEntry listToVerbEntry( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error );


		string get_kind_reference(ParserStore *pstore, string vx, PreCodeGenerate** error);


	

		
		SelectorItem* getSelector( ParserStore *pstore, string val, string sufix, PreCodeGenerate** error );






		HeaderPhaseEntry listToArgumentsEntryPatthen(ParserStore *pstore, string x, string sufix  )
		{
			HMatchExpended ah = getPMatchExpended("?H XA which XB ?RE", pstore->grammar());
			list< MatchResultStr > rss = CInform::Match::MatchThis(x, ah);
			for (auto r : rss)
			{
				cout << r.repr() << endl;
				if (r.hasBind("XA"))
				{
					cout << "    slector? "<< r.getResult("XA").repr() << endl;
				}
				if (r.hasBind("XB"))
				{
					cout << "    bool preposition ? " << r.getResult("XB").repr() << endl;
				}
			}
			return HeaderPhaseEntry();
		}


		HeaderPhaseEntry listToActionSelector_nomSelector( ParserStore *pstore, list<HNoum> xj, string sufix   )
		{
			PreCodeGenerate* error = nullptr;

			HMatchExpended ah = getPMatchExpended( "V  XA", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );

			for (auto r : rss)
			{
				string v_repr = r.getResult( "V" ).repr();
				string v_name = v_repr;

				HeaderPhaseEntry sel;
				sel.header = v_repr + " " + sufix + "1"; 

				string xa = r.getResult( "XA" ).repr();
				sel.args.push_back( new SelectorUnify( sufix + "1", xa ) ); 
				return sel;
			}
			return HeaderPhaseEntry();
		}


		HeaderPhaseEntry listToXKSeletor( ParserStore *pstore, list<HNoum> x_sel, list<HNoum> k_sel,  string sufix , PreCodeGenerate** error )
		{
		  
			 
				HeaderPhaseEntry sel;
				sel.header =   sufix + "1";

				string akind = nlistToString( k_sel);
				string kind_ref = get_kind_reference( pstore, akind, error );
				if (kind_ref == "")
				{
					return HeaderPhaseEntry();
				}

				string xa = nlistToString( x_sel );

				sel.args.push_back( new SelectorUnify( sufix + "1", xa ) );
				sel.args.back()->next = new SelectorUnify( xa, kind_ref );

				return sel;
			 
			return HeaderPhaseEntry();
		}



		HeaderPhaseEntry listToActionSelector( ParserStore *pstore, list<HNoum> xj, string sufix   )
		{
			PreCodeGenerate* error = nullptr;

			HMatchExpended ah = getPMatchExpended( "V  (XA - K)", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );

			for (auto r : rss)
			{

				string v_repr = r.getResult( "V" ).repr();
				string v_name = v_repr; 

				HeaderPhaseEntry sel;
				sel.header = v_repr + " " + sufix + "1";

				string akind = r.getResult( "K" ).repr();
				string kind_ref = get_kind_reference( pstore, akind, &error );
				if (kind_ref == "")
				{ 
					kind_ref = akind;
				} 

				string xa = r.getResult( "XA" ).repr();
				
				sel.args.push_back( new SelectorUnify( sufix + "1", xa ) );
				sel.args.back()->next = new SelectorUnify( xa, kind_ref ) ;
				
				return sel; 
			}


			//Sem Seletor nenhum


			auto r = listToActionSelector_nomSelector( pstore, xj, sufix );
			if (r.header != "") return r;


			return HeaderPhaseEntry();

		}

		list< SelectorItem*> getArguments( ParserStore *pstore, string val , string argumentName  )
		{
			PreCodeGenerate* error = nullptr;
			list< SelectorItem*> args;
			{
				HMatchExpended ah = getPMatchExpended( " (X - K) ", pstore->grammar() ); 
				list< MatchResultStr  > rss = CInform::Match::MatchThis( val, ah );
				
	 
				for (auto r : rss)
				{
			 

					string akind = r.getResult( "K" ).repr();
					string kind_ref;
					if (isMuteVariable( akind ) )
					{
						kind_ref = akind;
					}
					else
					{
						kind_ref = get_kind_reference( pstore, akind, &error );
						if (kind_ref == "")
						{
							error = new   PreCodeGenerateDependency( akind );
							//continue;
							kind_ref = akind;
						}
					}

					string xa = r.getResult( "X" ).repr();
					args.push_back( new SelectorUnify  ( argumentName, xa ) );
					args.back()->next = new SelectorUnify( xa, kind_ref );
					return args;
				}
			}


			{
			        if (pstore->isInstance( val ))
					{
						args.push_back( new SelectorUnify( argumentName, val ) );
						return args;
					} 
					 
			}



			return args;
		}


		SelectorItem* getSelector( ParserStore *pstore  , string val,   PreCodeGenerate** error )
		{
			if ((val.find( '(' ) != string::npos))
			{
				if ((val.find( ')' ) != val.size()-1))
				{
					return nullptr;
				}				
			}


			if (isMuteVariable( val ))
			{
				return new SelectorKind( "$", val );
			}

			if (val == "something")
			{
				//*error = nullptr;
				return new SelectorUnify( "$", "thing" );
			}

			{
				//(name of kind of value K)
				list< MatchResultStr  > nss = getMatchs( pstore, val, "X or Y " );

				for (auto r : nss)
				{					
					auto tmp1 = "$x" + pstore->next_temp();
					auto sx = getSelector( pstore, r.getResult( "X" ).repr(), tmp1, error );
					if (sx != nullptr)
					{
						auto tmp2 = "$x" + pstore->next_temp();
						auto sy = getSelector( pstore, r.getResult( "Y" ).repr(), tmp2, error );
						if (sy !=nullptr)
						{
							//*error = nullptr;
						   return 	new SelectorOr( "$",  sx , sy);
						}
						
					}
				}
			}


			{
				//(relation of values of kind K to Ls)
				list< MatchResultStr  > nss = getMatchs( pstore, val, "values/value of kind K" );
				for (auto r : nss)
				{

					auto sk = getSelector( pstore, r.getResult( "K" ).repr(), error );
					if (sk == nullptr) continue;

					if (sk->target == "$")
					{
						sk->target = "$k_" + pstore->next_temp();
					}
					auto s1 = new SelectorUnify( "$", "value_of<" + sk->target +   ">" );				 
					s1->next = sk;
					return s1;
				}

			}

			{
				//(relation of values of kind K to Ls)
				list< MatchResultStr  > nss = getMatchs( pstore, val, "relation of K to Z" );
				for (auto r : nss)
				{

					auto sk = getSelector( pstore, r.getResult( "K" ).repr(), error );
					if (sk == nullptr) continue;

					if (sk->target == "$")
					{
						sk->target = "$k_" + pstore->next_temp();
					}

					auto sz = getSelector( pstore, r.getResult( "Z" ).repr(), error );
					if (sk == nullptr) continue;
					if (sz->target == "$")
					{
						sz->target = "$k_" + pstore->next_temp();
					}


					auto s1 = new SelectorUnify( "$", "relation_of<" + sk->target + ","+ sz->target + ">" );
					sk->next = sz;
					s1->next = sk;
					return s1;
				}

			}

			{
				//(name of kind of value K)
				list< MatchResultStr  > nss = getMatchs( pstore, val, "list of K" );
				for (auto r : nss)
				{
					string akind = r.getResult( "K" ).repr();
					auto sk = getSelector( pstore, akind, error );
					if (sk !=nullptr)
					{
						if (sk->target == "$")
						{
							sk->target = "$k_"+pstore->next_temp();
						}
						auto s1 = new SelectorUnify( "$", "list_of<" + sk->target + ">" );
						s1->next = sk;
						 
						return s1;
					}
				}
			}



			{
				//(name of kind of value K)
				list< MatchResultStr  > nss = getMatchs( pstore, val, "name of kind of value K" );
				for (auto r : nss)
				{
					string akind = r.getResult( "K" ).repr();
					if (pstore->isImplicityVar( akind ))
					{
						auto s1 = new SelectorUnify( "$", "name_of<" + akind + ">" );
						auto s2 = new SelectorKind( akind, "kind_of_value" );
						s1->next = s2;
					 
						return s1;
					}
				}
			}
			{
				list< MatchResultStr  > rss = getMatchs( pstore, val, "X - K" );
				for (auto r : rss)
				{
					string akind = r.getResult( "K" ).repr();
					string atarget = r.getResult( "X" ).repr();
					auto sel = getSelector( pstore, akind, error );
					if (sel != nullptr)
					{
						*error = nullptr;

						sel->target = "$k_" + pstore->next_temp();
						auto s1 = new SelectorKind( atarget, sel->target );
						s1->next = sel;
						return s1;
					}
					else
					{
						return nullptr;
					} 

				}

				for (auto r : rss)
				{
					string akind = r.getResult( "K" ).repr();
					string atarget = r.getResult( "X" ).repr();
					if (pstore->isImplicityVar( akind ))
					{
						akind = pstore->mangleg( akind );

						return new SelectorUnify( atarget, akind );
					}
				}
				if (rss.empty() == false)
				{
					return nullptr;
				}
			}

			{
				string kind_ref = get_kind_reference( pstore, val, error );
				if (kind_ref != "")
				{
					*error = nullptr;
					return new SelectorKind( "$", kind_ref );					 
				}
				 
			} 
			return nullptr;
		}


		SelectorItem* getSelector_np( ParserStore *pstore, string val, PreCodeGenerate** error )
		{
			auto sel = getSelector( pstore, val, error );
			if (sel != nullptr) return sel;
			
			{
				val = removeArticle(pstore, val );
				string inst_ref = get_instance_reference( pstore, val, error );
				if (inst_ref != "")
				{
					*error = nullptr;
					return new SelectorKind( "$", inst_ref );
				} 
			}
			return nullptr;
		}

		SelectorItem* getSelector( ParserStore *pstore, string val, string avar, PreCodeGenerate** error )
		{
			auto sel = getSelector( pstore, val, error );
			if (sel != nullptr)
			{
				if (sel->target != "$")
				{
					auto sa = new SelectorUnify( avar, sel->target );
					sa->next = sel;
					return sa;					
				}
				else
				{
					sel->target = avar;
				}
				*error = nullptr;
				return sel;
			}
			return nullptr;
		}


		SelectorItem* getSelector_np( ParserStore *pstore, string val, string avar, PreCodeGenerate** error )
		{
 
			auto sel = getSelector_np( pstore, val, error );
			if (sel != nullptr)
			{
				if (sel->target != "$")
				{
					auto sa = new SelectorUnify( avar, sel->target );
					sa->next = sel;
					return sa;
				}
				else
				{
					sel->target = avar;
				}
				return sel;
			}
			return nullptr;
		}


		HeaderPhaseEntry listToComposePreposition_1( ParserStore *pstore, list<HNoum> xj, string sufix )
		{
			HMatchExpended ah = getPMatchExpended( "Z0 ZB ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			PreCodeGenerate* error = nullptr;  
			rss = sorteReverseBy( rss, "ZB" ); 
			for (auto r : rss)
			{
				//if (r.getResult( "ZB" ).literals.size() <= 1) continue;
				auto hpB = listToVerbEntry( pstore, r.getResult( "ZB" ).literals, sufix + "2", &error );
				if (hpB.empty()) continue; 
				SelectorItem* arg0 = getSelector_np( pstore, r.getResult( "Z0" ).repr(), sufix + "1", &error );
				if (arg0 != nullptr)
				{
					hpB.header = sufix + "1" + " " + hpB.header;
					hpB.args.push_back( arg0 );
					return hpB;
				}
				else
				{
					hpB.header = r.getResult( "Z0" ).repr() + " " + hpB.header;					
					return hpB;
				} 
			}



			return HeaderPhaseEntry();
		}


	 
		HeaderPhaseEntry listToComposeNP_2_AB( ParserStore *pstore, list<HNoum> xj, string sufix )
		{
			PreCodeGenerate* error = nullptr;
			if (xj.size() == 1)
			{
				if (isAuxWord(pstore, nlistToString( xj ) ))
				{
					return HeaderPhaseEntry( nlistToString( xj ) );
				}

				if ( pstore->isVerb(nlistToString( xj ) ))
				{
					return HeaderPhaseEntry( nlistToString( xj ) );
				}
			}

			{
				auto h = listToNP( pstore, xj, sufix, &error);
				if (h.empty() == false) return h;
			}


			{
				HMatchExpended ah = getPMatchExpended( "?X Z ?Y", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );

				rss = sorteReverseBy( rss, "Z" );
				for (auto r : rss)
				{
					auto zz = r.getResult( "Z" );
					auto hz = listToComposeNP_2_AB( pstore, zz.literals, sufix );
					if (hz.empty()) continue;
					HeaderPhaseEntry ha ;
					HeaderPhaseEntry hw;

					if (r.getResult( "X" ).size() > 0)
					{
						ha = listToComposeNP_2_AB( pstore, r.getResult( "X" ).literals, sufix );
						if (ha.empty()) continue;
					}
					if (r.getResult( "Y" ).size() > 0)
					{
						hw = listToComposeNP_2_AB( pstore, r.getResult( "Y" ).literals, sufix );
						if (hw.empty()) continue;
					}

					return ha + hz + hw;
				}
			}
			return HeaderPhaseEntry();
		}

		HeaderPhaseEntry listToComposeNP_2( ParserStore *pstore, list<HNoum> xj, string sufix )
		{
			return listToComposeNP_2_AB( pstore, xj, sufix );

			HMatchExpended ah = getPMatchExpended( "Z0 ZA ZB ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			PreCodeGenerate* error = nullptr;

			rss = sorteReverseBy( rss, "ZA" );
			rss = sorteReverseBy( rss, "ZB" );


			for (auto r : rss)
			{

				cout << r.getResult( "ZB" ).repr() << endl;
				cout << r.getResult( "ZA" ).repr() << endl;


				//if (r.getResult( "ZB" ).literals.size() <= 1) continue;
				auto hpB = listToNP( pstore, r.getResult( "ZB" ).literals, sufix + "2", &error );
				if (hpB.empty()) continue;
				auto hpA = listToNP( pstore, r.getResult( "ZA" ).literals, sufix + "3", &error );
				if (hpA.empty()) continue;
				HeaderPhaseEntry sel = hpA + hpB;
				SelectorItem* arg0 = getSelector_np( pstore, r.getResult( "Z0" ).repr(), sufix + "1", &error );
				if (arg0 == nullptr) continue;
				sel.header = sufix + "1" + " " + sel.header;
				sel.args.push_back( arg0 );
				return sel;

			}
			return HeaderPhaseEntry();
		}

		HeaderPhaseEntry listToComposePreposition_3( ParserStore *pstore, list<HNoum> xj, string sufix   )
		{
			HMatchExpended ah = getPMatchExpended( "Z0 ZA ZB ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			PreCodeGenerate* error = nullptr;

			rss = sorteReverseBy( rss, "ZA" );
			rss= sorteReverseBy( rss ,"ZB");


			for (auto r : rss)
			{ 
				//if (r.getResult( "ZB" ).literals.size() <= 1) continue;
				auto hpB = listToVerbEntry( pstore, r.getResult( "ZB" ).literals, sufix+"2", &error );
				if (hpB.empty()) continue; 
				auto hpA = listToVerbEntry( pstore, r.getResult( "ZA" ).literals, sufix+"3", &error );
				if (hpA.empty()) continue; 
				HeaderPhaseEntry sel = hpA + hpB;  
				SelectorItem* arg0 = getSelector_np( pstore, r.getResult( "Z0" ).repr(), sufix + "1" , &error ); 
				if (arg0 ==nullptr) continue; 
				sel.header = sufix + "1" + " " + sel.header; 
				sel.args.push_back( arg0 );  
				return sel;

			}
			return HeaderPhaseEntry();
		}


		HeaderPhaseEntry listToComposePreposition_2( ParserStore *pstore, list<HNoum> xj, string sufix   )
		{

			{
				auto h3 = listToComposeNP_2( pstore, xj, sufix );
				if (h3.header.empty() == false)
				{
					return h3;
				}
			}


			{
				auto h3 = listToComposePreposition_3( pstore, xj, sufix );
				if (h3.header.empty() == false)
				{
					return h3;
				}
			}

			{
				auto h3 = listToComposePreposition_1( pstore, xj, sufix );
				if (h3.header.empty() == false)
				{
					return h3;
				}
			}



			HMatchExpended ah = getPMatchExpended( "VA  ZA  VB  ZB ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			PreCodeGenerate* error = nullptr;
			for (auto r : rss)
			{

				string va_repr = r.getResult( "VA" ).repr();
				string va_name_ref = pstore->getVerbReference( va_repr );
				if (va_name_ref == "")	va_name_ref = va_repr;


				string vb_repr = r.getResult( "VB" ).repr();
				string vb_name_ref = pstore->getVerbReference( vb_repr );
				if (vb_name_ref == "")	vb_name_ref = vb_repr;



				HeaderPhaseEntry sel;
				sel.header = va_name_ref + " " + sufix + "1" + " " + vb_name_ref + " " + sufix + "2";


				string za = r.getResult( "ZA" ).repr();
				string zb = r.getResult( "ZB" ).repr();




				list< SelectorItem*> args1 = getArguments( pstore, za, sufix + "1" );
				if (args1.empty()) continue;

				cout << "    " << va_name_ref << endl;
				cout << "    " << za << endl;
				cout << "    " << vb_name_ref << endl;
				cout << "    " << zb << endl;


				list< SelectorItem*> args2 = getArguments( pstore, zb, sufix + "2" );
				if (args2.empty()) continue;


				sel.args.insert( sel.args.end(), args1.begin(), args1.end() );
				sel.args.insert( sel.args.end(), args2.begin(), args2.end() );

				return sel;

			}
			return HeaderPhaseEntry();
		}


		HeaderPhaseEntry prepositionSelector( ParserStore *pstore, list<HNoum> xj, string argument, PreCodeGenerate** error )
		{

			HMatchExpended ah = getPMatchExpended( "P   Sy   ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			//PreCodeGenerate* error = nullptr;

			rss = sorteReverseBy( rss, "Sy" );
			for (auto r : rss)
			{

				string sy = r.getResult( "Sy" ).repr();
				list< SelectorItem*> args1 = getArguments( pstore, sy, argument );
				if (args1.empty()) continue;
				if (pstore->isPreposition( r.getResult( "P" ).repr() ) == false) continue; 
				HeaderPhaseEntry PHentry;
				PHentry.header = r.getResult( "P" ).repr() + " " + argument;
				PHentry.args = args1;
				return PHentry;

			}

			{
				auto seletor = getSelector_np( pstore, nlistToString( xj ), argument, error );
				if (seletor != nullptr)
				{
					HeaderPhaseEntry PHentry;
					PHentry.header =   argument;
					PHentry.args = { seletor };
					return PHentry;
				}
			}

			return HeaderPhaseEntry();
		}

		// NP
		HeaderPhaseEntry listToVerbPreposition( ParserStore *pstore, list<HNoum> xj,   string sufix,PreCodeGenerate** error   )
		{
			 

			{
				HMatchExpended ah = getPMatchExpended( "Sx   Sy   ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
				//PreCodeGenerate* error = nullptr;

				rss = sorteReverseBy( rss, "Sy" );
				for (auto r : rss)
				{
					auto h2 = prepositionSelector( pstore, r.getResult( "Sy" ).literals, sufix + "2", error );
					if (h2.header.empty()) continue;

					auto h1 = prepositionSelector( pstore, r.getResult( "Sx" ).literals, sufix + "1", error );
					if (h1.header.empty()) continue;
 
					HeaderPhaseEntry PHentry;
					PHentry.header = h1.header + " " + h2.header;
					PHentry.args = h1.args;
					PHentry.args.insert( PHentry.args.end(), h2.args.begin(), h2.args.end() );

					return PHentry; 
				}
				//aceita que Sx é um termo auxiliar
				for (auto r : rss)
				{
					auto ls = r.getResult( "Sy" ).repr();
					auto h2 = prepositionSelector( pstore, r.getResult( "Sy" ).literals, sufix + "2", error );
					if (h2.header.empty()) continue;

	 


					HeaderPhaseEntry PHentry;
					PHentry.header = r.getResult( "Sx" ).repr()+ " " + h2.header;
					PHentry.args = h2.args;
					

					return PHentry;
				}
			}

			auto seletor_kd = getSelector( pstore, nlistToString( xj ), sufix + "1", error );
			if (seletor_kd != nullptr)
			{
				HeaderPhaseEntry PHentry;
				PHentry.header = seletor_kd->target;
				PHentry.args.push_back( seletor_kd );
				return PHentry;

			}
			return HeaderPhaseEntry();
		}

		// NP = PP + NN  ou  NP = NN
		HeaderPhaseEntry listToNP( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error )
		{			
			 
			//eh um seletor puro ?
			{				
				auto np = nlistToString( xj );
				cout << np << endl;
				auto seletor = getSelector_np( pstore, nlistToString( xj ), sufix + "1", error );
				if (seletor != nullptr)				
				{
					HeaderPhaseEntry PHentry;
					PHentry.header = sufix + "1";
					PHentry.args.push_back( seletor );
					return PHentry;
				}
				cout << "NOT "<< np << endl;
			}



			// QQcoisa + Seletor
			//{
			//	HMatchExpended ah = getPMatchExpended( "Z ( X - K ) ", pstore->grammar() );
			//	list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			//	for (auto r : rss)
			//	{
			//		auto hsel = listToXKSeletor( pstore, r.getResult( "X" ).literals, r.getResult( "K" ).literals, sufix, error );
			//		if (hsel.empty()) return HeaderPhaseEntry(); 
			//		return hsel;
			//	}
			//}


			// QQcoisa + Seletor
			//{
			//	HMatchExpended ah = getPMatchExpended( "Ad[\\W] ( X - K ) ", pstore->grammar() );
			//	list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );				
			//	for (auto r : rss)
			//	{
			//		auto hsel = listToXKSeletor( pstore, r.getResult( "X" ).literals, r.getResult( "K" ).literals , sufix, error );
			//		if (hsel.empty()) return HeaderPhaseEntry(); 
			//		hsel.header = r.getResult( "Ad" ).repr() + " " + hsel.header;					
			//		return hsel; 
			//	}
			//}

			{
				// P + NN
				HMatchExpended ah = getPMatchExpended( "PP[\\W] Np ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
				rss = sorteReverseBy( rss, "Np" );
				for (auto r : rss)
				{
					string pp_repr = r.getResult( "PP" ).repr();
					if (pstore->isPreposition( pp_repr ) == false) continue;
					string sx = r.getResult( "Np" ).repr();
					auto hpp = listToNP( pstore, r.getResult( "Np" ).literals, sufix, error );
					if( hpp.empty() ) continue;					 
					hpp.header = pp_repr +" " + hpp.header;					 
					return hpp;
				}
			}


			{
				auto np = nlistToString( xj );
				auto seletor = getSelector_np( pstore, nlistToString( xj ), sufix + "1", error );
				if (seletor != nullptr)
				{
					HeaderPhaseEntry PHentry;
					PHentry.header = sufix + "1";
					PHentry.args.push_back( seletor );
					return PHentry;
				}
			}



			{
				// NN + P + NN
				HMatchExpended ah = getPMatchExpended( "Np PP[\\W] Nq ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
				rss = sorteReverseBy( rss, "Nq" );
				for (auto r : rss)
				{
					string pp_repr = r.getResult( "PP" ).repr();
					if (pstore->isPreposition( pp_repr ) == false) continue; 
	 
					if (pstore->isPreposition( r.getResult( "Np" ).repr() ) == true) continue;

					auto hp = listToNP( pstore, r.getResult( "Np" ).literals, sufix+"1", error );
					if (hp.empty()) continue; 
		 
					auto hq = listToNP( pstore, r.getResult( "Nq" ).literals, sufix+"2", error );
					if (hq.empty()) continue;

					hq.header = pp_repr +" "+ hq.header;

					hp = hp + hq;
					
					return hp;

				}
			}


			// NP = NN
			//Apenas um seletor simples ?

			//{
			//	auto np = nlistToString( xj );
			//	auto seletor = getSelector_np( pstore, nlistToString( xj ), sufix + "1", error );
			//	if (seletor == nullptr)
			//	{
			//		if (*error == nullptr) *error = new   PreCodeGenerateDependency( np );
			//		return HeaderPhaseEntry();
			//	}
			//	HeaderPhaseEntry PHentry;
			//	PHentry.header =  sufix + "1";
			//	PHentry.args.push_back( seletor );
			//	return PHentry;
			//}


			return HeaderPhaseEntry();

		}

		HeaderPhaseEntry listToVerbOnly( ParserStore * pstore, list<HNoum> xj, string sufix, PreCodeGenerate ** error )
		{
			if (xj.size() == 1)
			{
				string vrep = nlistToString( xj );
				if (pstore->isVerb( vrep ))
				{
					HeaderPhaseEntry e;
					e.header = vrep;
					return e;
				}
			}

			HMatchExpended ah = getPMatchExpended( "V[\\W] Ax[\\W] ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );

			rss = sorteReverseBy( rss, "Ax" );
			//PreCodeGenerate* error = nullptr;
			for (auto r : rss)
			{
				string vv_repr = r.getResult( "V" ).repr();
				if (pstore->isVerb( vv_repr ) == false) continue;

				string ax_repr = r.getResult( "Ax" ).repr();
				HeaderPhaseEntry e;
				e.header = vv_repr+ " "+ ax_repr;
				return e;

			}

			return HeaderPhaseEntry();
		}














		//VP = V + (VP  || NP )
		HeaderPhaseEntry listToVerbEntry( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error )
		{
			cout << nlistToString( xj ) << endl;

			{HMatchExpended ah = getPMatchExpended( "X - K", pstore->grammar() );
			   list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			   if (rss.empty() == false) return HeaderPhaseEntry();
			}

			{
			 
				HeaderPhaseEntry vp_entry = listToNP( pstore, xj, sufix + "0", error );
				if (vp_entry.empty() == false) return vp_entry;
			}
			//VP = V + VP + NP )

//sem preposicao
			{
				HMatchExpended ah = getPMatchExpended( "V[\\W] Vp Np ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );

				rss = sorteReverseBy( rss, "Np" );
				//PreCodeGenerate* error = nullptr;
				for (auto r : rss)
				{

					string vv_repr = r.getResult( "V" ).repr();
					if (pstore->isVerb( vv_repr ) == false) continue;


					HeaderPhaseEntry np_entry = listToNP( pstore, r.getResult( "Np" ).literals, sufix + "1", error );
					if (np_entry.empty()) continue;


					HeaderPhaseEntry vp_entry = listToVerbEntry( pstore, r.getResult( "Vp" ).literals, sufix + "0", error );
					if (vp_entry.empty()) continue;


					HeaderPhaseEntry PHentry = vp_entry + np_entry;
					PHentry.header = vv_repr + " " + PHentry.header;

					return PHentry;

				}
			}



			//VP = V  + NP )
			{
				HMatchExpended ah = getPMatchExpended( "V[\\W] Np ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );

				rss = sorteReverseBy( rss, "Np" );
				//PreCodeGenerate* error = nullptr;
				for (auto r : rss)
				{
					string vv_repr = r.getResult( "V" ).repr();
					if (pstore->isVerb( vv_repr ) == false) continue;
					
					HeaderPhaseEntry np_entry = listToNP( pstore, r.getResult( "Np" ).literals, sufix, error );
					if (np_entry.empty()) continue;


					np_entry.header = vv_repr + " " + np_entry.header;
					return np_entry;


					//HeaderPhaseEntry vp =listToVerbEntry( pstore, r.getResult( "V" ).literals, sufix + "0", error );
					//if (vp.empty()) continue; 
 
				}

			}

			//VP = V + VP + NP )

			//sem preposicao
			{
				HMatchExpended ah = getPMatchExpended( "V[\\W] Vp Np ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
				rss = sorteReverseBy( rss, "Np" );
				//PreCodeGenerate* error = nullptr;
				for (auto r : rss)
				{
					string vv_repr = r.getResult( "V" ).repr();
					if (pstore->isVerb( vv_repr ) == false) continue;
					HeaderPhaseEntry np_entry = listToNP( pstore, r.getResult( "Np" ).literals , sufix + "1", error );
					if (np_entry.empty()) continue;
					HeaderPhaseEntry vp_entry = listToVerbEntry( pstore, r.getResult( "Vp" ).literals, sufix + "0", error );
					if (vp_entry.empty()) continue;
					HeaderPhaseEntry PHentry = vp_entry + np_entry;
					PHentry.header = vv_repr + " " + PHentry.header; 
					return PHentry;					
				}
			}
	 

			//VP = aux1 + aux2 + NP
			{
				HMatchExpended ah = getPMatchExpended( "Va[\\W] Vb[\\W] Np ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
				rss = sorteReverseBy( rss, "Np" );
				//PreCodeGenerate* error = nullptr;
				for (auto r : rss)
				{
					string va_repr = r.getResult( "Va" ).repr();
					string vb_repr = r.getResult( "Vb" ).repr();
					if (va_repr.find( "(" ) != string::npos) continue;
					if (vb_repr.find( "(" ) != string::npos) continue;

					cout << r.repr() << endl;

					HeaderPhaseEntry np_entry = listToNP( pstore, r.getResult( "Np" ).literals, sufix + "1", error );
					if (np_entry.empty()) continue;
 
					HeaderPhaseEntry PHentry =  np_entry;
					PHentry.header = va_repr + " "+vb_repr + " " + PHentry.header;
					return PHentry;
				}
			}



			//sem preposicao .. apenas NP


			////nao tem NP .. entao deve ser tudo valido como verbo
			//{
			//	auto v_repr = nlistToString( xj );
			//	HeaderPhaseEntry PHentry;
			//	PHentry.header = v_repr   ; 
			//	return PHentry;
			//}

			return HeaderPhaseEntry();

		}

		HeaderPhaseEntry listToArgumentsEntryPatthen_variableSelector(ParserStore *pstore, list<HNoum> xj, string sufix , PreCodeGenerate** error )
		{ 
			{
				string terms = nlistToString( xj );
				auto seletor = getSelector( pstore, terms, sufix+"1", error );
				if (seletor != nullptr)
				{
					HeaderPhaseEntry sel;
					sel.header = sufix + "1";
					sel.args.push_back( seletor );
					return sel;
				}
			}

			//string x = expand(xj);
			HMatchExpended ah = getPMatchExpended("XA - K", pstore->grammar());
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine(xj, ah);

			

			for (auto r : rss)
			{
				HeaderPhaseEntry sel;
				string xa = r.getResult("XA").repr();
				sel.header = sufix+"1";
				    cout << r.repr() << endl;
					//string xa = r.getResult("XA").repr();
					 
					string akind = r.getResult("K").repr();
					auto k_sel= getSelector(pstore, akind,  error);
					if (k_sel == nullptr)
					{
						 
						if (error != nullptr)
						{
							*error = new   PreCodeGenerateDependency( akind + " cannot be used as Selector" );
						}
						//error = new   PreCodeGenerateDependency( akind );
						continue;
						//kind_ref = akind;
					}
					k_sel->target = "$k_" + pstore->next_temp();

					sel.args.push_back(new  SelectorUnify( sufix + "1", xa ) );
					sel.args.back()->next = (new SelectorUnify( xa, k_sel->target ) );
					sel.args.back()->next->next = k_sel;
					
					return sel;
			}
			if (rss.empty())
			{
				{
					//raw Selector
					string raw_selector = nlistToString( xj );
					HeaderPhaseEntry sel;
					sel.header = raw_selector;
					return sel;

				}
			}

			return HeaderPhaseEntry();
		}


 



 

		class ArgumentSelector
		{
		public:
			string variable;
			string Selector;

			ArgumentSelector(string _variable, string _Selector):variable(_variable),Selector(_Selector){ }
		};

		ArgumentSelector split_argument(ParserStore *pstore, string x)
		{
			HMatchExpended ah = getPMatchExpended("XA - XB", pstore->grammar());
			std::list<MatchResultStr> rss = MatchThis(x, ah);

			for (auto rs : rss)
			{
				string h1 = rs.bind["XA"].repr();
				if (tolower(h1[0]) == h1[0])
				{
					h1 = "$" + h1;
				}
				string h2 = rs.bind["XB"].repr();
				return 	ArgumentSelector(h1, h2);
			}
			return 	ArgumentSelector("", "");
		}


		bool isSameNoum(string a, string b)
		{
			if (a.size() != b.size()) return false;
			size_t n = a.size();
			for (size_t i = 0; i < n; ++i)
			{
				if (towlower(a[i]) != towlower(b[i])) return false;
			}
			return true;
		}


 

		void replaceAll(string &s, const string &search, const string &replace) {
			for (size_t pos = 0; ; pos += replace.length()) {
				// Locate the substring to replace
				pos = s.find(search, pos);
				if (pos == string::npos) break;
				// Replace by erasing and inserting
				s.erase(pos, search.length());
				s.insert(pos, replace);
			}
		}

		list<string> get_list_comma(ParserStore *pstore, string slist)
		{
			replaceAll(slist, ",", " , ");
			HMatchExpended ah = getPMatchExpended("XA , XB  ", pstore->grammar());
			std::list<MatchResultStr> rss = MatchThis(slist, ah);

			for(auto rs : rss)
			{
				auto h1 =  (rs.bind["XA"].repr());
				auto h2 =  (rs.bind["XB"].repr());

				if (h2.find(',') != string::npos)
				{
				 auto h2_list =	get_list_comma(pstore, h2);
				 h2_list.push_front(h1);
				 return h2_list;
				}
				else
				{
					return list<string>({ h1,h2 });
				}
			}
			return {};
		}
		list<string> get_list_or(ParserStore *pstore, string slist)
		{
			HMatchExpended ah = getPMatchExpended("X or Y ", pstore->grammar());
			std::list<MatchResultStr> rs = MatchThis(slist, ah);

			if (rs.empty() == false)
			{
				auto head = rs.front().bind["X"];
				auto rem = rs.front().bind["Y"];

				auto s_head =  (head.repr());
				auto s_rem =  (rem.repr());
				if (s_head.find(',') != string::npos)
				{
					list<string> cs = get_list_comma(pstore, s_head);
					cs.push_back(s_rem);
					return cs;
					 
				}
				else
				{
					return list<string>({ s_head , s_rem });
				} 
			}
			return list<string>({ slist }); 
		}


		list<string> get_list_and(ParserStore *pstore, string slist)
		{
			HMatchExpended ah = getPMatchExpended("X and Y ", pstore->grammar());
			std::list<MatchResultStr> rs = MatchThis(slist, ah);

			if (rs.empty() == false)
			{
				auto head = rs.front().bind["X"];
				auto rem = rs.front().bind["Y"];

				auto s_head = listToString(head.literals);
				auto s_rem = listToString(rem.literals);

				if (s_head.find(',') != string::npos)
				{
					list<string> cs =  get_list_comma(pstore,  s_head);
					cs.push_back(s_rem);
					return cs;
				}
				else
				{
					return list<string>({ s_head , s_rem });
				}
			}
			return list<string>({ slist });
		}



		string removeArticle(ParserStore *pstore, string compound)
		{
			auto i = compound.find( " " );
			if ( i == string::npos ) return compound;

			string art = compound.substr( 0, i );
			if (pstore->isArticle(art))
				return compound.substr( i+1, compound.size() );
			return compound;

			//HMatchExpended ah = getPMatchExpended("?X[a/an/some/the] Y ", pstore->grammar());	 
			//std::list<MatchResultStr> rs = MatchThis(compound, ah);
			//if (rs.empty() == false)
			//{	 
			//	auto rem = rs.front().bind["Y"];
			//	return listToString(rem.literals);
			//}
			//return compound;
		}

 

		string  isCompound(ParserStore *pstore, string kindname, PreCodeGenerate** error)
		{

			{
				auto kvs = getMatchs( pstore, kindname, "Kind K" );
				for (auto kv : kvs)
				{
					auto atype = kv.getResult( "K" ).repr();
					string subKind = get_kind_reference( pstore, atype, error );
					if (subKind != "")		return  pstore->mangleg( "kind", subKind );
				}
			}


			{
				auto kvs = getMatchs( pstore, kindname, "list of K" );
				for (auto kv : kvs)
				{
					auto atype = kv.getResult( "K" ).repr();
					string subKind = get_kind_reference( pstore, atype, error );
					if (subKind != "")	return  pstore->mangleg( "list", subKind );
				}
			}


			{
				auto kvs = getMatchs( pstore, kindname, "value of kind K" );
				for (auto kv : kvs)
				{
					auto atype = kv.getResult( "K" ).repr();
					string subKind = get_kind_reference( pstore, atype, error );
					if (subKind != "")	return  pstore->mangleg( "value", subKind );
				}
			}

			{
				auto kvs = getMatchs( pstore, kindname, "values of kind K" );
				for (auto kv : kvs)
				{
					auto atype = kv.getResult( "K" ).repr();
					string subKind = get_kind_reference( pstore, atype, error );
					if (subKind != "")	return  pstore->mangleg( "set", pstore->mangleg( "value", subKind ) );
				}
			}


			{
			 
				std::list<MatchResultStr> rs = getMatchs( pstore, kindname, "action applying to ?one X " );
				if (rs.empty() == false)
				{
					auto atype = rs.front().bind["X"].repr();
					string subKind = get_kind_reference(pstore, atype, error);

					if (*error != nullptr) return "";
					if (subKind != "")	return  pstore->mangleg( "action" , subKind );
				}
			}

			{
				auto kds = getMatchs( pstore, kindname, "relation of X to Y" );
				for (auto ks : kds)
				{
					auto atype_x = ks.getResult( "X" ).repr();
					auto atype_y = ks.getResult( "Y" ).repr();
					string subKind_x = get_kind_reference( pstore, atype_x, error );
					if (subKind_x == "") continue;
					string subKind_y = get_kind_reference( pstore, atype_y, error );
					if (subKind_y == "") continue;
					return pstore->mangleg( "relation", subKind_x, subKind_y );
				}
			}



			//*error = new   PreCodeGenerateDependency( kindname );
			return "";
		}




		GenerateEntry codeGenerateArticle(ParserStore *pstore, PreCodeGenerate* prev_generate, TBlockGroupItemNoum vx)
		{
			CInform::GrammarContext* gtx = pstore->grammar(); 
			HMatchExpended ah = getPMatchExpended("?X[a/an/some/the] Y ", gtx);
			string compound =  (vx.repr());
			std::list<MatchResultStr> rs =  MatchThis(compound, ah); 
			if (rs.empty() == false)
			{
				auto rem = rs.front().bind["Y"];
				if (rs.front().hasBind("X"))
				{
					auto article = rs.front().bind["X"];
					auto p_article = new   PreCodeGenerateIL("Set", "Article", rem.literals.front(), article.literals.front());
					//PreCodeGenerateSession *psess = new   PreCodeGenerateSession("@init " + rem.literals.front());
					//psess->inner->add(new   PreCodeGenerateIL("Set", "Article", rem.literals.front(), article.literals.front()));
					return GenerateEntry(rem.repr(), p_article);
				}
			} 
			return GenerateEntry( compound, new  PreCodeGenerateEmpty());
		}


		GenerateEntry codeGenerateArticle( ParserStore *pstore, PreCodeGenerate* prev_generate, string compound )
		{
			CInform::GrammarContext* gtx = pstore->grammar();
			HMatchExpended ah = getPMatchExpended( "?X[a/an/some/the] Y ", gtx );	 
			std::list<MatchResultStr> rs = MatchThis( compound, ah );
			if (rs.empty() == false)
			{
				auto rem = rs.front().bind["Y"];
				if (rs.front().hasBind( "X" ))
				{
					auto article = rs.front().bind["X"];
					auto p_article = new   PreCodeGenerateIL( "Set", "Article", rem.literals.front(), article.literals.front() );
					//PreCodeGenerateSession *psess = new   PreCodeGenerateSession("@init " + rem.literals.front());
					//psess->inner->add(new   PreCodeGenerateIL("Set", "Article", rem.literals.front(), article.literals.front()));
					return GenerateEntry( rem.repr(), p_article );
				}
			}
			return GenerateEntry( compound, new  PreCodeGenerateEmpty() );
		}



		bool testsForSelector( ParserStore *pstore )
		{
			PreCodeGenerate* error = nullptr;

			auto seletor = getSelector_np( pstore, "(R - relation of values of kind K to Ls)",  "1", &error );
			if (seletor == nullptr)
			{
				throw "Implemetation Error";
			}

			return true;
		}

}
}