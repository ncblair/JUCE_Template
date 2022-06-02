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

    void update_state(NoteState* main_state);

    // Atomically Safe/Realtime safe Methods called on Audio Thread
    float paramValue(int param_id);
    float modulatorValue(int mod_id, NoteState* note_state);
    float modulatedParamValue(int param_id, NoteState* note_state, bool use_cache=false);
    void updateModulatorCache(NoteState* note_state);

    // State
    juce::AudioProcessorValueTreeState* getParamTree();
    juce::AudioProcessorValueTreeState* getPropertyTree();
    juce::ValueTree getPresetTree();
    juce::ValueTree getMatrix();
    juce::ValueTree getAudioTree();
    juce::ValueTree getLFOTree();
    juce::ValueTree get_state();
    

    // called on UI thread (This may need to be thread safe, but not realtime safe)
    Modulator* getModulator(int mod_id);
    juce::ValueTree getModulators(int param_id);
    std::unique_ptr<Modulator> create_modulator(int mod_id);
    
    // called from UI thread
    void connect(int mod_id, int param_id, float depth);
    void disconnect(int mod_id, int param_id);
    void disconnect_all(int param_id);

    // called from UI thread
    void save_preset(juce::String preset_name);
    void load_preset(juce::String preset_name);
    void load_from(juce::XmlElement* xml);
    void set_preset_name(juce::String preset_name);
    juce::String get_preset_name();

    // called from UI thread
    juce::RangedAudioParameter* get_parameter(int param_id);
    void set_parameter(int param_id, float value);
    void reset_parameter(int param_id);
    void init();

    // AUDIO from UI thread
    void load_audio_file();
    void remove_audio_file();
    // AUDIO from audio thread
    std::shared_ptr<juce::AudioBuffer<float>> get_audio_buffer();

    // Thread Safety (from UI thread)
    void audio_tree_changed();
    void modulation_matrix_changed();
    void lfo_tree_changed();
    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    void valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;

    //LFO (we need to store the LFO UI points separately because we can create them in real time)
    // void add_lfo_point(int mod_id, juce::Point<float> position);
    // void remove_lfo_point(int mod_id, int index);
    // void move_lfo_point(int mod_id, int index, juce::Point<float> new_position);
    std::shared_ptr<juce::ValueTree> get_read_only_lfo_tree();
    void init_lfo_tree();

    // CLI
    juce::String get_next_command(juce::String root);
    juce::String get_prev_command(juce::String root);
    juce::String enter_command(juce::String text);

    juce::UndoManager* getUndoManager();
    
    // constants
    const float DEFAULT_MOD_DEPTH{0.3};
    const int MAX_NUM_AUDIO_FILES{2};
    const juce::File PRESETS_DIR; // initialized in initializer list
    static const int NUM_VOICES{20};
    static const int CONTROL_RATE_SAMPLES{64};

    // const identifiers
    static inline const juce::Identifier MATRIX_ID{"MATRIX"};
    static inline const juce::Identifier PARAMETERS_ID{"PARAMETERS"};
    static inline const juce::Identifier PRESET_ID{"PRESET"};
    static inline const juce::Identifier PROPERTIES_ID{"PROPERTIES"};
    static inline const juce::Identifier AUDIO_ID{"AUDIO"};
    static inline const juce::Identifier STATE_ID{"STATE"};
    static inline const juce::Identifier AUDIO_NAME_ID{"AUDIO_NAME"};
    static inline const juce::Identifier AUDIO_NUM_CHANNELS_ID{"AUDIO_NUM_CHANNELS"};
    static inline const juce::Identifier AUDIO_NUM_SAMPLES_ID{"AUDIO_NUM_SAMPLES"};
    static inline const juce::Identifier AUDIO_DATA_ID{"AUDIO_DATA"};
    static inline const juce::Identifier MOD_ID_ID{"MOD_ID"};
    static inline const juce::Identifier MOD_DEPTH_ID{"MOD_DEPTH"};
    static inline const juce::Identifier MOD_INFO_ID{"MOD_INFO"};
    static inline const juce::Identifier PRESET_NAME_ID{"PRESET_NAME"};
    static inline const juce::Identifier LFO_TREE_ID{"LFO_TREE"};
    static inline const juce::Identifier LFO_HEIGHT_ID{"LFO_HEIGHT"};
    static inline const juce::Identifier LFO_CURVE_1_ID{"LFO_CURVE_1"};
    static inline const juce::Identifier LFO_POINT_ID{"LFO_POINT"};
    static inline const juce::Identifier LFO_POINT_X_ID{"LFO_POINT_X"};
    static inline const juce::Identifier LFO_POINT_Y_ID{"LFO_POINT_Y"};
    static inline const juce::Identifier LFO_POINT_CURVE_ID{"LFO_POINT_CURVE"};

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
    std::array<std::unique_ptr<Modulator>, MOD::TotalNumberModulators> modulators;
    std::array<float, MOD::TotalNumberModulators> modulator_value_cache;

    // Undo Manager
    juce::UndoManager undo_manager;
    
    // necessary dummy processor for property_tree_ptr. maybe a better way to do this
    std::unique_ptr<DummyProcessor> plugin_processor_dummy;

    // file loading
    std::unique_ptr<juce::FileChooser> file_chooser;
    juce::AudioFormatManager format_manager;

    // Thread Safe Objects 
    ThreadSafePointer<juce::AudioBuffer<float>> audio_buffer;
    // juce::AudioBuffer<float> temporary_audio_buffer;
    ThreadSafePointer<juce::ValueTree> read_only_matrix;
    ThreadSafePointer<juce::ValueTree> read_only_lfo_tree;
};