#ifndef HYDRA_2D_COLLISION_MESH
#define HYDRA_2D_COLLISION_MESH

#include <iostream>
#include <algorithm>
#include <vector>
#include <edge.hpp>
#include <transform.hpp>
#include <vec2.hpp>

namespace hydra_2d {
    class collision_mesh {
    public:
        collision_mesh(const std::vector<vec2>&);
        transform tf;
        bool is_point_in_mesh(const vec2&)const;
        const std::vector<vec2>&points()const{return pts;}
        vec2 get_collision_point(const edge&)const;
        bool is_being_collided_by(const collision_mesh&,vec2&)const;
    private:
        std::vector<vec2> pts;
        std::vector<edge> edges;
        std::vector<vec2> transform_points(const collision_mesh&)const;
    };
    collision_mesh::collision_mesh(const std::vector<vec2>& points):tf{}{
        pts = std::vector<vec2>{points};
        edges = std::vector<edge>(points.size() - 1);
        for(size_t i {0}; i != points.size() - 1; ++i) {
            edges[i] = edge{points[i],points[i + 1]};
        }
    }
    bool collision_mesh::is_point_in_mesh(const vec2& point)const{
        return std::all_of(edges.begin(),edges.end(),[&](const edge& e){return e.is_point_behind(point);});
    }
    vec2 collision_mesh::get_collision_point(const edge& e)const{
        vec2 point;
        for (const edge& ed : edges) {
            if (ed.is_intersecting(e,point)) {
                return point;
            }
        }
        return vec2{};
    }
    bool collision_mesh::is_being_collided_by(const collision_mesh& mesh,vec2& col_point)const{
        const std::vector<vec2>& transformed_points {transform_points(mesh)};
        for(size_t i{1}; i != transformed_points.size(); ++i) {
            if (is_point_in_mesh(transformed_points[i])) {
                col_point = transformed_points[i];
                return true;
            }
        }
        return false;
    }
    std::vector<vec2> collision_mesh::transform_points(const collision_mesh& mesh)const{
        std::vector<vec2> transformed_points{mesh.points()};
        //rotate by other object's rotation
        //then translate to space relative to this object
        vec2 translation {mesh.tf.position - tf.position};
        double inv_rot = -tf.rotation;
        std::transform(transformed_points.begin(),transformed_points.end(),transformed_points.begin(),[&](vec2 point){
            point.rotate(mesh.tf.rotation);
            point += translation;
            point.rotate(inv_rot);
            return point;
        });
        return transformed_points;
    }
}

#endif