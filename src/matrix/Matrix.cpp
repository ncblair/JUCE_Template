#include "Matrix.h"
#include "../modulators/Modulator.h"
#include "../modulators/ADSRModulator.h"
#include "../modulators/LFOModulator.h"
#include "../modulators/NoteState.h"
#include "../plugin/PluginProcessor.h"
#include "../plugin/DummyProcessor.h"


Matrix::Matrix(PluginProcessor* proc) : 
    PRESETS_DIR(juce::File::getSpecialLocation(juce::File::SpecialLocationType::userMusicDirectory).getChildFile("nthn_plugins").getChildFile("template").getChildFile("presets"))
{
    //==============================================================================
    //-> ADD PARAMS/PROPERTIES
    //==============================================================================
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> properties;

    for (int p_id = 0; p_id < PARAM::TotalNumberParameters; ++p_id) {
        (PARAMETER_AUTOMATABLE[p_id] ? params : properties).push_back(
            std::make_unique<juce::AudioParameterFloat>(
                PARAMETER_NAMES[p_id],   // parameter ID
                PARAMETER_NAMES[p_id],   // parameter name
                PARAMETER_RANGES[p_id],  // range
                PARAMETER_DEFAULTS[p_id],// default value
                "", // parameter label (description?)
                juce::AudioProcessorParameter::Category::genericParameter,
                [this, p_id](float value, int maximumStringLength) { // Float to String Precision 2 Digits
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << value;
                    return juce::String(ss.str());
                },
                [this, p_id](juce::String text) {
                    return text.getFloatValue(); // Convert Back to Value
                }
            )
        );
    }

    apvts_ptr.reset(new juce::AudioProcessorValueTreeState(*proc, &undo_manager, PARAMETERS_ID, {params.begin(), params.end()}));

    plugin_processor_dummy = std::make_unique<DummyProcessor>();
    property_tree_ptr.reset(new juce::AudioProcessorValueTreeState(*plugin_processor_dummy, &undo_manager, PROPERTIES_ID, {properties.begin(), properties.end()}));

    //==============================================================================
    //-> Set Up Modulators + SETUP LFO TREE FOR DYNAMICALLY STORING LFO POINTS
    //==============================================================================
    lfo_tree = juce::ValueTree(LFO_TREE_ID);

    for (int m_id = 0; m_id < MOD::TotalNumberModulators; ++m_id) {
        modulators[m_id] = create_modulator(m_id);
    }
    modulator_value_cache.fill(0.0f);

    //==============================================================================
    //-> Set Up Matrix
    //==============================================================================

    matrix = juce::ValueTree(MATRIX_ID);
    for (int p_id = 0; p_id < PARAM::TotalNumberParameters; ++p_id) {
        if (PARAMETER_AUTOMATABLE[p_id]) {
            matrix.appendChild(juce::ValueTree(PARAMETER_IDS[p_id]), nullptr);
        }
    }

    //==============================================================================
    //-> SETUP PRESETS
    //==============================================================================
    preset_tree = juce::ValueTree(PRESET_ID);
    preset_tree.setProperty(PRESET_NAME_ID, "INIT", nullptr);

    //==============================================================================
    //-> SETUP AUDIO FILE TREE
    //==============================================================================
    
    audio_tree = juce::ValueTree(AUDIO_ID);
    format_manager.registerBasicFormats();

    //==============================================================================
    //-> SAFE BUFFER INIT and ADD LISTENERS
    //==============================================================================
    // queue cleared buffer so that no audio glitches play
    // auto new_buf = new juce::AudioBuffer<float>(1, 1);
    auto new_buf = std::make_shared<juce::AudioBuffer<float>>(1, 1);
    // temporary_audio_buffer = juce::AudioBuffer<float>(1, 1);
    new_buf->clear();
    audio_buffer.queue(new_buf);

    // Queue Valid Audio Tree
    // auto matrix_copy = std::make_shared<juce::ValueTree>(matrix.createCopy());
    // temporary_matrix_tree = juce::ValueTree(MATRIX_ID);
    read_only_matrix.queue(std::make_shared<juce::ValueTree>(matrix.createCopy()));

    // Queue initial LFO Tree
    read_only_lfo_tree.queue(std::make_shared<juce::ValueTree>(lfo_tree.createCopy()));

    audio_tree.addListener(this);
    matrix.addListener(this);
    lfo_tree.addListener(this);
}

