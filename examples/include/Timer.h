#ifndef MOVETK_EXAMPLES_TIMER_H
#define MOVETK_EXAMPLES_TIMER_H
#include <chrono>

class Timer {
public:
	using time_t = std::decay_t<decltype(std::chrono::high_resolution_clock::now())>;
	Timer(bool auto_start = false) {
		if (auto_start) {
			start();
		}
	}
	void start() {
		if (m_is_running) {
			throw std::runtime_error("Timer already running");
		}
		m_is_running = true;
		m_start = std::chrono::high_resolution_clock::now();
	}
	void stop() {
		m_is_running = false;
		m_end = std::chrono::high_resolution_clock::now();
	}
	friend std::ostream &operator<<(std::ostream &stream, const Timer &timer) {
		stream << timer.elapsed_ns() << " ns";
		return stream;
	}

	long long elapsed_ns() const { return (m_end - m_start).count(); }

private:
	time_t m_start;
	time_t m_end;
	bool m_is_running = false;
};

#endif