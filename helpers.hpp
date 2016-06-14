#ifndef __HELPERS_H__
#define  __HELPERS_H__

typedef std::vector< std::vector < double > > polygon;


// za http://sdl.beuc.net/sdl.wiki/Pixel_Access
Uint32 getpixel(SDL_Surface *surface, int x, int y);
// za http://sdl.beuc.net/sdl.wiki/Pixel_Access
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

std::vector < double > operator+(const std::vector < double > &a, const std::vector < double > &b);
std::vector < double > operator-(const std::vector < double > &a, const std::vector < double > &b);
std::vector < double > operator*(const std::vector < double > &a, const std::vector < double > &b);
std::vector < double > operator*(const std::vector < double > &a, const double &b);
std::vector < double > operator/(const std::vector < double > &a, const double &b);

bool operator==(const std::vector < double > &a, const std::vector < double > &b);

double dot(const std::vector < double > &v, const std::vector < double > &w);

double norm(const std::vector < double > &v);
double distance(const std::vector < double > &v1, const std::vector < double > &v2);
std::vector <double > angleConvert(double a);

double angleConvert( std::vector < double > &v);

double distance(const std::vector < double > & P, std::vector< std::vector < double > > &S);

#endif
