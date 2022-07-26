#include "Renderer.h"

#include "Walnut/Random.h"

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
	for (uint32_t i = 0; i < m_FinalImage->GetWidth() * m_FinalImage->GetHeight(); ++i)
		m_ImageData[i] = Walnut::Random::UInt() | 0xff000000; // keep alpha 1

	m_FinalImage->SetData(m_ImageData);
}
