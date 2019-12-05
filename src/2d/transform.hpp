#ifndef HYDRA_2D_TRANSFORM
#define HYDRA_2D_TRANSFORM

#include <vec2.hpp>

namespace hydra_2d {
    class transform {
    public:
        transform():position{},rotation{0.0}{}
        transform(vec2 position,double rotation):position{position},rotation{rotation}{}
        vec2 position;
        double rotation;
    };
}

#endif