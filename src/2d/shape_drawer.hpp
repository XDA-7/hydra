#ifndef HYDRA_2D_SHAPE_DRAWER
#define HYDRA_2D_SHAPE_DRAWER

#include <iostream>
#include <SDL.h>
#include <vec2.hpp>

namespace hydra_2d {
    class shape_drawer {
    public:
        shape_drawer(SDL_Renderer *r):r{r}{}
        template<typename Iter>
        void draw_polygon(Iter,Iter);
    private:
        SDL_Renderer *r;
    };
    template<typename Iter>
    void shape_drawer::draw_polygon(Iter begin,Iter end) {
        SDL_SetRenderDrawColor(r,0xFF, 0xFF, 0xFF, 0xFF);
        for(;;) {
            vec2 left {*begin};
            ++begin;
            if (begin == end) {
                break;
            }
            vec2 right {*begin};
            SDL_RenderDrawLine(r,left.x,left.y,right.x,right.y);
        }
    }
}

#endif