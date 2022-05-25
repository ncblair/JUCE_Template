#include "SafeBuffer.h"
/**** SafeBuffer Class Implementation ****/

SafeBuffer::SafeBuffer(int channels, int samples) {
    //set up buffers
    // buf_1.setSize(num_channels, num_samples);
    // buf_2.setSize(num_channels, num_samples);
    // buf_1.clear();
    // buf_2.clear();

    auto init_buf = new juce::AudioBuffer<float>(channels, samples);
    init_buf->clear();

    cur_buf_ptr_atomic.store(init_buf);
    temp_buf_ptr_atomic.store(init_buf);

    // num_channels.store(channels);
    // num_samples.store(samples);

    // ready_to_update.store(false);
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

// int SafeBuffer::getNumSamples() {
//     // get number of samples
//     return num_samples.load();
// }

// int SafeBuffer::getNumChannels() {
//     // get number of channels
//     return num_channels.load();
// }

void SafeBuffer::queue_new_buffer(juce::AudioBuffer<float>* new_buffer) {
    // called from background thread or editor thread
    // while (ready_to_update.load());
    auto temp_ptr = temp_buf_ptr_atomic.load();
    auto cur_ptr = cur_buf_ptr_atomic.load();
    temp_buf_ptr_atomic.store(new_buffer);
    if (temp_ptr != cur_ptr) {
        delete temp_ptr;
    }
    
    // ready_to_update.store(true);
}

void SafeBuffer::update() {
    // If a new buffer is ready, switch the pointers atomically
    auto temp_ptr = temp_buf_ptr_atomic.load();
    auto cur_ptr = cur_buf_ptr_atomic.load();
    // if (ready_to_update.load()) {
    if (temp_ptr != cur_ptr) {
        std::cout << "buffer swapped" << std::endl;
        cur_buf_ptr_atomic.store(temp_ptr);
        delete cur_ptr;
        // num_samples.store(temp_ptr->getNumSamples());
        // num_channels.store(temp_ptr->getNumChannels());

        // ready_to_update.store(false);
    }
}