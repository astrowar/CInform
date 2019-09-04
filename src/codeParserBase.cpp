#include "CodeParser.hpp"

#include "cinform.hpp"
#include "Grammar.hpp"
#include "PreCodeGenerate.hpp"
#include <pmatch_baseClass.hpp>
#include "parseEntry.hpp"
#include "Nsource.hpp"
#include <string>
#include <iostream>


using namespace std;

namespace CInform
{
	using namespace Match;

	namespace CodeParser
	{

 

		PreCodeGenerate* Higienyze(PreCodeGenerate* e)
		{
			if (e == nullptr) return nullptr;
			if (auto gEmp = dynamic_cast<const PreCodeGenerateEmpty*>(e))
			{
				return  Higienyze(gEmp->tail);
			}
			e->tail = Higienyze(e->tail);
			return e;
		}
		
		PreCodeGenerate* processSourcePreCodeGenerateTerm(ParserStore *pstore, PreCodeGenerate* prev_generate, CInform::Match::TMatch&   ematch,
			list<CInform::Match::HNoum >& hs, pair<string, HMatch> parserentry)
		{
			PreCodeGenerate* lastError = nullptr;
			list< MatchResult >  mList = ematch.MatchLine(hs, parserentry.second);
			if (mList.empty() == false)
			{
				for (auto& mm : mList)
				{
					if (lastError ==nullptr) lastError = new PreCodeGenerateError( "Syntax Error" );
					//cout << mm.repr();
					//cout << endl;
					//cout << "M:" << parserentry.entryName << " -> ";
					//cout << endl;
					TBlockGroupItemNoum vx = mm.getResult("X");
					TBlockGroupItemNoum vy = mm.getResult("Y");
					TBlockGroupItemNoum vz = mm.getResult("Z");

					PreCodeGenerate* codeGen = codeGenerate(pstore, prev_generate, parserentry.first, vx, vy,vz);
					codeGen = Higienyze(codeGen);

					if (codeGen == nullptr)
					{
						continue;
						return nullptr;
					}

					if (codeGen->isError() == false) 
					{ 
						 
						return codeGen; 
					}
					else
					{
						lastError = codeGen;
					} 
				}
			} 

			return lastError;
		}

		PreCodeGenerate* processSourcePreCodeGeneratePeriod(HSourceBlock line, PreCodeGenerate* prev_generate ,ParserStore *pstore, CInform::Match::TMatch&   ematch, ParserEntryGroup* parserentries)
		{
			list<CInform::HSourceBlock> xs = CInform::Match::split_code(line, ' ');
			list<CInform::Match::HNoum > hs;
			std::transform(xs.begin(), xs.end(), std::back_inserter(hs), [&](CInform::HSourceBlock si) { return std::make_shared<NSource>(si); });
			 

			PreCodeGenerate* this_generate = nullptr;
			PreCodeGenerate* lastError = nullptr;

			//testa todas as entrada possiveis para dar match na struct de entrada
			for (auto kv : parserentries->entryName_patten)
			{

				PreCodeGenerate* codeGen = processSourcePreCodeGenerateTerm(pstore, prev_generate, ematch, hs, kv);
							   
				 
				if (codeGen == nullptr)  continue;

				if (codeGen->isError())
				{
					lastError = codeGen;
					break;
					continue;
				}
				else
				{
					this_generate = codeGen;
					break;
				}
			}

			if (this_generate == nullptr)
			{
				//nada foi criado ... retorna o ultimo erro entao
				return lastError;
			}
			return this_generate;

		 
		}


		PreCodeGenerate* processSourcePreCodeGenerate(ParserStore *pstore, CInform::Match::TMatch&   ematch, SParagraph* code_line, ParserEntryGroup* parserentries)
		{
			PreCodeGenerate* generate = nullptr;
			PreCodeGenerate* prev_generate = nullptr;
	  

			for (HSourceBlock period : code_line->periods)
			{
	  
				PreCodeGenerate* this_generate = processSourcePreCodeGeneratePeriod(period, prev_generate, pstore,  ematch, parserentries);


				if (this_generate == nullptr)
				{  
					
					return new PreCodeGenerateError("syntax error: "+ getRepr( period));
				} 
				//this_generate != null
		 

					if (this_generate->isError())
					{
						return this_generate; //eh um erro de qualquer forma
					}

				 
					if (auto gEmp = dynamic_cast<PreCodeGenerateDependency*>(this_generate))
					{
						//oh no ! essa linha eh valida , mas depende de um simbolo que ainda nao foi definido


					}


					if (generate == nullptr)
					{
						generate = this_generate;
					}
					else
					{
						prev_generate = this_generate;
					} 
			}
			return generate;
		}
	 
		 



		class DelayedTerm
		{
		public:
			string symbol;
			SParagraph* code_paragraph;
			DelayedTerm(string _symbol, SParagraph* _code_paragraph) : symbol(_symbol), code_paragraph(_code_paragraph) {}
		};


