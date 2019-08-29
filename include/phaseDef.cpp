#include "phaseDef.hpp"
namespace CInform
{
	std::string listToString(std::list<std::string> x);
	std::string SSplitedPhaseItem::repr()
	{
		return listToString(this->literals);
	}

}