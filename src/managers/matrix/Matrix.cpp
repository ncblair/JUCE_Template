#include "Matrix.h"
#include "../../modulators/Modulator.h"
#include "../../modulators/ADSRModulator.h"
#include "../../modulators/NoteState.h"
#include "../../plugin/PluginProcessor.h"
#include "../../plugin/DummyProcessor.h"


Matrix::Matrix(PluginProcessor* proc) : 
    PRESETS_DIR(juce::File::getSpecialLocation(juce::File::SpecialLocationType::userMusicDirectory).getChildFile("nthn_plugins").getChildFile("template").getChildFile("presets"))
{
    //==============================================================================
    //-> ADD PARAMS
    //==============================================================================
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    for (int i = 0; i < TotalNumberParameters; ++i) {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
                PARAMETER_NAMES[i],   // parameter ID
                PARAMETER_NAMES[i],   // parameter name
                PARAMETER_RANGES[i],  // range
                PARAMETER_DEFAULTS[i],// default value
                "", // parameter label (description?)
                juce::AudioProcessorParameter::Category::genericParameter,
                [this, i](float value, int maximumStringLength) { // Float to String Precision 2 Digits
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << value;
                    return juce::String(ss.str());
                },
                [this, i](juce::String text) {
                    return text.getFloatValue(); // Convert Back to Value
                }
            )
        );
    }

    apvts_ptr.reset(new juce::AudioProcessorValueTreeState(*proc,
                                                        &undo_manager,
                                                        "PARAMETERS",
                                                        { params.begin(), params.end() }));

    //==============================================================================
    //-> Set Up Modulators
    //==============================================================================
    for (int m_id = 0; m_id < TotalNumberModulators; ++m_id) {
        modulators[m_id] = std::make_unique<ADSRModulator>(
            &MODULATOR_PARAMS[m_id]
        );
    }

    //==============================================================================
    //-> Set Up Matrix
    //==============================================================================
    matrix = juce::ValueTree("MATRIX");
    for (int p_id = 0; p_id < TotalNumberParameters; ++p_id) {
        matrix.appendChild(juce::ValueTree(PARAMETER_NAMES[p_id]), nullptr);
    }

    //==============================================================================
    //-> ADD PROPERTIES
    //==============================================================================
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> properties;

    for (int i = 0; i < TotalNumberProperties; ++i) {
        properties.push_back(std::make_unique<juce::AudioParameterFloat>(
                PROPERTY_NAMES[i],   // parameter ID
                PROPERTY_NAMES[i],   // parameter name
                PROPERTY_RANGES[i],  // range
                PROPERTY_DEFAULTS[i],// default value
                "", // parameter label (description?)
                juce::AudioProcessorParameter::Category::genericParameter,
                [this, i](float value, int maximumStringLength) { // Float to String Precision 2 Digits
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << value;
                    return juce::String(ss.str());
                },
                [this, i](juce::String text) {
                    return text.getFloatValue(); // Convert Back to Value
                }
            )
        );
    }

    plugin_processor_dummy = std::make_unique<DummyProcessor>();

    property_tree_ptr.reset(new juce::AudioProcessorValueTreeState(*plugin_processor_dummy,
                                                        &undo_manager,
                                                        "PROPERTIES",
                                                        { properties.begin(), properties.end() }));

    //==============================================================================
    //-> SETUP PRESETS
    //==============================================================================
    preset_tree = juce::ValueTree("PRESET");
    preset_tree.setProperty("NAME", "INIT", nullptr);

    //==============================================================================
    //-> SETUP AUDIO FILE TREE
    //==============================================================================
    
    audio_tree = juce::ValueTree("AUDIO");
    audio_tree.addListener(this);
    format_manager.registerBasicFormats();
}

Matrix::~Matrix(){

}

float Matrix::paramValue(int param_id) {
    // returns the parameter value of a certain ID in a thread safe way
    return apvts_ptr->getRawParameterValue(PARAMETER_NAMES[param_id])->load();
}

Modulator* Matrix::getModulator(int mod_id) {
    return modulators[mod_id].get();
}

