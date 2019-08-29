#ifndef PHDEFLS_HPP
#define PHDEFLS_HPP


#include <list>
#include <string>
#include <map>
#include <functional>

 
namespace CInform
{

	class GrammarContext; 

	class SSplitedPhaseItem {
	public:
		std::list<std::string> literals;
		SSplitedPhaseItem(std::list<std::string> _literals) : literals(std::move(_literals)) {}
		SSplitedPhaseItem(std::list<std::string>::iterator _begin, std::list<std::string>::iterator _end) : literals(_begin, _end) {}
		SSplitedPhaseItem() {}
		//SSplitedPhaseItem(list<string> _literals) : Base(_literals){}
		std::list<std::string>::const_iterator    begin() { return literals.begin(); }
		std::list<std::string>::const_iterator   end() { return literals.end(); }
		 std::string  repr();
	};
	class SSplitedPhase 
	{
	public:
		std::list<SSplitedPhaseItem> items;
	};

	//std::list< SSplitedPhase >  getCombinatoria(string x, int n);
	void getCombinatoria(std::string x, int n, std::function<  void(SSplitedPhase&)> func);
}

#endif