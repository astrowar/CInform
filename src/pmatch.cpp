
//Composicao e criacao do matchs diversos

#include "phaseDef.hpp"
#include "pmatch.hpp"
#include "utils.hpp"
#include "Grammar.hpp"
#include <iostream>

using   namespace std;

namespace CInform
{
	namespace Match
	{
		std::list<std::list<HMatch> >  _ExpandMatchAcc(std::list< std::list<HMatch> > & acc, std::list<HMatch>& r_next);
		std::list<HMatch>  _ExpandMatch(HMatch  m);


		bool isBalanced( MatchResultStr r )
		{
			for (auto &kv : r.bind)
			{
				int p = 0;
				int c = 0;
				for (auto &s : kv.second.literals)
				{
					if (s == "(") p++;
					if (s == ")") p--;
					if (s == "[") c++;
					if (s == "]") c--;
					if (p < 0) return false;
					if (c < 0) return false;
				}
				if (p != 0) return false;
				if (c != 0) return false;
			}
			return true;
		}

		string __getPMatchRepr(HMatch x)
		{
			if (x == nullptr)
			{
				return "NULLPTR?";
			}
			if (auto popt = dynamic_cast<const POptional*>(x.get()))
			{
				return "?(" + __getPMatchRepr(popt->val) + ")";
			}

			if (auto plit = dynamic_cast<const PLiteral*>(x.get()))
			{
				return "'" + plit->val;
			}

			if (auto pvar = dynamic_cast<const PVar*>(x.get()))
			{
				return "V(" + pvar->name + ")";
			}


			if (auto porr = dynamic_cast<const POr*>(x.get()))
			{
				list<string> slist;
				std::transform(porr->val.begin(), porr->val.end(), std::back_inserter(slist), [](HMatch  i) { return __getPMatchRepr(i); });
				return  "OR(" + stringList(slist) + ")";
			}

			if (auto plst = dynamic_cast<const PList*>(x.get()))
			{
				list<string> slist;
				std::transform(plst->val.begin(), plst->val.end(), std::back_inserter(slist), [](HMatch  i) { return __getPMatchRepr(i); });
				return   stringList(slist);
			}

			if (auto pvar = dynamic_cast<const PVarCond*>(x.get()))
			{
				return "V(" + pvar->name + ",[" + __getPMatchRepr(pvar->inner) + "])";
			}


			return "ERROR";
		}

		string getPMatchRepr(HMatchExpended x)
		{
			return __getPMatchRepr(x->base);
		}

		bool isVariablePrefix(string x)
		{
			if (x.size() <= 2)
			{
				if (x[0] == towupper(x[0]) && isalpha(x[0]))
				{
					return  true;
				}
			}
			return false;
		}

		HMatch getVariableConditional(string x, GrammarContext* gtx)
		{
			if (x.back() == ']')
			{
				if (x.size() > 3)
				{

					if (x[1] == '['  && isVariablePrefix(x.substr(0, 1)))
					{
						string x_inner = x.substr(2, x.size() - 3);
						//cout << "inner = " << x_inner;						 
						HMatch p_inner = getPMatch(x_inner, gtx);
						return  std::make_shared<PVarCond>(x.substr(0, 1), p_inner);
					}

					else if (x[2] == '[' && isVariablePrefix(x.substr(0, 2)))
					{
						string x_inner = x.substr(3, x.size() - 4);
						//cout << "inner = " << x_inner;
						HMatch p_inner = getPMatch(x_inner, gtx);
						return  std::make_shared<PVarCond>(x.substr(0, 2), p_inner);
					}
				}
			}

			return nullptr;
		}

		size_t endofBlock(const string &x ) //x[0] must be '('
		{
			int lv = 1;
			for (size_t i = 1; i < x.size(); ++i)
			{
				if (x[i] == ')') lv--;
				if (x[i] == '(') lv++;
				if (lv == 0) return i;
			}
			
			return string::npos; //error
		}

		HMatch getPMatch_item(string x, GrammarContext* gtx)
		{

			auto hvar = getVariableConditional(x, gtx);
			if (hvar != nullptr) return hvar;

			if (x[0] == '?')
			{
				string  remm = x.substr(1, x.size() - 1);
				return std::make_shared<POptional>(getPMatch(remm, gtx));
			}


			std::size_t found = x.find("/");
			if (found != std::string::npos)
			{
				list<string>  xs = split(x, '/');
				list<HMatch> hlist;
				std::transform(xs.begin(), xs.end(), std::back_inserter(hlist), [&](string  i) { return getPMatch(i, gtx); });
				return std::make_shared<POr>(hlist);
			}

			if (x.size() <= 2)
			{
				if (gtx->isReservedWord(x) == false)
				{
					if (isVariablePrefix(x))
					{
						return  std::make_shared<PVar>(x);
					}
				}
			}

			if (x.front() == '(')
				if (x.back() == ')')
			{				
					auto h1 = getPMatch( x.substr( 1, x.size() - 2 ), gtx );
					return h1;
			}

			return  std::make_shared<PLiteral>(x);
		}

 

