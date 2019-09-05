#include "selector.hpp"

namespace CInform
{

	namespace CodeParser
	{

		SelectorItem*  SelectorItem::add( HSelectorItem __next )
		{
			if (this->_next != nullptr)
			{
				this->_next->add( __next );
			}
			else
			{
				this->_next = __next;
			}
			return this;

		}

		HSelectorItem SelectorItem::next()
		{
			return this->_next;
		}

		 

	}
}