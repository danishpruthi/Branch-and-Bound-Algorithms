#include <cstdio>
#include <queue>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <algorithm>
using namespace std;

class Solution{
	public:
		void *prob_ptr;
		//methods to be implemented in subclass for particular prob.
		virtual int get_cost()  =0;
		virtual bool is_feasible() =0 ;
		virtual void print_sol() =0;
		virtual int get_bound() =0;
};

template <typename T1> class Problem{
	public:
		//methods to be implemented in subclass for particular prob.
		virtual bool get_goal() =0;
		virtual T1 empty_sol() =0;
		virtual T1 worst_sol() =0;
		virtual vector<T1> expand(T1 s) =0;
};

template <typename T1, typename T2> class BnB_solver{
	//T1 is problem, T2 is solution subclass
	public:
		void solve(T1 p);
};
