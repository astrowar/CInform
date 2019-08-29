#pragma once

#include <list>
#include <vector>
#include <string>
#include <opcodes.hxx>


namespace CInform
{
	namespace cruntime
	{

		class Command
		{
		public:
			Opcode op;
			std::string x, y;
			virtual ~Command() {};
			Command( Opcode _op  ); 
			Command( Opcode _op, std::string _x );
			Command( Opcode _op, std::string _x, std::string _y );
			std::string repr();
		};

		 

 

		//-------------------------------------------------


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

