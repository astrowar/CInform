
#include "CodeParser.hpp"
 
#include <map>
#include <list>

using namespace std;

namespace CInform
{
	namespace CodeParser
	{

		map<string, list<PreCodeGenerate*> > processSource(ParserStore *pstore, SParagraph* code );

		ParserEntryGroup*  createParseEntries( ParserStore *pstore )
		{
			CInform::GrammarContext* gtx = pstore->grammar();
			list<ParserEntry > parserentries_list;
			parserentries_list.emplace_back( "phaseDeclVerb", getPMatchExpended( " to X", gtx ) );
			parserentries_list.emplace_back( "phaseDeclVerb2", getPMatchExpended( " to X Y  ", gtx ) );

			parserentries_list.emplace_back( "phaseInsteadwhen", getPMatchExpended( "Instead of X when Y", gtx ) );
			parserentries_list.emplace_back( "phaseInstead", getPMatchExpended( "Instead of X , Y", gtx ) );
			parserentries_list.emplace_back( "phaseInstead", getPMatchExpended( "Instead of X : Y", gtx ) );
			parserentries_list.emplace_back( "phaseInstead", getPMatchExpended( "Instead of X :", gtx ) );


			parserentries_list.emplace_back( "phaseDeclDecide", getPMatchExpended( " to decide if X is Y ", gtx ) );
			parserentries_list.emplace_back( "phaseDeclwhich", getPMatchExpended( " to decide which X is Y ", gtx ) );
			//parserentries_list.emplace_back( "phaseDeclVerb", getPMatchExpended( " to X which  Y ", gtx ) );


			parserentries_list.emplace_back( "assertKind", getPMatchExpended( " X is a/an kind ", gtx ) );
			parserentries_list.emplace_back( "assertKindValue", getPMatchExpended( " X is a/an kind of value", gtx ) );
			parserentries_list.emplace_back( "assertKindOf", getPMatchExpended( " X is/are a/an kind of   Y ", gtx ) );
			parserentries_list.emplace_back( "assertInstance", getPMatchExpended( " X is/are a/an   Y   ", gtx ) );
			parserentries_list.emplace_back( "assertAdjetive", getPMatchExpended( " X is/are Y ", gtx ) );

			//A thing can be fixed in place or portable
			parserentries_list.emplace_back( "assertAdjetiveEnum", getPMatchExpended( " X can be Y ", gtx ) );

			//A dead end has some text called the river sound.
			parserentries_list.emplace_back( "assertMemberVariable", getPMatchExpended( " X has ?a/an/some Y called Z ", gtx ) );

			//A object  is usually portable
			parserentries_list.emplace_back( "assertUsually", getPMatchExpended( " X is/are usually Y ", gtx ) ); 

			ParserEntryGroup*  parserentries = new ParserEntryGroup( parserentries_list );
			return parserentries;
		}


		std::list<std::string> emiteInter( ParserStore *pstore, SParagraph* code)
		{
		 
		 
			if (pstore->parserentries == nullptr) pstore->parserentries = createParseEntries( pstore );
			//ParserEntryGroup*  parserentries = createParseEntries( pstore );

			map<string, list<PreCodeGenerate*> >  itList = processSource( pstore,code  );

			list<string> asm_out;
			for (auto &kv : itList)
			{
				asm_out.push_back(kv.first + ":");
				auto ls = emiteAsm(kv.second);
				asm_out.insert(asm_out.end(), ls.begin(), ls.end());
			}
			return asm_out;
		}
	
		std::list<std::string> emiteInter(   SParagraph* code )
		{
			auto gtx = new GrammarContextEnglish();
			ParserStore *pstore = new ParserStore( gtx );

			//fill pstore
			 
			pstore->add( new CInform::Kind( "kind", "", "kind" ));
			pstore->add( new CInform::Kind( "value", "kind", "value" ) );
			pstore->add( new CInform::Kind( "number", "value", "number" ) );
			pstore->add( new CInform::Kind( "action", "kind", "action" ) );
			


			return emiteInter( pstore , code );
		}
	}
}
