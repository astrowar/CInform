
#ifndef PREPROSHPP
#define  PREPROSHPP
#include  "cinform.hpp"
#include  <vector>
#include  "preprocess.hpp"
#include <iostream>
#include <memory>

using namespace std;
namespace CInform
{
	//pre processador basico do parser...   
	// subdivide o texto em partes .. separa a identacao, comentarios , literais , multiplas frases no mesmo paragrafo ....





	//HTextBlock  getTextBlockLiteral(int &i, string& txt)
	//{
	//	//processa ate achar um " solto
	//	string buffer = "";
	//	int n = txt.size();
	//	while (i < n)
	//	{
	//		char c = txt[i];
	//		i++;
	//		if (c == '"') //fim do comando 
	//		{
	//			return std::shared_ptr< TextBlock >(make_shared<LiteralBlock>(buffer));
	//		}
	//	}
	//	return std::make_shared< LiteralBlock >(buffer);  //acabou o texto
	//}

	//HTextBlock  getTextBlock(int &i, string& txt, int barcketLevel)
	//{
	//	string buffer = "";
	//	int n = txt.size();
	//	list<HTextBlock> acc;
	//	while (i < n)
	//	{
	//		char c = txt[i];
	//		i++;
	//		if (c == '"')
	//		{

	//			acc.push_back(std::make_shared<TextBlock>(buffer));
	//			buffer = "";
	//			auto binner = getTextBlockLiteral(i, txt);
	//			acc.push_back(binner);
	//			continue;
	//		}

	//		if (c == '(')
	//		{

	//			acc.push_back(std::make_shared<TextBlock>(buffer));
	//			buffer = "";
	//			auto binner = getTextBlock(i, txt, barcketLevel + 1);
	//			acc.push_back(binner);
	//			continue;
	//		}
	//		if (c == ')')
	//		{
	//			if (barcketLevel > 0)
	//			{
	//				return std::make_shared<TextBlock>(buffer);
	//			}
	//			else
	//			{
	//				// return std::make_shared<TextBlockError>( "Unbalanced Brackets" );
	//				return nullptr;
	//			}

	//		}
	//	}

	//	acc.push_back(std::make_shared<TextBlock>(buffer));
	//	return std::make_shared<TextBlock>(acc);
	//}



	list<wstring_located>  getParagraphsSplited(string& txt, string filename)
	{
		list<wstring_located>  actual;

		int pivot = 0;
		int i = 0;
		int n = txt.size();
		int col = 1;
		int line = 1;


		while (i < n)
		{
			if (i == '\n')
			{
				actual.push_back(wstring_located(txt.substr(pivot, i - pivot), filename, line, 0));
				pivot = i;
				line++;
			}
			i++;
		}
		//adiciona o resto
		actual.push_back(wstring_located(txt.substr(pivot, i - pivot), filename, line, col));
		return actual;
	}

	int  getLiteralBlockEnd(string txt, int start)
	{
		int i = start;
		int n = txt.size();
		// c1, c2, c3 ...
		while (i < n)
		{
			char c1 = ' ';
			char c2 = ' ';
			char c3 = ' ';
			if (i > 2)   c1 = txt[i - 2];
			if (i > 1)  c2 = txt[i - 1];
			c3 = txt[i];
			i++;
			if ((c3 == '"') && (c2 == '"') && (c1 == '"'))
			{
				//fim do block Literal
				return i;
			}
		}
		return n;
	}


	int  getCharEnd(string txt, int start, char cend)
	{
		int i = start;
		int n = txt.size();
		// c1, c2, c3 ...
		while (i < n)
		{
			char c3 = txt[i];
			i++;
			if ((c3 == cend))
			{
				//fim do block Literal
				return i;
			}
		}
		return n;
	}