		list<string>  splitTokensBase(string x, bool &hasError)
		{
			hasError = false;
			list<string> ret;
			size_t pivot = 0;
			size_t  n = x.size();
			int  in_bracet = 0;
			int  in_parentesis = 0;

			for (size_t i = 0; i < n; ++i)
			{
				if (x[i] == ']') { in_bracet--; continue; }
				if (x[i] == '[') { in_bracet++; continue; }

				if (x[i] == ')') { in_bracet--; continue; }
				if (x[i] == '(') { in_bracet++; continue; }

				if (in_bracet == 0)
				{
					if (x[i] == ' ')
					{
						if (i != pivot)
						{
							ret.push_back(x.substr(pivot, i - pivot));
						}
						pivot = i + 1;
					}
				}
			}
			if (n != pivot)	ret.push_back(x.substr(pivot, n - pivot));
			if (in_bracet != 0)
			{
				hasError = true;
			}
			return ret;
		}


		HMatch getPMatch(string x, GrammarContext* gtx)
		{
			bool hasBracketError = false;
			list<string> xs = splitTokensBase(x, hasBracketError);
			//for (auto y : xs)
			//{
			//	cout << "|"<< y << "| ";
			//}
			//cout << endl;

			if (hasBracketError) return nullptr;

			//list<string>  xs = split(x, ' ');
			if (xs.size() == 0) return nullptr;
			if (xs.size() == 1) return getPMatch_item(xs.front(), gtx);
			list<HMatch> hlist;
			std::transform(xs.begin(), xs.end(), std::back_inserter(hlist), [&](string  i) { return getPMatch(i, gtx); });
			return std::make_shared<PList>(hlist);
		}




		HMatchExpended getPMatchExpended(string x, GrammarContext* gtx)
		{
			HMatch m = getPMatch(x, gtx);
			return std::make_shared<PMatchExpended>(m);
		}


		MatchResultStr MatchResultStr::join(MatchResultStr& other)
		{
			if (this->result == false) return this;
			if (other.result == false) return other;
			map<string, SSplitedPhaseItem> n;
			for (auto kv : this->bind)
			{
				n[kv.first] = kv.second;
			}
			for (auto kv : other.bind)
			{
				n[kv.first] = kv.second;
			}
			MatchResultStr r = MatchResultStr(true);
			r.bind = n;
			return r;
		}

		SSplitedPhaseItem MatchResultStr::getResult(string v)
		{
			if (this->bind.find(v) == this->bind.end())		return SSplitedPhaseItem();
			return this->bind[v];
		}

		bool MatchResultStr::hasBind(std::string x)
		{
			return this->bind.find(x) != this->bind.end();
		}

		string MatchResultStr::repr()
		{
			if (this->result == false) { return "FALSE "; }
			string s = "";
			for (auto kv : this->bind)
			{
				s = s + kv.first + ":" + stringList(kv.second.literals) + " ";
			}
			return "TRUE " + s;
		}




		// match this

		bool isSameString(const string &a, const string &b)
		{
			if (a.size() != b.size()) return false;
			int n = a.size();
			for (int i = 0; i < n; ++i)
			{
				if (towlower(a[i]) != towlower(b[i])) return false;
			}

			return true;
		}

		MatchResultStr  MatchThis_item(const SSplitedPhaseItem & x, const  PMatch* m)
		{
			if (auto pliteral = dynamic_cast<const PLiteral*>(m))
			{
				if (x.literals.size() == 1)    return MatchResultStr(isSameString(pliteral->val, x.literals.front()));
				return MatchResultStr(false);
			}

			if (auto pvar = dynamic_cast<const PVar*>(m))
			{
				return MatchResultStr(pvar->name, x);
			}

			if (auto pvarcd = dynamic_cast<const PVarCond*>(m))
			{
				MatchResultStr rr = MatchThis_item(x, pvarcd->inner.get());
				if (rr.result)
				{
					return MatchResultStr(pvarcd->name, x);
				}
			}

			return MatchResultStr(false);
		}



