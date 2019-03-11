#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "triple.h"
#include "image.h"

class Material
{
    public:
        Color color;        // base color
        Image texture;      // texture
        bool hasTexture;    // indicator whether texture is present
        double ka;          // ambient intensity
        double kd;          // diffuse intensity
        double ks;          // specular intensity
        double n;           // exponent for specular highlight size

        Material() = default;

        Material(Color const &color, double ka, double kd, double ks, double n)
        :
            color(color),
            hasTexture(false),
            ka(ka),
            kd(kd),
            ks(ks),
            n(n)
        {}

        Material(Image const img, double ka, double kd, double ks, double n)
        :
            texture(img),
            hasTexture(true),
            ka(ka),
            kd(kd),
            ks(ks),
            n(n)
        {}
};

#endif
