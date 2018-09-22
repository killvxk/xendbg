//
// Created by Spencer Michaels on 9/20/18.
//

#ifndef XENDBG_UVTIMER_HPP
#define XENDBG_UVTIMER_HPP

#include <functional>

#include <uv.h>

namespace xd::uv {

  class UVLoop;

  class UVTimer {
  public:
    using OnTickFn = std::function<bool()>;

    UVTimer(const UVLoop &loop);
    ~UVTimer();

    UVTimer(UVTimer&& other) = default;
    UVTimer(const UVTimer& other) = delete;
    UVTimer& operator=(UVTimer&& other) = default;
    UVTimer& operator=(const UVTimer& other) = delete;

    uv_timer_t *get() const { return _timer; };

    void start(OnTickFn on_tick, uint64_t interval, uint64_t initial_delay = 0);

  private:
    const UVLoop &_loop;
    uv_timer_t *_timer;
    OnTickFn _on_tick;
  };
}

#endif //XENDBG_UVTIMER_HPP