	int  getLiteralLineEnd(string txt, int start)
	{
		return getCharEnd(txt, start, '"');
	}

 
	std::pair<shared_ptr<LiteralBlock>, int>  getParagraphsSplited_LiteralTerm(string txt, int start, function<void(int)> error_handle)
	{
		int i = start;
		int n = txt.size();
		while (i < n)
		{
			char c1 = ' ';
			char c2 = ' ';
			if (i > 2)   c1 = txt[i - 2];
			if (i > 1)  c2 = txt[i - 1];
			char   c3 = txt[i];
			i++;
			if (c3 == '"')
			{
				//bloco literal de muitas linhas 
				shared_ptr<LiteralBlock> a = std::make_shared<	LiteralBlock>(txt.substr(start, i - start - 1));
				return std::make_pair(a, i);
			}
		}
		error_handle(start);
		return std::make_pair(nullptr, i);

	}

	//espera opelo comenratio de bloco final
	std::pair<shared_ptr<LiteralBlock>, int>  getParagraphsSplited_LiteralBlock(string txt, int start, function<void(int)> error_handle)
	{
		int i = start;
		int n = txt.size();
		while (i < n)
		{
			char c1 = ' ';
			char c2 = ' ';
			if (i > 2)   c1 = txt[i - 2];
			if (i > 1)  c2 = txt[i - 1];
			char   c3 = txt[i];
			i++;
			if (c3 == '"')
			{
				//bloco literal de muitas linhas
				if (c2 == '"' && c1 == '"')
				{
					shared_ptr<LiteralBlock> a = make_shared<LiteralBlock>(txt.substr(start, i - start - 1));
					return std::make_pair(a, i);
				}
			}
		}
		error_handle(start);
		return std::make_pair(nullptr, i);

	}
	std::pair<HSourceBlock, int>   getParagraphsSplited_line( string txt, int start, int end, function<void( int )> error_handle );

	//apenas separa o codeigo dos demais termos
	std::pair<HSourceBlock,int>  getParagraphsSplited_Parantsisi(string txt, int start, int end, function<void(int)> error_handle)
	{
		int i = start;
		int n = end;
		list<HSourceBlock> groups;


		while (i < n)
		{
			char c1 = ' ';
			char c2 = ' ';
			if (i > 2)   c1 = txt[i - 2];
			if (i > 1)  c2 = txt[i - 1];
			char   c3 = txt[i];
			i++;

			if (c3 == '"')
			{
				//bloco literal de muitas linhas
				if (c2 == '"' && c1 == '"')
				{
					std::pair<shared_ptr<LiteralBlock>, int> ret = 	getParagraphsSplited_LiteralBlock(txt, i, error_handle);
					i = ret.second;
					groups.push_back(ret.first);
					start = i;
				}
				else
				{
					std::pair<shared_ptr<LiteralBlock>, int> ret = getParagraphsSplited_LiteralTerm(txt, i, error_handle);
					i = ret.second;
					groups.push_back(ret.first);
					start = i;
				}
			} 

			if (c3 == '(')
			{
				std::pair<HSourceBlock, int> ret = getParagraphsSplited_Parantsisi(txt, i, end, error_handle);
				i = ret.second;
				start = i;
				groups.push_back(ret.first);
			}

			if (c3 == ')')
			{			 
				if (i - 1 - start > 0)
				{
					string atxt = txt.substr( start, i - start -1 );
					auto ps = getParagraphsSplited_line( txt, start, i-1,   error_handle );
					groups.push_back( ps.first );

					//groups.push_back( make_shared<CodeBlock>( txt.substr( start, i - 1 - start ) ) );
				}
				if (groups.size() == 1)
				{
					return std::make_pair(groups.front(), i);
				}
				if (groups.empty())   return std::make_pair(nullptr, i);

				shared_ptr<ListBlock> q = make_shared<ListBlock>( groups );
				return std::make_pair(q, i);
			}

			if (c3 == '\n')
			{
				error_handle(start);
				return std::make_pair(nullptr, i);
			} 
		}

		error_handle(start);
		return std::make_pair(nullptr, i);
	}

