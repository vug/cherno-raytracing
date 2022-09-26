#include "Renderer.h"

#include "Walnut/Random.h"

#include <glm/gtx/extended_min_max.hpp>

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		const auto r = static_cast<uint8_t>(color.r * 255.0f);
		const auto g = static_cast<uint8_t>(color.g * 255.0f);
		const auto b = static_cast<uint8_t>(color.b * 255.0f);
		const auto a = static_cast<uint8_t>(color.a * 255.0f);
		const uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);

	delete[] m_ImageData; // It's OK to delete a nullptr, no need for if
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	Ray ray;
	ray.Origin = camera.GetPosition();

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); ++y)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); ++x)
		{
			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

			glm::vec4 color = TraceRay(scene, ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	// (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2 + ay^2 - r^2) = 0
	// (bx^2 + by^2 + bz^2)t^2 + 2(axbx + ayby + azbz)t + (ax^2 + ay^2 +az^2 - r^2) = 0
	// a: ray origin
	// b: ray direction
	// r: radius of circle/sphere
	// t: hit distance

	if (scene.Spheres.size() == 0)
		return glm::vec4(0, 0, 0, 1);

	const Sphere* closestSphere = nullptr;
	float hitDistance = std::numeric_limits<float>::max();
	for (const Sphere& sphere : scene.Spheres)
	{
		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		// quadratic discriminant formula: b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0)
			continue;

		// (-b +- sqrt(discriminant)) / 2a
		const float sqrtDisc = glm::sqrt(discriminant);
		float t0 = (-b + sqrtDisc) / (2.0f * a); // distance from origin to hit-point
		float closestT = (-b - sqrtDisc) / (2.0f * a);
		if (closestT < hitDistance)
		{
			hitDistance = closestT;
			closestSphere = &sphere;
		}
		
	}

	if (closestSphere == nullptr)
		return glm::vec4{ 0,0,0,1 };


	glm::vec3 origin = ray.Origin - closestSphere->Position;
	glm::vec3 hitPoint = origin + ray.Direction * hitDistance;
	glm::vec3 normal = glm::normalize(hitPoint); // sphere center is not parametrized yet

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec3 sphereColor = closestSphere->Albedo;
	//sphereColor = (hitPoint + 1.f) * 0.5f; // debugging
	//sphereColor = normal * 0.5f + 0.5f;
	sphereColor *= d;
	return glm::vec4(sphereColor, 1);
}
