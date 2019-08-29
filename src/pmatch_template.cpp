#include "pmatch_template.hpp"
#include <list>

#ifdef HAVDEF 

namespace CInform
{
	namespace Match
	{
		 

		template<typename T>
		class TBlockGroupItem {
		public:
			list<T> literals;
			TBlockGroupItem(list<T>  _literals) : literals(std::move(_literals)) {}
			TBlockGroupItem(typename list<T>::iterator _begin, typename list<T>::iterator _end) : literals(_begin, _end) {}
			TBlockGroupItem() {}
			typename	list<T>::const_iterator    begin() { return literals.begin(); }
			typename	list<T>::const_iterator   end() { return literals.end(); }
		};


		template<typename T>
		class TBlockGroup : public list<TBlockGroupItem<T>> {
		};



 


template<typename T>
		MatchResult <T> MatchResult <T>::join(MatchResult <T>& other)
		{
			if (this->result == false) return this;
			if (other.result == false) return other;
			map<string, TBlockGroupItem<T>> n;
			for (auto kv : this->bind)
			{
				n[kv.first] = kv.second;
			}
			for (auto kv : other.bind)
			{
				n[kv.first] = kv.second;
			}
			MatchResult <T> r = MatchResult <T>(true);
			r.bind = n;
			return r;
		}

		template<typename T>
		string MatchResult <T>::repr()
		{
			if (this->result == false) { return "FALSE "; }
			string s = "";
			for (auto kv : this->bind)
			{
				//s = s + kv.first + ":" + stringList(kv.second.literals) + " ";
				//s = s + kv.first + ":" + ListSrcRepr(kv.second.literals) + " ";
			}
			return "TRUE " + s;
		}




		template<typename T>
		MatchResult <T>  MatchThis_item(const TBlockGroupItem<T>& x, const  PMatch* m , std::function<bool(const string&, T)> MT )
		{
			if (auto pliteral = dynamic_cast<const PLiteral*>(m))
			{
				if (x.literals.size() == 1)    return MatchResult <T>(MT(pliteral->val, x.literals.front()));
				return MatchResult <T>(false);
			}
			if (auto pvar = dynamic_cast<const PVar*>(m))
			{
				return MatchResult <T>(pvar->name, x);
			}
			return MatchResult <T>(false);
		}



		template<typename T>
		void  getCombinatoriaN_partial(MatchResult <T> mAcc, TBlockGroup<T>& acc, std::list<T>& remainder, int n, 
			list<HMatch>::const_iterator   mstart, list<HMatch>::const_iterator   mend,
			std::function<bool(const string&, T)> MT,
			std::function<void(MatchResult <T>&)> F)
		{
			if (  int(remainder.size()) < n) return;
			if (n == 1)
			{
				MatchResult <T> rj = MatchThis_item(TBlockGroupItem<T>(remainder), (*mstart).get(),MT);
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
				if (  int(tail.size()) < n - 1) continue;
				MatchResult <T> rj = MatchThis_item(TBlockGroupItem<T>(remainder.begin(), it), (*mstart).get(),MT);
				if (rj.result == false) return;
				MatchResult <T> mm = mAcc.join(rj);
				acc.emplace_back(remainder.begin(), it);
				getCombinatoriaN_partial(mm, acc, tail, n - 1, std::next(mstart), mend,MT, F);
				acc.pop_back();
			}
		}

		template<typename T>
		list<MatchResult <T>>  MatchLine_s(list<T> xs, HMatch  m, std::function<bool(const string&, T)> MT)
		{
			list<MatchResult <T>> rlist;
			//std::list<HSourceBlock>  xs = split_code(phase, ' ');
			TBlockGroup<T> acc;
			if (auto pList = dynamic_cast<const PList*>(m.get()))
			{
				list<HMatch>::const_iterator mstart = pList->val.begin();
				list<HMatch>::const_iterator mend = pList->val.end();
				getCombinatoriaN_partial(MatchResult <T>(true), acc, xs, pList->val.size(), mstart, mend,MT, [&](MatchResult <T>& mres)
					{
						if (mres.result)
						{
							rlist.push_back(mres);
						}
					});				
			}
			return rlist;
		}




		template<typename T>
		list<MatchResult <T>>  MatchLine_inner(list<T> phase, HMatch  m, std::function<bool(const string&, T)> MT  )
		{
			list<MatchResult <T>> rlist;
			if (auto pLST = dynamic_cast<const PList*>(m.get()))
			{
				list<HMatch> all_maths_combines = ExpandMatch(pLST);
				for (auto mcomb : all_maths_combines)
				{
					auto rlist_sub = MatchLine_s(phase, mcomb, MT);
					rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
				}				
			}
			else
			{
				return MatchLine_s(phase, m, MT);
			}
			return rlist;
		}



		template<typename T>
		list< MatchResult <T> > TMatch<T>::MatchLine(list<T> line_parts, HMatchExpended m, std::function<bool(const string&,   T)> MT )
		{
			list<MatchResult <T>> rlist;
			for (auto& mcomb : m->expended)
			{
				auto rlist_sub = MatchLine_inner(line_parts, mcomb, MT);
				rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
			}
			return rlist;
		}


	 
	}
}

#endif