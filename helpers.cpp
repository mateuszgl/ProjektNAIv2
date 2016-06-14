
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


// za http://sdl.beuc.net/sdl.wiki/Pixel_Access
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	if ((x < 0) || (y < 0) || (x >= surface->w) || (y >= surface->h)) return 0;
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}
// za http://sdl.beuc.net/sdl.wiki/Pixel_Access
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	if ((x < 0) || (y < 0) || (x >= surface->w) || (y >= surface->h)) return;
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}


std::vector < double > operator+(const std::vector < double > &a, const std::vector < double > &b) {
    assert(a.size() == b.size());
    std::vector < double > ret;
    ret.resize(a.size());
    for (int i = 0; i < a.size(); i++) ret[i] = a[i] + b[i];
    return ret;
}
std::vector < double > operator-(const std::vector < double > &a, const std::vector < double > &b) {
    assert(a.size() == b.size());
    std::vector < double > ret;
    ret.resize(a.size());
    for (int i = 0; i < a.size(); i++) ret[i] = a[i] - b[i];
    return ret;
}
std::vector < double > operator*(const std::vector < double > &a, const std::vector < double > &b) {
    assert(a.size() == b.size());
    std::vector < double > ret;
    ret.resize(a.size());
    for (int i = 0; i < a.size(); i++) ret[i] = a[i]*b[i];
    return ret;
}
std::vector < double > operator*(const std::vector < double > &a, const double &b) {
    std::vector < double > ret;
    ret.resize(a.size());
    for (int i = 0; i < a.size(); i++) ret[i] = a[i]*b;
    return ret;
}
std::vector < double > operator/(const std::vector < double > &a, const double &b) {
    std::vector < double > ret;
    ret.resize(a.size());
    for (int i = 0; i < a.size(); i++) ret[i] = a[i]/b;
    return ret;
}

bool operator==(const std::vector < double > &a, const std::vector < double > &b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); i++) if (a[i] != b[i]) return false;
    return true;
}

double dot(const std::vector < double > &v, const std::vector < double > &w) {
    double s = 0;
    for (int i = 0; i < v.size(); i++) {
        s += v[i]*w[i];
    }
    return s;
}

double norm(const std::vector < double > &v) {
    return sqrt ( dot(v,v) );
}

double distance(const std::vector < double > &v1, const std::vector < double > &v2) {
    return norm(v2-v1);
}

vector <double > angleConvert(double a) {
	vector < double > v(2);
	v[0] = cos(a);
	v[1] = sin(a);
	return v;
}

double angleConvert( vector < double > &v) {
	vector < double > vNorm = v/norm(v);
	return atan2 ( vNorm[1], vNorm[0] );
}



// dist_Point_to_Segment(): get the distance of a point to a segment
//     Input:  a Point P and a Segment S (in any dimension)
//     Return: the shortest distance from P to S
double distance(const std::vector < double > & P, vector< vector < double > > &S)
{
	double minDistance;
	for (int i = 1; i < S.size(); i++) {
		std::vector < double > v = S[i] - S[i-1];
		std::vector < double > w = P - S[i-1];
		double dist;
		double c1 = dot(w,v);
		if ( c1 <= 0 ) {
			dist = distance(P, S[i-1]);
		} else {
			double c2 = dot(v,v);
			if ( c2 <= c1 ) {
				dist = distance(P, S[i]);
			} else {
				double b = c1 / c2;
				std::vector < double > Pb = S[i-1] + v*b;
				dist = distance(P, Pb);
			}
		}
		if (i == 1) {
			minDistance = dist;
		} else {
			if (minDistance < dist) minDistance = dist;
		}

	}
return minDistance;
}
