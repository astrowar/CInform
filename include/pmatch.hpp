#ifndef PMATCH_HPP
#define PMATCH_HPP

 
#include <string>
#include <list>
#include <memory>
#include <map>
#include <numeric>


#include "phaseDef.hpp"


namespace CInform
{
	namespace Match
	{
		class PMatch
		{
		public:
			virtual ~PMatch();;
			virtual size_t size() = 0 ;
		};

		using HMatch = std::shared_ptr<PMatch>;

		class PLiteral : public PMatch
		{
		public:
			std::string val;
			PLiteral(std::string s);
			size_t size();
		};

		class POr : public PMatch
		{
		public:
			std::list<HMatch> val;
			POr(std::list<HMatch> s);
			size_t size();
		};


		class PEmpty : public PMatch
		{
		public: 
			PEmpty();
			size_t size();
		};


		class POptional : public PMatch
		{
		public:
			HMatch val;
			POptional(HMatch s);
			size_t size();
		};


		class PList : public PMatch
		{
		public:
			std::list<HMatch> val;
			PList(std::list<HMatch> s);
			size_t size();
		};
		//using HPList = std::shared_ptr<PList>;


		class PVar : public PMatch
		{
		public:
			std::string name;
			PVar(std::string s);
			size_t size();
		};

		class PVarCond : public PMatch
		{
		public:
			std::string name;
			HMatch inner;			
			PVarCond(std::string s, HMatch _inner);
			size_t size();
		};


 


	

		class PMatchExpended
		{
		public:
			HMatch base;
			std::list<HMatch> expended;
			PMatchExpended(HMatch _base);
		};

		using HMatchExpended = std::shared_ptr<PMatchExpended>;


		class  MatchResultStr
		{
		public:
			bool result;
			std::map<std::string, SSplitedPhaseItem> bind;
			SSplitedPhaseItem getResult(std::string v);
			bool hasBind(std::string x);
			MatchResultStr(bool r) : result(r) {}
			MatchResultStr(std::string r, const SSplitedPhaseItem &val) : result(true) 
			{ 
				bind[r] = val; 
			}
			std::string repr();
			MatchResultStr join(MatchResultStr &other);
		};

		bool isSameString(const std::string &a, const std::string &b);
		HMatchExpended getPMatchExpended(std::string x, GrammarContext* gtx);
		HMatch getPMatch(std::string x, GrammarContext* gtx);
		//std::string getPMatchRepr(HMatch x);
		std::string __getPMatchRepr( HMatch x );
		std::string getPMatchRepr(HMatchExpended x);

		//MatchResult  MatchThis( SSplitedPhase  xs ,  HMatch  m );
		//std::list<MatchResult>  MatchThis( std::string phase ,  HMatch  m );
		std::list<MatchResultStr>  MatchThis(std::string phase, HMatchExpended  m); 
		std::list<HMatch>  ExpandMatch(const PList*  mlist); 
	}
}

#endif