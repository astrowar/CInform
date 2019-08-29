#include "CInterpreter.hpp"
#include "Grammar.hpp"

#include "pmatch.hpp"
#include "pmatch_code.hpp"
#include "pmatch_baseClass.hpp"

#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include "opcodes.hxx"


using namespace std;


namespace CInform
{
	using namespace Match;
	namespace cruntime
	{

		//quebra a linha de forma logica
		list<string>    logical_split( string  line )
		{
			//corta a string considerando parentesis e aspas
			list<string> vals;

			size_t i = 0;
			size_t pivot = 0;
			size_t n = line.size();
			bool code = true;
			int p = 0;
			for (i = 0; i < n; i++)
			{
				char cc = line[i];
				if (code)
				{
					if (cc == '(')
					{
						p++;
					} 
					if (cc == ')')
					{
						p--;
					}
					if (p == 0)
					{
						if (cc == '"') { code = false; }

						if (cc == ' ')
						{
							if (i > pivot  )
							{
								string part = line.substr( pivot, i - pivot );
								vals.push_back( part   );
							}
							pivot = i + 1;
						}
					}
				}
				else
				{
					if (cc == '"') code = true;
				}

			}
			if (n > pivot ) vals.push_back( line.substr( pivot, pivot - n ) );
			return vals;
		}


		//match atual
		class NOpcode : public  CInform::Match::MNoum
		{
		public:
			string item;
			virtual string repr() {
				return item ;
			}

			virtual  bool isMatch( const string& s )
			{
				if (s.size() != item.size()) return false;
				auto result = std::equal( s.begin(), s.end(), item.begin(), []( char c1, char c2 ) { return (tolower( c1 ) == tolower( c2 )); } );
				return result;
			}

			NOpcode( string _item ) :item( _item ) {};
		};


		Command* generateOpcode( Opcode op, string x, string y )
		{
			if (op == OPNewVar)return new Command(op, x ,y );
			if (op == OPLabel)return new Command(op, x );
			if (op == OPSet)return new Command(op, x ,y  );
			if (op == OPWrite)return new Command(op, x );
			if (op == OPQuit)return new Command( op );

			
			return nullptr;
		}


		vector< Command*>  parseOpcodes( list<std::string> lines )
		{
			CInform::GrammarContext* gtx = new CInform::GrammarContextCode();
			map<Opcode, HMatchExpended > opcodes;
			opcodes[Opcode::OPNewVar] = CInform::Match::getPMatchExpended( "new variable X Y ", gtx );
			opcodes[Opcode::OPLabel] = CInform::Match::getPMatchExpended( "label X ", gtx );
			opcodes[Opcode::OPSet] = CInform::Match::getPMatchExpended( "set X Y", gtx );
			opcodes[Opcode::OPWrite] = CInform::Match::getPMatchExpended( "write X ", gtx );
			opcodes[Opcode::OPQuit] = CInform::Match::getPMatchExpended( "quit  ", gtx );

			CInform::Match::TMatch   ematch;
			list< Command*>  block;

			for (auto line : lines)
			{
				//cout << "[" << line << "]" << endl;
				list<string>  xs = logical_split( line );
				//for (auto& x : xs){cout << x << "|";}cout << endl;

				list<CInform::Match::HNoum > hs;
				std::transform( xs.begin(), xs.end(), std::back_inserter( hs ), [&]( string si ) { return std::make_shared<NOpcode>( si ); } );

				bool hasOC = false;
				for (auto kv : opcodes)
				{
					auto mList = ematch.MatchLine( hs, kv.second );

					for (auto& mm : mList)
					{
						cout << kv.first << "  " << mm.repr();
						cout << endl;
						hasOC = true;
						//cout << "X:"<<  mm.bind["X"].literals.front()->repr();
						//cout << endl;
						string sx = "";
						string sy = "";

						if (mm.bind.find("X") != mm.bind.end()) sx = mm.bind["X"].literals.front()->repr();
						if (mm.bind.find( "Y" ) != mm.bind.end()) sy = mm.bind["Y"].literals.front()->repr();

						
						auto g = generateOpcode( kv.first,sx,sy);
						block.push_back( g );
						break;;

					}
				}
				if (hasOC == false)	cout << "fail " << line << endl;


			}
			return 	vector< Command*>( block.begin() , block.end() );
		}
	}
}