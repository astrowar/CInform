#include "PreCodeGenerate.hpp"
#include "NSource.hpp"
#include <selector.hpp>

#include <cctype>
using namespace std;

namespace CInform
{
	using namespace Match;

	namespace CodeParser
	{


		list<PreCodeGenerateIL*> emiteILCode( ParserStore *pstore, SelectorItem  *sel );

		list<PreCodeGenerateIL*> emiteILCode_i( ParserStore *pstore, SelectorItem  *sel )
		{
			if (SelectorUnify* s_unify = dynamic_cast<SelectorUnify*>(sel))
			{
				return { new   PreCodeGenerateIL( "UNIFY", s_unify->target, s_unify->other ) };
			}

			if (SelectorKind* s_unify = dynamic_cast<SelectorKind*>(sel))
			{
				return { new   PreCodeGenerateIL( "UNIFY", s_unify->target, s_unify->kindRef ) };
			}

			if (SelectorOr* s_unify = dynamic_cast<SelectorOr*>(sel))
			{
				string c1_label = "lb_" + pstore->next_temp();
				string c2_label = "lb_" + pstore->next_temp();
				string end_label = "lb_" + pstore->next_temp();

				list<PreCodeGenerateIL*> ret;
				ret.push_front( new   PreCodeGenerateIL( "FORKSTART", c1_label, c2_label ) );

				//return new   PreCodeGenerateIL( "UNIFY_OR", s_unify->target, s_unify->sel1->target, s_unify->sel2->target );				
				auto code_block_1 = emiteILCode( pstore,s_unify->sel1 );
				
				code_block_1.push_front( new   PreCodeGenerateIL( "UNIFY", s_unify->target, s_unify->sel1->target ) );
				code_block_1.push_front( new   PreCodeGenerateIL( "LABEL", c1_label, "" ) );
				code_block_1.push_back( new   PreCodeGenerateIL( "GOTO", end_label ,"" ) );

				auto code_block_2 = emiteILCode( pstore, s_unify->sel2 );
				code_block_2.push_front( new   PreCodeGenerateIL( "UNIFY", s_unify->target, s_unify->sel2->target ) );
				code_block_2.push_front( new   PreCodeGenerateIL( "LABEL", c2_label, "" ) );
			 
				ret.insert( ret.end(), code_block_1.begin(), code_block_1.end() );
				ret.insert( ret.end(), code_block_2.begin(), code_block_2.end() );


				ret.push_back( new   PreCodeGenerateIL( "LABEL", end_label, "" ) );
				ret.push_back( new   PreCodeGenerateIL( "FORKEND",  "","" ) );
				return ret;
			}

			return {};
		}

		list<PreCodeGenerateIL*> emiteILCode( ParserStore *pstore, SelectorItem  *sel )
		{
			list<PreCodeGenerateIL*> ret;
			list<PreCodeGenerateIL*> il_s = emiteILCode_i( pstore, sel );
			ret.insert(ret.end(), il_s.begin(),il_s.end() );

			while (sel->next != nullptr)
			{
				sel = sel->next;
				il_s = emiteILCode_i( pstore, sel );
				ret.insert( ret.end(), il_s.begin(), il_s.end() );
			}
			return ret;

		}
		
		






	}
}