#include <limits>
#include "RayTracer.h"
#include "Material.h"
#include "Geometry.h"
#include "Lighting.h"

//Базовый алгоритм трассировки луча
float3 SimpleRT::TraceRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geo)
{
  float tnear = std::numeric_limits<float>::max();
  int   geoIndex = -1;

  SurfHit surf;

  for (int i = 0; i < geo.size(); ++i)
  {
    SurfHit temp;

    if (geo.at(i)->Intersect(ray, 0.001, tnear, temp))
    {
      if (temp.t < tnear)
      {
        tnear = temp.t;
        geoIndex = i;
        surf = temp;
      }
    }
  }


  if (geoIndex == -1)
  {
    float3 unit_direction = normalize(ray.d);
    float t = 0.5f * (unit_direction.y + 1.0f);

    return (1.0f - t) * float3(1.0f, 1.0f, 1.0f) + t * bg_color;
  }

  float3 surfColor(0.0f, 0.0f, 0.0f);
  if (dot(ray.d, surf.normal) > 0)
  {
    surf.normal = -surf.normal;
  }
   
}

 // Уиттед
float3 WhittedRT::TraceRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& objects, const std::vector<std::shared_ptr<Lighting>>& light) {
	float3 color = float3(1.0f, 0.9f, 0.8f);
	float3 timeColor = float3(1.0f, 1.0f, 1.0f);
	SurfHit surf;
	Ray timeRay = ray;

	while(1)
	{
		color *= timeColor;
		float tnear = std::numeric_limits<float>::max();

		int   geoIndex = -1;
		SurfHit surf;

		for (int i = 0; i < objects.size(); ++i)
		{
			SurfHit temp;
			
			if (objects.at(i)->Intersect(timeRay, 0.001, tnear, temp))//  есть пересечение
			{
				if (temp.t < tnear)
				{
					tnear = temp.t;
					geoIndex = i;
					surf = temp;
				}
			}
		}
		
		if (geoIndex == -1)// не пересекли
		{
			float3 unit_direction = normalize(timeRay.d);
			float t = 0.5f * (unit_direction.y + 1.0f);
			timeColor = (1.0f - t) * float3(1.0f, 1.0f, 1.0f) + t * bg_color;

			break;
		}

		if (dot(timeRay.d, surf.normal) > 0) surf.normal = -surf.normal;

		Ray scattered;
		
		if (typeid(*surf.m_ptr) != typeid(Light))
		{
			
			if ((typeid(*surf.m_ptr).name() == typeid(Diffusion_material).name()))
			{
				timeColor = float3(0.0f, 0.0f, 0.0f);
				float3 time;
				int countOfLightSourses = 0;
				for (int i = 0; i < light.size(); i++) {
					Ray rayIn;
					rayIn.o = light.at(i)->position;
					rayIn.d = normalize(rayIn.o - surf.hitPoint);


					Ray shadow(surf.hitPoint + normalize(surf.normal) * 10e-5, rayIn.d);
				
					if (!ShadowRay(shadow, objects))	// нет проблем на пути
					{
						surf.m_ptr->Scatter(rayIn, surf, time, scattered);
						timeColor += time * light.at(i)->color;
						++countOfLightSourses;
					}
				}
				break;
			}
			
			else if (surf.m_ptr->Scatter(timeRay, surf, timeColor, scattered))// пересечение с зеркалом
			{
				timeRay = scattered;
			}
			else
			{
				timeColor = float3(0.0f, 0.0f, 0.0f);
			}
		}

		else {		// источник света
			surf.m_ptr->Scatter(timeRay, surf, timeColor, scattered);
			break;
		}
	}
	color *= timeColor;
	return color;

}

// есть ли какие-то объекты на пути луча от найденной точки пересечения  до источника света или нет
bool WhittedRT::ShadowRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geo) {
	Ray timeRay = ray;
	float tnear = std::numeric_limits<float>::max();
	int   geoIndex = -1;
	SurfHit surf;
	for (int i = 0; i < geo.size(); ++i)
	{
		SurfHit temp;
		if (geo.at(i)->Intersect(timeRay, 0.001, tnear, temp))
		{
			if (temp.t < tnear)
			{
				tnear = temp.t;
				geoIndex = i;
				surf = temp;
			}
			return true;
		}
	}
	return false;
}
