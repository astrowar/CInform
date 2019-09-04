 

#include "pmatch_baseClass.hpp"
#include <NSource.hpp>
#include <list>

using namespace std;

namespace CInform
{
	namespace Match
	{
	

		list<HNoum> unfold ( ListBlock* a )
		{
			list<HNoum> ret;
			for (auto x : a->inner)
			{
				ret.push_back( make_shared<CodeParser::NSource >( x ));
			}
			return ret;

		}

		string ListSrcRepr( const list<HNoum>& val )
		{
			string s = "";
			bool initial = true;
			for (auto& v : val)
			{
				if (initial)
				{
					s = s + v->repr();
				}
				else {
					s = s + " " + v->repr();
				}

				initial = false;
			}
			return s + "";
		}

		list<PMatch*> reinterpret_list( list<HMatch> a )
		{
			list<PMatch*> s;
			for (auto x : a) 
				s.push_back( x.get() );
			return s;
		}


		MatchResult  MatchThis_item(  const TBlockGroupItemNoum& x,    PMatch* m  )
		{
			if (auto pliteral = dynamic_cast<const PLiteral*>(m))
			{
				if (x.literals.size() == 1)
				{
					if (pliteral->val == "\\W") return MatchResult( true );
					return MatchResult( x.literals.front()->isMatch( pliteral->val ) );
				}
				return MatchResult( false );
			}

			if (auto pvar = dynamic_cast<const PVar*>(m))
			{
				return MatchResult( pvar->name, x );
			}
			 
			if (auto pvarc = dynamic_cast<const PVarCond*>(m))
			{
				auto rs = MatchThis_item( x, pvarc->inner.get() );
				if (rs.result)
				{
					return MatchResult( pvarc->name, x );
				}
				return MatchResult( false );
			}


			if (auto plst = dynamic_cast<const PList*>(m))
			{
				throw "error";
			}


			return MatchResult( false );
		}

		list<MatchResult>  MatchLine_s( list<HNoum>& xs, PMatch*  m );


		void  getCombinatoriaN_partial( MatchResult mAcc, TBlockGroupNoum& acc, std::list<HNoum>& remainder, int n,
			typename list<PMatch*>::const_iterator   mstart, typename list<PMatch*>::const_iterator   mend,
			std::function<bool( const string&, HNoum )> MT,
			std::function<void( MatchResult& )> F );




		void  getCombinatoriaN_partial( MatchResult mAcc, TBlockGroupNoum& acc, std::list<HNoum>& remainder, int n,
			typename list<PMatch*>::const_iterator   mstart, typename list<PMatch*>::const_iterator   mend,
			std::function<void( MatchResult& )> F )
		{
			if (int( remainder.size() ) < n) return;
			if (n == 1)
			{
				PMatch* m = (*mstart) ;
				if (auto pList = dynamic_cast<  PList*>(m))
				{
					MatchResult racc = MatchResult( true );
					list<MatchResult> rss = MatchLine_s( remainder, pList );
				 
					{
						for (auto rj : rss)
						{
							auto rmend = mAcc.join( rj );
							F( rmend );
						}
					}
					return;

				}

				MatchResult rj = MatchThis_item(TBlockGroupItemNoum( remainder ), (*mstart)  );
				if (rj.result == false) return;
				{
					auto rmend = mAcc.join( rj );
					F( rmend );
				}
				return;
			}
			if (mstart == mend) return;
			for (auto it = remainder.begin(); it != remainder.end(); ++it)
			{
				if (it == remainder.begin()) continue; //evita header de tamanho zero
				auto tail = std::list<HNoum>( it, remainder.end() );
				if (int( tail.size() ) < n - 1) continue;
				MatchResult rj = MatchThis_item( TBlockGroupItemNoum( remainder.begin(), it ), (*mstart)  );
				if (rj.result == false) return;
				MatchResult mm = mAcc.join( rj );
				acc.items.emplace_back( remainder.begin(), it );
				getCombinatoriaN_partial( mm, acc, tail, n - 1, std::next( mstart ), mend, F );
				acc.items.pop_back();
			}
		}


