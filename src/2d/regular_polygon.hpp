#ifndef HYDRA_2D_REGULAR_POLYGON
#define HYDRA_2D_REGULAR_POLYGON

#include <vector>
#include <vec2.hpp>
#include <shape.hpp>
#include <window.hpp>

namespace hydra_2d {
    class regular_polygon : public shape {
    public:
        regular_polygon(const window& w, int sides, double rad,vec2 origin,double rot)
            :shape(w,get_points(sides,rad),origin,rot),rad{rad}{}
        vec2 get_upper_aab()override;
        vec2 get_lower_aab()override;
        collision_mesh get_collision_mesh()override;
    private:
        double rad;
        std::vector<vec2> get_points(int,double);
    };
    std::vector<vec2> regular_polygon::get_points(int sides,double rad){
        std::vector<vec2> pts(sides + 1);
        double rot = (2.0 * M_PI) / (double)(sides);
        vec2 hand{0.0,rad};
        for (int i = 0; i != sides + 1; ++i) {
            pts[i] = hand;
            hand.rotate(rot);
        }
        return pts;
    }
    vec2 regular_polygon::get_upper_aab(){
        return vec2{rad,rad};
    }
    vec2 regular_polygon::get_lower_aab(){
        return vec2{-rad,-rad};
    }
    collision_mesh regular_polygon::get_collision_mesh(){
        return collision_mesh{pts};
    }
}

#endif