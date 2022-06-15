#pragma once 
#include <JuceHeader.h>

// This Class will LEAD TO MEMORY LEAKS if not managed properly
template <class T>
class ThreadSafePointer {
  public:
    ThreadSafePointer();
    ~ThreadSafePointer();
    // editor thread
    void queue(std::shared_ptr<T> new_ptr);
    // audio thread
    void update();

    // returns a pointer to the current
    std::shared_ptr<T> load();

  private:
    // an atomic pointer to the current
    std::shared_ptr<T> m_cur_ptr;
    // an atomic pointer to the temporary
    std::shared_ptr<T> m_temp_ptr;
};

#include "ThreadSafePointer.cpp"