		void add_code(string ident, map<string, list<PreCodeGenerate*> > &sessions, string session_name, PreCodeGenerate* gs)
		{
			do
			{
				cout << ident <<  gs->repr() << endl;
				if (auto gSee = dynamic_cast<const PreCodeGenerateSession*>(gs))
				{
					add_code(ident+"  ",sessions, gSee->session, gSee->inner);
				}
				else if (auto gSee = dynamic_cast<const PreCodeGenerateBlock*>(gs))
				{
					add_code(ident ,sessions, session_name, gSee->header);
					add_code(ident + "  ",sessions, session_name, gSee->inner);
				}
				else
				{
					sessions[session_name].push_back(gs);
				}
				gs = gs->tail;
			} while (gs != nullptr);

		}

		map<string, list<PreCodeGenerate*> > processSource( ParserStore *pstore, SParagraph* code, ParserEntryGroup* parserentries )
		{

			map<string ,list<PreCodeGenerate*> > sessions;

 

			//list<PreCodeGenerate*> resuts;
			CInform::Match::TMatch   ematch;
			list<DelayedTerm> delayed;


			SParagraph* sp = code;
			int dependecy_cicle_loop = 0;

			while (sp != nullptr || delayed.size() >0  )
			{
				if (dependecy_cicle_loop > 10)
				{
					add_code("",sessions,"error", new PreCodeGenerateError("ciclic dependecy"));
					break;
				}
				bool useDelayed_code = false;
				SParagraph* actural_sp = sp;
				//procura por dependencias 
				for (auto it  = delayed.begin(); it != delayed.end(); ++it )
				{
					dependecy_cicle_loop++;
					DelayedTerm &dep = *it;
					if (pstore->isSymbol(it->symbol)) // o simbolo ja existe ?
					{
						actural_sp = it->code_paragraph;
						delayed.erase(it);
						useDelayed_code = true;
						break;
					}
				}


				if (actural_sp == nullptr) break;
				
				
					if (actural_sp->periods.empty()) continue;
				 

					string r_actural_sp = ListSrcRepr( actural_sp->periods );
					if (r_actural_sp == "(exit)") break;

					PreCodeGenerate* generateCode = processSourcePreCodeGenerate(pstore, ematch, actural_sp, parserentries);
					PreCodeGenerate* gs = generateCode;

					if (auto gDep = dynamic_cast<const PreCodeGenerateDependency*>(generateCode))
					{
						delayed.push_back(DelayedTerm(gDep->requiredSymbol, actural_sp)); //isere no final da fila
						goto end_loop;
					}
					else
					{
						dependecy_cicle_loop = 0; //alguma linha foi devidamente processada ?
					}
				
			
				add_code("",sessions, "code", gs);
				/*do
				{
					cout << gs->repr() << endl;
					resuts.push_back(gs);
					gs = gs->tail;
				} while (gs != nullptr);*/

				//resuts.push_back(generateCode);

				if (generateCode->isError())
				{
					break; //pode parar ...nao da mais para confiar no resto do codigo
				}

			end_loop:

				if (useDelayed_code == false)
				{
					sp = sp->next;					
				}

			}

			for (auto d : delayed)
			{
				add_code("",sessions, "error", new   PreCodeGenerateDependency(d.symbol));
		 
			}

			return sessions;
		}


		list<string> emiteAsm(list<PreCodeGenerate*> itList)
		{
			string bg = "BEGIN";
			string ed = "END";
			string ident = "";
			bool is_end = false;
			list<string> asm_out;
			for (auto e : itList)
			{
				bool is_end = false;
				string s = e->repr();		
				if (strncmp(s.c_str(), ed.c_str(), ed.size()) == 0)
				{
				if (ident.size() > 0)
				{
					ident = ident.substr(0, ident.size() - 2);
					is_end = true;
				}
				}

				if (s.empty() == false)
				{
					asm_out.push_back(ident + s);
					if (is_end)asm_out.push_back("");
				}
				if (strncmp(s.c_str(), bg.c_str(), bg.size())==0)
				{
					ident = ident + "  ";
				}
				 

			}
			return asm_out;
		}

		  MatchedParagraph::MatchedParagraph(SParagraph * _paragraph, list<MatchResult> _mList) : paragraph(_paragraph), mList(_mList) {}

		  ParserEntry::ParserEntry(string _entryName, HMatchExpended _patten) : entryName(_entryName), patten(_patten) {}

		  ParserEntryGroup::ParserEntryGroup(list<ParserEntry> parserentries)
		{
			for (auto p : parserentries)
			{
				for (auto pe : p.patten->expended)
				{
					entryName_patten.emplace_back(p.entryName, pe);
				}
			}
			entryName_patten.sort([&](std::pair< std::string, HMatch > &a, std::pair< std::string, HMatch > &b) { return a.second->size() > b.second->size(); });
		}

		  void ParserEntryGroup::Add(ParserEntry p)
		{
			for (auto pe : p.patten->expended)
			{
				entryName_patten.emplace_back(p.entryName, pe);
			}
			entryName_patten.sort([&](std::pair< std::string, HMatch > &a, std::pair< std::string, HMatch > &b) { return a.second->size() > b.second->size(); });

		}

}
}