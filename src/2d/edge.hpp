#ifndef HYDRA_2D_EDGE
#define HYDRA_2D_EDGE

#include <iostream>
#include <vec2.hpp>

namespace hydra_2d {
    class edge {
    public:
        edge(){};
        edge(const vec2&,const vec2&);
        bool is_point_behind(const vec2&)const;
        const vec2& normal()const{return norm;}
        bool is_intersecting(const edge&,vec2&)const;
        const vec2& left_point()const{return left;}
        const vec2& right_point()const{return right;}
    private:
        vec2 left;
        vec2 right;
        vec2 direc;
        vec2 norm;
        vec2 inv_norm;
        vec2 norm_point;
        vec2 inv_norm_point;
        double grad;
        double b;
    };
    edge::edge(const vec2& left,const vec2& right):left{left},right{right}{
        direc = right - left;
        direc.normalize();
        norm = direc;
        norm.rotate_270();
        inv_norm = direc;
        inv_norm.rotate_90();
        norm_point = norm + left;
        inv_norm_point = inv_norm + left;

        grad = (right.y - left.y) / (right.x - left.x);
        b = left.y - grad * left.x;
    }
    bool edge::is_point_behind(const vec2& point)const{
        return point.dist_squared(norm_point) > point.dist_squared(inv_norm_point);
    }
    bool edge::is_intersecting(const edge& e,vec2& point)const{
        if (grad == e.grad) {
            return false;
        }

        // knowing that the computed x value intersects both planes,
        // only the x value needs to be checked to be in the range of the edges
        double x = (e.b - b) / (grad - e.grad);
        point = vec2{x,grad * x + b};
        std::pair<double,double> x_bounds{left.x,right.x};
        if (x_bounds.first > x_bounds.second) {
            std::swap(x_bounds.first,x_bounds.second);
        }
        std::pair<double,double> x_e_bounds{e.left.x,e.right.x};
        if (x_e_bounds.first > x_e_bounds.second) {
            std::swap(x_e_bounds.first,x_e_bounds.second);
        }
        return x >= x_bounds.first && x <= x_bounds.second &&
            x >= x_e_bounds.first && x <= x_e_bounds.second;
    }
}

#endif