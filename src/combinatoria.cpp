#include  "cinform.hpp"
#include  <vector>
#include  <functional>

using namespace std;
namespace CInform
{


	std::string ReplaceAll( std::string str, const std::string& from, const std::string& to ) {
		size_t start_pos = 0;
		while ((start_pos = str.find( from, start_pos )) != std::string::npos) {
			str.replace( start_pos, from.length(), to );
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
		return str;
	}

	bool enclosed_by_parenesis( std::list<string>  &x )
	{
		if (x.size() < 2) return false;
		if ((x.front() == "(") && (x.back() == ")"))
		{
			auto i_begin = std::next( x.begin() );
			auto i_end = std::prev( x.end() );
			int p = 0;
			for (auto it = i_begin; it != i_end;++it)
			{
				if (*it == "(") p++;
				if (*it == ")") p--;
				if (p < 0) return false;
			}
			return p == 0;
		}
		return false;
	}

	std::list<string>   split(string x)
	{
		x = ReplaceAll( x, "(", " ( " );
		x = ReplaceAll( x, ")", " ) " );

		std::list<string> xs = split(x, ' ');

		//remove first and last parentesis
		if (enclosed_by_parenesis( xs ))
		{
			xs.pop_back();
			xs.pop_front();
		}
		return xs;
	}

	bool is_puct(char x)
	{
		if (x == '.') return true;
		if (x == ':') return true;
		if (x == ',') return true;
		if (x == ';') return true;
		return false;
	}

	std::list<string>   split(string x, char sep)
	{
		std::list<string>  s;
		size_t n = x.size();
		string a = "";
		for (size_t i = 0; i < n; ++i)
		{
			if (x[i] == sep)
			{
				if (a.size() > 0) { s.push_back(a); a = ""; }
			}
			else if (is_puct(x[i]))
			{
				if (a.size() > 0) s.push_back(a);  
				s.push_back(x.substr(i,1));
				a = ""; 
			}			 
			else
			{
				a = a + x[i];
			}
		}
		if (a.size() > 0) { s.push_back(a); a = ""; }
		return s;
	}



	void  getCombinatoriaN(SSplitedPhase & acc, std::list<string>& remainder, size_t n, std::function<void(SSplitedPhase&)> &F)
	{
		if (remainder.size() < n) return;
		if (n == 1)
		{
			acc.items.emplace_back(remainder);
			F(acc);
			acc.items.pop_back();
			return;
		}
		for (auto it = remainder.begin(); it != remainder.end(); ++it)
		{
			if (it == remainder.begin()) continue;

			auto tail = std::list<string>(it, remainder.end());
			if (tail.size() < n - 1) continue;
			// auto head =  std::list<string>( xs.begin() , it) ;     
		   // acc.emplace_back(head);
			acc.items.emplace_back(remainder.begin(), it);
			getCombinatoriaN(acc, tail, n - 1, F);
			acc.items.pop_back();
		}
	}



	void getCombinatoria(string x, int n, function<  void(SSplitedPhase&)> func)
	{
		std::list<string>  xs = split(x);
		SSplitedPhase acc;
		//std::list< SSplitedPhase >  ret;
		getCombinatoriaN(acc, xs, n, func);
		//return ret;
	}



}