	std::pair<HSourceBlock, int>   getParagraphsSplited_line(string txt, int start, int end, function<void(int)> error_handle)
	{
		int i = start;
		//int n = txt.size();
		int  pivot = start;
		list<HSourceBlock> groups;


		while (i < end)
		{
			char c1 = ' ';
			char c2 = ' ';
			if (i > 2)   c1 = txt[i - 2];
			if (i > 1)  c2 = txt[i - 1];
			char   c3 = txt[i];
			i++;


			if (c3 == '"')
			{

				//bloco literal de muitas linhas
				if (c2 == '"' && c1 == '"')
				{
					if (i - 1 - pivot > 0)groups.push_back(make_shared<CodeBlock>(txt.substr(pivot, i - 1 - pivot)));

					std::pair<HSourceBlock, int> ret = getParagraphsSplited_LiteralBlock(txt, i, error_handle);
					i = ret.second;
					groups.push_back(ret.first);
					pivot = i;
				}
				else
				{
					if (i - 1 - pivot > 0)groups.push_back( make_shared<CodeBlock>(txt.substr(pivot, i - 1 - pivot)));

					std::pair<HSourceBlock, int> ret = getParagraphsSplited_LiteralTerm(txt, i, error_handle);
					i = ret.second;
					groups.push_back(ret.first);
					pivot = i;
				}
			}

			if (c3 == '(')
			{
				//adiciona os termos ate agora
				string conteudo = txt.substr(pivot, i - 1 - pivot);
				if (i - 1 - pivot > 0) groups.push_back(make_shared<CodeBlock>(conteudo));

				std::pair<HSourceBlock, int> ret = getParagraphsSplited_Parantsisi(txt, i , end, error_handle);
				i = ret.second;
				groups.push_back( make_shared <ParentesisSourceBlock>(ret.first));

				pivot = i;
			}

			if (c3 == '\n')
			{
				if (i - 1 - pivot > 0)
				{
					string subtext = txt.substr(pivot, i - 1 - pivot);
					groups.push_back(make_shared<CodeBlock>(subtext));
				}
				if (groups.size() == 1)
				{
					return std::make_pair(groups.front(), i);
				}				
				if (groups.empty())
				{
					std::make_pair(nullptr, i);
				}
				if (groups.empty())   return std::make_pair(nullptr, i);
				return std::make_pair( make_shared<ListBlock>(groups), i);
			}

			if (c3 == ')')
			{
				error_handle(start);
				return std::make_pair(nullptr, i);
			}

		}

		if (i -  pivot > 0 ) groups.push_back(make_shared<CodeBlock>(txt.substr(pivot, i - pivot)));
		if (groups.size() == 1)
		{
			return std::make_pair(groups.front(), i);
		}
		if (groups.empty())std::make_pair(nullptr, i);
		 
			
		return std::make_pair(make_shared<ListBlock>(groups), i);
	}


	bool isEmpty(string txt)
	{
		for (size_t i = 0; i < txt.size(); ++i)
		{
			if ((txt[i] != ' ') && (txt[i] != '\t') && (txt[i] != '\n') && (txt[i] != '\r')) return false;
		}
		return true;
	}

	bool isEmpty(HSourceBlock s)
	{
		if (s == nullptr) return true;
		if (s->type == TLIST)
		{
			auto ss =  dynamic_pointer_cast<ListBlock>(s );
			HSourceBlock inicio = ss->inner.front();
			if (ss->inner.size() == 1) return isEmpty(inicio);
			return false;
		}

		if (s->type == TCODE)
		{
			HCodeBlock c = dynamic_pointer_cast<CodeBlock>(s );
			return  isEmpty(c->txt);
		}

		if (s->type == TLITERAL)
		{
			HLiteralBlock  c = dynamic_pointer_cast<LiteralBlock>(s );
			return  isEmpty(c->txt);
			
		}

		throw "error";
		return false;

	}

	list<HSourceBlock>   getParagraphsSplited(string txt, int start,int end, function<void(int)> error_handle)
	{
		int i = start;
		//int n = txt.size(); 
		
		int ipivot = start;
		list<HSourceBlock> groups;
		while (i < end)
		{
			auto ret = getParagraphsSplited_line(txt, i, end, error_handle);
			if (isEmpty(ret.first) ==false ) 
			{
			 
				groups.push_back(ret.first);
			}
			i = ret.second;

		}

		return groups;
	}

