#include "Renderer.h"

#include "Walnut/Random.h"

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

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); ++y)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); ++x)
		{
			const glm::vec2 coord01 = { static_cast<float>(x) / m_FinalImage->GetWidth(), static_cast<float>(y) / m_FinalImage->GetHeight() };
			const glm::vec2 coord = coord01 * 2.0f - 1.0f; // [0, 1] -> [-1, 1]
			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = static_cast<uint8_t>(coord.x * 255.0f);
	uint8_t g = static_cast<uint8_t>(coord.y * 255.0f);

	glm::vec3 rayOrigin{0.0f, 0.0f, 2.0f};
	glm::vec3 rayDirection{ coord.x, coord.y, -1.0f };
	// rayDirection = glm::normalize(rayDirection);
	float radius = 0.5f; // sphere

	// (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2 + ay^2 - r^2) = 0
	// (bx^2 + by^2 + bz^2)t^2 + 2(axbx + ayby + azbz)t + (ax^2 + ay^2 +az^2 - r^2) = 0
	// a: ray origin
	// b: ray direction
	// r: radius of circle/sphere
	// t: hit distance

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// quadratic discriminant formula: b^2 - 4ac

	// (-b +- sqrt(discriminant)) / 2a
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0)
		return glm::vec4(0, 0, 0, 1);

	return glm::vec4(1, 0, 1, 1);
}
