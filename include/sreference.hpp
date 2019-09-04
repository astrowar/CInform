#pragma once

#ifndef  HREFSS_HPP
#define  HREFSS_HPP

#include <string>
namespace CInform
{

	class SReference
	{
		std::string ref;
	public:
		std::string repr() {	return ref;	}

		SReference( std::string _ref ) :ref( _ref ) {}
		SReference():ref("") { }
		bool empty() { return ref.empty(); }

		SReference( const SReference& other ) : ref(other.ref )
		{			 
			
		}
	};
}

#endif