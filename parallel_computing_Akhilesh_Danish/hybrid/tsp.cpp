#include "bnb_hybrid.cpp"
class TSP_Problem;

class TSP_Solution: public Solution{
	public:
		int cost;
		vector<int> taken; // list of vertices traversed
		int pos; // length of the current path
		int get_cost();
		bool is_feasible();
		void print_sol();
		int get_bound();
		int to_str(char *buf);
};

class TSP_Problem: public Problem<TSP_Solution>{
	public:
    	vector<vector<int> > graph;
		int n; // num of vertices in the graph
    	int min_edge;
		bool get_goal();
		TSP_Solution empty_sol();
		TSP_Solution worst_sol();
		vector<TSP_Solution> expand(TSP_Solution s);
		int to_str(char *buf);
		static TSP_Problem decode_prob(char *buf, int &pos);
		static TSP_Solution decode_sol(char *buf, int &pos);
};

/* ----------------- method defs: ------------------------------ */

int TSP_Solution::to_str(char *buf){
	int p = 0;
	p += sprintf(buf+p," %d %d ",pos,cost);
	for(int i=0;i<pos;i++)
		p += sprintf(buf+p," %d ",taken[i]);
	return p;
}

TSP_Solution TSP_Problem::decode_sol(char *buf, int &pos){
	TSP_Solution s;
	int b;
	sscanf(buf+pos,"%d%d%n",&(s.pos),&(s.cost),&b);
	pos += b;
	for(int i=0;i<s.pos;i++){
		int temp;
		sscanf(buf+pos,"%d%n",&temp,&b);
		pos += b;
		s.taken.push_back(temp);
	}
	return s;
}

int TSP_Problem::to_str(char *buf){
	int p = 0;
	p += sprintf(buf+p," %d %d ",n,min_edge);
	for(int i=0;i<n;i++)
		for(int j=0;j<n;j++)
			p += sprintf(buf+p," %d ",graph[i][j]);
	return p;
}

TSP_Problem TSP_Problem::decode_prob(char *buf, int &pos){
	TSP_Problem prob;
	int b;
	sscanf(buf+pos,"%d%d%n",&(prob.n),&(prob.min_edge),&b);
	pos += b;
	for(int i=0;i<prob.n;i++){
		vector<int> v;
		for(int j=0;j<prob.n;j++){
			int temp;
			sscanf(buf+pos,"%d%n",&temp,&b);
			pos += b;
			v.push_back(temp);
		}
		prob.graph.push_back(v);
	}
	return prob;
}

int TSP_Solution::get_cost(){
	return cost;
}

int TSP_Solution::get_bound(){
  // need to give a lower bound!
	int left = (((TSP_Problem *)prob_ptr)->n) - taken.size();
  return cost+(left*(((TSP_Problem *)prob_ptr)->min_edge));
  /*t bound = cost;
	for(int i=pos;i<((TSP_Problem *)prob_ptr)->n;i++){
		bound += ((TSP_Problem *)prob_ptr) -> costs[i];
	} // loose upper bound assuming all remaining items are taken
	return bound;*/
}


bool TSP_Solution::is_feasible(){
   //cout<<"checking feasibility"<<endl;
   return (pos == (((TSP_Problem *) prob_ptr) -> n));
   /*
   if(pos == (((TSP_Problem *)prob_ptr)->n))
   {
      bool flag[((TSP_Problem *)prob_ptr)->n];
      for(int i=0;i<((TSP_Problem *)prob_ptr)->n;i++)
        flag[i] = false;
      for(int i=0;i<taken.size();i++)
      {
        if(!flag[taken[i]])
          flag[taken[i]] = true;
        else return false;
      }
      return true;
   }
   return false;*/
}

void TSP_Solution::print_sol(){
  cout<<"Minimum cost of "<<cost<<" achieved for the path with vertices ";
	for(int i=0;i<taken.size();i++)
		cout<<taken[i]<<", ";
	cout<<endl;
}

bool TSP_Problem::get_goal(){
	return false; // it is a minimization problem
}

TSP_Solution TSP_Problem::empty_sol(){
	TSP_Solution s1;
  // include the starting vertex 
	s1.pos = 0;
	s1.prob_ptr = this;
	s1.cost = 0;
	return s1;
}

TSP_Solution TSP_Problem::worst_sol(){
	TSP_Solution s1;
	s1.prob_ptr = this;
	s1.cost = 100000000;
  s1.pos = 0;
	return s1;
}

vector<TSP_Solution> TSP_Problem::expand(TSP_Solution s){
  bool flag[((TSP_Problem*)s.prob_ptr)->n];
  vector<TSP_Solution> ret;
  memset(flag,false,sizeof(flag));
  for(int i=0;i<s.taken.size();i++)
  {   
    flag[s.taken[i]] = true;
  }
  if(s.taken.size()==0)
  {
     for(int i=0;i<((TSP_Problem *)s.prob_ptr)->n;i++)
     {
       TSP_Solution first = s;
       first.pos = 1;
       first.taken.push_back(i);
       ret.push_back(first);
     }
     return ret;
  }
  for(int i=0;i<((TSP_Problem *)s.prob_ptr)->n;i++)
  {
    if(!flag[i])
    {
	    TSP_Solution s1 = s;
      s1.pos++;
      s1.taken.push_back(i);
      int prev;
      prev = s1.taken[s1.taken.size()-2];
      //cout<<"Position of the new q element"<<s1.pos<<endl;
      s1.cost += graph[prev][i];
      ret.push_back(s1);
      //return;
    }
      
  }
  return ret;
}

int main(){
	TSP_Problem p;
	BnB_solver<TSP_Problem, TSP_Solution> bnbs;
	cin>>p.n;
  	p.min_edge = 1000000;
	for(int i=0;i<p.n;i++){
    vector<int> temp;
    for(int j=0;j<p.n;j++)
    {
      int val;  
      cin>>val;
      p.min_edge = min(p.min_edge,val);
      temp.push_back(val);
    }
    p.graph.push_back(temp);
	}

	bnbs.solve(p);
	return 0;
}
