#include <SDL.h>

#include <aabb.hpp>
#include <collision_mesh.hpp>
#include <edge.hpp>
#include <range_index.hpp>
#include <regular_polygon.hpp>
#include <simulation.hpp>
#include <window.hpp>

constexpr int screen_width = 900;
constexpr int screen_height = 900;

hydra_2d::vec2 random_vel() {
    return hydra_2d::vec2 {(double)(std::rand() % 50), (double)(std::rand() % 50)};
}

int main(int argc, char * argv[]) {
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
    hydra_2d::window w {screen_width,screen_height};
    hydra_2d::simulation sim;
    sim.set_limits(hydra_2d::vec2{0.0,0.0},hydra_2d::vec2{screen_width,screen_height});
    std::vector<hydra_2d::regular_polygon> o{
        hydra_2d::regular_polygon{w,3,150.0,hydra_2d::vec2{300.0,300.0},0.0},
        hydra_2d::regular_polygon{w,3,150.0,hydra_2d::vec2{600.0,300.0},3.14},
        hydra_2d::regular_polygon{w,3,150.0,hydra_2d::vec2{300.0,600.0},0.0},
        hydra_2d::regular_polygon{w,3,150.0,hydra_2d::vec2{600.0,600.0},3.14},
    };
    std::cout << "shapes constructed" << std::endl;
    for (int i{0}; i != 4; ++i) {
        sim.add_shape(&o[i],hydra_2d::vec2{o[i].translation_x(),o[i].translation_y()},o[i].rotation(),random_vel(),0.0);
    }
    std::cout << "added shapes" << std::endl;
    while(!w.quit) {
        sim();
        w.refresh();
    }
    return 0;
}