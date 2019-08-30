 	
#include "PreCodeGenerate.hpp"
#include "NSource.hpp"
#include <headerPhase.hpp>

#include <cctype>

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

		std::list<MatchResult > getMatchs( ParserStore *pstore,  list<HNoum> xj , string match )
		{
			HMatchExpended ah = getPMatchExpended( match, pstore->grammar() );
			return  CInform::Match::TMatch::MatchLine( xj, ah );
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


		string get_kind_reference(ParserStore *pstore, string vx, PreCodeGenerate** error);


	

		
		//SelectorItem* getSelector( ParserStore *pstore, string val, string sufix, PreCodeGenerate** error );

		SelectorItem* getSelector( ParserStore *pstore, list<HNoum> xj  , string sufix, PreCodeGenerate** error );




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
					cout << "    bool predicate ? " << r.getResult("XB").repr() << endl;
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

		list< SelectorItem*> getArguments( ParserStore *pstore, list<HNoum> val , string argumentName  )
		{
			PreCodeGenerate* error = nullptr;
			list< SelectorItem*> args;
			{
				//HMatchExpended ah = getPMatchExpended( " (X - K) ", pstore->grammar() ); 
				//list< MatchResultStr  > rss = CInform::Match::MatchThis( val, ah );
				
				auto rss  = getMatchs( pstore, val, " (X - K) " );
	 
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
				string txt = nlistToString( val );
			        if (pstore->isInstance( txt ))
					{
						args.push_back( new SelectorUnify( argumentName, txt ) );
						return args;
					}  
			} 

			return args;
		}


		SelectorItem* getSelector( ParserStore *pstore  , list<HNoum>  val,   PreCodeGenerate** error )
		{

			if (nlistToString(val) == "something")
			{
				*error = nullptr;
				return new SelectorUnify( "$", "thing" );
			}

			{
				//(name of kind of value K)
				auto nss = getMatchs( pstore, val, "X or Y " );

				for (auto r : nss)
				{					
					auto tmp1 = "$x" + pstore->next_temp();
					auto sx = getSelector( pstore, r.getResult( "X" ).literals, tmp1, error );
					if (sx != nullptr)
					{
						auto tmp2 = "$x" + pstore->next_temp();
						auto sy = getSelector( pstore, r.getResult( "Y" ).literals, tmp2, error );
						if (sy !=nullptr)
						{
							*error = nullptr;
						   return 	new SelectorOr( "$",  sx , sy);
						}
						
					}
				}
			}




			{
				//(name of kind of value K)
			auto nss = getMatchs( pstore, val, "list of K" );
				for (auto r : nss)
				{
					auto akind = r.getResult( "K" ).literals;
					auto sk = getSelector( pstore, akind, error );
					if (sk !=nullptr)
					{
						if (sk->target == "$")
						{
							sk->target = "$k_"+pstore->next_temp();
						}
						auto s1 = new SelectorUnify( "$", "list_of<" + sk->target + ">" );
						s1->next = sk;
						*error = nullptr;
						return s1;
					}
				}
			}



			{
				//(name of kind of value K)
				auto nss = getMatchs( pstore, val, "name of kind of value K" );
				for (auto r : nss)
				{
					string akind = r.getResult( "K" ).repr();
					if (pstore->isImplicityVar( akind ))
					{
						auto s1 = new SelectorUnify( "$", "name_of<" + akind + ">" );
						auto s2 = new SelectorKind( akind, "kind_of_value" );
						s1->next = s2;
						*error = nullptr;
						return s1;
					}
				}
			}

			auto rss = getMatchs( pstore, val, "X - K" );
			for (auto r : rss)
			{
				auto akind = r.getResult( "K" ).literals;
				string atarget = r.getResult( "X" ).repr();
				auto sel = getSelector( pstore, akind, error );
				if (sel !=nullptr)
				{
					*error = nullptr;

					sel->target = "$k_" + pstore->next_temp();
					auto s1=  new SelectorKind( atarget, sel->target );
					s1->next = sel;
					return s1;
				}
				
			} 

			for (auto r : rss)
			{
				string akind = r.getResult( "K" ).repr();
				string atarget = r.getResult( "X" ).repr();
				if (pstore->isImplicityVar( akind ))
				{
					akind = pstore->mangleg( akind );
					*error = nullptr;
					return new SelectorUnify( atarget, akind );
				}
			}

			{
				string kind_ref = get_kind_reference( pstore, nlistToString(val), error );
				if (kind_ref != "")
				{
					*error = nullptr;
					return new SelectorKind( "$", kind_ref );					 
				}
				 
			} 
			return nullptr;
		}


		SelectorItem* getSelector_np( ParserStore *pstore, list<HNoum> val, PreCodeGenerate** error )
		{
			auto sel = getSelector( pstore, val, error );
			if (sel != nullptr) return sel;
			
			{
				string inst_ref = get_instance_reference( pstore, nlistToString(val), error );
				if (inst_ref != "")
				{
					*error = nullptr;
					return new SelectorKind( "$", inst_ref );
				} 
			}
			return nullptr;
		}


		SelectorItem* getSelector( ParserStore *pstore, list<HNoum> xj, string avar, PreCodeGenerate** error )
		{
			auto sel = getSelector( pstore, xj, error );
			if (sel != nullptr)
			{
				if (sel->target != "$")
				{
					auto sa = new SelectorUnify( avar, sel->target );
					sa->next = sel;
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



		//SelectorItem* getSelector( ParserStore *pstore, string  val, string avar, PreCodeGenerate** error )
		//{
		//	//auto sel = getSelector( pstore, val, error );
		//	//if (sel != nullptr)
		//	//{
		//	//	if (sel->target != "$")
		//	//	{
		//	//		auto sa = new SelectorUnify( avar, sel->target );
		//	//		sa->next = sel;					
		//	//	}
		//	//	else
		//	//	{
		//	//		sel->target = avar;
		//	//	}
		//	//	*error = nullptr;
		//	//	return sel;
		//	//}
		//	throw "not implemented";
		//	return nullptr;
		//}


		SelectorItem* getSelector_np( ParserStore *pstore, list<HNoum>  val, string avar, PreCodeGenerate** error )
		{
			auto sel = getSelector_np( pstore, val, error );
			if (sel != nullptr)
			{
				if (sel->target != "$")
				{
					auto sa = new SelectorUnify( avar, sel->target );
					sa->next = sel;
				}
				else
				{
					sel->target = avar;
				}
				return sel;
			}
			return nullptr;
		}

		HeaderPhaseEntry noumSelector( ParserStore *pstore, list<HNoum> xj, string argument, PreCodeGenerate** error )
		{ 
	 
			{ 
				auto rss = getMatchs( pstore, xj, " (X - K) " );

				for (auto r : rss)
				{
					string akind = r.getResult( "K" ).repr();
					string kind_ref;
					if (isMuteVariable( akind ))
					{
						kind_ref = akind;
					}
					else
					{
						kind_ref = get_kind_reference( pstore, akind,  error );
						if (kind_ref == "")
						{
							*error = new   PreCodeGenerateDependency( akind ); 
							kind_ref = akind;
						}
					}
					HeaderPhaseEntry PHentry;
					PHentry.header = argument;
					string xa = r.getResult( "X" ).repr();
					PHentry.args.push_back( new SelectorUnify( argument, xa ) );
					PHentry.args.back()->next = new SelectorUnify( xa, kind_ref );
					return PHentry;
				}
			}


			{
				string txt = nlistToString( xj );
				if (pstore->isInstance( txt ))
				{
					HeaderPhaseEntry PHentry;
					PHentry.args.push_back( new SelectorUnify( argument, txt ) );
					return PHentry;
				}
			}

			return HeaderPhaseEntry(); 

 
		}

		HeaderPhaseEntry listToComposePredicate_3( ParserStore *pstore, list<HNoum> xj, string sufix , PreCodeGenerate** error )
		{
			HMatchExpended ah = getPMatchExpended( "Z0[\\W]  ZA   ZB ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			rss = sorteReverseBy( rss, "ZB" );
	 
			for (auto r : rss)
			{

				//string va_repr = r.getResult( "ZA" ).repr();
				//string va_name_ref = pstore->getVerbReference( va_repr );
				//if (va_name_ref == "")	va_name_ref = va_repr;


				//string vb_repr = r.getResult( "ZB" ).repr();
				//string vb_name_ref = pstore->getVerbReference( vb_repr );
				//if (vb_name_ref == "")	vb_name_ref = vb_repr;



				HeaderPhaseEntry sel;
				 

				auto z0 = r.getResult( "Z0" );
				auto za = r.getResult( "ZA" );
				auto zb = r.getResult( "ZB" );


				auto h2 = noumSelector( pstore, zb.literals, sufix + "3" , error );
				if (h2.empty()) continue;

				auto h1 = noumSelector( pstore, za.literals, sufix + "2", error );
				if (h1.empty()) continue;


				SelectorItem* arg0 = getSelector( pstore, z0.literals, sufix + "1" , error );
				if (arg0 ==nullptr) continue;
				 

				sel.header = z0.repr()+" "+  h1.header  + " " + h2.header; 

			 
				sel.args.insert( sel.args.end(), h1.args.begin(), h1.args.end() );
				sel.args.insert( sel.args.end(), h2.args.begin(), h2.args.end() );

				return sel;

			}
			return HeaderPhaseEntry();
		}


		HeaderPhaseEntry listToComposePredicate_2( ParserStore *pstore, list<HNoum> xj, string sufix , PreCodeGenerate** error )
		{
			{
				auto h3 = listToComposePredicate_3( pstore, xj, sufix , error );
				if (h3.header.empty() == false)
				{
					return h3;
				}
			}

			HMatchExpended ah = getPMatchExpended( "VA  ZA  VB  ZB ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
		 
			for (auto r : rss)
			{

				auto va_repr = r.getResult( "VA" );
				string va_name_ref = pstore->getVerbReference( va_repr.repr() );
				if (va_name_ref == "")	va_name_ref = va_repr.repr();


				auto vb_repr = r.getResult( "VB" );
				string vb_name_ref = pstore->getVerbReference( vb_repr.repr() );
				if (vb_name_ref == "")	vb_name_ref = vb_repr.repr();



				HeaderPhaseEntry sel;
				sel.header = va_name_ref + " " + sufix + "1" + " " + vb_name_ref + " " + sufix + "2";


				auto za = r.getResult( "ZA" ) ;
				auto zb = r.getResult( "ZB" ) ;




				list< SelectorItem*> args1 = getArguments( pstore, za.literals, sufix + "1" );
				if (args1.empty()) continue;

				cout << "    " << va_name_ref << endl;
				cout << "    " << za.repr() << endl;
				cout << "    " << vb_name_ref << endl;
				cout << "    " << zb.repr() << endl;


				list< SelectorItem*> args2 = getArguments( pstore, zb.literals, sufix + "2" );
				if (args2.empty()) continue;


				sel.args.insert( sel.args.end(), args1.begin(), args1.end() );
				sel.args.insert( sel.args.end(), args2.begin(), args2.end() );

				return sel;

			}
			return HeaderPhaseEntry();
		}




		HeaderPhaseEntry predicativeSelector( ParserStore *pstore, list<HNoum> xj, string argument, PreCodeGenerate** error )
		{

			HMatchExpended ah = getPMatchExpended( "P   Sy   ", pstore->grammar() );
			list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
			//PreCodeGenerate* error = nullptr;

			rss = sorteReverseBy( rss, "Sy" );
			for (auto r : rss)
			{
				auto sy = r.getResult( "Sy" );
				list< SelectorItem*> args1 = getArguments( pstore, sy.literals, argument );
				if (args1.empty()) continue;

				HeaderPhaseEntry PHentry;
				PHentry.header = r.getResult( "P" ).repr()+ " "+ argument;
				PHentry.args = args1;
				return PHentry;
			}

			{
				auto seletor = noumSelector( pstore,   xj  , argument, error );
				if (seletor.empty() == false) return seletor;
				
			}

			return HeaderPhaseEntry();
		}

		// NP
		HeaderPhaseEntry listToVerbPredicate( ParserStore *pstore, list<HNoum> xj,   string sufix,PreCodeGenerate** error   )
		{

			auto seletor_kd = getSelector( pstore,  xj , sufix + "1", error );
			if (seletor_kd != nullptr)
			{
				HeaderPhaseEntry PHentry;
				PHentry.header = seletor_kd->target;
				PHentry.args.push_back( seletor_kd );
				return PHentry;
				
			}


			{
				HMatchExpended ah = getPMatchExpended( "Sx   Sy   ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
				//PreCodeGenerate* error = nullptr;

				rss = sorteReverseBy( rss, "Sy" );
				for (auto r : rss)
				{
					auto h2 = predicativeSelector( pstore, r.getResult( "Sy" ).literals, sufix + "2", error );
					if (h2.header.empty()) continue;


					auto h1 = listToVerbPredicate( pstore, r.getResult( "Sx" ).literals, sufix + "1", error );					
					if (h1.header.empty() == false)
					{
						HeaderPhaseEntry PHentry;
						PHentry.header = h1.header + " " + h2.header;
						PHentry.args = h1.args;
						PHentry.args.insert( PHentry.args.end(), h2.args.begin(), h2.args.end() );
						return PHentry;
					}

				}


				//aceita que Sx é um termo auxiliar
				for (auto r : rss)
				{
					auto ls = r.getResult( "Sy" ).repr();
					auto h2 = predicativeSelector( pstore, r.getResult( "Sy" ).literals, sufix + "2", error );
					if (h2.header.empty()) continue;

	 


					HeaderPhaseEntry PHentry;
					PHentry.header = r.getResult( "Sx" ).repr()+ " " + h2.header;
					PHentry.args = h2.args;
					

					return PHentry;
				}
			}
			return HeaderPhaseEntry();
		}


		//VP = V + (VP  || NP )
		HeaderPhaseEntry listToVerbEntry( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error )
		{
			{
				HMatchExpended ah = getPMatchExpended( "V PP[\\W] Np ", pstore->grammar() );
				list< MatchResult  > rss = CInform::Match::TMatch::MatchLine( xj, ah );
				//PreCodeGenerate* error = nullptr;
				for (auto r : rss)
				{
					auto  sx = r.getResult( "Np" ) ;
					auto seletor = getSelector_np( pstore, sx.literals, sufix + "1", error );
					if (seletor == nullptr)
					{
						if (*error == nullptr) *error = new   PreCodeGenerateDependency( sx.repr() );
						continue;
					}
					string pp_repr = r.getResult( "PP" ).repr();
					string v_repr = r.getResult( "V" ).repr();
					string v_name = pstore->getVerbReference( v_repr );
					if (v_name == "") 	v_name = v_repr;
					HeaderPhaseEntry PHentry;
					PHentry.header = v_repr + " " + pp_repr + " " + sufix + "1";
					PHentry.args.push_back( seletor );
					return PHentry;
				}
			}

			//nao tem NP .. entao deve ser tudo valido como verbo
			{
				auto v_repr = nlistToString( xj );
				HeaderPhaseEntry PHentry;
				PHentry.header = v_repr   ; 
				return PHentry;
			}

			return HeaderPhaseEntry();

		}

		HeaderPhaseEntry listToArgumentsEntryPatthen_variableSelector(ParserStore *pstore, list<HNoum> xj, string sufix , PreCodeGenerate** error )
		{ 
			{
				 
				auto seletor = getSelector( pstore, xj, sufix+"1", error );
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
					 
					auto akind = r.getResult("K") ;
					auto k_sel= getSelector(pstore, akind.literals,  error);
					if (k_sel == nullptr)
					{

						if(error!=nullptr) *error = new   PreCodeGenerateDependency( akind.repr() + " cannot be used as Selector" );
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

}
}