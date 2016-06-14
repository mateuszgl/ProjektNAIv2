// Program przykładowy przygotowany na zajęcia NAI na PJATK
// Tadeusz Puźniakowski, 2016

#include "SDL/SDL.h"
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <cmath>
#include <fstream>
#include <cassert>
#include "helpers.hpp"

using namespace std;




class World;
vector < vector < double > > getCollisionPoints(SDL_Surface *obstacles, vector < double > p, double maxR, double dA = M_PI/100.0);

/******************************************************************
 * W tym miejscu będziemy implementowali sztuczną inteligencję
 * **************************************************************** */
 class Agent {
	public:
	
	vector <double> p; // aktualna pozycja
	vector <double> v; // wektor predkosci
	
	
	vector <double> dv; // zmiana predkosci
	vector <double> currentTarget; // aktualny cel
	
	void (*ai)(Agent *, SDL_Surface *, vector < Agent > &, double );
	
	void updateIntentions(SDL_Surface *obstacles, vector < Agent > &agents, double dt) {
		if (ai != NULL) ai(this, obstacles, agents, dt);
		else {
			auto cp = getCollisionPoints(obstacles, p, 100, M_PI/18);
			vector < double > newDv(2);

			for (int i = 0; i < cp.size(); i++) {
				auto colisionVector = p - cp[i];
				double l = norm(colisionVector);
				colisionVector = colisionVector / l;
				newDv = newDv + colisionVector*2000/(l*l);
			}
			
			
			for (int i = 0; i < agents.size(); i++) {
				auto colisionVector = p - agents[i].p;
				double l = norm(colisionVector);
				colisionVector = colisionVector / l;
				newDv = newDv + colisionVector*10000/(l*l);
			}
			
			
			
			dv = currentTarget - p;
			if (norm(dv) > 0) dv = (dv/norm(dv))*100;
			dv = dv + newDv;
		}
	}
	
	Agent() {
		ai = NULL;
	};
};




class World {
public:
	SDL_Surface *obstacles;
	vector < Agent > agents;

	// aktualizacja intencji -- wykonanie AI dla kazdego agenta
	void ai(double dt, World &w) {
		#pragma omp parallel for
		for (int i = 0; i < agents.size(); i++) {
			vector < Agent > agents0;
			for (int j = 0; j < agents.size(); j++) {
				if (j != i) {
					int collision = 0;
					auto vec =  agents[j].p - agents[i].p;
					vec = vec / norm(vec);
					auto p = agents[i].p;
					
					for (;norm(agents[j].p-p) > 1; p = p + vec) {
						if (getpixel(obstacles, p[0],p[1]) < 128) {
							collision = 1;
							break;
						}
					}
					if (collision == 0) {
						agents0.push_back(agents[j]);
					}
				}
			}
			//agents0.erase (agents0.begin()+i);
			#pragma omp critical
			agents[i].updateIntentions(obstacles, agents0, dt);
		}
	}

	// obliczenia fizyki
	void move(double dt, World &w) {
		// korekta wartosci (nie mozna przekroczyc maksymalnego przyspieszenia)
		for (int i = 0; i < agents.size(); i++) {
			if (norm(agents[i].dv) > 100) 
				agents[i].dv = (agents[i].dv/norm(agents[i].dv))*100;
		}

		// sily i predkosci
		for (int i = 0; i < agents.size(); i++) {
			agents[i].v = agents[i].v + agents[i].dv*dt;
		}
		
		// opory
		for (int i = 0; i < agents.size(); i++) {
			agents[i].v = agents[i].v - agents[i].v*0.4*dt;
		}
		// aktualizacja pozycji
		for (int i = 0; i < agents.size(); i++) {
			auto newPos = agents[i].p + agents[i].v*dt;
			if (getpixel(obstacles, newPos[0], newPos[1])) {
				agents[i].p = newPos;
			} else {
				agents[i].v[0] = 0;
				agents[i].v[1] = 0;
			}
			
		}
	}
	
	vector <Agent> &getAgents() {
		return agents;
	}
};


// sprawdza punkty kolizji
vector < vector < double > > getCollisionPoints(SDL_Surface *obstacles, vector < double > p, double maxR, double dA) {
	vector < vector < double > > ret;
	// skanujemy dookola w poszukiwaniu przeszkod
	for (double a = -M_PI; a < M_PI; a+= dA) {
		auto v = angleConvert(a);
		for (double r = 1.5; r <= maxR; r+= 1) {
			auto pp = p + v*r;
			if (getpixel(obstacles,pp[0], pp[1]) < 128) {// przeszkoda
				ret.push_back (pp);
				r = maxR;
			}
		}
	}
	return ret;
}


const int _FINISHED = 1;
const int _GAME = 2;

class Game {
public:
    SDL_Surface *_window;
    World world;
    map<string,SDL_Surface*> tiles;
    unsigned long int lastTick;
    int _state;