Matrix::~Matrix(){

}

void Matrix::update_state(NoteState* main_state) {
    audio_buffer.update();
    read_only_matrix.update();
    read_only_lfo_tree.update();

    // This function updates the parameters associated with modulators based on modulator current state 
    // TODO: update modulators first and cache
    // We compute certain modulator values multiple times. 
    // We need to get the modulator value for every parameter
    // The modulators can be computed and cached, and then for each parameter accessed every time.
    updateModulatorCache(main_state);
    for (unsigned long m_id = 0; m_id < MOD::TotalNumberModulators; ++m_id) {
        modulators[m_id]->update_parameters(this, main_state);
    }
}

float Matrix::paramValue(int param_id) {
    // returns the parameter value of a certain ID in a thread safe way
    if (PARAMETER_AUTOMATABLE[param_id]) {
        return apvts_ptr->getRawParameterValue(PARAMETER_IDS[param_id])->load();
    }
    else {
        return property_tree_ptr->getRawParameterValue(PARAMETER_IDS[param_id])->load();
    }
}

Modulator* Matrix::getModulator(int mod_id) {
    return modulators[mod_id].get();
}

juce::ValueTree Matrix::getModulators(int param_id) {
    jassert (PARAMETER_AUTOMATABLE[param_id]);
    return matrix.getChildWithName(PARAMETER_IDS[param_id]);
}

std::unique_ptr<Modulator> Matrix::create_modulator(int mod_id) {
    if (MODULATOR_TYPES[mod_id] == MOD_TYPES::ADSR_MOD) {
        return std::make_unique<ADSRModulator>(MODULATOR_PARAMS[mod_id]);
    }
    else if (MODULATOR_TYPES[mod_id] == MOD_TYPES::LFO_MOD) {
        // add to the LFO tree (TODO: modulators should really be a value tree)
        auto lfo_sub_tree = juce::ValueTree(MODULATOR_IDS[mod_id]);
        lfo_sub_tree.setProperty(LFO_HEIGHT_ID, 0.0f, nullptr);
        lfo_sub_tree.setProperty(LFO_CURVE_1_ID, 1.0f, nullptr);
        auto lfo_point = juce::ValueTree(LFO_POINT_ID);
        lfo_point.setProperty(LFO_POINT_X_ID, 0.5f, nullptr);
        lfo_point.setProperty(LFO_POINT_Y_ID, 1.0f, nullptr);
        lfo_point.setProperty(LFO_POINT_CURVE_ID, 1.0f, nullptr);
        lfo_sub_tree.appendChild(lfo_point, nullptr);
        lfo_tree.appendChild(lfo_sub_tree, nullptr);
        set_parameter(MODULATOR_PARAMS[mod_id][LFO_PARAM::MOD_ID], mod_id);
        return std::make_unique<LFOModulator>(MODULATOR_PARAMS[mod_id]);
    }
}

float Matrix::modulatorValue(int mod_id, NoteState* note_state) {
    // returns the current modulator value
    auto result = modulators[mod_id]->get(note_state);
    return result;
}

float Matrix::modulatedParamValue(int param_id, NoteState* note_state, bool use_cache) {
    // returns the param value modulated by currently active modulators
    jassert (PARAMETER_AUTOMATABLE[param_id]);
    float v = 0.0f;
    juce::ValueTree mods = read_only_matrix.load()->getChildWithName(PARAMETER_IDS[param_id]);
    
    if (mods.getNumChildren() == 0) {
        return paramValue(param_id);
    }

    for (int i = 0; i < mods.getNumChildren(); ++i) {
        auto mod_node = mods.getChild(i);
        float mod_depth = mod_node.getProperty(MOD_DEPTH_ID);
        int mod_id = mod_node.getProperty(MOD_ID_ID);
        if (use_cache) {
            v += mod_depth * modulator_value_cache[mod_id];
        }
        else {
            v += mod_depth * modulatorValue(mod_id, note_state);
        }
        
    }

    auto param_range = PARAMETER_RANGES[param_id];
    auto param_value = paramValue(param_id);
    v = std::clamp(v + param_range.convertTo0to1(param_value), 0.0f, 1.0f);

    return param_range.convertFrom0to1(v);
}

