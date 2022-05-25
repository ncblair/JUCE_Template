#pragma once

class Modulator;
class PluginProcessor;
class DummyProcessor;
class NoteState;
// class SafeBuffer;

#include <JuceHeader.h>
#include "ParameterDefines.h"
#include "ModulatorDefines.h"
#include "PropertyDefines.h"
#include "../../util/SafeBuffer.h"

/*
Matrix manages Parameters, Properties, Presets, Modulation, and Audio

  -> In other words, Matrix manages the plugin state.

*/

class Matrix : public juce::ValueTree::Listener {
  public:
    Matrix(PluginProcessor* proc);
    ~Matrix();
    // Atomically Safe/Realtime safe Methods called on Audio Thread
    float paramValue(int param_id);
    float propertyValue(int property_id);
    // ADSR needs ms_elapsed, release_time. LFO needs ms_elapsed (either global or local, depending on trigger mode)
    // Velocity modulator needs a note_velocity. // Note modulator needs note_number (or frequency for mpe?)
    // random modulators needs to know note number and maybe ms_elapsed. 
    // each note could have a noteState valueTree or just even a struct
    float modulatorValue(int mod_id, NoteState note_state);
    float modulatedParamValue(int param_id, NoteState note_state);
    void update_modulator_parameters(NoteState main_state);

    // State (might wanna replace this with wraped functions, like getParameterValue()?)
    juce::AudioProcessorValueTreeState* getParamTree();
    juce::AudioProcessorValueTreeState* getPropertyTree();
    juce::ValueTree getPresetTree();
    juce::ValueTree getMatrix();
    juce::ValueTree getAudioTree();

    // called on UI thread (This may need to be thread safe, but not realtime safe)
    Modulator* getModulator(int mod_id);
    juce::ValueTree getModulators(int param_id);
    juce::String get_preset_name();
    
    // need to make these use an atomic pointer swap (for modulatedParamValue() call)
    // will that mess up undo?
    void connect(int mod_id, int param_id, float depth);
    void disconnect(int mod_id, int param_id);
    void disconnect_all(int param_id);

    // juce::NormalisableRange<float> propertyRange(int property_id);
    juce::UndoManager* getUndoManager();

    // void reload_state(); // must be called to correctly load
    // need to do undo-able state switches
    // can use ValueTree copyPropertiesAndChildrenFrom (const ValueTree &source, UndoManager *undoManager)
    // can use APVTS replaceState()
    juce::ValueTree get_state();
    // size_t get_state_size_without_audio();
    void save_preset(juce::String preset_name);
    void load_preset(juce::String preset_name);
    void load_from(juce::XmlElement* xml);
    void set_preset_name(juce::String preset_name);

    // Needs to be atomically safe bc these objects are read on realtime, 
    // but APVTS should take care of that
    void set_parameter(juce::String id, float value);
    void set_property(juce::String id, float value);
    void reset_parameter(int param_id);
    void reset_property(int param_id);
    void init();

    // AUDIO
    void load_audio_file();
    void queue_buffer_from_value_tree();
    void remove_audio_file();
    juce::AudioBuffer<float>* get_audio_buffer();
    void update_audio_buffers();
    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    
    // constants
    const float DEFAULT_MOD_DEPTH{0.3};
    const int MAX_NUM_AUDIO_FILES{2};
    // constant file that points to the directory we store presets in
    const juce::File PRESETS_DIR; // initialized in initializer list

  private:

    juce::ValueTree state;
    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts_ptr;
    std::unique_ptr<juce::AudioProcessorValueTreeState> property_tree_ptr;
    juce::ValueTree matrix;
    juce::ValueTree preset_tree;
    juce::ValueTree audio_tree;

    // What happens if we write to this? Add a modulator, say by command line? 
    // We will probably need to store modulators serialized in a valueTree
    // and also thread safe. oof. 
    std::array<std::unique_ptr<Modulator>, TotalNumberModulators> modulators;

    // Undo Manager
    juce::UndoManager undo_manager;
    // std::atomic<bool> performing_transaction;
    
    // necessary dummy processor for property_tree_ptr. maybe a better way to do this
    std::unique_ptr<DummyProcessor> plugin_processor_dummy;

    // file loading
    std::unique_ptr<juce::FileChooser> file_chooser;
    juce::AudioFormatManager format_manager;

    //Audio 
    // TODO: Turn this into array maybe? Maybe number of buffers is a param? Maybe not
    SafeBuffer audio_buffer;
};