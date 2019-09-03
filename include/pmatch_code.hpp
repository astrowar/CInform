#ifndef PMATCHCODE_HPP
#define PMATCHCODE_HPP

#include "phaseDef.hpp"
#include "preprocess.hpp"
#include "pmatch.hpp"

#include <string>
#include <list>
#include <memory>
#include <map>


 


namespace CInform
{
	namespace Match
	{

		// Code ====================================================================


		class SBlockGroupItem {
		public:
			std::list<HSourceBlock> literals;
			SBlockGroupItem(std::list<HSourceBlock> _literals) : literals(std::move(_literals)) {}
			SBlockGroupItem(std::list<HSourceBlock>::iterator _begin, std::list<HSourceBlock>::iterator _end) : literals(_begin, _end) {}
			SBlockGroupItem() {}

			std::list<HSourceBlock>::const_iterator    begin() { return literals.begin(); }
			std::list<HSourceBlock>::const_iterator   end() { return literals.end(); }

		};
		class SBlockGroup : public std::list<SBlockGroupItem> {
		};

		std::list<HSourceBlock>   split_code(HSourceBlock phase, char sep);
		bool isSameCodeString(const std::string& a, HSourceBlock b);


		class  MatchBlockResult
		{
		public:
			bool result;
			map<std::string, SBlockGroupItem> bind;
			MatchBlockResult(bool r) : result(r) {}
			MatchBlockResult(std::string r, const SBlockGroupItem& val) : result(true) { bind[r] = val; }
			std::string repr();
			MatchBlockResult join(MatchBlockResult& other);
		};


		std::list<MatchBlockResult>  MatchLine(HSourceBlock line, HMatchExpended  m);
	std::string ListSrcRepr( std::list<HSourceBlock> val );

	}
}

#endif