#include <iostream>
#include <vector>
#include <random>

#include "LiteMath.h"
#include "Geometry.h"
#include "Camera.h"
#include "Lighting.h"
using namespace HydraLiteMath;

struct Scene {
    std::vector<std::shared_ptr<GeoObject>> geoObjects;
    std::vector<std::shared_ptr<Lighting>> lights;
};

void RenderScene(uint32_t w, uint32_t h, uint32_t num_samples, const Scene &scene, const Camera &cam, const std::string &filename)
{
  auto  background_color = float3(0.1f, 0.1f, 0.1f);
  auto  film = std::make_unique<Film>(w, h, num_samples);
  auto  tracer = std::make_unique<WhittedRT>(16, background_color);
  float invWidth  = 1.0f / float(w);
  float invHeight = 1.0f / float(h);

  for (int y = 0; y < h; ++y)
  {
    for (int x = 0; x < w; ++x)
    {
      float3 pixel_color = float3(0.0f, 0.0f, 0.0f);

      for (int s = 0; s < num_samples; ++s)
      {
        Ray ray = cam.genRay(w, h, x, h - y); //генерируем луч из камеры через текущий пиксель
        pixel_color += tracer->TraceRay(ray, scene.geoObjects, scene.lights, 0); //трассируем луч и получаем цвет
      }
      pixel_color /= film->num_samples;      // усредняем полученные цвета
      pixel_color *= cam.getExposureTime();  // умножаем на время экспонирования сенсора - выдержка виртуальной камеры
      film->SetPixelColor(x, y, pixel_color); //записываем цвет на виртуальный сенсор
    }
  }
  film->SaveImagePPM(filename); //сохраняем картинку
}

void create_scene()
{
  Scene myScene;
  //объекты
  auto plane = std::make_shared<Plane>(float3(-2.0f, -1.0f, +0.0f), float3(0.0f, 1.0f, 0.0f), new IdealMirror(float3(0.5f, 0.5f, 0.5f)));

  auto sphere1= std::make_shared<Sphere>(float3(1.0f, 1.6f, +6.0f), 5.5, new IdealMirror(float3(5.00f, 4.0f, 0.36f)));
  auto sphere2= std::make_shared<Sphere>(float3(8.0f, +0.6f, +9.0f), 1.5, new Defuse_material(float3(1.00f, 1.00f, 1.00f)));

  auto triangle = std::make_shared<Triangle>(float3(2.0f, 10.0f, 6.0f), float3(5.0f, 15.0f, 4.0f),float3(8.0f, 10.0f, 5.0f), new Defuse_material(float3(3.00f, 7.0f, 2.36f)));

  auto square  = std::make_shared<Square >(float3(-4.0f, 10.0f, 1.0f),5.3,new Defuse_material(float3(3.00f, 4.0f, 7.0f)));

  auto  parallelepiped  = std::make_shared<Parallelepiped>(float3(-15.0f, -0.0f, -10.0f), float3(-11.0f, +6.0f, -2.0f), new IdealMirror(float3(2.00f, 5.0f, 4.36f)));

  // освещение
  auto light1 = std::make_shared<Lighting>(float3(-15.0f, 16.0f, 5.0f), float3(1.0f, 1.0f, 1.0f));
 

 myScene.geoObjects.push_back(plane);
 myScene.geoObjects.push_back(sphere1);
 myScene.geoObjects.push_back(sphere2);
 myScene.geoObjects.push_back(triangle);
 myScene.geoObjects.push_back(square);
 myScene.geoObjects.push_back(parallelepiped);

 myScene.lights.push_back(light1);

 //информация о "глазе"
  float3        eye   (0.0f, 2.0f, 20.0f);
  float3        lookat(0.0f, 2.0f, 0.0f);
  float3        up    (0.0f, 1.0f, 0.0f);
  float         field_of_view = 90.0f;
  unsigned int  w = 640;
  unsigned int  h =  480;
  Camera        cam(eye, lookat, up, field_of_view, float(w) / float(h));


  RenderScene(w, h, 1, myScene, cam, "Adutova_Scene");
}

int main()
{
  create_scene();

  return 0;
}
