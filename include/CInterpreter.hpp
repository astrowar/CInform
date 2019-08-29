#pragma once

#include <list>
#include <vector>
#include <string>


namespace CInform
{
	namespace cruntime
	{

		class Command
		{

			virtual ~Command() {};
		};

		class CLabel : public Command
		{
		public:
			std::string label;
		};




		class CommandBlock
		{
		public:
			std::vector<Command*> comands; //RAM da memoria de comandos
		};

		class CParser
		{
		  public:
			  CommandBlock  groupBlocks( std::string  txt);
			 
		};


		class CInterpreter
		{
			CommandBlock prog;
		public :
			CInterpreter( CommandBlock prog ); 
			void exec( Command x );
			void call( std::string start_point);
			int findLabel( std::string lb );
		};
	}
}