		list<MatchResult>  MatchLine_s( list<HNoum>& xs, PMatch*  m )
		{

 
				if (xs.size() == 1)
				{
					auto q = xs.front();
					if (auto nList = dynamic_cast<CodeParser::NSource*>(q.get()))
					{
						auto nitem = nList->item;
						if (auto nsItem = dynamic_cast<ListBlock*>(nitem.get()))
						{
							auto ss = unfold( nsItem );
							return MatchLine_s( ss,m );
						} 
					} 
				}
	 
			

			{

				if (auto pList = dynamic_cast<const PList*>(m))
				{

		/*			if (pList->val.size() == 1)
					{
						return MatchLine_s( xs, pList->val.front().get() );
					}*/

					list<MatchResult> rlist;		 
				    TBlockGroupNoum acc;
					list<PMatch*> vList = reinterpret_list( pList->val );
					list<PMatch*>::const_iterator mstart = vList.begin();
					list<PMatch*>::const_iterator mend = vList.end();
					std::function<void( MatchResult& )> F = [&]( MatchResult& mres )
					{
						if (mres.result)
						{
							rlist.push_back( mres );
						}
					};
					getCombinatoriaN_partial( MatchResult( true ), acc, xs, int( vList.size() ), mstart, mend, F );
					return rlist;
				}
				
			}


			{
				if (xs.size() == 1)
				{ 
					auto tq = TBlockGroupItemNoum( xs );
					MatchResult rj = MatchThis_item( tq,  m  );
					if (rj.result)
					{
						return list<MatchResult>( { rj } );
					} 
				}
			}

			return list<MatchResult>();
		}


	 

		list<MatchResult>  MatchLine_inner( list<HNoum> phase, HMatch  m )
		{
			 
			list<MatchResult> rlist;
			if (auto pLST = dynamic_cast<const PList*>(m.get()))
			{
				list<HMatch> all_maths_combines = ExpandMatch( pLST );
				for (auto mcomb : all_maths_combines)
				{
					auto rlist_sub = MatchLine_s( phase, mcomb.get() );
					rlist.insert( rlist.end(), rlist_sub.begin(), rlist_sub.end() );
				}
			}
			else
			{
				return MatchLine_s( phase, m.get() );
			}
			return rlist;
		}






		list< MatchResult > TMatch::MatchLine( list<HNoum> line_parts, HMatchExpended m )
		{
			list<MatchResult> rlist;
			for (auto& mcomb : m->expended)
			{
				auto rlist_sub = MatchLine_inner( line_parts, mcomb );
				rlist.insert( rlist.end(), rlist_sub.begin(), rlist_sub.end() );
			}
			 
			return rlist;
		}

		list< MatchResult > TMatch::MatchLine(list<HNoum> line_parts, HMatch  mcomb)
		{
			list<MatchResult> rlist;			 
			{
				auto rlist_sub = MatchLine_inner(line_parts, mcomb);
				rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
			}
			return rlist;
		}



		  string MatchResult::repr(   )
		{
			if (this->result == false) { return "FALSE "; }
			string s = "";
			for (auto kv : this->bind)
			{
				//s = s + kv.first + ":" + stringList(kv.second.literals) + " ";
				 
				 
					s = s + kv.first + ":" + kv.second.repr() + " ";
				 
				 

			}
			return "TRUE " + s;
		}

		 
		  
		  bool MatchResult::hasBind(string varName)
		  {
			  auto it = bind.find(varName);

			  if (it != bind.end()) return  true;
			  return false;
		  }

		  TBlockGroupItemNoum MatchResult::getResult( string varName )
		  {
			  auto it = bind.find( varName );

			  if (it != bind.end()) return  it->second;
			  //not found
			  return TBlockGroupItemNoum();
			  
		  }

		  MatchResult MatchResult::join( MatchResult& other )
		{
			if (this->result == false) return this;
			if (other.result == false) return other;
			map<string, TBlockGroupItemNoum> n;
			for (auto kv : this->bind)
			{
				n[kv.first] = kv.second;
			}
			for (auto kv : other.bind)
			{
				n[kv.first] = kv.second;
			}
			MatchResult r = MatchResult( true );
			r.bind = n;
			return r;
		}

		    TBlockGroupItemNoum::TBlockGroupItemNoum(list<HNoum> _literals) : literals(std::move(_literals)) 
			{
			
			}

		    TBlockGroupItemNoum::TBlockGroupItemNoum(typename list<HNoum>::iterator _begin, typename list<HNoum>::iterator _end) : literals(_begin, _end) 
			{
			
			}

		    TBlockGroupItemNoum::TBlockGroupItemNoum() {}

			std::string TBlockGroupItemNoum::repr()
			{
				string s = "";
				if (literals.size() == 0)
				{
					return s;
				}

				if ( literals.size() > 1)
				{
					s =   ListSrcRepr( literals) ;
				}
				else
				{
					s =  literals.front()->repr()   ;
				}
				return s;
			}

			  size_t TBlockGroupItemNoum::size() { return literals.size(); }

		  bool TBlockGroupItemNoum::empty()
		  {
			  return literals.empty();
		  }

		  inline typename list<HNoum>::const_iterator TBlockGroupItemNoum::end() { return literals.end(); }

		  inline typename list<HNoum>::const_iterator TBlockGroupItemNoum::begin() { return literals.begin(); }

}
}


 