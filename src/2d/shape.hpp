#ifndef HYDRA_2D_SHAPE
#define HYDRA_2D_SHAPE

#include <iostream>
#include <vector>
#include <collision_mesh.hpp>
#include <shape_drawer.hpp>
#include <vec2.hpp>
#include <window.hpp>

namespace hydra_2d {
    class shape {
    public:
        shape(const window& window,const std::vector<vec2>& pts,const vec2& origin,double rot)
            :pts{pts},resolved_pts{std::vector<vec2>(pts.size())},is_resolved{false},origin{origin},rot{rot},drawer{window.get_shape_drawer()}{}

        void draw();
        void translate(vec2 t);
        void rotate(double r);

        double rotation()const;
        double translation_x()const;
        double translation_y()const;

        virtual vec2 get_upper_aab()=0;
        virtual vec2 get_lower_aab()=0;
        virtual collision_mesh get_collision_mesh()=0;
    protected:
        std::vector<vec2> pts;
    private:
        std::vector<vec2> resolved_pts;
        bool is_resolved;
        vec2 origin;
        //measured in radians
        double rot;

        shape_drawer drawer;
        void resolve_pts();
    };
    void shape::draw(){
        if(!is_resolved) {
            resolve_pts();
        }
        drawer.draw_polygon(resolved_pts.begin(), resolved_pts.end());
    }
    void shape::translate(vec2 t){
        origin += t;
        is_resolved = false;
    }
    void shape::rotate(double r){
        rot += r;
        is_resolved = false;
    }

    void shape::resolve_pts() {
        for(size_t i = 0; i != pts.size(); ++i) {
            vec2& resolved_pt{resolved_pts[i]};
            resolved_pt = pts[i];
            resolved_pt.rotate(rot);
            resolved_pt += origin;
        }
        is_resolved = true;
    }

    double shape::rotation()const{return rot;}
    double shape::translation_x()const{return origin.x;}
    double shape::translation_y()const{return origin.y;}
}

#endif