void Matrix::updateModulatorCache(NoteState* note_state) {
    for (int m_id = 0; m_id < MOD::TotalNumberModulators; ++m_id) {
        modulator_value_cache[m_id] = modulatorValue(m_id, note_state);
    }
}

void Matrix::connect(int mod_id, int param_id, float depth) {
    jassert (PARAMETER_AUTOMATABLE[param_id]);
    
    // called from UI thread
    juce::ValueTree param_modulators = matrix.getChildWithName(PARAMETER_IDS[param_id]);
    bool already_connected = false;
    for (int i = 0; i < param_modulators.getNumChildren(); ++i) {
        auto mod_node = param_modulators.getChild(i);
        if (mod_node.isValid()) { // make sure node hasn't been disconnected asynchronously
            float mod_depth = mod_node.getProperty(MOD_DEPTH_ID);
            int modulator_id = mod_node.getProperty(MOD_ID_ID);

            if (modulator_id == mod_id) {
                already_connected = true;
                mod_node.setProperty(MOD_DEPTH_ID, depth, &undo_manager);
            }
        }
    }
    if (!already_connected) {
        auto new_modulator_connection = juce::ValueTree(MOD_INFO_ID);
        new_modulator_connection.setProperty(MOD_DEPTH_ID, depth, nullptr);
        new_modulator_connection.setProperty(MOD_ID_ID, mod_id, nullptr);
        param_modulators.appendChild(new_modulator_connection, &undo_manager);
    }
}

void Matrix::disconnect(int mod_id, int param_id) {
    jassert (PARAMETER_AUTOMATABLE[param_id]);

    std::cout << "DISCONNECT " << MODULATOR_NAMES[mod_id] << " FROM " << PARAMETER_NAMES[param_id] << std::endl;
    // called from UI Thread
    juce::ValueTree param_modulators = matrix.getChildWithName(PARAMETER_IDS[param_id]);
    param_modulators.removeChild(param_modulators.getChildWithProperty(MOD_ID_ID, mod_id), &undo_manager);
}

void Matrix::disconnect_all(int param_id) {
    jassert (PARAMETER_AUTOMATABLE[param_id]);

    auto mods = getModulators(param_id);
    while (mods.getNumChildren()) {
        auto mod_node = mods.getChild(0);
        int mod_id = mod_node.getProperty(MOD_ID_ID);
        disconnect(mod_id, param_id);
    }
}

juce::AudioProcessorValueTreeState* Matrix::getParamTree(){
    return apvts_ptr.get();
}

juce::AudioProcessorValueTreeState* Matrix::getPropertyTree(){
    return property_tree_ptr.get();
}

juce::ValueTree Matrix::getPresetTree() {
    return preset_tree;
}

juce::ValueTree Matrix::getMatrix() {
    return matrix;
}

juce::ValueTree Matrix::getAudioTree() {
    return audio_tree;
}

juce::ValueTree Matrix::getLFOTree() {
    return lfo_tree;
}

juce::ValueTree Matrix::get_state() {
    state = juce::ValueTree(STATE_ID);
    state.appendChild(apvts_ptr->copyState(), nullptr);
    state.appendChild(property_tree_ptr->copyState(), nullptr);
    state.appendChild(matrix.createCopy(), nullptr);
    state.appendChild(preset_tree.createCopy(), nullptr);
    state.appendChild(audio_tree.createCopy(), nullptr);
    state.appendChild(lfo_tree.createCopy(), nullptr);
    return state;
}

