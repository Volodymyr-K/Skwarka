#ifndef RENDER_UPDATE_CALLBACK_H
#define RENDER_UPDATE_CALLBACK_H

#include <vector>

#include <Raytracer/Core/Renderer.h>
#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Raytracer/Core/Color.h>

#include "RenderCanvas.h"

class RenderUpdateCallback: public DisplayUpdateCallback
  {
  public:

    RenderUpdateCallback(RenderCanvas *ip_canvas): mp_canvas(ip_canvas)
      {
      }

    void Update(intrusive_ptr<const Film> ip_film)
      {
      const Film *p_film = ip_film.get();
      size_t height = p_film->GetYResolution(), width = p_film->GetXResolution();

      std::vector<unsigned char> data(width*height * 4);

      for(int y=0;y<(int)height;++y)
        for(int x=0;x<(int)width;++x)
          {
          Spectrum_d sp;
          p_film->GetPixel(Point2D_i(x,y),sp);
          sp *= 500;//250;

          RGBColor_d color = global_sRGB_D65_ColorSystem.XYZ_To_RGB(SpectrumRoutines::SpectrumToXYZ(sp),true);
          color.Clamp(0.0,256.0);
          color[0]/=256.0;color[1]/=256.0;color[2]/=256.0;
          color = global_sRGB_D65_ColorSystem.GammaEncode(color);
          int r = std::min(255,(int)(color[0]*256.0));
          int g = std::min(255,(int)(color[1]*256.0));
          int b = std::min(255,(int)(color[2]*256.0));

          unsigned int pixel_index = (unsigned int) ((y*width+x)*4);
          data[pixel_index+0] = unsigned char(b);
          data[pixel_index+1] = unsigned char(g);
          data[pixel_index+2] = unsigned char(r);
          data[pixel_index+3] = unsigned char (~0);
          }

      mp_canvas->setImageData(width, height, std::move(data));
      }

  private:
    RenderCanvas *mp_canvas;
  };

#endif // RENDER_UPDATE_CALLBACK_H

