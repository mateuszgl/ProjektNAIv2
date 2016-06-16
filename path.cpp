#include "SDL/SDL.h"
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <cmath>
#include <fstream>
#include "helpers.hpp"
using namespace std;

typedef class World {
public:
    vector<int> area; // obszar mapy
    int w=20, h=15;
    SDL_Surface *obstacles;
    
    World(){
		area.resize(w*h);
	}
    
    void createMap(){
        cout << "Mapa rozmiaru " << w << " na " << h << endl;
       
       
       //tworzenie mapy z obrazka
		for (int i = 0; i < h;)
        for (int j = 0; j < w; j++)
        { 
			if (obstacles == NULL) cout<<"Błąd tworzenia mapy!"<<endl;
			if (getpixel(obstacles,j,i) < 128){
			//przeszkoda na mapie zaznaczona kolerem czarnym
			area[i*w+j]=1;
		}
			else 
			area[i*w+j]=0;
			
			if (j%w==w-1){
				cout<<"nowa linia"<<endl;
				i++;
			}
		}	
    }

    int get(int x, int y) const {
        return area[y*w+x];
    }

    int canWalk(int x, int y) const {
        if (x < 0) return 0;
        if (y < 0) return 0;
        if (x >= w) return 0;
        if (y >= h) return 0;
       //if (getpixel(obstacles,x,y) < 128) return 0;
        return (area[y*w+x] == 0);
    }
} World;
typedef class PathElement {
public:
    int x, y;
    PathElement(int xx = 0, int yy = 0) {
        x = xx;
        y = yy;
    }
    bool operator() (const PathElement& lhs, const PathElement& rhs) const {
        return (lhs.y*1000+lhs.x)<(rhs.y*1000+rhs.x);
    }
} PathElement;
inline bool operator==(const PathElement& lhs, const PathElement& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}
inline bool operator!=(const PathElement& lhs, const PathElement& rhs) {
    return !(lhs == rhs);
}

typedef class Player {
    double _x, _y;

    vector<PathElement> path;
    int pathIndex = 0;

public:

    double h(PathElement p, PathElement dest) {
        return sqrt(
                   (dest.y-p.y)*(dest.y-p.y) +
                   (dest.x-p.x)*(dest.x-p.x));
    }

    // TU IMPLEMENTACJA WYZNACZANIA TRASY!!!
    // p - punkt startu,
    // t - punkt docelowy,
    // visited - lista odwiedzonych
    // path - wygenerowana trasa
    // world - swiat (mapa)
    int searchPath (PathElement start, PathElement goal, vector <int> &visited,
                    vector<PathElement> &path, World &world) {

        set<PathElement, PathElement> closedSet;
        set<PathElement, PathElement> openSet;
        map<PathElement, PathElement, PathElement> came_from;
        map<PathElement, double, PathElement> g_score;
        map<PathElement, double, PathElement> f_score;

        openSet.insert(start);
        g_score[start] = 0;
        f_score[start] = 0 + h(start, goal);

        while (openSet.size() > 0) {
            // szukamy z openSet tego o najnizszym f_score
            std::set<PathElement>::iterator it = openSet.begin();
            PathElement best = *it, toCheck;
            for ( ; it!=openSet.end(); ++it) {
                if (f_score[best] >	f_score[*it]) best = *it;
            }

            if (best == goal) {
                vector<PathElement> pathInvert;
                PathElement current = goal;
                pathInvert.push_back(current);
                while (came_from.count(current) > 0) {
                    current = came_from[current];
                    pathInvert.push_back(current);
                }
                for (; pathInvert.size() > 0; pathInvert.pop_back()) {
                    path.push_back(pathInvert.back());
                }
                return 1; //path;
            }

            openSet.erase(best);
            closedSet.insert(best);

            for (int i = 0; i < 4; i++) {
                toCheck.x = best.x + (((i&2)>>1)*2-1)  *   (1-(i&1));
                toCheck.y = best.y + (((i&2)>>1)*2-1)  *      (i&1);

                if ((world.canWalk(toCheck.x, toCheck.y)) && (closedSet.count(toCheck) == 0)) {
                    double t_g_score = g_score[best] + h(toCheck,best);
                    openSet.insert(toCheck);
                    if (g_score.count(toCheck) == 0) g_score[toCheck] = 999999999;
                    if (t_g_score < g_score[toCheck]) {
                        came_from[toCheck] = best;
                        g_score[toCheck] = t_g_score;
                        f_score[toCheck] = g_score[toCheck] + h(toCheck, goal);
                    }

                }
            }
        }

        return 0;
    }

    void setTarget(double tx, double ty, World &world ) {
        PathElement p, t;
        pathIndex = 0;
        vector <int> visited;
        visited.resize(world.w*world.h);
        for (unsigned i = 0; i < visited.size(); i++) visited[i] = 0;

        path.clear();
        p.x = (int)(_x+0.5);
        p.y = (int)(_y+0.5);
        t.x = (int)(tx+0.5);
        t.y = (int)(ty+0.5);
        if (searchPath(p, t, visited, path, world)) {
           cout<<"znaleziono trase do punktu("<<tx<<","<<ty<<") w kolorze: "<<world.get(tx,ty)<<endl;
        } else {
           cout<<"brak trasy do punktu("<<tx<<","<<ty<<") w kolorze: "<<world.get(tx,ty)<<endl;
        }
    }

    double &x() {
        return _x;
    }

    double &y() {
        return _y;
    }

    vector<PathElement> getPath() {
        return path;
    }

    // krok co dt - czas
    void step(double dt, World &world) {
        double speed = 10;
        if (path.size() > 0) {
            double v[2] = {(((double)path[pathIndex].x)) - _x, (((double)path[pathIndex].y)) - _y};
            double l = sqrt (v[0]*v[0]+v[1]*v[1]);
            if (dt*speed < l) {
                if (world.canWalk(_x + dt*speed*v[0]/l+0.5,_y+0.5))
                    _x += dt*speed*v[0]/l;
                if (world.canWalk(_x+0.5,_y + dt*speed*v[1]/l+0.5))
                    _y += dt*speed*v[1]/l;
            }
            if (pathIndex < path.size()-1) {
                if (l < 0.3) pathIndex++;
            }
        }
    }
} Player;

