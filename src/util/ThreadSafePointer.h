#pragma once 
#include <JuceHeader.h>

template <class T>
class ThreadSafePointer {
  public:
    ThreadSafePointer();
    ~ThreadSafePointer();
    // editor thread
    void queue(T* new_ptr);
    // audio thread
    void update();

    // returns a pointer to the current
    T* load();

  private:
    // an atomic pointer to the current
    std::atomic<T*> cur_ptr_atomic;
    // an atomic pointer to the temporary
    std::atomic<T*> temp_ptr_atomic;
};

#include "ThreadSafePointer.cpp"