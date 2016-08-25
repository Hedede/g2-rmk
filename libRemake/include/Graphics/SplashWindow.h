#ifndef Gothic_SplashWindow_H
#define Gothic_SplashWindow_H
extern "C" {
typedef struct SDL_Window SDL_Window;
}

#include <thread>
#include <mutex>
struct SplashWindow {
	SplashWindow() = default;
	~SplashWindow()
	{
		Stop();
	}

	SplashWindow(SplashWindow const&) = delete;

	bool IsRunning() const
	{
		std::lock_guard<std::mutex> lock{mutex};
		return running;
	}

	void Stop()
	{
		if (!IsRunning())
			return;
		Notify();

		/*PEG:*/std::this_thread::yield();

		if (thread.joinable())
			thread.join();

		/*PEG:*/std::this_thread::yield();
	}

private:
	void Notify()
	{
		std::lock_guard<std::mutex> lock{mutex};
		running = false;
	}

	static void EventLoop(SplashWindow& wnd);

	bool running = true;
	std::mutex mutable mutex;
	std::thread thread{ EventLoop, std::ref(*this) };
	SDL_Window* window;
};
#endif//Gothic_SplashWindow_H
