#include "selector.hpp"

namespace CInform
{

	namespace CodeParser
	{

		SelectorItem *  SelectorItem::add( SelectorItem * __next )
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

		SelectorItem * SelectorItem::next()
		{
			return this->_next;
		}

		SelectorItem * SelectorItem::operator<<( SelectorItem * __next )
		{
			return this->add( __next );
		}

	}
}