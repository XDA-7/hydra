#ifndef HYDRA_2D_RANGE_INDEX
#define HYDRA_2D_RANGE_INDEX

#include <map>
#include <vector>

namespace hydra_2d {
    template<typename T>
    class range_index {
    public:
        range_index():range{}{}
        std::vector<T*> get_range(double,double)const;
        void add_index(T*,double);
        void remove_index(T*,double);
    private:
        std::map<double,std::vector<T*>> range;
    };
    template<typename T>
    std::vector<T*> range_index<T>::get_range(double lower,double upper)const{
        auto begin = range.lower_bound(lower);
        auto end = range.upper_bound(upper);
        std::vector<T*> result{};
        while (begin != end) {
            for(T *ref : begin->second) {
                result.push_back(ref);
            }
            ++begin;
        }
        return result;
    }
    template<typename T>
    void range_index<T>::add_index(T *item,double value){
        auto pair = range.find(value);
        if (pair != range.end()) {
            pair->second.push_back(item);
        }
        else {
            range.emplace(value, std::vector<T*>{item});
        }
    }
    template<typename T>
    void range_index<T>::remove_index(T *item,double value){
        auto pair = range.find(value);
        if (pair != range.end()) {
            auto begin = pair->second.begin();
            auto end = pair->second.end();
            while (begin != end) {
                if (*begin == item) {
                    pair->second.erase(begin);
                    break;
                }
                ++begin;
            }
        }
    }
}

#endif