	//list<SParagraph>  getParagraphsSplited_old(string txt, int start, function<void(int)> error_handle)
	//{
	//	int pLevel = 0;
	//	int ipivot = start;
	//	int i = start;
	//	int n = txt.size();
	//	// c1, c2, c3 ...

	//	list<SParagraph> sp;
	//	list<StringGroup> groups;

	//	while (i < n)
	//	{
	//		char c1 = ' ';
	//		char c2 = ' ';
	//		if (i > 2)   c1 = txt[i - 2];
	//		if (i > 1)  c2 = txt[i - 1];
	//		char   c3 = txt[i];
	//		i++;

	//		if (c3 == '"')
	//		{
//			//bloco literal de muitas linhas
//			if (c2 == '"' && c1 == '"')
//			{
//				if ((i - ipivot) > 0)  groups.push_back(StringGroup(txt.substr(ipivot, i - ipivot), GTextType::TCODE));

//				int inext = getLiteralBlockEnd(txt, i);
//				if (inext == n)
//				{
//					error_handle(i);
//				}
//				groups.push_back(StringGroup(txt.substr(i, inext - i -1 ), GTextType::TLITERAL));
//				i = inext;
//				ipivot = i;
//			}
//			else
//			{   
//				if ((i - ipivot - 1) > 0 ) 	groups.push_back(StringGroup(txt.substr(ipivot, i - ipivot-1), GTextType::TCODE));

//				int inext = getLiteralLineEnd(txt, i);
//				groups.push_back(StringGroup(txt.substr(i, inext - i- 1), GTextType::TLITERAL));
//				i = inext;
//				ipivot = i;
//			}
//		}



//		if (c3 == '\n')
//		{
//			//make_shared<line> in code part ....
//			groups.push_back(StringGroup(txt.substr(ipivot, i - ipivot - 1), GTextType::TCODE));
//			ipivot = i;
//			sp.push_back(SParagraph(groups));
//			groups.clear();
//		}
//	}
//	groups.push_back(StringGroup(txt.substr(ipivot, i - ipivot), GTextType::TCODE));
//	sp.push_back(SParagraph(groups));

//	return sp;
//}
int get_ident(string& x)
{
	int cc = 0;
	for (size_t i = 0; i < x.size(); ++i)
	{		
		if (x[i] == ' ') { cc += 1; }
		else if (x[i] == '\t') { cc += 4; }
		else if (x[i] == '\r') { return -1; }
		else if (x[i] == '\n') { return -1; }
		else return cc;
	}
	return cc;
}

	int get_ident(HSourceBlock b)
	{
		if (b->type == TCODE)
		{
			auto  c = dynamic_pointer_cast<CodeBlock>(b);
			return c->ident;
			//return get_ident(c->txt);
		}

		if (b->type == TLIST)
		{
			auto  c = dynamic_pointer_cast<ListBlock>(b);
			if (c->inner.empty()) return -1;
			return get_ident(c->inner.front());
		}

		if (b->type == TLITERAL)
		{
			auto  c = dynamic_pointer_cast<LiteralBlock>(b);
			return get_ident(c->txt);
		}


		throw "error";
		return 0;
	}

	string  clear_ident(string  s)
	{
		int i = 0;
		while (s[i] == ' ' || s[i] == '\t') { i++; }
		return  s.substr(i, s.size() - i); // remove o inicio
	}
	
	HSourceBlock   clear_ident(HSourceBlock  s)
	{
		if (s->type == TLIST)
		{
			auto ss = dynamic_pointer_cast<ListBlock>(s);
			HSourceBlock inicio = ss->inner.front();
			clear_ident(inicio);
			return s;
		}
		if (s->type == TCODE)
		{
			auto c = dynamic_pointer_cast<CodeBlock>(s);
	 
			c->txt = clear_ident(c->txt);
			return s;
		}

		if (s->type == TLITERAL)
		{
			auto c = dynamic_pointer_cast<LiteralBlock>(s);
			c->txt = clear_ident(c->txt);
			return s;
		}


		throw "error";
		return nullptr;
	}


