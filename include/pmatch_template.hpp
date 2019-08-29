//#ifndef PMATCHT_HPP
//#define PMATCHT_HPP
//
//#include "pmatch.hpp"
//#include < list>
//#include < string>
//
//
//namespace CInform
//{
//	namespace Match
//	{
//
//
//		template<typename T>
//		class TBlockGroupItem {
//		public:
//			std::list<T> literals;
//			TBlockGroupItem(std::list<T>  _literals) : literals(std::move(_literals)) {}
//			TBlockGroupItem(typename std::list<T>::iterator _begin, typename std::list<T>::iterator _end) : literals(_begin, _end) {}
//			TBlockGroupItem() {}
//			typename	std::list<T>::const_iterator    begin() { return literals.begin(); }
//			typename	std::list<T>::const_iterator   end() { return literals.end(); }
//		};
//
//
//		template<typename T>
//		class TBlockGroup : public std::list<TBlockGroupItem<T>> {
//		};
//
//
//		template<typename T> std::string ListSrcRepr(std::function<std::string(const T&)> F , const std::list<T>& val)
//		{
//			std::string s = "[";
//			bool initial = true;
//			for (auto &v : val)
//			{
//				if (initial)
//				{
//					s = s + F(v);
//				}
//				else {
//					s = s + " " + F(v);
//				}
//
//				initial = false;
//			}
//			return s + "]";
//		}
//
//
//	
//		template<typename T>
//		class  MatchResult
//		{
//		public:
//			bool result;
//			std::map<std::string, TBlockGroupItem<T> > bind;
//			MatchResult(bool r) : result(r) {}
//			MatchResult(std::string r, const  TBlockGroupItem<T>& val) : result(true) { bind[r] = val; }
//			std::string repr( std::function<std::string(const T&)> F )
//			{
//				if (this->result == false) { return "FALSE "; }
//				std::string s = "";
//				for (auto kv : this->bind)
//				{
//					//s = s + kv.first + ":" + stringList(kv.second.literals) + " ";
//					s = s + kv.first + ":" + ListSrcRepr(F, kv.second.literals) + " ";
//				}
//				return "TRUE " + s;
//			}
//
//			MatchResult<T> join(MatchResult<T>& other)
//			{
//				if (this->result == false) return this;
//				if (other.result == false) return other;
//				std::map<std::string, TBlockGroupItem<T>> n;
//				for (auto  kv : this->bind)
//				{
//					n[kv.first] = kv.second;
//				}
//				for (auto kv : other.bind)
//				{
//					n[kv.first] = kv.second;
//				}
//				MatchResult <T> r = MatchResult <T>(true);
//				r.bind = n;
//				return r;
//			}
//		};
//
//
//
//	 
// 
//
//
//
//
//		template<typename T>
//		MatchResult <T>  MatchThis_item(const TBlockGroupItem<T>& x, const  PMatch* m, std::function<bool(const std::string&, T)> MT)
//		{
//			if (auto pliteral = dynamic_cast<const PLiteral*>(m))
//			{
//				if (x.literals.size() == 1)    return MatchResult <T>(MT(pliteral->val, x.literals.front()));
//				return MatchResult <T>(false);
//			}
//			if (auto pvar = dynamic_cast<const PVar*>(m))
//			{
//				return MatchResult <T>(pvar->name, x);
//			}
//			return MatchResult <T>(false);
//		}
//
//		template<typename T>
//		void  getCombinatoriaN_partial(MatchResult <T> mAcc, TBlockGroup<T>& acc, std::list<T>& remainder, int n,
//			typename std::list<HMatch>::const_iterator   mstart, typename std::list<HMatch>::const_iterator   mend,
//			std::function<bool(const std::string&, T)> MT,
//			std::function<void(MatchResult <T>&)> F);
//
//
//
//		template<typename T>
//		void  getCombinatoriaN_partial(MatchResult <T> mAcc, TBlockGroup<T>& acc, std::list<T>& remainder, int n,
//			typename std::list<HMatch>::const_iterator   mstart,typename std::list<HMatch>::const_iterator   mend,
//			std::function<bool(const std::string&, T)> MT,
//			std::function<void(MatchResult <T>&)> F)
//		{
//			if (int(remainder.size()) < n) return;
//			if (n == 1)
//			{
//				MatchResult <T> rj = MatchThis_item(TBlockGroupItem<T>(remainder), (*mstart).get(), MT);
//				if (rj.result == false) return;
//				{
//					auto rmend = mAcc.join(rj);
//					F(rmend);
//				}
//				return;
//			}
//			if (mstart == mend) return;
//			for (auto it = remainder.begin(); it != remainder.end(); ++it)
//			{
//				if (it == remainder.begin()) continue; //evita header de tamanho zero
//				auto tail = std::std::list<T>(it, remainder.end());
//				if (int(tail.size()) < n - 1) continue;
//				MatchResult <T> rj = MatchThis_item(TBlockGroupItem<T>(remainder.begin(), it), (*mstart).get(), MT);
//				if (rj.result == false) return;
//				MatchResult <T> mm = mAcc.join(rj);
//				acc.emplace_back(remainder.begin(), it);
//				getCombinatoriaN_partial(mm, acc, tail, n - 1, std::next(mstart), mend, MT, F);
//				acc.pop_back();
//			}
//		}
//
//		template<typename T>
//		std::list<MatchResult <T>>  MatchLine_s(std::list<T> xs, HMatch  m, std::function<bool(const std::string&, T)> MT)
//		{
//			std::list<MatchResult <T>> rlist;
//			//std::std::list<HSourceBlock>  xs = split_code(phase, ' ');
//			TBlockGroup<T> acc;
//			if (auto pList = dynamic_cast<const PList*>(m.get()))
//			{
//				std::list<HMatch>::const_iterator mstart = pList->val.begin();
//				std::list<HMatch>::const_iterator mend = pList->val.end();
//				std::function<void(MatchResult <T>&)> F = [&](MatchResult <T>& mres)
//				{
//					if (mres.result)
//					{
//						rlist.push_back(mres);
//					}
//				};
//
//				getCombinatoriaN_partial(MatchResult <T>(true), acc, xs, int(pList->val.size()), mstart, mend, MT, F);
//					
//			}
//			return rlist;
//		}
//
//
//
//
//		template<typename T>
//		std::list<MatchResult <T>>  MatchLine_inner(std::list<T> phase, HMatch  m, std::function<bool(const std::string&, T)> MT)
//		{
//			std::list<MatchResult <T>> rlist;
//			if (auto pLST = dynamic_cast<const PList*>(m.get()))
//			{
//				std::list<HMatch> all_maths_combines = ExpandMatch(pLST);
//				for (auto mcomb : all_maths_combines)
//				{
//					auto rlist_sub = MatchLine_s(phase, mcomb, MT);
//					rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
//				}
//			}
//			else
//			{
//				return MatchLine_s(phase, m, MT);
//			}
//			return rlist;
//		}
//
//
//
//
//		template <typename T>
//		class   TMatch
//		{
//		public:
//			//std::list<MatchResult<T> >  MatchLine(std::list<T> line_parts, HMatchExpended  m, std::function<bool(const std::string&,  T)> MT);
//			std::list< MatchResult <T> > MatchLine(std::list<T> line_parts, HMatchExpended m, std::function<bool(const std::string&, T)> MT)
//			{
//				std::list<MatchResult <T>> rlist;
//				for (auto& mcomb : m->expended)
//				{
//					auto rlist_sub = MatchLine_inner(line_parts, mcomb, MT);
//					rlist.insert(rlist.end(), rlist_sub.begin(), rlist_sub.end());
//				}
//				return rlist;
//			}
//		};
//	}
//}
//
//	 
//
//
// 
//
//
//
//#endif