typedef class Game {
public:
    SDL_Surface *_window;
    int _state;
    static const int _GAME;
    static const int _FINISHED;
    static const int _LOADING;
    Player player;
    World world;
    map<string,SDL_Surface*> tiles;
    unsigned long int lastTick;

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
        _state = _LOADING;
        SDL_Init( SDL_INIT_EVERYTHING );
        _window = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
        SDL_WM_SetCaption( "NAI - wyszukiwanie trasy", NULL );
        load("player", "player.bmp" );
        load("water", "czarny.bmp" );
        load("grass", "bialy.bmp" );
        load("dot", "gold.bmp" );
        load("cross", "cross.bmp" );
        
        
        //ladowanie pliku mapy
        load("map","map.bmp");
        world.obstacles = tiles["map"];
	    world.createMap();
        
        player.x() = player.y() = 0;
        player.setTarget(16,8,world);
        
        _state = _GAME;
        lastTick = SDL_GetTicks(); // w milisekundach
    }
    // umieszcza zadany obrazek na ekranie na zadanych wspolrzednych
    void blit(string elem, int x = 0, int y = 0, int w = 32, int h = 32) {
        SDL_Rect dst;
        dst.x = x;
        dst.y = y;
        dst.w = w;
        dst.h = h;
        SDL_BlitSurface( tiles[elem] , NULL, _window, &dst );
    }

    // wyswietlenie planszy gry (tlo, ludzik, ewentualnie jakies efekty
    void display() {
        // czyszczenie ekranu
        SDL_FillRect( _window, NULL, SDL_MapRGB( _window->format, 0x00, 0x00, 0x00 ) );
        // narysowanie mapy
        for (int x = 0; x < world.w; x++) {
            for (int y = 0; y < world.h; y++) {
                if (world.get(x,y) == 1) blit("water",x*32,y*32);
                else blit("grass",x*32,y*32);
            }
        }
        // narysowanie sciezki
        //for (unsigned i = 0; i < player.getPath().size(); i++)
         //   blit("dot",player.getPath()[i].x*32,player.getPath()[i].y*32);
        if (player.getPath().size() > 0)
            blit("cross",player.getPath().back().x*32,player.getPath().back().y*32);

        // narysowanie gracza
        blit("player",player.x()*32.0,player.y()*32.0);
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
        // zmienne fps (maksymalne)
        SDL_Delay( 1 ); // #pcmasterrace
        unsigned long int now = SDL_GetTicks();
        double dt= (double)(now - lastTick)/1000.0;
        lastTick = now;
        // aktualizacja o dt - liczenie ''fizyki''
        player.step(dt, world);
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
        player.setTarget((x)/32,(y)/32, world);
    }
} Game;

const int Game::_GAME = 0;
const int Game::_FINISHED = 1;
const int Game::_LOADING = 2;

int main( int argc, char *args[] )
{
    Game game;
    game.init();
    while (game.state() == game._GAME) {
        game.input();
        game.display();
        game.calculations();
    }
    game.shutdown();
    return 0;
}
