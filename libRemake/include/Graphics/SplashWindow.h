#ifndef Gothic_SplashWindow_H
#define Gothic_SplashWindow_H
extern "C" {
typedef struct SDL_Window SDL_Window;
}

#include <thread>
#include <condition_variable>
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
		if (IsRunning()) {
			Notify();

			std::unique_lock<std::mutex> lk(mutex);
			cv.wait(lk, [&] { return done; });
		}

		if (thread.joinable())
			thread.join();
	}

private:
	void Notify()
	{
		std::lock_guard<std::mutex> lock{mutex};
		running = false;
	}

	void Done()
	{
		{
			std::lock_guard<std::mutex> lk{mutex};
			done = true;
		}
		cv.notify_one();
	}

	static void EventLoop(SplashWindow& wnd);

	bool running = true;

	std::mutex mutable mutex;
	std::thread thread{ EventLoop, std::ref(*this) };
	SDL_Window* window;

	bool done = false;
	std::condition_variable cv;
};
#endif//Gothic_SplashWindow_H
