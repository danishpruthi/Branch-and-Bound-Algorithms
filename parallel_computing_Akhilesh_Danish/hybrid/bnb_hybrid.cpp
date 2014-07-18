#include "bnb.h"
#include <mpi.h>

char buffer[2000];

#define NUM_THREADS 4

#ifdef _OPENMP
	#include <omp.h>
#else
    #define omp_get_thread_num() 0
#endif

template <typename T1, typename T2>
void BnB_solver<T1,T2>::solve(T1 p){
	MPI_Init(0,NULL);
	int pid, num;
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	MPI_Status st;
	T1 prb;
	queue<T2> q;
	int best_cost;
	if(pid == 0){
		//master processor
		T2 start = p.empty_sol(), best = p.worst_sol();
		bool is_idle[num+1];
		int num_idle;
		for(int i=1;i<num;i++){
			is_idle[i] = true; // mark all as idle
		}
		num_idle = num - 1;
		//encode initial problem and empty sol into buffer
		int pos  =0;
		pos += sprintf(buffer+pos,"%s ","PROB ");
		pos += p.to_str(buffer+pos);
		pos += start.to_str(buffer+pos);
		pos += sprintf(buffer+pos, "%d \0",best.get_cost());
		//send it to idle processor no. 1
		MPI_Send(buffer,strlen(buffer),MPI_CHAR,1,0,MPI_COMM_WORLD);
		is_idle[1] = false;
		num_idle--;
		while(num_idle < num - 1){
			MPI_Recv(buffer,2000,MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&st);
			char msg[10];
			int pos = 0, b;
			sscanf(buffer+pos,"%s%n",msg,&b);
			pos += b;
			if(!strcmp(msg,"GET_SLAVES")){
				int sl_needed;
				int r = st.MPI_SOURCE;
				sscanf(buffer+pos,"%d%n",&sl_needed,&b);
				pos += b;
				int sl_given = min(sl_needed, num_idle);
				pos = 0;
				pos += sprintf(buffer+pos,"%d ",sl_given);
				pos += sprintf(buffer+pos,"%d ",best.get_cost());
				int i = 1;
				while(sl_given){
					if(!is_idle[i])
						i++;
					else{
						pos += sprintf(buffer+pos,"%d ",i);
						is_idle[i] = false;
						num_idle--;
						sl_given--;
					}
				}
				pos += sprintf(buffer+pos,"\0");
				MPI_Send(buffer,strlen(buffer)+1,MPI_CHAR,r,0,MPI_COMM_WORLD);
			}
			else if(!strcmp(msg,"IDLE")){
				//slave has become idle
				num_idle++;
				is_idle[st.MPI_SOURCE] = true;
			}
			else if(!strcmp(msg,"DONE")){
				//slave has sent a feasible solution
				T2 candidate_sol = T1::decode_sol(buffer,pos);
				if( ( p.get_goal() && candidate_sol.get_cost() > best.get_cost()) || 
						(!p.get_goal() && candidate_sol.get_cost() < best.get_cost()) ){
					best = candidate_sol;
					best_cost = candidate_sol.get_cost();
				}
			}

		}
		for(int i=1;i<num;i++){
			strcpy(buffer,"FINISH \0");
			MPI_Send(buffer,strlen(buffer)+1,MPI_CHAR,i,0,MPI_COMM_WORLD);
		}
		best.print_sol();
	}
	else{
		//slave processor
		while(1){
			MPI_Recv(buffer,2000,MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&st);
			int pos = 0; //position of pointer in buffer
			char msg[10]; // message telling the slave what to do
			int b;
			sscanf(buffer+pos,"%s%n",msg,&b);
			pos += b;
			char req[2000];
			if(!strcmp(msg,"PROB")){
				//slave has been given a partially solved problem to expand
				prb = T1::decode_prob(buffer,pos);
				T2 s = T1::decode_sol(buffer,pos);
				s.prob_ptr = &prb;
				q.push(s);
				fflush(stdout);
				sscanf(buffer+pos,"%d%n",&best_cost,&b);
				pos += b;
				while(!q.empty()){
					//take out one element from queue, expand it
					int nt = min(NUM_THREADS, (int)q.size());
					T2 data[nt];
					for(int j=0;j<nt;j++){
						data[j] = q.front();
						q.pop();
					}
					#pragma omp parallel for
					for(int j=0;j<nt;j++){
						T2 sol = data[j];
						//ignore if its bound is worse than already known best sol.
						if( (prb.get_goal() && (sol.get_bound()<best_cost) )
								|| (!prb.get_goal() && (sol.get_bound()>best_cost) )  ){
							continue;
						}
						bool fl = false;

						#pragma omp critical
						if(sol.is_feasible()){
							fflush(stdout);
							pos = 0;
							pos += sprintf(buffer+pos, "DONE ");
							pos += sol.to_str(buffer+pos);
							pos += sprintf(buffer+pos," \0");
							fflush(stdout);
							//tell master that feasible solution is reached
							MPI_Send(buffer,strlen(buffer)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
							fl = true;
						}

						if(fl)
							continue;

						vector<T2> v = prb.expand(sol);

						#pragma omp critical
						for(int i=0;i<v.size();i++)
							q.push(v[i]);
					}
					if(q.empty())
						break;

					//request master for slaves
					sprintf(req,"GET_SLAVES %d ",(int)q.size());
					MPI_Send(req,strlen(req),MPI_CHAR,0,0,MPI_COMM_WORLD);
					//get master's response
					//master sends num_avbl_slaves <space> best_cost <space> list of slaves
					req[0] = '\0';
					MPI_Recv(req,200,MPI_CHAR,0,0,MPI_COMM_WORLD,&st);
					int slaves_avbl, p1 = 0;
					sscanf(req+p1,"%d%n",&slaves_avbl,&b);
					p1 += b;
					sscanf(req+p1,"%d%n",&best_cost,&b);
					p1 += b;
					for(int i=0;i<slaves_avbl;i++){
						//give a problem to each slave
						int sl_no;
						sscanf(req+p1,"%d%n",&sl_no,&b);
						p1 += b;
						pos = 0;
						pos += sprintf(buffer+pos,"%s ","PROB ");
						pos += prb.to_str(buffer+pos);
						T2 sln = q.front();
						q.pop(); 
						pos += sln.to_str(buffer+pos);
						pos += sprintf(buffer+pos, "%d \0",best_cost);
						//send it to idle processor
						MPI_Send(buffer,strlen(buffer),MPI_CHAR,sl_no,0,MPI_COMM_WORLD);
					}
				}
				//This slave has now become idle (its queue is empty). Inform master.
				sprintf(req,"IDLE \0");
				MPI_Send(req,strlen(req)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
			}
			else if(!strcmp(msg,"FINISH")){
				assert(st.MPI_SOURCE==0); // only master can tell it to finish
				break; //from the while(1) loop
			}
		}
	}
	MPI_Finalize();
}
