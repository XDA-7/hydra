#ifndef HYDRA_2D_SIMULATION
#define HYDRA_2D_SIMULATION

#include <SDL.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <aabb.hpp>
#include <collision_mesh.hpp>
#include <shape.hpp>
#include <transform.hpp>
#include <vec2.hpp>

namespace hydra_2d {
    class sim_obj {
    public:
        sim_obj(shape *shape,const transform& transform,const vec2& velocity,double torque,bool is_static,aabb_set<sim_obj>& aabb_set,vec2 upper_aab,vec2 lower_aab)
            :shape{shape},tf{transform},velocity{velocity},torque{torque},is_static{is_static},aabb_set{aabb_set},upper_aab{upper_aab},lower_aab{lower_aab},col_mesh{shape->get_collision_mesh()}{}
        //shapeless object ctor
        sim_obj(const collision_mesh& col_mesh,const transform& transform,const vec2& velocity,double torque,bool is_static,aabb_set<sim_obj> aabb_set,vec2 upper_aab,vec2 lower_aab)
            :shape{nullptr},tf{transform},velocity{velocity},torque{torque},is_static{is_static},aabb_set{aabb_set},upper_aab{upper_aab},lower_aab{lower_aab},col_mesh{col_mesh}{}
        shape *shape;
        transform tf;
        vec2 velocity;
        double torque;
        bool is_static;
        void operator()(double);
        void accelerate(const vec2&);
        void resolve_collision(sim_obj&,const vec2&);
        bool will_obj_collide(sim_obj&,vec2&,double);
    private:
        aabb_set<sim_obj>& aabb_set;
        vec2 upper_aab;
        vec2 lower_aab;
        collision_mesh col_mesh;
    };

    class simulation {
    public:
        simulation()
            :aabb_set{},objs{},static_objs{},last_time{SDL_GetTicks()}{}
        ~simulation();
        void operator()();
        void add_shape(shape*,vec2,double,vec2,double);
        void set_limits(vec2,vec2);
    private:
        aabb_set<sim_obj> aabb_set;
        std::vector<sim_obj*> objs;
        std::vector<sim_obj*> static_objs;
        Uint32 last_time;
        void resolve_collisions(double);
        void create_static_boundary(vec2,vec2);
    };

    simulation::~simulation() {
        for (sim_obj *obj : objs) {
            delete obj;
        }
        for (sim_obj *obj : static_objs) {
            delete obj;
        }
    }
    void simulation::operator()(){
        Uint32 time = SDL_GetTicks();
        double delta = (double)(time - last_time) / 1000.0;
        resolve_collisions(delta);
        last_time = time;
        for(auto obj : objs) {
            (*obj)(delta);
        }
    }
    void simulation::add_shape(shape *shape,vec2 position,double rotation,vec2 velocity,double torque){
        vec2 upper_bound {shape->get_upper_aab() + position};
        vec2 lower_bound {shape->get_lower_aab() + position};
        sim_obj *obj = new sim_obj{
            shape,
            transform{position,rotation},
            velocity,
            torque,
            false,
            aabb_set,
            upper_bound,
            lower_bound
        };
        objs.push_back(obj);
    }
    void simulation::set_limits(vec2 lower,vec2 upper) {
        //left
        create_static_boundary(vec2{lower.x - 100.0,lower.y},vec2{lower.x,upper.y});
        //right
        create_static_boundary(vec2{upper.x,lower.y},vec2{upper.x + 100.0,upper.y});
        //top
        create_static_boundary(vec2{lower.x,upper.y},vec2{upper.x,upper.y + 100.0});
        //bottom
        create_static_boundary(vec2{lower.x,lower.y - 100.0},vec2{upper.x,lower.y});
    }
    void simulation::resolve_collisions(double delta) {
        vec2 col_point;
        for(col_pair<sim_obj> col : aabb_set.get_colliding_pairs()) {
            if (col.first->will_obj_collide(*col.second,col_point,delta)) {
                col.first->resolve_collision(*col.second,col_point);
            }
        }
    }
    void simulation::create_static_boundary(vec2 lower,vec2 upper) {
        vec2 position{(lower.x + upper.x) / 2.0,(lower.y + upper.y) / 2.0};
        vec2 local_lower{lower - position};
        vec2 local_upper{upper - position};
        sim_obj *obj = new sim_obj{
            collision_mesh{std::vector<vec2>{
                vec2{local_lower.x,local_lower.y},
                vec2{local_lower.x,local_upper.y},
                vec2{local_upper.x,local_upper.y},
                vec2{local_upper.x,local_lower.y},
                vec2{local_lower.x,local_lower.y}
            }},
            transform{position,0.0},
            vec2{},
            0.0,
            true,
            aabb_set,
            upper,
            lower
        };
        aabb_set.set_aabb(aabb<sim_obj>{lower.x,upper.x,lower.y,upper.y,obj});
        static_objs.push_back(obj);
    }

    void sim_obj::accelerate(const vec2& acc){
        if(!is_static) {
            velocity += acc;
        }
    }
    void sim_obj::operator()(double delta){
        vec2 displacement = velocity * delta;
        shape->translate(displacement);
        double rot = torque * delta;
        tf.rotation += rot;
        shape->rotate(rot);
        shape->draw();
        tf.position += displacement;
        upper_aab += displacement;
        lower_aab += displacement;
        aabb_set.set_aabb(aabb<sim_obj>{lower_aab.x,upper_aab.x,lower_aab.y,upper_aab.y,this});
    }
    void sim_obj::resolve_collision(sim_obj& obj,const vec2& col_point){
        vec2 point_tan{col_point};
        point_tan.rotate_90();
        vec2 rel_velocity{obj.velocity - velocity};
        vec2 reaction_component{-rel_velocity.proj(col_point)};
        vec2 angular_component{rel_velocity.proj(point_tan)};
        double torque_component = angular_component.mag() / 6.28;
        if (!is_static){
            velocity += reaction_component;
            obj.velocity -= reaction_component;
            torque += torque_component;
            obj.torque -= torque_component;
        }
        else{
            obj.velocity -= 2.0 * reaction_component;
            obj.torque -= 2.0 * torque_component;
        }
        /*if (!is_static) {
            velocity += rel_velocity;
            obj.velocity -= rel_velocity;
        }
        else {
            obj.velocity -= 2.0 * rel_velocity;
        }*/
    }
    bool sim_obj::will_obj_collide(sim_obj& obj,vec2& col_point,double delta){
        col_mesh.tf = tf;
        col_mesh.tf.position += velocity * delta;
        col_mesh.tf.rotation += torque * delta;
        obj.col_mesh.tf = obj.tf;
        obj.col_mesh.tf.position += obj.velocity * delta;
        obj.col_mesh.tf.rotation += obj.torque * delta;
        return col_mesh.is_being_collided_by(obj.col_mesh,col_point);
    }
}

#endif