    // laduje obrazek (kafelek)
    void load(string name, string filename) {
        SDL_Surface *image = SDL_LoadBMP( filename.c_str() );
        if( image == NULL ) {
            cout << "Blad ladowania pliku " << filename << "n";
            throw "Blad ladowania pliku";
        }
        auto search = tiles.find(name);
        if(search != tiles.end()) SDL_FreeSurface( tiles[name] );
        auto tmpImg = SDL_LoadBMP( filename.c_str() );
        tiles[name] = SDL_DisplayFormat( tmpImg );
        SDL_FreeSurface( tmpImg );
        auto colorkey = SDL_MapRGB( tiles[name]->format, 0, 0xFF, 0xFF );
        SDL_SetColorKey( tiles[name], SDL_SRCCOLORKEY, colorkey );
    }
    // inicjalizacja "gry"
    void init() {
        SDL_Init( SDL_INIT_EVERYTHING );
        _window = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
        SDL_WM_SetCaption( "NAI - PFN", NULL );
        load("player", "player.bmp" );
        //load("water", "water.bmp" );
        load("map", "map.bmp" );
        load("cross", "cross.bmp" );
        lastTick = SDL_GetTicks(); // w milisekundach
        _state = _GAME;
        world.obstacles = tiles["map"];

    }
    // umieszcza zadany obrazek na ekranie na zadanych wspolrzednych
    void blit(string elem, int x = 0, int y = 0, int w = 32, int h = 32) {
        SDL_Rect dst;
        dst.x = x; dst.y = y; dst.w = w; dst.h = h;
        SDL_BlitSurface( tiles[elem] , NULL, _window, &dst );
    }

    // wyswietlenie planszy gry (tlo, ludzik, ewentualnie jakies efekty
    void display() {
        // czyszczenie ekranu
        SDL_FillRect( _window, NULL, SDL_MapRGB( _window->format, 0x00, 0x00, 0x00 ) );
        // narysowanie mapy
        blit("map",0,0,tiles["map"]->w, tiles["map"]->h);

		blit("cross",world.agents[0].currentTarget[0]-16,world.agents[0].currentTarget[1]-16);
        
        for (int i = 0; i < world.agents.size(); i++) {
			blit("player",world.agents[i].p[0]-16,world.agents[i].p[1]-16);
		}
		
		auto cp = getCollisionPoints(world.obstacles, world.agents[0].p, 100, M_PI/18);
		for (int i = 0; i < cp.size(); i ++) {
			blit("cross",cp[i][0]-16,cp[i][1]-16);
		}
		

        //blit("player",player.x()-16,player.y()-16);
        SDL_Flip( _window );
    }
    // obsluga wejscia (klawiatura, mysz)
    void input() {
        SDL_Event event;
        while( SDL_PollEvent( &event ) ) {
            if( event.type == SDL_QUIT ) onQuit();
            if( event.type == SDL_KEYDOWN ) {
                switch( event.key.keysym.sym ) {
                case SDLK_ESCAPE:
                    onQuit();
                    break;
                case SDLK_q:
                    onQuit();
                    break;
                }
            }
            if( event.type == SDL_MOUSEBUTTONDOWN ) {
                if( event.button.button == SDL_BUTTON_LEFT ) {
                    onClick(event.button.x, event.button.y);
                }
            }
        }
    }

    // cala "fizyka" gry
    void calculations() {
        SDL_Delay( 1 ); // #pcmasterrace
        unsigned long int now = SDL_GetTicks();
        double dt= (double)(now - lastTick)/1000.0;
        lastTick = now;
        world.ai(dt,world);
        world.move(dt,world);
        //player.step(dt, world);
    }

    // funkcja wywolywana przed zamknieciem programu
    void shutdown() {
        for (auto it = tiles.cbegin(); it != tiles.end(); ++it)
            SDL_FreeSurface(it->second);
        tiles.clear();
        SDL_Quit();
    }

    int state() {
        return _state;
    }

    void onQuit() {
        _state = _FINISHED;
    }

    void onClick(int x, int y) {
		for (int i = 0; i < world.agents.size(); i++) {
			world.agents[i].currentTarget[0] = x;
			world.agents[i].currentTarget[1] = y;
		}
    }
};


int main( int argc, char *args[] )
{
    Game game;
    Agent agentO;
    
    agentO.p.push_back(200);
    agentO.p.push_back(100);
    agentO.v.push_back(0);
    agentO.v.push_back(0);
    agentO.currentTarget.push_back(200);
    agentO.currentTarget.push_back(200);
    
    game.init();
	game.world.agents.push_back(agentO);

	for (int i = 0; i < 0; i ++) {
		agentO.p[0] = 202+i;
		agentO.p[1] = 100;
		game.world.agents.push_back(agentO);
	}
	
    while (game.state() == _GAME) {
        game.input();
        game.display();
        game.calculations();
    }
    game.shutdown();
    return 0;
}
