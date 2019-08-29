 

#ifndef PREPROCC_HPP
#define  PREPROCC_HPP

#include  <vector>
#include  <list>
#include  <functional>
#include  <memory>
#include  <string>


using namespace std;
namespace CInform
{

	enum  GTextType { TCODE, TCOMMENT, TLITERAL , TLIST , TPARENTESIS , TWORD	};

	class TextBlock;
	using HTextBlock = shared_ptr<TextBlock>;

	class TextBlock
	{
	public:
		//class Base
		list<HTextBlock> contents;
		list<HTextBlock> childs;
		TextBlock() {};
		TextBlock(string x) {};
		TextBlock(list<HTextBlock> x) {};
	};


 

	class CodeParagph : public TextBlock
	{

	};

	 


	class CommentBlock : public TextBlock
	{
		string value;
	};

	class TextBlockError : public TextBlock
	{
		string value;
	};


	class wstring_located
	{
	public:
		string str;
		int lineNumber;
		int colNumber;
		string  filename;

		wstring_located(string _str, string  _filename, int _lineNumber, int _colNumber) :str(_str), filename(_filename), lineNumber(_lineNumber), colNumber(_colNumber)
		{

		}
	};

 

	class SourceBlock
	{
	public: 
		GTextType type;
		SourceBlock(GTextType _type) : type(_type) {}
		virtual ~SourceBlock() {};
		virtual string repr() const = 0 ;
	    static SourceBlock* copy(SourceBlock* c);
	};

	using HSourceBlock =  shared_ptr<SourceBlock>;


	class ParentesisSourceBlock:public SourceBlock
	{
	public:
		HSourceBlock inner;
		ParentesisSourceBlock(HSourceBlock _inner) : inner(_inner), SourceBlock(TPARENTESIS) 
		{ 
			//if (inner->repr().find( ' ' ) != string::npos)
			//{
			//	throw "error on PR";
		 // }

		}
		virtual ~ParentesisSourceBlock() {};
		virtual string repr() const;
 
	};


	class ListBlock : public SourceBlock
	{
	public:
		list<HSourceBlock> inner;
		ListBlock(list<HSourceBlock> _inner) :inner(_inner), SourceBlock(TLIST)
		{
			if (_inner.empty())
			{
				throw "error on list";
			}
		}
		virtual string repr() const  ;
	};


	class CodeBlock : public SourceBlock
	{
	public:
		string txt;
		int ident;
		CodeBlock(string _txt);
		virtual string repr() const {
			return txt;
		};
	};

 
		class CodeWord : public SourceBlock
	{
	public:
		string txt;
		 
		CodeWord( string _txt );
		virtual string repr() const {
			return txt;
		};
	};


	class LiteralBlock : public SourceBlock
	{
	public:
		string txt;
	 
		LiteralBlock(string _txt ) : txt(_txt), SourceBlock(TLITERAL)
		{
		}
		virtual string repr() const  {
			return "LITERAL";
		};
	};




	using HCodeBlock = shared_ptr<CodeBlock>;
	using HLiteralBlock = shared_ptr<LiteralBlock>;


	//class StringGroup
	//{
	//public:
	//	string txt;
 //
	//	StringGroup(string _txt, GTextType _type) : txt(_txt), type(_type)
	//	{
	//	}
	//};
	HSourceBlock  clear_ident(HSourceBlock b);
	list<HSourceBlock>  clear_ident(list<HSourceBlock> b);
	int get_ident(string& x);

	list<HSourceBlock> splitPeriod(HSourceBlock);

	class SParagraph
	{
	public:
		list<HSourceBlock> periods;
		SParagraph* next;
		SParagraph* inner; //linhas de identacao
		SParagraph(HSourceBlock _line) : periods({ splitPeriod(clear_ident(_line)) }), next(nullptr), inner(nullptr){ }
		SParagraph(list<HSourceBlock> _periods) : periods(clear_ident(_periods)), next(nullptr), inner(nullptr) { }
	};
	 

	string getRepr(HSourceBlock s);
	void printRepr(HSourceBlock s);
	string printRepr(string prev, SParagraph *s_in);
	void printRepr(SParagraph* s);
	list<HSourceBlock>  getParagraphs(string txt);
	HTextBlock  getTextBlock(string txt);

	SParagraph*  getParagraphGroups(list<HSourceBlock> plist);
	

}


#endif