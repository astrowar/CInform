#ifndef OpcodeHPP
#define OpcodeHPP

#include <string>

namespace CInform
{

	enum Opcode
	{
		OPNewVar = 10, 
		OPLabel,
		OPSet ,
		OPWrite,
		OPQuit,
		OPExit
	};

	std::string OpcodeRepr( Opcode  op );
}

#endif