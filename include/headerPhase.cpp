#include "headerPhase.hpp"

CInform::CodeParser::HeaderPhaseEntry  CInform::CodeParser::HeaderPhaseEntry::operator+( const HeaderPhaseEntry & other )
{
	CInform::CodeParser::HeaderPhaseEntry h;
	h.header = this->header + " " + other.header;
	h.args = this->args;
	h.args.insert( h.args.end(), other.args.begin(), other.args.end() );
	return h;
}
