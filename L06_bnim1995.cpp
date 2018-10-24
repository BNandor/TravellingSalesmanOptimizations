//Bandi Nandor
//bnim1995
//L06
#include <algorithm>
#include <random>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <vector>
#include <math.h>
#include <limits.h>
#include "qPARALLEL.h"
#include <queue>
#include "rect.h"
#ifndef improvement_num
#define improvement_num 2
#endif


//#ifndef FILENAME
//#define FILENAME "tsp2.txt"
//#endif



typedef std::vector<varos> varoslanc;
typedef std::vector<int> utvonal;

bool getVaros(std::fstream& input,varos& uj){
	int dummy;
	if (input>>dummy>>uj.x>>uj.y){
		std::cout << "read" << std::endl;
		return true;
	}else{
		return false;
	}
}

std::vector<varos> szerezVarosok(){
	
	std::fstream input;
	input.open(FILENAME);

	std::vector<varos> varosok;

	varos uj;
	while(getVaros(input,uj)){
		varosok.push_back(uj);
	}

return varosok;
}

void kiir(const std::vector<int> & a){
	for (int i = 0; i < a.size(); i++) {
		std::cout << a[i]+1<<" " ;
	}
}

// int dist(int i, int j,const std::vector<varos>& varosok){
// 	return ((varosok[i].x-varosok[j].x)*(varosok[i].x-varosok[j].x)+(varosok[i].y-varosok[j].y)*(varosok[i].y-varosok[j].y));
// }

inline int dist(const int& i, const int& j, const std::vector<varos>& varosok) {
	return (pow(varosok[i].x - varosok[j].x, 2) + pow(varosok[i].y - varosok[j].y, 2));
}


double path_distance(const std::vector<varos> &varosok,const std::vector<int>& path){
	double sum=0;
	for (int i = 0; i < path.size() -1; i++) {
		sum+=sqrt(dist(path[i],path[i+1],varosok));
	}

	return sum;	
}

int legkozelebbi(int start,const varoslanc& varosok,int citynum,const std::vector<bool> &visited){
	double MIN = static_cast<double>(INT_MAX);
	int index;
	int d;

	for (int i = 0; i < start; i++) {
		if(visited[i])continue;
		d=dist(start,i,varosok);
		if(d<MIN){
			MIN=d;
			index=i;
		}
	}

	for (int i = start+1; i < citynum;i++) {
		if(visited[i])continue;
		d=dist(start,i,varosok);
		if(d<MIN){
			MIN=d;
			index=i;
		}
	}
	return index;
}

utvonal get_path(int start,int citynum,const varoslanc& varosok){
	std::vector<bool> visited(citynum,false);
	visited[start]=true;
	utvonal ut;
	ut.push_back(start);
	int jelenlegi=start;
	int next;
	for (int i = 0; i < citynum-1; i++) {
		next = legkozelebbi(jelenlegi,varosok,citynum, visited);
		jelenlegi=next;	
		ut.push_back(next);
		visited[next]=true;
		
	}
	ut.push_back(ut[0]);
return ut;
}

struct job{
	pathVisualizer* viz;
	int start;
	int citynum;
	const varoslanc* varosok_p;
	concurrent::Output<utvonal >* output;
};

struct get_path_functor{
	void operator()(job& tmp)const{
	
	std::vector<bool> visited(tmp.citynum,false);
	visited[tmp.start]=true;
	utvonal ut;
	ut.push_back(tmp.start);
	int jelenlegi=tmp.start;
	int next;
	for (int i = 0; i < tmp.citynum-1; i++) {
		next = legkozelebbi(jelenlegi,*(tmp.varosok_p),tmp.citynum, visited);
		jelenlegi=next;	
		ut.push_back(next);
		visited[next]=true;
		
	}ut.push_back(tmp.start);
	std::cout << "done"<<tmp.start << std::endl;
	#ifdef GR
		tmp.viz->setPath(ut);
		tmp.viz->visualize();
	#endif
		tmp.output->push(ut);	
	};
};
utvonal get_minim_path(int citynum,const varoslanc& varosok,pathVisualizer &viz){
	double min_dist=static_cast<double>(INT_MAX);
	double c_dist;
	utvonal min_path;
	utvonal c_path;
	std::queue<job> jobs;
	job jobdata;
	concurrent::Output<utvonal> out;
	for (int i = 0; i < citynum; i++) {
		jobdata.viz = &viz;
		jobdata.start=i;
		jobdata.citynum=citynum;
		jobdata.varosok_p = &varosok;
		jobdata.output = &out;
		jobs.push(jobdata);
	}
	get_path_functor func;
	concurrent::PARALLEL<get_path_functor,job> magic(func,jobs,4);
	magic.pstart();
	int p;
	utvonal tmp;
	while(out.size() > 0){
		tmp = out.pop();
		//kiir(tmp);
		c_dist = path_distance(varosok,tmp);
		
		if(c_dist<min_dist){
			std::cout<<"new shortest"<<std::endl;
			#ifdef GR
			viz.setPath(tmp);
			viz.visualize();
			#endif
			min_dist = c_dist;
			min_path = tmp;
		}
	
	}
	return min_path;
}

