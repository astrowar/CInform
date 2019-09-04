
#include "cinform.hpp"
#include  "preprocess.hpp"
#include "pmatch_code.hpp"
#include "pmatch_baseClass.hpp"
#include "Grammar.hpp"
#include "CodeParser.hpp"
#include <iostream>
#include <fstream>

string repr(CInform::SSplitedPhase  Ls)
{
	string sr = "";
	for (CInform::SSplitedPhaseItem s : Ls.items)
	{
		sr = sr + " ( ";
		for (auto si : s)
		{
			sr = sr + si + " ";
		}
		sr = sr + ")";
	}
	return  sr;
}


class NSource : public  CInform::Match::MNoum
{
public:
	CInform::HSourceBlock  item;
	virtual string repr() {
		return item->repr();
	}
	virtual  bool isMatch(const string& s)
	{
		return CInform::Match::isSameCodeString(s, item);
	}
	NSource(CInform::HSourceBlock  _item) :item(_item) {};
};







int testes()
{
	std::list<string> file_names = { "instancias" , "phase_decl"   };
	for (auto n : file_names)
	{
		std::ifstream t_input("tests/parser/" + n + ".txt");
		std::string str((std::istreambuf_iterator<char>(t_input)), std::istreambuf_iterator<char>());
		auto sap = CInform::getParagraphs(str);
		auto sp = getParagraphGroups(sap);
		auto sp_start = sp;

		
		auto emite_code = CInform::CodeParser::emiteInter(sp);

		std::ofstream t_out("tests/parser/" + n + "_out.txt");
		for (auto e : emite_code) t_out << e << std::endl;
	}
	return 0;
}



int main()
{
	//for(int k =0;k< 300;++k)
	testes();
	return 0;


	//apenas faz os testes basicos
	CInform::GrammarContext* gtx = new CInform::GrammarContextEnglish();

	auto pk = CInform::Match::getPMatchExpended("?D[A/An/The/Some]  X is/are a/an Y ", gtx);
	string  ss = CInform::Match::getPMatchRepr(pk);
	 cout << ss << endl;
	 for (auto m : pk->expended)
	 {
		 cout << CInform::Match::__getPMatchRepr( m ) << endl;
	 }

	{


		std:: ifstream t_input("teste.ni"); 
		std::string str((std::istreambuf_iterator<char>(t_input)), std::istreambuf_iterator<char>());


		auto sap = CInform::getParagraphs(str);
		for (auto &s : sap)
		{
			// CInform::printRepr(s);
			// cout << "\n";
		}

		auto sp = getParagraphGroups(sap);
		auto sp_start = sp;
		//CInform::printRepr(sp);
		
		//while (sp != nullptr)
		//{
		//	cout << endl;		
		//	if (sp->periods.size() == 1)
		//	{
		//		cout << sp->periods.front()->repr();
		//	}
		//	else
		//	{
		//		for (auto line : sp->periods)
		//		{
		//			cout << "{";
		//			cout << line->repr();
		//			cout << "}";
		//		}
		//	}
		//	sp = sp->next;
		//	cout << endl;
		//}

		while (sp != nullptr)
		{
			
			for (auto line : sp->periods)
			{
				auto mList = CInform::Match::MatchLine( line, pk);
				//cout <<  line->repr() << endl;
				for (auto &mm : mList)
				{
					cout << mm.repr();
					cout << endl;
				}
				sp = sp->next;
				//cout << endl;
			}
		}

		sp = sp_start;
		{
			//testa com template
			
			//funcao que exibe cada item
			std::function<string(  CInform::HSourceBlock  const &x)> FREP = [&](  CInform::HSourceBlock const &x)
			{
				return x->repr();
			};


 


            CInform::Match::TMatch   ematch;

			while (sp != nullptr)
			{
				for (auto line : sp->periods)
				{
					list<CInform::HSourceBlock> xs = CInform::Match::split_code(line, ' ');
					list<CInform::Match::HNoum > hs;
					std::transform(xs.begin(), xs.end(), std::back_inserter(hs), [&]( CInform::HSourceBlock si){ return std::make_shared<NSource>( si ); } );
					auto mList = ematch.MatchLine ( hs, pk);
					
					for (auto& mm : mList)
					{
						cout << mm.repr( );
						cout << endl;
					}
					sp = sp->next;					
				}
			}

		}
		
	}


	if (false)
	{
		std:: ifstream t_input("teste.ni");
		int mline_counter = 0;
		for (std::string line; getline(t_input, line); )
		{
			string teste_input = line;
			auto mList = CInform::Match::MatchThis(teste_input, pk);
			for (auto &mm : mList)
			{
				 cout << mm.repr() << endl;
				mline_counter++;
				//if (mline_counter >= 200) exit (0);

			}
		}
	}

	return 0;
}