void Matrix::save_preset(juce::String preset_name) {
    // set_preset_name(preset_name);
    // not undo-able
    preset_tree.setProperty(PRESET_NAME_ID, preset_name, nullptr); 
    auto file = PRESETS_DIR.getChildFile(preset_name).withFileExtension(".template_preset");
    if (!PRESETS_DIR.exists()) {
        // create directory if it doesn't exist
        PRESETS_DIR.createDirectory();
    }
    if (!file.existsAsFile()) {
        // create file
        file.create();
    }
    std::unique_ptr<juce::XmlElement> xml (get_state().createXml());
    auto temp = juce::File::createTempFile("template_preset_temp");
    xml->writeTo(temp);
    temp.replaceFileIn(file);
}

void Matrix::load_preset(juce::String preset_name) {
    auto file = PRESETS_DIR.getChildFile(preset_name).withFileExtension(".template_preset");
    if (file.existsAsFile()) {
        std::unique_ptr<juce::XmlElement> xmlState = juce::XmlDocument::parse(file);
        load_from(xmlState.get());
    } 
}

void Matrix::load_from(juce::XmlElement* xml) {
    if (xml != nullptr){
        if (xml->hasTagName (STATE_ID)){
            auto new_tree = juce::ValueTree::fromXml(*xml);
            apvts_ptr->state.copyPropertiesAndChildrenFrom(new_tree.getChildWithName(PARAMETERS_ID), &undo_manager);
            property_tree_ptr->state.copyPropertiesAndChildrenFrom(new_tree.getChildWithName(PROPERTIES_ID), &undo_manager);
            matrix.copyPropertiesAndChildrenFrom(new_tree.getChildWithName(MATRIX_ID), &undo_manager);
            preset_tree.copyPropertiesFrom(new_tree.getChildWithName(PRESET_ID), &undo_manager);
            audio_tree.copyPropertiesFrom(new_tree.getChildWithName(AUDIO_ID), &undo_manager);
            lfo_tree.copyPropertiesAndChildrenFrom(new_tree.getChildWithName(LFO_TREE_ID), &undo_manager);
        }
    }
}

void Matrix::set_preset_name(juce::String preset_name) {
    preset_tree.setProperty(PRESET_NAME_ID, preset_name, &undo_manager);
}

juce::String Matrix::get_preset_name() {
    return preset_tree.getProperty(PRESET_NAME_ID);
}

juce::RangedAudioParameter* Matrix::get_parameter(int param_id) {
    if (PARAMETER_AUTOMATABLE[param_id]) {
        return apvts_ptr->getParameter(PARAMETER_IDS[param_id]);
    }
    else {
        return property_tree_ptr->getParameter(PARAMETER_IDS[param_id]);
    }
}

void Matrix::set_parameter(int param_id, float value) {
    auto parameter = get_parameter(param_id);
    auto range = PARAMETER_RANGES[param_id];
    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost(range.convertTo0to1(range.snapToLegalValue(value)));
    parameter->endChangeGesture();
}

void Matrix::reset_parameter(int param_id) {
    set_parameter(param_id, PARAMETER_DEFAULTS[param_id]);
}

void Matrix::init() {
    for (int i = 0; i < PARAM::TotalNumberParameters; ++i) {
        reset_parameter(i);
        if (PARAMETER_AUTOMATABLE[i]) {
            disconnect_all(i);
        }
        
    }
    set_preset_name("INIT");
    remove_audio_file();
    init_lfo_tree(); // must be called last bc it involves setting params
}

