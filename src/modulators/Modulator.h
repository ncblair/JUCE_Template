#pragma once

// forward declarations
class Matrix;
class NoteState;

class Modulator {
  public:
    virtual ~Modulator() {}
    virtual float get(NoteState* note_state) = 0;
    virtual void update_parameters(Matrix* matrix, NoteState* main_state) = 0;
    virtual float get_parameter(int param_id) = 0;
};