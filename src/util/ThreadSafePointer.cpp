/**** ThreadSafePointer Class Implementation ****/
template <class T>
ThreadSafePointer<T>::ThreadSafePointer() {
    //set up buffers

    auto init_buf = new T();

    cur_ptr_atomic.store(init_buf);
    temp_ptr_atomic.store(init_buf);
}

template <class T>
ThreadSafePointer<T>::~ThreadSafePointer() {
    auto temp_ptr = temp_ptr_atomic.load();
    auto cur_ptr = cur_ptr_atomic.load();
    if (temp_ptr != cur_ptr) {
        delete temp_ptr;
    }
    delete cur_ptr;
}

template <class T>
T* ThreadSafePointer<T>::load() {
    // load a pointer to the current
    return cur_ptr_atomic.load();
}

template <class T>
void ThreadSafePointer<T>::queue(T* new_ptr) {
    // called from background thread or editor thread
    auto temp_ptr = temp_ptr_atomic.load();
    auto cur_ptr = cur_ptr_atomic.load();
    temp_ptr_atomic.store(new_ptr);
    if (temp_ptr != cur_ptr) {
        delete temp_ptr;
    }
    
}

template <class T>
void ThreadSafePointer<T>::update() {
    // If a new buffer is ready, switch the pointers atomically
    // called from audio thread
    auto temp_ptr = temp_ptr_atomic.load();
    auto cur_ptr = cur_ptr_atomic.load();

    if (temp_ptr != cur_ptr) {
        std::cout << "ptr swapped" << std::endl;
        cur_ptr_atomic.store(temp_ptr);
        delete cur_ptr;
    }
}