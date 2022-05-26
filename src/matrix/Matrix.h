#pragma once

class Modulator;
class PluginProcessor;
class DummyProcessor;
class NoteState;

#include <JuceHeader.h>
#include "ParameterDefines.h"
#include "ModulatorDefines.h"
// #include "PropertyDefines.h"
#include "../util/ThreadSafePointer.h"

/*
Matrix manages Parameters, Properties, Presets, Modulation, and Audio

  -> In other words, Matrix manages the plugin state.

*/

class Matrix : public juce::ValueTree::Listener {
  public:
    Matrix(PluginProcessor* proc);
    ~Matrix();

    void update_state(NoteState main_state);

    // Atomically Safe/Realtime safe Methods called on Audio Thread
    float paramValue(int param_id);
    float modulatorValue(int mod_id, NoteState note_state);
    float modulatedParamValue(int param_id, NoteState note_state);

    // State (might wanna replace this with wraped functions, like getParameterValue()?)
    juce::AudioProcessorValueTreeState* getParamTree();
    juce::AudioProcessorValueTreeState* getPropertyTree();
    juce::ValueTree getPresetTree();
    juce::ValueTree getMatrix();
    juce::ValueTree getAudioTree();
    juce::ValueTree get_state();

    // called on UI thread (This may need to be thread safe, but not realtime safe)
    Modulator* getModulator(int mod_id);
    juce::ValueTree getModulators(int param_id);
    std::unique_ptr<Modulator> create_modulator(int mod_id);
    
    // need to make these use an atomic pointer swap (for modulatedParamValue() call)
    // will that mess up undo? at least, connections should happen on Audio Thread?
    void connect(int mod_id, int param_id, float depth);
    void disconnect(int mod_id, int param_id);
    void disconnect_all(int param_id);

    // size_t get_state_size_without_audio();
    void save_preset(juce::String preset_name);
    void load_preset(juce::String preset_name);
    void load_from(juce::XmlElement* xml);
    void set_preset_name(juce::String preset_name);
    juce::String get_preset_name();

    // Needs to be atomically safe bc these objects are read on realtime, 
    // but APVTS should take care of that
    juce::RangedAudioParameter* get_parameter(int param_id);
    void set_parameter(int param_id, float value);
    void reset_parameter(int param_id);
    void init();

    // AUDIO
    void load_audio_file();
    void remove_audio_file();
    juce::AudioBuffer<float>* get_audio_buffer();

    // Thread Safety
    void audio_tree_changed();
    void modulation_matrix_changed();
    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    void valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;

    //LFO (we need to store the LFO UI points separately because we can create them in real time)
    juce::ValueTree get_lfo_points(int lfo_mod_id); 

    juce::UndoManager* getUndoManager();
    
    // constants
    const float DEFAULT_MOD_DEPTH{0.3};
    const int MAX_NUM_AUDIO_FILES{2};
    const juce::File PRESETS_DIR; // initialized in initializer list

  private:

    juce::ValueTree state;
    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts_ptr;
    std::unique_ptr<juce::AudioProcessorValueTreeState> property_tree_ptr;
    juce::ValueTree matrix;
    juce::ValueTree preset_tree;
    juce::ValueTree audio_tree;
    juce::ValueTree lfo_tree;

    // What happens if we write to this? Add a modulator, say by command line? 
    // We will probably need to store modulators serialized in a valueTree
    // and also thread safe. oof. 
    std::array<std::unique_ptr<Modulator>, TotalNumberModulators> modulators;

    // Undo Manager
    juce::UndoManager undo_manager;
    
    // necessary dummy processor for property_tree_ptr. maybe a better way to do this
    std::unique_ptr<DummyProcessor> plugin_processor_dummy;

    // file loading
    std::unique_ptr<juce::FileChooser> file_chooser;
    juce::AudioFormatManager format_manager;

    // Thread Safe Objects 
    ThreadSafePointer<juce::AudioBuffer<float>> audio_buffer;
    ThreadSafePointer<juce::ValueTree> read_only_matrix;
};