void Matrix::load_audio_file() {
    // adapted from https://docs.juce.com/master/tutorial_playing_sound_files.html
    file_chooser = std::make_unique<juce::FileChooser> ("Select a .wav file to load...", juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    file_chooser->launchAsync (chooserFlags, [this] (const FileChooser& fc)
    {
        auto file = fc.getResult();

        if (file != File{})
        {
            auto* reader = format_manager.createReaderFor(file);

            if (reader != nullptr)
            {
                const int num_channels = int(reader->numChannels);
                const int num_samples = int(reader->lengthInSamples);

                // get audio from file into an audio buffer
                auto audio = std::make_unique<juce::AudioBuffer<float>>(num_channels, num_samples);
                reader->read(audio.get(), 0, num_samples, 0, true, true);

                juce::MemoryBlock data = juce::MemoryBlock();

                data.setSize(num_samples * num_channels * sizeof(float));

                size_t offset = 0;
                for (int c = 0; c < num_channels; ++c) {
                    auto channel_info = audio->getReadPointer(c);
                    auto num_bytes = num_samples * sizeof(float);
                    data.copyFrom((void *) channel_info, offset, num_bytes);
                    offset += num_bytes;
                }

                audio_tree.setProperty(AUDIO_NAME_ID, file.getFileNameWithoutExtension(), &undo_manager);
                audio_tree.setProperty(AUDIO_NUM_CHANNELS_ID, num_channels, &undo_manager);
                audio_tree.setProperty(AUDIO_NUM_SAMPLES_ID, num_samples, &undo_manager);
                audio_tree.setProperty(AUDIO_DATA_ID, data, &undo_manager);
            }
        }
    });
}

void Matrix::remove_audio_file() {
    audio_tree.removeAllProperties(&undo_manager);
}

std::shared_ptr<juce::AudioBuffer<float>> Matrix::get_audio_buffer() {
    return audio_buffer.load();
}

void Matrix::audio_tree_changed() {
    if (audio_tree.hasProperty(AUDIO_NUM_CHANNELS_ID) && audio_tree.hasProperty(AUDIO_NUM_SAMPLES_ID) && audio_tree.hasProperty(AUDIO_DATA_ID)) {
        auto data = audio_tree.getProperty(AUDIO_DATA_ID).getBinaryData();
        auto num_channels = int(audio_tree.getProperty(AUDIO_NUM_CHANNELS_ID));
        auto num_samples = int(audio_tree.getProperty(AUDIO_NUM_SAMPLES_ID));
        if (data->getSize() == num_channels * num_samples * sizeof(float)) {
            auto new_buf = std::make_shared<juce::AudioBuffer<float>>(num_channels, num_samples);
            auto data_ptr = (float *) data->begin();
            for (int c = 0; c < num_channels; ++c) {
                new_buf->copyFrom(c, 0, (num_samples * c) + data_ptr, num_samples);
            }
            audio_buffer.queue(new_buf);
        }
    }
    else if (audio_tree.getNumProperties() == 0) {
        auto new_buf = std::make_shared<juce::AudioBuffer<float>>(1, 1);
        audio_buffer.queue(new_buf);
    }
}

void Matrix::modulation_matrix_changed() {
    // // auto matrix_copy = new juce::ValueTree(MATRIX_ID);
    // auto matrix_copy = std::make_shared<juce::ValueTree>(MATRIX_ID);
    // // temporary_matrix_tree = juce::ValueTree(MATRIX_ID);
    // matrix_copy->copyPropertiesAndChildrenFrom(matrix, nullptr);
    // read_only_matrix.queue(matrix_copy);
    read_only_matrix.queue(std::make_shared<juce::ValueTree>(matrix.createCopy()));
}

void Matrix::lfo_tree_changed() {
    read_only_lfo_tree.queue(std::make_shared<juce::ValueTree>(lfo_tree.createCopy()));
}

void Matrix::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) {
    if (treeWhosePropertyHasChanged.getRoot().hasType(AUDIO_ID)) {
        audio_tree_changed();
    }
    else if (treeWhosePropertyHasChanged.getRoot().hasType(MATRIX_ID)) {
        modulation_matrix_changed();
    }
    else if (treeWhosePropertyHasChanged.getRoot().hasType(LFO_TREE_ID)) {
        lfo_tree_changed();
    }
}

void Matrix::valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded) {
    if (parentTree.getRoot().hasType(MATRIX_ID)) {
        modulation_matrix_changed();
    }
    else if (parentTree.getRoot().hasType(LFO_TREE_ID)) {
        lfo_tree_changed();
        std::cout << "LFO Point Added" << std::endl;
    }
}

