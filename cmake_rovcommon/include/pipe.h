//
// Created by thm on 04/05/2021.
//

#pragma once

#include <atomic>
#include <cstdint>
#include <deque>
#include <log.h>
#include <mutex>
#include <vector>

namespace rov {


  template<class T>
  class fifo {
public:
    std::deque<T> fifo;
  };


  // Note: Made a three-way pipe with peek and benchmarked it.
  // This solution with std::move is by far the most efficient when it comes to wait times for the consumer.
  // Furthermore, this solution is the least complex. It's easy to see who owns the object at any given time.
  // This is not to say it can not be further optimized but if you think it's faster by peeking at references in
  // the middle of the pipe I will just inform you that I have tried it out with 2 different approaches with worse results than this.
  // Remember that all iterators are invalidated by emplace_back so if you use only one stream you need to maintain your own indexes etc.
  // This will lead to a lot of wait times with atomic index-counters or mutexes.
  // Just keep it simple and remember to use move when pushing and popping to limit the copy of data.

  template<class T>
  class pipe {
public:
    T faucet;

    bool empty() {
      return approxSize() == 0;
    }

    bool faucetReady() {
      if (clearBusy) {
        return false;
      }

      std::unique_lock<std::mutex> lckOutlet(mtxOutlet);

      if (streams[current(OUTLET)].fifo.empty()) {
        if (streams[current(INLET)].fifo.empty()) {
          return false;
        } else if (!swap()) {// Only ONE is swapping!
          return false;
        }
      }

      faucet = std::move(streams[current(OUTLET)].fifo.front());
      streams[current(OUTLET)].fifo.pop_front();
      return true;
    }

    void inlet(T data) {
      inletBusy = true;
      std::unique_lock<std::mutex> lckInlet(mtxInlet);
      streams[current(INLET)].fifo.push_back(std::move(data));// using push_back instead of emplace_back as the object is already created
      inletBusy = false;
    }

    T outlet() {
      return std::move(faucet);
    }

    size_t approxSize() {
      return streams[INLET].fifo.size() + streams[OUTLET].fifo.size();
    }


    void clear() {
      clearBusy = true;
      std::unique_lock<std::mutex> lckOutlet(mtxOutlet);
      std::unique_lock<std::mutex> lckInlet(mtxInlet);
      streams[INLET].fifo.clear();
      streams[OUTLET].fifo.clear();
      clearBusy = false;
    }


    bool isLockFree() {
      return std::atomic_is_lock_free(&inletBusy);
    };

private:
    inline static constexpr uint8_t INLET = 1;
    inline static constexpr uint8_t OUTLET = 0;
    inline static constexpr uint8_t DEFAULT = 1;

    [[nodiscard]] constexpr uint64_t current(const uint8_t direction) const { return ioSelect ^ direction; };

    bool swap() {
      if (inletBusy) {
        return false;
      }
      std::unique_lock<std::mutex> lckInlet(mtxInlet);
      ioSelect ^= DEFAULT;
      return true;
    }

    uint8_t ioSelect = DEFAULT;
    std::atomic_bool inletBusy = false;
    std::atomic_bool clearBusy = false;

    std::mutex mtxOutlet;
    std::mutex mtxInlet;
    fifo<T> streams[2];
  };


  template<class T>
  class repeatedPipe {
public:
    T next() {

      std::unique_lock<std::mutex> lckPipe(mtx);
      auto n = stream[readerIdx++];
      readerIdx %= stream.size();

      return std::move(n);
    }

    const T &peek() const {
      return stream[readerIdx];
    }

    bool isDisabled() {
      return disabled;
    }

    void add(T data) {
      stream.emplace_back(data);
    }

    [[nodiscard]] uint32_t size() const {
      return stream.size();
    }

    [[nodiscard]] uint32_t index() const {
      return readerIdx;
    }

    void lock(bool lockUnlock) {
      std::unique_lock<std::mutex> lckPipe(mtx);
      disabled = lockUnlock;
    }

    void clear() {
      std::unique_lock<std::mutex> lckPipe(mtx);
      disabled = true;
      readerIdx = false;
      stream.clear();
    }

    bool empty() {
      return stream.empty();
    }

private:
    std::atomic_bool disabled = true;
    uint32_t readerIdx = 0;
    std::mutex mtx;
    std::vector<T> stream;
  };

  /****************************************************************************
  *
  *       Vector version of pipe.
  *       Overall faster but not as consistent in fetch time as deque version
  *
  ***************************************************************************/

  template<class T>
  class fifoV {
public:
    std::vector<T> fifo;
  };
  template<class T>
  class pipeV {
public:
    T faucet;

    bool faucetReady() {
      if (clearBusy) {
        return false;
      }
      std::unique_lock<std::mutex> lckOutlet(mtxOutlet);
      if (outletIdx >= streams[current(OUTLET)].fifo.size()) {
        if (streams[current(INLET)].fifo.empty()) {
          return false;
        } else if (!swap()) {// Only ONE is swapping!
          return false;
        }
      }
      faucet = std::move(streams[current(OUTLET)].fifo.at(outletIdx++));
      return true;
    }

    void inlet(T data) {
      inletBusy = true;
      std::unique_lock<std::mutex> lckInlet(mtxInlet);
      //      rov::DBP("Add data " + std::to_string(approxSize()));
      streams[current(INLET)].fifo.push_back(std::move(data));// using push_back instead of emplace_back as the object is already created
      inletBusy = false;
    }

    T outlet() {
      //      rov::DBP("Remove data: " + std::to_string(approxSize()));
      return std::move(faucet);
    }

    size_t approxSize() {
      return streams[INLET].fifo.size() + streams[OUTLET].fifo.size();
    }


    void clear() {
      clearBusy = true;
      std::unique_lock<std::mutex> lckOutlet(mtxOutlet);
      std::unique_lock<std::mutex> lckInlet(mtxInlet);
      //      rov::DBP(rDye::red("Clear"));
      streams[INLET].fifo.clear();
      streams[OUTLET].fifo.clear();
      clearBusy = false;
    }

    bool isLockFree() {
      return std::atomic_is_lock_free(&inletBusy);
    };

private:
    inline static constexpr uint8_t INLET = 1;
    inline static constexpr uint8_t OUTLET = 0;
    inline static constexpr uint8_t DEFAULT = 1;

    [[nodiscard]] constexpr uint64_t current(const uint8_t direction) const { return ioSelect ^ direction; };

    bool swap() {
      if (inletBusy) {
        return false;
      }
      std::unique_lock<std::mutex> lckInlet(mtxInlet);
      //      rov::DBP(rDye::cyan("Swap"));
      streams[current(OUTLET)].fifo.clear();

      ioSelect ^= DEFAULT;
      outletIdx = 0;
      return true;
    }

    uint8_t ioSelect = DEFAULT;
    std::atomic_bool inletBusy = false;
    std::atomic_bool clearBusy = false;


    std::mutex mtxOutlet;
    std::mutex mtxInlet;

    size_t outletIdx = 0;
    fifoV<T> streams[2];
  };
}// namespace rov
