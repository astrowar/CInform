#ifndef CINFORM_HPP
#define CINFORM_HPP


#include "pmatch.hpp"
#include "utils.hpp"
#include "phaseDef.hpp"

#include <list>


using namespace std;
namespace CInform
{

	 


	class MatchProcess
	{
	public:
		MatchProcess() {}
		std::list<Match::HMatch> matchs;
	};







	class CinformParser
	{
 
		void registerMatch(std::string patten);

	};

}

#endif