#pragma once
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include "Walnut/Image.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <memory>

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
	glm::vec4 TraceRay(const Scene& scene, const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};
