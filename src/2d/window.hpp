#ifndef HYDRA_2D_WINDOW
#define HYDRA_2D_WINDOW

#include <SDL.h>
#include <shape_drawer.hpp>

namespace hydra_2d
{
    class window {
    public:
        window(int width, int height);
        ~window();

        shape_drawer get_shape_drawer() const;
        void refresh();
        bool quit = false;
    private:
        SDL_Window *w;
        SDL_Surface *s;
        SDL_Renderer *r;
        void clear_screen()const;
    };
    window::window(int width, int height) {
        SDL_Init(SDL_INIT_VIDEO);
        w = SDL_CreateWindow(
            "Hydra",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_SHOWN);
        s = SDL_GetWindowSurface(w);
        r = SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED);
    }
    window::~window() {
        SDL_DestroyRenderer(r);
        SDL_DestroyWindow(w);
        SDL_Quit();
    }
    shape_drawer window::get_shape_drawer()const{return shape_drawer{r};}
    void window::refresh() {
        SDL_Event e;
        if(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_RenderPresent(r);
        clear_screen();
    }
    void window::clear_screen()const{
        SDL_SetRenderDrawColor(r,0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(r);
    }
}

#endif