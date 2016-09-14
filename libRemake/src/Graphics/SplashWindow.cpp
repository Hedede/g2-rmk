#include <Graphics/SplashWindow.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
void SplashWindow::EventLoop(SplashWindow& wnd)
{
	auto image = IMG_Load("splash.png");
	if (!image) {
		wnd.Stop();
		return;
	}

	int screen = 0;
	int posX = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);
	int posY = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);

	int width  = image->w;
	int height = image->h;

	SDL_Window* window = SDL_CreateWindow(
			"SDL TEST",
			posX,
			posY,
			width,
			height,
			SDL_WINDOW_BORDERLESS);

	if (!window) {
		wnd.Stop();
		return;
	}

	auto surface = SDL_GetWindowSurface(window);
	SDL_BlitSurface(image, NULL, surface, NULL);
	SDL_UpdateWindowSurface(window);
	SDL_RaiseWindow(window);
	while (wnd.IsRunning()) {
		SDL_Event evt;
		while( SDL_PollEvent(&evt) );
		std::this_thread::yield();
	}

	SDL_DestroyWindow(wnd.window);

	wnd.Done();
}
