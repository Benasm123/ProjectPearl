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

		void AddEventCallback(std::function<void(SDL_Event)>* callback);

		[[nodiscard]] bool IsMinimised() const { return (SDL_GetWindowFlags(sdl_window_) & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED; }

		[[nodiscard]] std::vector<const char*> GetRequiredVulkanExtensions() const;
		[[nodiscard]] glm::uvec2 GetSize() const;
		[[nodiscard]] SDL_Window* Get() const { return sdl_window_; }

	private:
		SDL_Window* sdl_window_;

		std::vector<std::function<void(SDL_Event)>*> event_callbacks_;
	};
}