	list<HSourceBlock> clear_ident(list<HSourceBlock> ss)
	{
		list<HSourceBlock> ret;
		for (auto s : ss) ret.push_back(clear_ident(s));
		return ret;
	}

	HSourceBlock concatenate_ps(list<HSourceBlock> ss)
	{
		if (ss.size() == 0)
		{
			throw "empty List";
		}
		if (ss.size() == 1) return ss.front();
		return make_shared<ListBlock>(ss);
	}


	list<HSourceBlock> splitPeriod_inner(list<HSourceBlock> ss)
	{
		list<HSourceBlock> Ret; 
		list<HSourceBlock> acc; //acumulados ate agora 

		for (auto s : ss)
		{
			if (s->type == TCODE)
			{
				auto  c = dynamic_pointer_cast<CodeBlock>(s);

				string buffer = c->txt + "";
				std::size_t found = 0;
				while (found != std::string::npos)
				{
					found = buffer.find('.');
					if (found != std::string::npos)
					{
						string tail = buffer.substr(found+1, buffer.size() - found -1);
						string head = buffer.substr(0, found);
						if (head.size() > 0)
						{
							acc.push_back(make_shared<CodeBlock>(head));
						}
						Ret.push_back(concatenate_ps(acc));
						acc.clear();
						buffer = tail;
					}
					else
					{
						if (buffer.size() > 0)
						{
							acc.push_back(make_shared<CodeBlock>(buffer));
						}
					}
				}
			}
			else
			{
				acc.push_back( s );
			}
		}

		if (acc.empty() == false)	Ret.push_back(concatenate_ps(acc));
		acc.clear();

		return Ret;

	}

	list<HSourceBlock> splitPeriod(HSourceBlock s)
	{
		list<HSourceBlock> ret;

		if (s->type == TLIST)
		{
			auto  ss = dynamic_pointer_cast<ListBlock>(s);
			if (ss->inner.size() == 1)
			{
				return splitPeriod(ss->inner.front());
			}
			return splitPeriod_inner(ss->inner);
		}

		return splitPeriod_inner({ s });
	}


	string getRepr(HSourceBlock s)
	{
		if (s->type == TLIST)
		{
			auto  ss = dynamic_pointer_cast<ListBlock>(s);
			for (auto g : ss->inner)
			{
				
				return "[" + getRepr(g) + "]";
				
			}
		}

		if (s->type == TCODE)
		{
			auto c = dynamic_pointer_cast<CodeBlock>(s);
			return  c->txt;
		}

		if (s->type == TLITERAL)
		{
			auto  c = dynamic_pointer_cast<LiteralBlock>(s);
			//cout << c->txt;
			return  "LITERAL";
		}
		return "?";
	}


	void printRepr(  HSourceBlock s)
{
	if (s->type == TLIST)
	{
		auto  ss = dynamic_pointer_cast<ListBlock>(s);
		for (auto g : ss->inner)
		{
			cout << "[";
			printRepr(g);
			cout << "]";
		} 
	}

	if (s->type == TCODE)
	{
		auto c = dynamic_pointer_cast<CodeBlock>(s);
		cout << c->txt;
	}

	if (s->type == TLITERAL)
	{
		auto  c = dynamic_pointer_cast<LiteralBlock>(s);
		//cout << c->txt;
		cout << "LITERAL";
	}

}

 


	
	string printRepr(string prev , SParagraph *s_in)
	{
		string se = "";
		SParagraph *s = s_in;

		while (s != nullptr)
		{ 
			se =se + prev;
			for(auto line: s->periods)	se += getRepr( line);
			se+=  "\n";
			if (s->inner != nullptr) se+=printRepr(prev + "_", s->inner);
			s = s->next;
		}
		return se;
	}

void printRepr(SParagraph *s)
{
	cout << printRepr("", s);
}


//HTextBlock  getTextBlock(string& txt)
//{
//	int  i = 0;
//	int barcketLevel = 0;
//	return getTextBlock(i, txt, barcketLevel);
//}

void CallError(int charPos)
{
	cout << "Error !! \n";
}


list<HSourceBlock>  getParagraphs(string txt)
{
	list<HSourceBlock>  s = getParagraphsSplited(txt, 0, txt.size(), [](int _i) { CallError(_i); });
	return s;
}


