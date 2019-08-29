
//Composicao e criacao do matchs diversos

#include "phaseDef.hpp"
#include "utils.hpp"
#include "pmatch.hpp"
#include "pmatch_code.hpp"


#include <iostream>
using namespace CInform::Match;

using   namespace std;

namespace CInform
{
	namespace Match
	{

		 

		std::list<HSourceBlock>   split_code__(HSourceBlock  phase, char sep)
		{
			//segundo nivel do split

			std::list<HSourceBlock>   s;
			if (phase->type == TCODE)
			{
				auto  cx = dynamic_pointer_cast<CodeBlock>(phase);
				std::list<string> sub_txt = CInform::split(cx->txt);
				for (auto xx : sub_txt)
				{
					if (xx != " " && xx != "\t")
					{
						s.push_back( make_shared<CodeWord>( xx ) );
					}
				}
				return s;
			}

			//if (phase->type == TPARENTESIS)
			//{
			//	auto  px = dynamic_pointer_cast<ParentesisSourceBlock>(phase);
			//	auto hs = split_code__( px->inner ,sep);
			//	for (auto xx : hs) s.push_back( xx );
			//	return s;
			//}

			throw "error";
			//todo o resto nao sofre quebra
			s.push_back(phase);
			return s;


		}

		void  split_code( std::list<HSourceBlock> &acc, HSourceBlock  phase, char sep )
		{
		 
			if (phase->type == TCODE)
			{
				for (auto ss : split_code__( phase, sep ))
				{
					acc.push_back( ss );
				}
				return;
			}
			if (phase->type == TLIST)
			{
				auto cx = dynamic_pointer_cast<ListBlock>(phase);
				 
				for (auto inn : cx->inner)
				{
					std::list<HSourceBlock> acc_2;					
					split_code( acc, inn, sep );
					//acc.insert( acc.end(), acc_2.begin(), acc_2.end() );
				}
				return ;
			}
			if (phase->type == TPARENTESIS)
			{
				auto cx = dynamic_pointer_cast<ParentesisSourceBlock>(phase);
				
				std::list<HSourceBlock> acc_2;
				split_code( acc_2, cx->inner , sep );
				auto h = make_shared<ListBlock>( acc_2 );
				acc.push_back( h );
				return;
			}
			if (phase->type == TLITERAL)
			{
				acc.push_back( phase );
				return;
	 
			}


			throw "item not expected";
			return  ;
		}

		std::list<HSourceBlock>   split_code(HSourceBlock  phase, char sep)
		{
			std::list<HSourceBlock>   s;
			split_code( s, phase, sep );
			return s;
			if (phase->type == TCODE)
			{
				return split_code__(phase, sep);

			}
			if (phase->type == TLIST)
			{
				auto cx = dynamic_pointer_cast<ListBlock>(phase);
				if (cx->inner.size() == 1)
				{
					return split_code(cx->inner.front(), sep);
				}
				for (auto inn : cx->inner)
				{
					auto partial = split_code(inn, sep);
					s.push_back( std::make_shared<ListBlock>( partial ));
				}
				return s;
			}

			if (phase->type == TLITERAL)
			{
				s.push_back(phase);
				return s;
			}

			if (phase->type == TPARENTESIS)
			{
				auto px = dynamic_pointer_cast<ParentesisSourceBlock>(phase);
				{
					auto partial = split_code( px->inner, sep );
					s.push_back( std::make_shared<ListBlock>( partial ) );
					return s;
				}

			}


			throw "item not expected";
			return s;
		}

		bool isSameCodeString(const string &a, HSourceBlock b )
		{
			if (b->type == TCODE)
			{
				throw "error, missing pre processing";
			}
			if (b->type == TWORD)
			{
				auto  cx = dynamic_pointer_cast<CodeWord>(b);
				if (cx->txt == "\\W")
				{
					return isSameString( a, cx->txt );
				}

				
				return isSameString(a, cx->txt);
			}


			return false;
        }


		MatchBlockResult  MatchThis_item(const SBlockGroupItem & x, const  PMatch* m)
		{
			if (auto pliteral = dynamic_cast<const PLiteral*>(m))
			{
				if (x.literals.size() == 1)    return MatchBlockResult(isSameCodeString(pliteral->val, x.literals.front()));
				return MatchBlockResult(false);
			}
			if (auto pvar = dynamic_cast<const PVar*>(m))
			{
				return MatchBlockResult(pvar->name, x);
			}
			
			if (auto pvarc = dynamic_cast<const PVarCond*>(m))
			{
				auto r = MatchThis_item(   x , pvarc->inner.get() );
				if (r.result)
				{
					return MatchBlockResult( pvarc->name, x );
				}
				return MatchBlockResult( false );
			}
			return MatchBlockResult(false);
		}




