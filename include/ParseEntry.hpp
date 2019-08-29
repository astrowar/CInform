#ifndef PARSEENTRY_HPP
#define PARSEENTRY_HPP

#include <preprocess.hpp>
#include <pmatch_baseClass.hpp>
#include <list>


using namespace std;

namespace CInform
{
	using namespace Match;

	namespace CodeParser
	{


		class MatchedParagraph
		{
		public:
			SParagraph* paragraph;
			list< MatchResult >  mList;
			MatchedParagraph(SParagraph* _paragraph, list< MatchResult >  	_mList);
		};



		class ParserEntry
		{
		public:
			HMatchExpended patten;
			string entryName;
			ParserEntry(string _entryName, HMatchExpended _patten);
		};


		class ParserEntryGroup
		{
		public:
			std::list< std::pair< std::string, HMatch > > entryName_patten;
			ParserEntryGroup(list< ParserEntry> parserentries);

			void Add(ParserEntry p);

		};

	}
}

#endif