#include "Window.h"
#include <vector>
#include <SDL2/SDL_vulkan.h>

using namespace PEARL_NAMESPACE ;

Window::Window(const std::string& title, const uint32_t width, const uint32_t height)
{
	sdl_window_ = SDL_CreateWindow(
		title.c_str()
		, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
		, static_cast<int>(width), static_cast<int>(height)
		, SDL_WindowFlags::SDL_WINDOW_RESIZABLE | SDL_WindowFlags::SDL_WINDOW_VULKAN);
}

Window::~Window()
{
	SDL_DestroyWindow(sdl_window_);
}


bool Window::Update()
{
	SDL_Event event;
	while ( SDL_PollEvent(&event) ) {
		for (const std::function<void(SDL_Event)>* func : event_callbacks_)
		{
			(*func)(event);
		}

		switch ( event.type )
		{
			case SDL_QUIT:
			return false;
			default:
			break;
		}
	}

	return true;
}

void Window::AddEventCallback(std::function<void(SDL_Event)>* callback)
{
	event_callbacks_.push_back(callback);
}


std::vector<const char*> Window::GetRequiredVulkanExtensions() const
{
	uint32_t numberOfExtensionsRequiredBySDL;
	SDL_Vulkan_GetInstanceExtensions(sdl_window_, &numberOfExtensionsRequiredBySDL, nullptr);

	std::vector<const char*> sdlExtensions(numberOfExtensionsRequiredBySDL);

	SDL_Vulkan_GetInstanceExtensions(sdl_window_, &numberOfExtensionsRequiredBySDL, sdlExtensions.data());
	return sdlExtensions;
}

glm::uvec2 PEARL_NAMESPACE::Window::GetSize() const
{
	int width = 0;
	int height = 0;
	SDL_GetWindowSize(sdl_window_, &width, &height);
	return glm::uvec2(width, height);
}
