#ifndef PMATCHBASE_HPP
#define PMATCHBASE_HPP

#include "pmatch.hpp"
#include <list>

namespace CInform
{
	namespace Match
	{
			   
		class MNoum
		{
		public :
			virtual std::string repr() = 0;
			virtual  bool isMatch( const std::string& s ) = 0;
			
		};
		using HNoum = std::shared_ptr<MNoum>;

		 


		class TBlockGroupItemNoum {
		public:
			const std::list<HNoum> literals;
			TBlockGroupItemNoum(std::list<HNoum>  _literals);
			TBlockGroupItemNoum(  std::list<HNoum>::iterator _begin,   std::list<HNoum>::iterator _end);
			TBlockGroupItemNoum();
			std::string repr();
			size_t size();
			bool empty();
			 	std::list<HNoum>::const_iterator    begin();
			 	std::list<HNoum>::const_iterator   end();
		};



		class TBlockGroupNoum
		{
		public:
			std::list<TBlockGroupItemNoum> items ;
		};

 
		class PResolveContext
		{
		public :
			PResolveContext() {}
			bool resolve(TBlockGroupItemNoum x) { return false; }
		};



		class  MatchResult
		{
		public:
			bool result;
			std::map<std::string, TBlockGroupItemNoum > bind;
			MatchResult( bool r ) : result( r ) {}
			MatchResult( std::string r, const TBlockGroupItemNoum&  val ) : result( true ), bind( std::map<std::string, TBlockGroupItemNoum >{ { r, val }} ) {} // { bind[r] = val; }
			std::string repr(  );
			TBlockGroupItemNoum getResult(std::string varName);
			bool hasBind(std::string varName);
			MatchResult join( MatchResult& other );
		};
		 



		class   TMatch
		{
		public:
			//list<MatchResult >  MatchLine(list<HNoum> line_parts, HMatchExpended  m, std::function<bool(const string&,  HNoum)> MT);
			static std::list< MatchResult > MatchLine(std::list<HNoum> line_parts, HMatchExpended m);
			static std::list< MatchResult > MatchLine(std::list<HNoum> line_parts, HMatch m);
		};
	}
}








#endif