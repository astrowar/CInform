#include "hsymbol.hpp"

using namespace  std;

namespace CInform
{
	bool isSameNoum( string a, string b )
	{
		if (a.size() != b.size()) return false;
		size_t n = a.size();
		for (size_t i = 0; i < n; ++i)
		{
			if (towlower( a[i] ) != towlower( b[i] )) return false;
		}
		return true;
	}


	std::string Kind::getName()
	{
		return this->name;
	}

	SYMBTYPE Kind::getType()
	{
		return SYMBTYPE::SKIND;
	}

	std::string Instance::getName()
	{
		return this->name;
	}

	SYMBTYPE Instance::getType()
	{
		return SYMBTYPE::SINSTANCE;
	}

 

	SYMBTYPE KindComposeDual::getType()
	{
		return SYMBTYPE::SKIND;
	}

	SYMBTYPE RelationKind::getType()
	{
		return SYMBTYPE::SRELATION;
	}

	SYMBTYPE Value::getType()
	{
		return SYMBTYPE::SVALUE;
	}

	std::string Verb::getName()
	{
		return  name;
	}

	std::string Variable::getName()
	{
		return name;
	}

	SYMBTYPE Variable::getType()
	{
		return  SYMBTYPE::SVAR;
	}

}