		void  getCombinatoriaN_partial(MatchBlockResult mAcc, SBlockGroup & acc, std::list<HSourceBlock>& remainder, int n, list<HMatch>::const_iterator   mstart, list<HMatch>::const_iterator   mend, std::function<void(MatchBlockResult&)> F)
		{
			if (long int(remainder.size()) < n) return;
			if (n == 1)
			{
				MatchBlockResult rj = MatchThis_item(SBlockGroupItem(remainder), (*mstart).get());
				if (rj.result == false) return;
				{
					auto rmend = mAcc.join(rj);
					F(rmend);
				}				
				return;
			}
			if (mstart == mend) return;
			for (auto it = remainder.begin(); it != remainder.end(); ++it)
			{
				if (it == remainder.begin()) continue; //evita header de tamanho zero
				auto tail = std::list<HSourceBlock>(it, remainder.end());
				if (long int(tail.size()) < n - 1) continue; 
				MatchBlockResult rj = MatchThis_item(SBlockGroupItem(remainder.begin(), it), (*mstart).get());
				if (rj.result == false) return;
				MatchBlockResult mm = mAcc.join(rj);
				acc.emplace_back(remainder.begin(), it);
				getCombinatoriaN_partial(mm, acc, tail, n - 1, std::next(mstart), mend, F);
				acc.pop_back();
			}
		}


		list<MatchBlockResult>  MatchLine_s(HSourceBlock phase, HMatch  m)
		{
			list<MatchBlockResult> rlist;
			std::list<HSourceBlock>  xs = split_code(phase,' ');
			SBlockGroup acc;
			if (auto pList = dynamic_cast<const PList*>(m.get()))
			{
				list<HMatch>::const_iterator mstart = pList->val.begin();
				list<HMatch>::const_iterator mend = pList->val.end();
				getCombinatoriaN_partial(MatchBlockResult(true), acc, xs, pList->val.size(), mstart, mend, [&](MatchBlockResult &mres)
				{
					if (mres.result)
					{
						rlist.push_back(mres);
					}
				});
				//std::transform(xs.begin(), xs.end(), std::back_inserter(rlist), [&]( SSplitedPhase si){ return MatchThis_i( si , m); } );                      
			}

			// apaga o buffer

			return rlist;
		}



		list<MatchBlockResult>  MatchLine_inner(HSourceBlock phase, HMatch  m)
		{
			list<MatchBlockResult> rlist;
			if (auto pLST = dynamic_cast<const PList*>(m.get()))
			{
				list<HMatch> all_maths_combines = ExpandMatch(pLST);
				for (auto mcomb : all_maths_combines)
				{
					
					auto rlist_sub = MatchLine_s(phase, mcomb);
					rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
				}
				//std::transform(all_maths_combines.begin(), all_maths_combines.end(), std::back_inserter(rlist), [&](HMatch  p){ return MatchThis_string( phase , p); } );          
			}
			else
			{
				return MatchLine_s(phase, m);
			}
			return rlist;
		}


		list<MatchBlockResult>  MatchLine(HSourceBlock  line, HMatchExpended  m)
		{
			list<MatchBlockResult> rlist;
			for (auto& mcomb : m->expended)
			{
				auto rlist_sub = MatchLine_inner(line, mcomb);
				rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
			}
			return rlist;
		}


		string ListSrcRepr(list<HSourceBlock> val)
		{
			string s = "(";
			bool initial = true;
			for (auto v : val)
			{
				if (initial)
				{
					s = s +  v->repr();
				}
				else{
					s = s + " " + v->repr();
				}

				initial = false;
			}
			return s +")";
		}

		string MatchBlockResult::repr()
		{
			if (this->result == false) { return "FALSE "; }
			string s = "";
			for (auto kv : this->bind)
			{
				//s = s + kv.first + ":" + stringList(kv.second.literals) + " ";
				s = s + kv.first + ":" + ListSrcRepr(kv.second.literals)+ " ";
			}
			return "TRUE " + s;
		}

		MatchBlockResult MatchBlockResult::join(MatchBlockResult & other)
		{
			if (this->result == false) return this;
			if (other.result == false) return other;
			map<string, SBlockGroupItem> n;
			for (auto kv : this->bind)
			{
				n[kv.first] = kv.second;
			}
			for (auto kv : other.bind)
			{
				n[kv.first] = kv.second;
			}
			MatchBlockResult r = MatchBlockResult(true);
			r.bind = n;
			return r;
		}




	}
}