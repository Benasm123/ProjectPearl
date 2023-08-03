#pragma once
#include "PearlCore.h"
#include <memory>
#include <string>
#include <vector>
#include <SDL2/SDL.h>

namespace PEARL_NAMESPACE
{
	class Window
	{
	public:
		Window(const std::string& title, uint32_t width, uint32_t height);
		~Window();

		bool Update();

		[[nodiscard]] std::vector<const char*> GetRequiredVulkanExtensions() const;
		[[nodiscard]] glm::uvec2 GetSize() const;
		[[nodiscard]] SDL_Window* Get() const { return sdlWindow_; }

	private:
		SDL_Window* sdlWindow_;
	};
}