void Matrix::valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) {
    if (parentTree.getRoot().hasType(MATRIX_ID)) {
        modulation_matrix_changed();
    }
    else if (parentTree.getRoot().hasType(LFO_TREE_ID)) {
        lfo_tree_changed();
    }
}

// void Matrix::add_lfo_point(int mod_id, juce::Point<float> position) {
//     jassert(MODULATOR_TYPES[mod_id] == MOD_TYPES::LFO_MOD);
//     auto lfo_sub_tree = lfo_tree.getChildWithName(MODULATOR_IDS[mod_id]);
//     auto lfo_point = juce::ValueTree(LFO_POINT_ID);
//     auto x = position.getX();
//     auto y = position.getY();
//     std::cout << x << " " << y << std::endl;
//     lfo_point.setProperty(LFO_POINT_X_ID, x, nullptr);
//     lfo_point.setProperty(LFO_POINT_Y_ID, y, nullptr);
//     lfo_point.setProperty(LFO_POINT_CURVE_ID, 1.0f, nullptr);
//     auto index = 0;
//     while (index < lfo_sub_tree.getNumChildren() && x > float(lfo_sub_tree.getChild(index).getProperty(LFO_POINT_X_ID))) {
//         index++;
//     }
//     std::cout << index << std::endl;
//     lfo_sub_tree.addChild(lfo_point, index, &undo_manager);
// }

// void Matrix::remove_lfo_point(int mod_id, int index) {
//     jassert(MODULATOR_TYPES[mod_id] == MOD_TYPES::LFO_MOD);
//     auto lfo_sub_tree = lfo_tree.getChildWithName(MODULATOR_IDS[mod_id]);
//     lfo_sub_tree.removeChild(index, &undo_manager);
// }

// void Matrix::move_lfo_point(int mod_id, int index, juce::Point<float> new_position) {
//     jassert(MODULATOR_TYPES[mod_id] == MOD_TYPES::LFO_MOD);
//     auto lfo_sub_tree = lfo_tree.getChildWithName(MODULATOR_IDS[mod_id]);
//     auto lfo_point = lfo_sub_tree.getChild(index);
//     // lfo_point.setProperty();
//     //TODO: fill this in?
// }

std::shared_ptr<juce::ValueTree> Matrix::get_read_only_lfo_tree() {
    return read_only_lfo_tree.load();
}

void Matrix::init_lfo_tree() {
    auto new_lfo_tree = juce::ValueTree(LFO_TREE_ID);

    for (int mod_id = 0; mod_id < TotalNumberModulators; ++mod_id) {
        if (MODULATOR_TYPES[mod_id] == MOD_TYPES::LFO_MOD) {
            auto lfo_sub_tree = juce::ValueTree(MODULATOR_IDS[mod_id]);
            lfo_sub_tree.setProperty(LFO_HEIGHT_ID, 0.0f, nullptr);
            lfo_sub_tree.setProperty(LFO_CURVE_1_ID, 1.0f, nullptr);
            auto lfo_point = juce::ValueTree(LFO_POINT_ID);
            lfo_point.setProperty(LFO_POINT_X_ID, 0.5f, nullptr);
            lfo_point.setProperty(LFO_POINT_Y_ID, 1.0f, nullptr);
            lfo_point.setProperty(LFO_POINT_CURVE_ID, 1.0f, nullptr);
            lfo_sub_tree.appendChild(lfo_point, nullptr);
            new_lfo_tree.appendChild(lfo_sub_tree, nullptr);
            set_parameter(MODULATOR_PARAMS[mod_id][LFO_PARAM::MOD_ID], mod_id);
        }
    }
    lfo_tree.copyPropertiesAndChildrenFrom(new_lfo_tree, &undo_manager);
}

juce::String Matrix::get_next_command(juce::String root) {
    return "";
}
juce::String Matrix::get_prev_command(juce::String root) {
    return "";
}
juce::String Matrix::enter_command(juce::String text) {
    return "command entered";
}

juce::UndoManager* Matrix::getUndoManager() {
    return &undo_manager;
}