juce::ValueTree Matrix::getModulators(int param_id) {
    return matrix.getChildWithName(PARAMETER_NAMES[param_id]);
}

float Matrix::modulatorValue(int mod_id, NoteState note_state) {
    // returns the current modulator value
    return modulators[mod_id]->get(note_state);
}

float Matrix::modulatedParamValue(int param_id, NoteState note_state) {
    // returns the param value modulated by currently active modulators
    float v = 0.0f;
    juce::ValueTree mods = matrix.getChildWithName(PARAMETER_NAMES[param_id]);
    for (int i = 0; i < mods.getNumChildren(); ++i) {
        auto mod_node = mods.getChild(i);
        float mod_depth = mod_node.getProperty("DEPTH");
        int mod_id = mod_node.getProperty("MOD_ID");

        v += mod_depth * modulatorValue(mod_id, note_state);
    }

    auto param_range = PARAMETER_RANGES[param_id];
    auto param_value = paramValue(param_id);
    v = std::clamp(v + param_range.convertTo0to1(param_value), 0.0f, 1.0f);
    return param_range.convertFrom0to1(v);
}

void Matrix::connect(int mod_id, int param_id, float depth) {
    // TODO: Do this on the UI Thread and MAKE THREAD SAFE
    juce::ValueTree param_modulators = matrix.getChildWithName(PARAMETER_NAMES[param_id]);
    bool already_connected = false;
    for (int i = 0; i < param_modulators.getNumChildren(); ++i) {
        auto mod_node = param_modulators.getChild(i);
        float mod_depth = mod_node.getProperty("DEPTH");
        int modulator_id = mod_node.getProperty("MOD_ID");

        if (modulator_id == mod_id) {
            already_connected = true;
            mod_node.setProperty("DEPTH", depth, &undo_manager);
        }
    }
    if (!already_connected) {
        auto new_modulator_connection = juce::ValueTree("MOD_INFO");
        new_modulator_connection.setProperty("DEPTH", depth, nullptr);
        new_modulator_connection.setProperty("MOD_ID", mod_id, nullptr);
        param_modulators.appendChild(new_modulator_connection, &undo_manager);
    }
}

void Matrix::disconnect(int mod_id, int param_id) {
    std::cout << "DISCONNECT " << MODULATOR_NAMES[mod_id] << " FROM " << PARAMETER_NAMES[param_id] << std::endl;
    // TODO: Do this on the UI Thread and MAKE THREAD SAFE
    juce::ValueTree param_modulators = matrix.getChildWithName(PARAMETER_NAMES[param_id]);
    param_modulators.removeChild(param_modulators.getChildWithProperty("MOD_ID", mod_id), &undo_manager);
}

