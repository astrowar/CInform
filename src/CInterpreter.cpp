// CInterpreter.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "CInterpreter.hpp"
#include "Grammar.hpp"

#include "pmatch.hpp"
#include <iostream>
#include <fstream>
#include <array>


using namespace std;

namespace CInform
{
	namespace cruntime
	{
		std::string::iterator findNextNonblanckChar( std::string::iterator h, std::string::iterator send )
		{
			auto i = h; 
			while (i != send)
			{
				if ((*i == '\n') || (*i == '\r'))
				{
					i++;
					continue;  //linha em branco
				}
				if ((*i == '\t'))
				{
					++i;
					continue;  //ignora tabs			
				}
				if (*i != ' ') break; //achei o inicio de algo
				i++;
			}
			return i;
		}

		std::string::iterator findNextBlankChar( std::string::iterator h, std::string::iterator send )
		{
			auto i = h;
			while (i != send)
			{
				if (*i == ' ') break;
				if ((*i == '\n') || (*i == '\r')) break;
				i++;
			}
			return i;
		}



		std::string::iterator   getEndOfString( std::string::iterator h, std::string::iterator send )
		{
			auto i = h;
			while (i != send)
			{
				if (*i == '"') break; //achei !
				if (*i == '\\')
				{   //ignore o proximo caracter
					i++;
					if (i != send) { i++; }
					continue; 
				}
				if ((*i == '\n') || (*i == '\r')) break;
				i++;
			}
			return i;
		}
	 

		void  getFirstOpcode( list<string> &opcodeLine  , std::string::iterator h, std::string::iterator send )
		{
			auto i = h; 
					   
   lineStart :
			i = findNextNonblanckChar( i, send );
			if ((*i == '#'))
			{
				auto inext = std::find( i, send, '\n' );
				if (inext == send) return; //nao tem mais opcode
				i = inext;
				goto lineStart; // proxima linha ....
			}
			 
			//h1 = inicio da linha;
			auto h1 = i;

			//acha o final da linha 
			auto iendline = std::find( i, send, '\n' );

			string line = string( h1, iendline );			
			opcodeLine.push_back( line );
			if (iendline == send) return; // acabou o arquivo 
			
			i = iendline;

			goto lineStart; //nao gosot de goto .. mas aqui parece ser melhor que a alternativa 

		}

		vector< Command*> parseOpcodes( list<std::string> lines );
		 
		CommandBlock CInform::cruntime::CParser::groupBlocks( std::string txt )
		{
			
			std::list<std::string> acc;
			getFirstOpcode( acc, txt.begin(), txt.end() );
			//for (auto q : acc)
			//{
			//	cout << q << endl;
			//} 

			vector< Command*>  cmds = parseOpcodes(acc );

			auto cb =  CommandBlock();
			cb.comands = cmds;
			return cb;
		}

		void CInterpreter::exec( Command  x )
		{
		}

		void CInterpreter::call( std::string start_point )
		{
			int addr = findLabel( start_point );
			if (addr == 0)
			{
				
			}
		}

		int CInterpreter::findLabel( std::string lb )
		{
			int n = prog.comands.size();
			for (int i = 0; i < n; ++i)
			{
				auto* c = prog.comands[i];
				if (c->op = OPLabel)
				{
					if (c->x == lb)		return i;
				}
			}
			return -1;
		}

		CInterpreter::CInterpreter( CommandBlock _prog ):prog(std::move(_prog))
		{
			 
		}

	 
 

		Command::Command( Opcode _op, string _x, string _y ): x(_x),y(_y),op(_op)
		{
		}

		string Command::repr()
		{

			return OpcodeRepr( op ) + " " + x + " " + y;
		}

		Command::Command( Opcode _op ) : x("" ), y( "" ), op( _op )
		{
		}

		Command::Command( Opcode _op, string _x  ): x( _x ), y( "" ), op( _op )
		{
		}
}
}