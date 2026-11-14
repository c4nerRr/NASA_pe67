
#ifndef NASA_HUBOUTLINE_H
#define NASA_HUBOUTLINE_H
#include <vector>

#include "HubPortal.h"
#include "Objects3D.h"


class HubOutline {
    Shader _maskShader{};
    Shader _outlineShader{};
    RenderTexture2D _mask{};

    int _selectedLoc = -1;
    int _texelLoc = -1;

    void DrawMaskObject(const Objects3D& object, bool selected) const;

    public:
    HubOutline();
    ~HubOutline();

    HubOutline(const HubOutline&) = delete;
    HubOutline& operator=(const HubOutline&) = delete;

    void Resize(int width, int height);
    void Draw(const Camera3D& camera,const Objects3D& moon,const std::vector<HubPortal>& portals,int selected,bool doubleSided) const;
};


#endif //NASA_HUBOUTLINE_H