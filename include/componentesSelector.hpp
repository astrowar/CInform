#pragma once

#ifndef COMPSELC_HPP
#define COMPSELC_HPP

#include "PreCodeGenerate.hpp"
#include "NSource.hpp"
#include <headerPhase.hpp>

#include <cctype>

using namespace std;

namespace CInform
{
	using namespace Match;
	namespace CodeParser
	{

		HeaderPhaseEntry listToActionSelector( ParserStore *pstore, list<HNoum> xj, string sufix  );

		HeaderPhaseEntry listToActionSelector_nomSelector( ParserStore *pstore, list<HNoum> xj, string sufix  );

		HeaderPhaseEntry listToComposePreposition_3( ParserStore *pstore, list<HNoum> xj, string sufix   );

		HeaderPhaseEntry listToComposePreposition_2( ParserStore *pstore, list<HNoum> xj, string sufix   );

		HeaderPhaseEntry listToVerbPreposition( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error );

			HeaderPhaseEntry listToVerbEntry( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error );
			HeaderPhaseEntry listToNP( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error );
			HeaderPhaseEntry listToVerbOnly( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error );
		 

		HeaderPhaseEntry listToArgumentsEntryPatthen_variableSelector( ParserStore *pstore, list<HNoum> xj, string sufix, PreCodeGenerate** error );

	}
}

#endif