bool optimal(int i, int k,const varoslanc& varosok){
	return  dist(i,i-1,varosok)+dist(k,k+1,varosok) > (dist(k,i-1,varosok)+dist(i,k+1,varosok));
}
utvonal swapper(const utvonal& ut,int i,int k,int citynum){
	utvonal uj;
	for (int j = 0; j < i; j++) {
		uj.push_back(ut[j]);
	}
	for (int j =k; j>=i; j--) {
		uj.push_back(ut[j]);
	}
	for (int j = k+1; j < ut.size(); j++) {
		uj.push_back(ut[j]);
	}
return uj;
}
utvonal get_linear_path(int citynum){
	utvonal ut;
	for (int i = 0; i < citynum; i++) {
		ut.push_back(i);
	}
	ut.push_back(0);
	return ut;
}

utvonal opt2(int citynum,const varoslanc& varosok,pathVisualizer& viz ){
	
//utvonal ut = get_linear_path(citynum);
	utvonal ut = get_minim_path(citynum,varosok,viz);
	std::cout << "end of greedy" << std::endl;
	kiir(ut);
	double min_dist = path_distance(varosok,ut);
	std::cout<<std::endl<<"dist="<<min_dist;
	utvonal tmp;
	double tdist;

	int improved =0;
	int p;
	bool again=true;
	while(improved<improvement_num){
		again=true;	
		for (int i =1; i < citynum-1&&again;i++) {
			for (int k = i+1; k <citynum && again; k++) {

				#ifdef GR_ACT
				viz.drawActivatedEdge(ut[i],ut[i-1]);				
				viz.drawActivatedEdge(ut[k],ut[k+1]);				
				
				viz.drawActivatedEdge(ut[k],ut[i-1]);				
				viz.drawActivatedEdge(ut[i],ut[k+1]);				
				#endif
				#ifdef OPTCHECK
				if( dist(ut[i],ut[i-1],varosok)+dist(ut[k],ut[k+1],varosok) > (dist(ut[k],ut[i-1],varosok)+dist(ut[i],ut[k+1],varosok))){	
				#endif
					tmp=swapper(ut,i,k,citynum);	
					tdist = path_distance(varosok,tmp);
					
					if( tdist < min_dist ){
						//std::cout<<dist(ut[i],ut[i]-1,varosok)+dist(ut[k],ut[k]+1,varosok) <<">"<< (dist(ut[k],ut[i]-1,varosok)+dist(ut[i],ut[k]+1,varosok))<<std::endl;
						std::cout<<tdist<<std::endl;
						
						improved=0;
						min_dist = tdist;
						ut = tmp;
					#ifdef fallback
						again=false;
					#endif

					#ifdef GR
					viz.setPath(tmp);
					viz.visualize();
					#endif

					}
				#ifdef OPTCHECK
				}
				#endif
				#ifdef GR
					viz.setPath(ut);
					viz.clear();
					viz.visualize();
					
				#endif
			}

		}
		++improved;
	}
return ut;
}

void print_coords(utvonal ut,const varoslanc& varosok){
	for (int i = 0; i < ut.size(); i++) {
		std::cout << varosok[ut[i]].x<<" "<<varosok[ut[i]].y << std::endl;
	}
}
int main(int argc, const char *argv[])
{
		
	varoslanc varosok = szerezVarosok();
	pathVisualizer	viz(varosok);

	int vszam = varosok.size();
	utvonal min =opt2(vszam,varosok,viz);
	kiir(min);
	std::cout << " dist:"<<path_distance(varosok,min) << std::endl;
	//std::cin>>vszam;
	return 0;
}
