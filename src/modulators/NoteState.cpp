#include "NoteState.h"
#include <JuceHeader.h>

double NoteState::get_time() const {
    return ms_elapsed;
}
double NoteState::get_release_time() const {
    return release_time;
}
bool NoteState::is_released() const {
    return ms_elapsed > release_time;
}

int NoteState::get_voice_id() const {
    return voice_id;
}


double NoteState::get_velocity() const {
    return velocity;
}

double NoteState::get_frequency() const {
    return frequency;
}

// double NoteState::get_dt() const {
//     return delta_time;
// }

void NoteState::set_time(double time_ms) {
    // delta_time = time_ms - ms_elapsed;
    ms_elapsed = time_ms;
}
void NoteState::set_release_time(double release_time_ms) {
    release_time = release_time_ms;
}

void NoteState::set_voice_id(int id) {
    voice_id = id;
}

void NoteState::release() {
    release_time = ms_elapsed;
}
void NoteState::mark_unreleased() {
    release_time = std::numeric_limits<double>::max();
}

void NoteState::set_frequency(double new_freq) {
    frequency = new_freq;
}

void NoteState::set_velocity(double new_vel) {
    velocity = new_vel;
}

void NoteState::increment_time(int num_samples, double sample_rate) {
    // delta_time = double(num_samples)*1000.0 / sample_rate;
    ms_elapsed += double(num_samples)*1000.0 / sample_rate;
}