	 list<HSourceBlock>::iterator   getParagraphGroup_it( int ident , SParagraph* actual_in, list<HSourceBlock>::iterator sbegin , list<HSourceBlock>::iterator send)
	{
		list<HSourceBlock>::iterator it = sbegin; 

		SParagraph* actual = actual_in;
		while (it != send)
		{
			int actual_ident = get_ident(*it);
			//cout  << "I:" << actual_ident << (*it)->repr() + "\n";;
			

			if (actual_ident < 0)
			{
				++it;
				continue; //empty paragraph
			}
			
			if (actual_ident < ident)
			{
				return   it;
			}


			if (actual_ident == ident)
			{
				actual->next =  new SParagraph(*it) ;
				actual = actual->next; 
				++it;
				continue;
			}

			if (actual_ident > ident)
			{				
				if (actual->inner != nullptr)
				{
					throw "identition Error";
				}

				  actual->inner = new SParagraph(*it); 
				  ++it;
				  list<HSourceBlock>::iterator   next_it = getParagraphGroup_it(actual_ident, actual->inner , it, send);
				  it = next_it; 
			} 
		}
	 

		return   it;
		

	}

	SParagraph*  getParagraphGroups(list<HSourceBlock> plist)
	{
		list<HSourceBlock>::iterator it = plist.begin();
		
		SParagraph* root  = new SParagraph( make_shared<CodeBlock>("") );
		
	    auto it_list = getParagraphGroup_it(0, root, it, plist.end());
		return  root->next;
	}
	string ListBlock::repr() const
	{
		string s =  "(";
		bool ft = true;
		for (auto si : this->inner)
		{
			if (ft == false) { 
				s = s + " " + si->repr(); 
			}
			else
			{ 
				s = s +  si->repr(); 
			}
			ft = false;
		}
		return s + ")";

	}

	//mantenha como ponteiros
	SourceBlock* SourceBlock::copy(SourceBlock *c)
	{
		if (c->type == TLITERAL)
		{
			auto cx = dynamic_cast<LiteralBlock*>(c);
			return new LiteralBlock(cx->txt);
		}

		if (c->type == TCODE)
		{
			auto cx = dynamic_cast<CodeBlock*>(c);
			return new CodeBlock(cx->txt);
		}

		if (c->type == TLIST)
		{
			auto cx = dynamic_cast<ListBlock*>(c);
			list<HSourceBlock> cc;
			for (auto x : cx->inner) cc.push_back( std::shared_ptr<SourceBlock>(SourceBlock::copy(x.get())));
			return new ListBlock( cc );
		}
		throw "error on copy";
	}


	string trimm(string x)
	{
		if (x.empty()) return x;

		while (x.empty()==false && x[0] == ' ')  x = x.substr(1, x.size());
		while (x.empty() == false && x[x.size()-1] == ' ')  x = x.substr(0, x.size()-1);
		return x;
	}

	CodeBlock::CodeBlock(string _txt) : txt( (_txt)), SourceBlock(TCODE)	
	{
		ident = get_ident(_txt);
		txt = trimm(_txt);
	 
	}
	string ParentesisSourceBlock::repr() const
	{
		return  inner->repr() ;
		//return "'(" + inner->repr() + ")";
	}
	 
	CodeWord::CodeWord( string _txt ) : txt(_txt), SourceBlock( TWORD )
	{
		if (txt == "\t")
		{
			throw "error on null string";
		}
	}

}
#endif