		MatchResultStr  MatchThis_q(MatchResultStr acc,
			list<SSplitedPhaseItem>::const_iterator  xrem_begin, list<SSplitedPhaseItem>::const_iterator  xrem_end,
			list<HMatch>::const_iterator  mrem_begin, list<HMatch>::const_iterator  mrem_end)
		{
			auto mbfront = (*mrem_begin);
			MatchResultStr mtemp = MatchThis_item((*xrem_begin), mbfront.get());
			if (mtemp.result == false) return mtemp;
			// cout << "J" << mtemp.repr() << " " << acc.repr() << endl;
			MatchResultStr mm = acc.join(mtemp);

			// cout << "R:" <<  mm.repr() << endl;

			list<SSplitedPhaseItem>::const_iterator  xrem_begin_ = std::next(xrem_begin);
			list<HMatch>::const_iterator  mrem_begin_ = std::next(mrem_begin);

			if (xrem_begin_ == xrem_end) return mm;
			if (mrem_begin_ == mrem_end) return mm;

			return MatchThis_q(mm, xrem_begin_, xrem_end, mrem_begin_, mrem_end);

		}

		MatchResultStr  MatchThis_i(SSplitedPhase & xs, PMatch*  m)
		{
			MatchResultStr  macc(true);
			if (xs.items.size() > 1)
			{
				if (auto plst = dynamic_cast<const PList*>(m))
				{
					if (xs.items.size() != plst->val.size())
					{
						return  MatchResultStr(false); //tamanhos diferentes nao sao compativeis
					}
					list<HMatch>::const_iterator   m1 = plst->val.begin();
					list<HMatch>::const_iterator   m2 = plst->val.end();
					macc = MatchThis_q(macc, xs.items.begin(), xs.items.end(), m1, m2);
				}
				return macc;
			}



			if (xs.items.size() == 1)
			{

				if (auto pcd = dynamic_cast<const PVarCond*>(m))
				{
					MatchResultStr res = MatchThis_i(xs, pcd->inner.get());
					if (res.result)
					{
						return MatchResultStr(pcd->name, xs.items.front());
					}
				}


				return  MatchThis_item(xs.items.front(), m);
			}

			return   MatchResultStr(false);
		}


		void  getCombinatoriaN_partial(MatchResultStr mAcc, SSplitedPhase & acc, std::list<string>& remainder, int n,
			list<HMatch>::const_iterator   mstart, list<HMatch>::const_iterator   mend, std::function<void(MatchResultStr&)> F)
		{
			if (isBalanced( mAcc ) == false) return;
			if (long int(remainder.size()) < n) return;
			if (n == 1)
			{
				MatchResultStr rj = MatchThis_item(SSplitedPhaseItem(remainder), (*mstart).get());
				if (rj.result == false) return;
				{
					auto rmend = mAcc.join(rj);
					F(rmend);
				}
				//acc.pop_back();
				return;
			}
			if (mstart == mend) return;
			for (auto it = remainder.begin(); it != remainder.end(); ++it)
			{
				if (it == remainder.begin()) continue; //evita header de tamanho zero
				auto tail = std::list<string>(it, remainder.end());
				if (long int(tail.size()) < n - 1) continue;
				//auto head =  std::list<string>( remainder.begin() , it) ;     
				//SSplitedPhaseItem head( remainder.begin() , it) ;   
				MatchResultStr rj = MatchThis_item(SSplitedPhaseItem(remainder.begin(), it), (*mstart).get());
				if (rj.result == false) return;
				MatchResultStr mm = mAcc.join(rj);
				// acc.emplace_back(head);
				acc.items.emplace_back(remainder.begin(), it);
				getCombinatoriaN_partial(mm, acc, tail, n - 1, std::next(mstart), mend, F);
				acc.items.pop_back();
			}
		}




		list<MatchResultStr>  MatchThis_string(string  phase, HMatch  m)
		{
			list<MatchResultStr> rlist;
			std::list<string>  xs = split(phase);
			SSplitedPhase acc;

			if (auto pList = dynamic_cast<const PList*>(m.get()))
			{
				list<HMatch>::const_iterator mstart = pList->val.begin();
				list<HMatch>::const_iterator mend = pList->val.end();
				getCombinatoriaN_partial(MatchResultStr(true), acc, xs, pList->val.size(), mstart, mend, [&](MatchResultStr & mres)
				{
					if (mres.result)
					{
						if (isBalanced( mres  ))
						{
							rlist.push_back( mres );
						}
					}
				});
				//std::transform(xs.begin(), xs.end(), std::back_inserter(rlist), [&]( SSplitedPhase si){ return MatchThis_i( si , m); } );                      
			}
			return rlist;
		}



