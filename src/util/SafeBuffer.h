#pragma once 

#include <JuceHeader.h>

class SafeBuffer {
  public:
    SafeBuffer(int channels=1, int samples=24000);
    ~SafeBuffer();
    // editor thread
    void queue_new_buffer(juce::AudioBuffer<float>* new_buffer);
    // audio thread
    void update();

    // returns a pointer to the current buffer
    juce::AudioBuffer<float>* load();

    // int getNumSamples();
    // int getNumChannels();

  private:
    // std::atomic<int> num_samples{0};
    // std::atomic<int> num_channels{0};

    // an atomic pointer to the current buffer
    std::atomic<juce::AudioBuffer<float>*> cur_buf_ptr_atomic;
    // an atomic pointer to the temporary buffer
    std::atomic<juce::AudioBuffer<float>*> temp_buf_ptr_atomic;
    // two grain buffers. One is always temporary, one is always active
    // juce::AudioBuffer<float> buf_1;
    // juce::AudioBuffer<float> buf_2;
    // whether there is a buffer queued (i.e. ready to update/swap pointers)
    // std::atomic<bool> ready_to_update{false};
};
