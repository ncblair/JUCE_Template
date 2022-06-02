/**** ThreadSafePointer Class Implementation ****/
template <class T>
ThreadSafePointer<T>::ThreadSafePointer() {
    //set up buffers

    auto init_buf = std::make_shared<T>();
    std::atomic_store(&m_cur_ptr, init_buf);
    std::atomic_store(&m_temp_ptr, init_buf);
}

template <class T>
ThreadSafePointer<T>::~ThreadSafePointer() {
}

template <class T>
std::shared_ptr<T> ThreadSafePointer<T>::load() {
    // load a pointer to the current
    return std::atomic_load(&m_cur_ptr);
    // return cur_ptr_atomic.load();
}

template <class T>
void ThreadSafePointer<T>::queue(std::shared_ptr<T> new_ptr) {
    // called from background thread or editor thread
    std::atomic_store(&m_temp_ptr, new_ptr);
}

template <class T>
void ThreadSafePointer<T>::update() {
    // If a new buffer is ready, switch the pointers atomically
    // called from audio thread
    std::atomic_store(&m_cur_ptr, std::atomic_load(&m_temp_ptr));
}