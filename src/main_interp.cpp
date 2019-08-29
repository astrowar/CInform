// CInterpreter.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#include <iostream>
#include <fstream>
#include "CInterpreter.hpp"

using namespace CInform;

int main()
{

	std::ifstream t_input( "test.txt" );
	std::string str( (std::istreambuf_iterator<char>( t_input )), std::istreambuf_iterator<char>() );

	// cada linha eh um comando
	// indetacao nao eh levada em consideracao

	//existem grupos de comando, mas estes veem de begin's e end's da vida  

	auto Parser = cruntime::CParser();
	cruntime::CommandBlock   prog = Parser.groupBlocks( str );
	for (cruntime::Command* g : prog.comands)
	{
		std::cout << g->repr() << std::endl;
	}
	auto engine = new cruntime::CInterpreter( prog );

	engine->call( "START" );

}

