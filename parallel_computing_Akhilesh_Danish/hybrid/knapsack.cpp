#include "bnb_hybrid.cpp"

class Knapsack_Problem;

class Knapsack_Solution: public Solution{
	public:
		int wt,cost;
		vector<int> taken; // object nos. chosen
		int pos; // number of objects "considered"
		int get_cost();
		bool is_feasible();
		void print_sol();
		int get_bound();
		int to_str(char *buf); // MPI message will be passed as char stream
};

class Knapsack_Problem: public Problem<Knapsack_Solution>{
	public:
		vector<int> costs;
		vector<int> weights;
		int n; // num of objects
		int W; // upper limit on total weight of objects selected
		bool get_goal();
		Knapsack_Solution empty_sol();
		Knapsack_Solution worst_sol();
		vector<Knapsack_Solution> expand(Knapsack_Solution s);
		int to_str(char *buf);
		static Knapsack_Solution decode_sol(char *buf, int &pos);
		static Knapsack_Problem decode_prob(char *buf, int &pos);
};

/* ----------------- method defs: ------------------------------ */

int Knapsack_Solution::get_cost(){
	return cost;
}

Knapsack_Solution Knapsack_Problem::decode_sol(char *buf, int &pos){
	int b, ntaken;
	Knapsack_Solution ks;
	sscanf(buf+pos,"%d%d%d%d%n",&(ks.wt),&(ks.cost),&(ks.pos),&ntaken,&b);
	pos += b;
	for(int i=0;i<ntaken;i++){
		int temp;
		sscanf(buf+pos,"%d%n",&temp,&b);
		pos += b;
		ks.taken.push_back(temp);
	}
	return ks;
}

int Knapsack_Solution::to_str(char *buf){
	int p = 0;
	p += sprintf(buf+p,"%d %d %d %d ",wt,cost,pos,(int)taken.size());
	for(int i=0;i<taken.size();i++)
		p += sprintf(buf+p, "%d ",taken[i]);
	return p;
}

struct item{
	int wt, cost;
	double ratio;
};

bool comp(item a, item b){
	return a.ratio > b.ratio;
}

int Knapsack_Solution::get_bound(){
	int bound = cost;
	vector<item> rem; // remaining items
	for(int i=pos;i< ((Knapsack_Problem *)prob_ptr) ->n;i++){
		item it;
		it.wt = ((Knapsack_Problem *)prob_ptr) -> weights[i];
		it.cost = ((Knapsack_Problem *)prob_ptr) -> costs[i];
		it.ratio = ( (double) it.cost) / ( (double) it.wt);
		rem.push_back(it);
	}
	int w = wt;
	sort(rem.begin(),rem.end(),comp); // decreasing order of ratio of cost to weight;
	for(int j=0;j<rem.size() && w <= ((Knapsack_Problem *)prob_ptr)->W;j++){
		//taking items till weight is exceeded
		bound += rem[j].cost;
		w += rem[j].wt;
	}
	return bound;
}

bool Knapsack_Solution::is_feasible(){
	return (pos == (((Knapsack_Problem *)prob_ptr)->n) );
}

void Knapsack_Solution::print_sol(){
	cout<<"Maximum val of "<<cost<<" achieved by taking objects ";
	for(int i=0;i<taken.size();i++)
		cout<<taken[i]<<", ";
	cout<<endl;
}

bool Knapsack_Problem::get_goal(){
	return true; // it is a maximization problem
}

Knapsack_Solution Knapsack_Problem::empty_sol(){
	Knapsack_Solution s1;
	s1.pos = 0;
	s1.prob_ptr = this;
	s1.wt = s1.cost = 0;
	return s1;
}

Knapsack_Solution Knapsack_Problem::worst_sol(){
	Knapsack_Solution s1;
	s1.prob_ptr = this;
	s1.cost = -1000000;
	s1.wt = 0;
	return s1;
}

int Knapsack_Problem::to_str(char* buf){
	int p = 0;
	p += sprintf(buf+p,"%d %d ",n,W);
	for(int i=0;i<n;i++)
		p += sprintf(buf+p,"%d ",weights[i]);
	for(int i=0;i<n;i++)
		p += sprintf(buf+p,"%d ",costs[i]);
	return p;
}

Knapsack_Problem Knapsack_Problem::decode_prob(char *buf, int &pos){
	int b;
	Knapsack_Problem kp;
	sscanf(buf+pos,"%d%d%n",&(kp.n),&(kp.W),&b);
	pos += b;
	for(int i=0;i<kp.n;i++){
		int temp;
		sscanf(buf+pos,"%d%n",&temp,&b);
		pos += b;
		kp.weights.push_back(temp);
	}
	for(int i=0;i<kp.n;i++){
		int temp;
		sscanf(buf+pos,"%d%n",&temp,&b);
		pos += b;
		kp.costs.push_back(temp);
	}
	return kp;
}

vector<Knapsack_Solution> Knapsack_Problem::expand(Knapsack_Solution s){
	//either take object at pos, or don't
	Knapsack_Solution s1 = s;
	s1.pos++;
	vector<Knapsack_Solution> ret;
	ret.push_back(s1); // without taking object at index pos
	if(weights[s1.pos-1] + s.wt <=  W){
		s1.taken.push_back(s1.pos-1);
		s1.wt +=  weights[s1.pos-1];
		s1.cost += costs[s1.pos-1];
		ret.push_back(s1); // taking object at index pos
	}
	return ret;
}

int main(){
	BnB_solver<Knapsack_Problem, Knapsack_Solution> bnbs;
	Knapsack_Problem p;
	cin>>p.n;
	cin>>p.W;
	for(int i=0;i<p.n;i++){
		int temp;
		cin>>temp;
		p.weights.push_back(temp);
	}
	for(int i=0;i<p.n;i++){
		int temp;
		cin>>temp;
		p.costs.push_back(temp);
	}
	bnbs.solve(p);
	return 0;
}
