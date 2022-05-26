#include "SafeBuffer.h"
/**** SafeBuffer Class Implementation ****/

SafeBuffer::SafeBuffer(int channels, int samples) {
    //set up buffers

    auto init_buf = new juce::AudioBuffer<float>(channels, samples);
    init_buf->clear();

    cur_buf_ptr_atomic.store(init_buf);
    temp_buf_ptr_atomic.store(init_buf);
}

SafeBuffer::~SafeBuffer() {
    auto temp_ptr = temp_buf_ptr_atomic.load();
    auto cur_ptr = cur_buf_ptr_atomic.load();
    if (temp_ptr != cur_ptr) {
        delete temp_ptr;
    }
    delete cur_ptr;
}

juce::AudioBuffer<float>* SafeBuffer::load() {
    // load a pointer to the current buffer
    return cur_buf_ptr_atomic.load();
}

void SafeBuffer::queue_new_buffer(juce::AudioBuffer<float>* new_buffer) {
    // called from background thread or editor thread
    auto temp_ptr = temp_buf_ptr_atomic.load();
    auto cur_ptr = cur_buf_ptr_atomic.load();
    temp_buf_ptr_atomic.store(new_buffer);
    if (temp_ptr != cur_ptr) {
        delete temp_ptr;
    }
    
}

void SafeBuffer::update() {
    // If a new buffer is ready, switch the pointers atomically
    // called from audio thread
    auto temp_ptr = temp_buf_ptr_atomic.load();
    auto cur_ptr = cur_buf_ptr_atomic.load();

    if (temp_ptr != cur_ptr) {
        std::cout << "buffer swapped" << std::endl;
        cur_buf_ptr_atomic.store(temp_ptr);
        delete cur_ptr;
    }
}