void Matrix::disconnect_all(int param_id) {
    auto mods = getModulators(param_id);
    while (mods.getNumChildren()) {
        auto mod_node = mods.getChild(0);
        int mod_id = mod_node.getProperty("MOD_ID");
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

void Matrix::update_modulator_parameters(NoteState main_state) {
    // This function updates the parameters associated with modulators based on modulator current state 
    // TODO: update modulators first and cache
    // We compute certain modulator values multiple times. 
    // We need to get the modulator value for every parameter
    // The modulators can be computed and cached, and then for each parameter accessed every time.
    for (unsigned long m_id = 0; m_id < TotalNumberModulators; ++m_id) {
        modulators[m_id]->update_parameters(this, main_state);
    }
}

float Matrix::propertyValue(int property_id) {
    return property_tree_ptr->getRawParameterValue(PROPERTY_NAMES[property_id])->load();
}

juce::UndoManager* Matrix::getUndoManager() {
    return &undo_manager;
}

void Matrix::save_preset(juce::String preset_name) {
    // set_preset_name(preset_name);
    // not undo-able
    preset_tree.setProperty("NAME", preset_name, nullptr); 
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
        if (xml->hasTagName ("STATE")){
            auto new_tree = juce::ValueTree::fromXml(*xml);
            apvts_ptr->state.copyPropertiesAndChildrenFrom(new_tree.getChildWithName("PARAMETERS"), &undo_manager);
            property_tree_ptr->state.copyPropertiesAndChildrenFrom(new_tree.getChildWithName("PROPERTIES"), &undo_manager);
            matrix.copyPropertiesAndChildrenFrom(new_tree.getChildWithName("MATRIX"), &undo_manager);
            preset_tree.copyPropertiesFrom(new_tree.getChildWithName("PRESET"), &undo_manager);
            audio_tree.copyPropertiesFrom(new_tree.getChildWithName("AUDIO"), &undo_manager);
        }
    }
}

juce::String Matrix::get_preset_name() {
    return preset_tree.getProperty("NAME");
}

void Matrix::set_preset_name(juce::String preset_name) {
    preset_tree.setProperty("NAME", preset_name, &undo_manager);
}

void Matrix::set_parameter(juce::String id, float value) {
    auto parameter = apvts_ptr->getParameter(id);
    auto range = apvts_ptr->getParameterRange(id);
    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost(range.convertTo0to1(range.snapToLegalValue(value)));
    parameter->endChangeGesture();
}

void Matrix::set_property(juce::String id, float value) {
    auto property = property_tree_ptr->getParameter(id);
    auto range = property_tree_ptr->getParameterRange(id);
    property->beginChangeGesture();
    property->setValueNotifyingHost(range.convertTo0to1(range.snapToLegalValue(value)));
    property->endChangeGesture();
}

void Matrix::reset_parameter(int param_id) {
    set_parameter(PARAMETER_NAMES[param_id], PARAMETER_DEFAULTS[param_id]);
}

void Matrix::reset_property(int property_id) {
    set_property(PROPERTY_NAMES[property_id], PROPERTY_DEFAULTS[property_id]);
}

void Matrix::init() {
    for (int i = 0; i < TotalNumberParameters; ++i) {
        reset_parameter(i);
        disconnect_all(i);
    }
    for (int i = 0; i < TotalNumberProperties; ++i) {
        reset_property(i);
    }
    set_preset_name("INIT");
    remove_audio_file();
}

juce::ValueTree Matrix::get_state() {
    state = juce::ValueTree("STATE");
    state.appendChild(apvts_ptr->copyState(), nullptr);
    state.appendChild(property_tree_ptr->copyState(), nullptr);
    state.appendChild(matrix.createCopy(), nullptr);
    state.appendChild(preset_tree.createCopy(), nullptr);
    state.appendChild(audio_tree.createCopy(), nullptr);
    return state;
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

                audio_tree.setProperty("NAME", file.getFileNameWithoutExtension(), &undo_manager);
                audio_tree.setProperty("NUM_CHANNELS", num_channels, &undo_manager);
                audio_tree.setProperty("NUM_SAMPLES", num_samples, &undo_manager);
                audio_tree.setProperty("DATA", data, &undo_manager);
            }
        }
    });
}

void Matrix::queue_buffer_from_value_tree() {
    auto num_channels = 1;
    auto num_samples = 1;
    if (audio_tree.hasProperty("NUM_CHANNELS")) {
        num_channels = audio_tree.getProperty("NUM_CHANNELS");
    }
    if (audio_tree.hasProperty("NUM_SAMPLES")) {
        num_samples = audio_tree.getProperty("NUM_SAMPLES");
    }

    auto new_buf = new juce::AudioBuffer<float>(num_channels, num_samples);
    new_buf->clear();
    if (audio_tree.hasProperty("DATA")) {
        auto data = (float *) audio_tree.getProperty("DATA").getBinaryData()->begin();

        for (int c = 0; c < num_channels; ++c) {
            new_buf->copyFrom(c, 0, (num_samples * c) + data, num_samples);
        }
    }

    audio_buffer.queue_new_buffer(new_buf);
}

void Matrix::remove_audio_file() {
    audio_tree.removeAllProperties(&undo_manager);
}

juce::AudioBuffer<float>* Matrix::get_audio_buffer() {
    return audio_buffer.load();
}

void Matrix::update_audio_buffers() {
    audio_buffer.update();
}

void Matrix::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) {
    if (treeWhosePropertyHasChanged.hasType("AUDIO")) {
        queue_buffer_from_value_tree();
    }
}