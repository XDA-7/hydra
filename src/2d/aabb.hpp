#ifndef HYDRA_2D_AABB
#define HYDRA_2D_AABB

#include <iostream>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <vector>
#include <range_index.hpp>

namespace hydra_2d {
    template<typename T>
    class aabb {
    public:
        aabb(double x1,double x2,double y1,double y2,T *ref):x1{x1},x2{x2},y1{y1},y2{y2},ref{ref}{}
        double x1,x2,y1,y2;
        T *ref;
    };

    template<typename T>
    using col_pair = std::pair<T*,T*>;
    template<typename T>
    class col_pair_hash {
    public:
        size_t operator()(const col_pair<T>& pair)const{
            return reinterpret_cast<size_t>(pair.first) + reinterpret_cast<size_t>(pair.second);
        }
    };
    template<typename T>
    class col_pair_equ {
    public:
        bool operator()(const col_pair<T>& a, const col_pair<T>& b)const{
            return (a.first == b.first && a.second == b.second) ||
                (a.first == b.second && a.second == b.first);
        }
    };
    template<typename T>
    using col_pair_set = std::unordered_set<col_pair<T>,col_pair_hash<T>,col_pair_equ<T>>;

    template<typename T>
    class aabb_set {
    public:
        using aabb_type = aabb<T>;
        using lookup_ptr = typename std::map<T*,aabb<T>>::iterator;
        aabb_set()
            :ref_lookup{},lower_x_tbl{},upper_x_tbl{},lower_y_tbl{},upper_y_tbl{}{}
        void set_aabb(const aabb<T>&);
        void remove_aabb(T*);
        std::vector<T*> get_colliding_elements(T*);
        std::vector<col_pair<T>> get_colliding_pairs();
    private:
        std::map<T*,aabb<T>> ref_lookup;
        range_index<T> lower_x_tbl;
        range_index<T> upper_x_tbl;
        range_index<T> lower_y_tbl;
        range_index<T> upper_y_tbl;
        std::vector<T*> get_sorted_range(double,double,const range_index<T>&)const;
        void trim_vec(std::vector<T*>& vec)const;
    };
    template<typename T>
    void aabb_set<T>::set_aabb(const aabb<T>& aabb) {
        lookup_ptr pair = ref_lookup.find(aabb.ref);
        if (pair != ref_lookup.end()) {
            aabb_type& prev = pair->second;
            lower_x_tbl.remove_index(prev.ref,prev.x1);
            upper_x_tbl.remove_index(prev.ref,prev.x2);
            lower_y_tbl.remove_index(prev.ref,prev.y1);
            upper_y_tbl.remove_index(prev.ref,prev.y2);
            prev = aabb;
            lower_x_tbl.add_index(aabb.ref,aabb.x1);
            upper_x_tbl.add_index(aabb.ref,aabb.x2);
            lower_y_tbl.add_index(aabb.ref,aabb.y1);
            upper_y_tbl.add_index(aabb.ref,aabb.y2);
        }
        else {
            ref_lookup.emplace(aabb.ref, aabb);
            aabb_type q {ref_lookup.find(aabb.ref)->second};
            lower_x_tbl.add_index(aabb.ref,aabb.x1);
            upper_x_tbl.add_index(aabb.ref,aabb.x2);
            lower_y_tbl.add_index(aabb.ref,aabb.y1);
            upper_y_tbl.add_index(aabb.ref,aabb.y2);
        }
    }
    template<typename T>
    void aabb_set<T>::remove_aabb(T* ref) {
        lookup_ptr pair = ref_lookup.find(ref);
        if (pair != ref_lookup.end()) {
            aabb_type& aabb = pair->second;
            lower_x_tbl.remove_index(ref,aabb.x1);
            upper_x_tbl.remove_index(ref,aabb.x2);
            lower_y_tbl.remove_index(ref,aabb.y1);
            upper_y_tbl.remove_index(ref,aabb.y2);
            ref_lookup.erase(ref);
        }
    }
    template<typename T>
    std::vector<T*> aabb_set<T>::get_colliding_elements(T* ref) {
        lookup_ptr pair = ref_lookup.find(ref);
        if (pair == ref_lookup.end()) {
            return std::vector<T*>{};
        }
        aabb_type& aabb = pair->second;
        std::vector<T*> lx_col = get_sorted_range(aabb.x1,aabb.x2, lower_x_tbl);
        std::vector<T*> ux_col = get_sorted_range(aabb.x1,aabb.x2, upper_x_tbl);
        std::vector<T*> ly_col = get_sorted_range(aabb.y1,aabb.y2,lower_y_tbl);
        std::vector<T*> uy_col = get_sorted_range(aabb.y1,aabb.y2,upper_y_tbl);

        std::vector<T*> inter_x(lx_col.size() + ux_col.size());
        std::set_union(lx_col.begin(),lx_col.end(),ux_col.begin(),ux_col.end(),inter_x.begin());
        trim_vec(inter_x);

        std::vector<T*> inter_y(ly_col.size() + uy_col.size());
        std::set_union(ly_col.begin(),ly_col.end(),uy_col.begin(),uy_col.end(),inter_y.begin());
        trim_vec(inter_y);

        std::vector<T*> inter(std::max(inter_x.size(),inter_y.size()));
        std::set_intersection(inter_x.begin(),inter_x.end(),inter_y.begin(),inter_y.end(),inter.begin());
        trim_vec(inter);
        return inter;
    }
    template<typename T>
    std::vector<col_pair<T>> aabb_set<T>::get_colliding_pairs() {
        col_pair_set<T> pairs{};
        lookup_ptr begin {ref_lookup.begin()};
        lookup_ptr end {ref_lookup.end()};
        while(begin != end) {
            T *left {begin->first};
            for(T *right : get_colliding_elements(left)) {
                if (left != right) {
                    pairs.insert(col_pair<T>{left,right});
                }
            }
            ++begin;
        }
        return std::vector<col_pair<T>>{pairs.begin(),pairs.end()};
    }
    template<typename T>
    std::vector<T*> aabb_set<T>::get_sorted_range(double lower, double upper, const range_index<T>& range)const{
        std::vector<T*> col = range.get_range(lower,upper);
        std::sort(col.begin(),col.end());
        return col;
    }
    template<typename T>
    void aabb_set<T>::trim_vec(std::vector<T*>& vec)const{
        for(size_t i {0}; i != vec.size(); ++i) {
            if(!vec[i]) {
                vec.resize(i);
                return;
            }
        }
    }
}

#endif