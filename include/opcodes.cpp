#include "opcodes.hxx"

std::string CInform::OpcodeRepr( Opcode op )
{
	if (op == OPNewVar) return "OPNewVar";
	if (op == OPLabel) return "OPLabel";
	if (op == OPSet) return "OPSet";
	if (op == OPQuit) return "OPQuit";	
	if (op == OPExit) return "OPExit"; 
	if (op == OPWrite) return "OPWrite";

		throw "opcode error";
		return "...";;
}
