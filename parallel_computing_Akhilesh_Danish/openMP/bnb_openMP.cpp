#include "bnb.h"
#ifdef _OPENMP
    #include <omp.h>
#else
    #define omp_get_thread_num() 0
#endif
template<typename T1,typename T2>
void BnB_solver<T1,T2>::solve(T1 p){
   queue<T2> q;
   q.push(p.empty_sol());
   T2 best = p.worst_sol();
  while(true)
  {
    int nt;
    if(q.size()>=4) nt = 4;
    else nt = q.size();
    T2 data[nt];
    for(int i=0;i<nt;i++)
    {
       data[i] = q.front();
       q.pop();
    }
    if(nt==0) break;
    #pragma omp parallel for 
    for(int i=0;i<nt;i++)
    {
      T2 s;
      s = data[i];
     // cout<<"thread is : "<<omp_get_thread_num()<<endl;
     //pragma omp critical
      if(s.is_feasible()){
  			//check if better than the best
  			if(p.get_goal()){ // maximization problem
          #pragma omp critical
  				if(s.get_cost() > best.get_cost()){ 
               best = s;
  				}
  			}
  			else{ // minimization problem
          #pragma omp critical
  				if(s.get_cost() < best.get_cost()){
               best = s;
  				}
  			}
  		}
  		else{

       // cout<<"hello"<<endl;
       // not yet feasible, to be expanded!
        if(p.get_goal()){ // maximization problem
  				if(s.get_bound() > best.get_cost() ) { 
              vector<T2> ret = p.expand(s);
              #pragma omp critical
              {
                for(int l=0;l<ret.size();l++)
                {
                  q.push(ret[l]);  
                }
              }
  				}
  			}
  			else{
  				if(s.get_bound() < best.get_cost()) {
              vector<T2> ret = p.expand(s);
              #pragma omp critical
              {
                for(int l=0;l<ret.size();l++)
                {
                  q.push(ret[l]);  
                }
              }
  				}
  			}
      }
  	}
  }
	best.print_sol();
}


