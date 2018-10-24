#ifndef _rec_H_
#define _rec_H_

#include <iostream>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <future>
typedef std::vector<int > path;
struct varos{
	int x,y;
};
#define W_HEIGHT 1024
#define W_WIDTH 1024
#define SQRLEN 4
#ifndef SCALE
#define SCALE 3
#endif

class pathVisualizer
{
public:

	pathVisualizer(const std::vector<varos> & cities):cities(cities){
		initG();	
	}	
	virtual ~pathVisualizer();
	void setPath(const path &p);
	void drawActivatedEdge(int,int);
	void visualize();
	void clear(){
    		SDL_RenderClear( renderer );
	}
protected:
		
	void initG();
	void setRcolor(int ,int,int,int);
    	SDL_Window* window;
    	SDL_Renderer* renderer;
	path m_path;
	std::mutex drawMx;
	std::vector<varos>cities;
};
void pathVisualizer::setPath(const path&p){

	std::lock_guard<std::mutex> lockGuard(drawMx);
	m_path = p;
}
void pathVisualizer::setRcolor(int r,int g,int b,int a){

    SDL_SetRenderDrawColor( renderer, r, g, b, a);
}
void pathVisualizer::initG(){

   		 window = SDL_CreateWindow
   		 (
   		     "2 OPT", SDL_WINDOWPOS_UNDEFINED,
   		     SDL_WINDOWPOS_UNDEFINED,
   		     W_HEIGHT,
   		     W_WIDTH,
   		     SDL_WINDOW_SHOWN
   		 );
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

}
pathVisualizer::~pathVisualizer(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}
void pathVisualizer::drawActivatedEdge(int i, int k){
	
	SDL_SetRenderDrawColor( renderer, 255, 0,0 , 255 );

	SDL_RenderDrawLine(renderer,cities[i].x*SCALE+SQRLEN /2,cities[i].y*SCALE+ SQRLEN /2,cities[k].x*SCALE+SQRLEN /2,cities[k].y*SCALE+ SQRLEN /2);

    	SDL_RenderPresent(renderer);
}
void pathVisualizer::visualize(){
	std::lock_guard<std::mutex> lockGuard(drawMx);
	setRcolor(0,0,0,255);	
    	SDL_RenderClear( renderer );
	SDL_Rect r;
	for (int i = 0; i <m_path.size()-1; i++) {
   		 r.x = cities[m_path[i]].x*SCALE;
   		 r.y = cities[m_path[i]].y*SCALE;
   		 r.w = SQRLEN;
   		 r.h = SQRLEN;

   		 SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
		
   		 SDL_RenderFillRect( renderer, &r );
   		 SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );

		 SDL_RenderDrawLine(renderer,r.x+SQRLEN /2,r.y+ SQRLEN /2,cities[m_path[i+1]].x*SCALE+SQRLEN /2,cities[m_path[i+1]].y*SCALE+ SQRLEN /2);
	}
    SDL_RenderPresent(renderer);
}
#endif