		list<MatchResultStr>  MatchThis_string_old(string  phase, HMatch  m)
		{
			list<MatchResultStr> rlist;
			if (auto pList = dynamic_cast<const PList*>(m.get()))
			{
				getCombinatoria(phase, pList->val.size(), [&](SSplitedPhase &si)
				{
					MatchResultStr mres = MatchThis_i(si, m.get());
					if (mres.result)
					{
						rlist.push_back(mres);
					}
				});
				//std::transform(xs.begin(), xs.end(), std::back_inserter(rlist), [&]( SSplitedPhase si){ return MatchThis_i( si , m); } );                      
			}
			return rlist;
		}

		list<HMatch>  ExpandMatch(const PList* mlist);
	 

		void  ExpandMatch_h(list< HMatch  >& results, list<HMatch>& actual, list<HMatch>::const_iterator m1, list<HMatch>::const_iterator mend)
		{
			if (m1 == mend)
			{
				results.push_back(std::make_shared<PList>(actual));
				return;
			}
			auto m = *m1;

			list<HMatch>::const_iterator  mnext = std::next(m1);

			if (auto pOpt = dynamic_cast<const POptional*>(m.get()))
			{
				actual.push_back(pOpt->val);
				ExpandMatch_h(results, actual, mnext, mend);
				actual.pop_back();
				ExpandMatch_h(results, actual, mnext, mend);
			}

			else  if (auto pOR = dynamic_cast<const POr*>(m.get()))
			{
				for (auto mx : pOR->val)
				{
					actual.push_back(mx);
					ExpandMatch_h(results, actual, mnext, mend);
					actual.pop_back();
				}
			}
			else  if (auto pVARC = dynamic_cast<const PVarCond*>(m.get()))
			{
				auto inner_options = _ExpandMatch(pVARC->inner);
				for (auto mx : inner_options)
				{
					actual.push_back(std::make_shared<PVarCond>(pVARC->name, mx));
					ExpandMatch_h(results, actual, mnext, mend);
					actual.pop_back();
				}
			}
			else
			{
				actual.push_back(m);
				ExpandMatch_h(results, actual, mnext, mend);
				actual.pop_back();
			}

		}

		list<HMatch>  ExpandMatch(const PList*  mlist)
		{
			list<HMatch> results;
			list<HMatch>  actual;
			ExpandMatch_h(results, actual, mlist->val.begin(), mlist->val.end());
			return results;
		}

		std::list<std::list<HMatch> >  _ExpandMatchAcc(std::list< std::list<HMatch> > & acc, std::list<HMatch>& r_next)
		{
			//expand a arvore de cada combinacao anterior

			std::list<std::list<HMatch> > ret;

			if (acc.empty())
			{
				for (HMatch r : r_next)
				{
					if (dynamic_cast<const PEmpty*>(r.get()) == nullptr)
					{
						ret.push_back(std::list<HMatch>({ r }));
					}
					else
					{
						ret.push_back(std::list<HMatch>({  }));
					}
				}
				return ret;
			}



			for (std::list<HMatch> &a : acc)
			{
				for (HMatch r : r_next)
				{
					ret.push_back(a);
					if (dynamic_cast<const PEmpty*>(r.get()) == nullptr)
					{
						ret.back().push_back(r);
					}
				}
			}




			return ret;
		}

		list<HMatch>  _ExpandMatch_old(HMatch  m)
		{
			list<HMatch> res;
			if (auto plt = dynamic_cast<const PLiteral*>(m.get()))
			{
				res.push_back(m);
				return res;

			}
			else if (auto pOr = dynamic_cast<const POr*>(m.get()))
			{
				for (auto mj : pOr->val)
				{
					auto olist = _ExpandMatch(mj);
					res.insert(res.end(), olist.begin(), olist.end());
				}
				return res;
			}
			else if (auto pv = dynamic_cast<const PVar*>(m.get()))
			{
				res.push_back(m);
				return res;
			}
			else if (auto pcc = dynamic_cast<const PVarCond*>(m.get()))
			{
				auto sib = _ExpandMatch(pcc->inner);
				for (auto mj : sib)
				{
					res.push_back(std::make_shared<PVarCond>(pcc->name, mj));
				}
				return res;
			}
			else if (auto pLST = dynamic_cast<const PList*>(m.get()))
			{
				return  ExpandMatch(pLST);
			}
			else
			{
				res.push_back(m);
			}
			return res;
		}


