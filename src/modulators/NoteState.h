#pragma once

#include <JuceHeader.h>

class NoteState {
  public:
    // NoteState();

    double get_time() const;
    double get_release_time() const;
    double get_frequency() const;
    int get_voice_id() const;
    bool is_released() const;
    double get_velocity() const;
    // double get_dt() const;
    
    void set_time(double time_ms);
    void set_release_time(double release_time_ms);
    void set_voice_id(int id);
    void release();
    void mark_unreleased();
    void set_frequency(double new_freq);
    void set_velocity(double new_vel);
    void increment_time(int num_samples, double sample_rate);

  private:
    double ms_elapsed{0.0};
    // double delta_time{0.0};
    double release_time{std::numeric_limits<double>::max()};
    double frequency{440.0f};

    double velocity{0.5}; // 0 to 1
    int mod_wheel{63};
    int pressure{63};
    int slide{63};
    int lift{63};
    int voice_id{0};
};