		list<MatchResultStr>  MatchThis(string phase, HMatch  m)
		{
			list<MatchResultStr> rlist;
			if (auto pLST = dynamic_cast<const PList*>(m.get()))
			{
				list<HMatch> all_maths_combines = ExpandMatch(pLST);
				for (auto mcomb : all_maths_combines)
				{

					auto rlist_sub = MatchThis_string(phase, mcomb);
					rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
				}
				//std::transform(all_maths_combines.begin(), all_maths_combines.end(), std::back_inserter(rlist), [&](HMatch  p){ return MatchThis_string( phase , p); } );          
			}
			else
			{
				return MatchThis_string(phase, m);
			}

			return rlist;
		}




		list<MatchResultStr>  MatchThis(string phase, HMatchExpended  m)
		{
			list<MatchResultStr> rlist;
			{
				for (auto& mcomb : m->expended)
				{
					auto rlist_sub = MatchThis_string(phase, mcomb);
					rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
				}
			}
			return rlist;
		}







		PMatch::~PMatch() {}

		PLiteral::PLiteral(std::string s) :val(std::move(s)) 
		{
			if ((val.find('(') != string::npos ) || (val.find( ')' ) != string::npos))
			{
				throw " Error on parser";
		    }
			if ((val.find( '?' ) != string::npos) )
			{
				throw " undecoded ?";
			}

		}

		size_t PLiteral::size() { return 3 + val.size(); }

		POr::POr(std::list<HMatch> s) : val(std::move(s)) {}

		size_t POr::size() { return val.size(); }

		PEmpty::PEmpty() {}

		size_t PEmpty::size() { return 0; }

		POptional::POptional(HMatch s) :val(std::move(s)) {}

		size_t POptional::size() { return 1; }

		PList::PList(std::list<HMatch> s) : val(std::move(s)) {}

		size_t PList::size()
		{
			//return std::accumulate(val.begin(), val.end(), 0, [](size_t c, const HMatch& a ) { return c + a->size(); });
			size_t v = 0;
			for (auto e : val) v += e->size();
			return v;
		}

		PVar::PVar(std::string s) :name(std::move(s)) {}

		size_t PVar::size() { return 1; }
		PVarCond::PVarCond(std::string s, HMatch _inner) : name(std::move(s)), inner(_inner) {}

		size_t PVarCond::size() { return  inner->size(); }

		PMatchExpended::PMatchExpended(HMatch _base) : base(_base) { expended = _ExpandMatch(base); }



		std::list<HMatch>  _ExpandMatch(HMatch  m)
		{
			if (auto poo = dynamic_cast<const POptional*>(m.get()))
			{
				std::list<HMatch>  poo_val = _ExpandMatch(poo->val);
				poo_val.push_back(make_shared<PEmpty>());
				return poo_val;
			}


			if (auto plt = dynamic_cast<const PLiteral*>(m.get()))
			{
				return std::list<HMatch>({ m });

			}
			if (auto pOr = dynamic_cast<const POr*>(m.get()))
			{
				std::list<HMatch> res;
				for (auto mj : pOr->val)
				{
					auto olist = _ExpandMatch(mj);
					res.insert(res.end(), olist.begin(), olist.end());
				}
				return res;
			}

			if (auto pv = dynamic_cast<const PVar*>(m.get()))
			{
				return std::list<HMatch>({ m });
			}


			if (auto pcc = dynamic_cast<const PVarCond*>(m.get()))
			{
				auto sib = _ExpandMatch(pcc->inner);
				std::list<HMatch> res;
				for (auto mj : sib)
				{
					res.push_back(std::make_shared<PVarCond>(pcc->name, mj));
				}
				return res;
			}

			else if (auto pLST = dynamic_cast<const PList*>(m.get()))
			{
				std::list< std::list<HMatch> > ret_acc; // combinacao[ lista interna ]
														//aqui eh complicado ...
				for (auto q : pLST->val)
				{
					auto ret_next = _ExpandMatch(q);
					ret_acc = _ExpandMatchAcc(ret_acc, ret_next);
				}
				std::list<HMatch>  ret_list;
				for (auto ir : ret_acc)
				{
					ret_list.push_back(make_shared<PList>(ir));
				}
				return ret_list;
			}
			else
			{
				return std::list<HMatch>({ m });
			}
		}

	}
}


 





