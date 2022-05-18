#include "Matrix.h"
#include "../../util/Modulator.h"
#include "../../plugin/PluginProcessor.h"
#include "../../plugin/DummyProcessor.h"

Matrix::Matrix(PluginProcessor* proc) {

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    //==============================================================================
    //-> ADD PARAMS
    //==============================================================================
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
    // add Matrix to state
    apvts_ptr->state.appendChild(matrix, nullptr);

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
    apvts_ptr->state.appendChild(property_tree_ptr->state, nullptr);

    //==============================================================================
    //-> SETUP PRESETS
    //==============================================================================
    // preset_tree = juce::ValueTree("PRESET");
    apvts_ptr->state.appendChild(juce::ValueTree("PRESET"), nullptr);
    set_preset_name("INIT");
    // std::initializer_list<juce::NamedValueSet::NamedValue>("NAME", "INIT")), nullptr);
    auto music_dir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userMusicDirectory);
    auto plugin_dir = music_dir.getChildFile("nthn_plugins").getChildFile("template");
    presets_dir = plugin_dir.getChildFile("presets");

}

Matrix::~Matrix(){

}

float Matrix::paramValue(int param_id) {
    // returns the parameter value of a certain ID in a thread safe way
    return apvts_ptr->getRawParameterValue(PARAMETER_NAMES[param_id])->load();
}

juce::NormalisableRange<float> Matrix::paramRange(int param_id) {
    // returns the parameter range of a certain ID in a thread safe way
    return PARAMETER_RANGES[param_id];
    // return apvts_ptr->getParameterRange(PARAMETER_NAMES[param_id]);
}

Modulator* Matrix::getModulator(int mod_id) {
    return modulators[mod_id].get();
}

juce::ValueTree Matrix::getModulators(int param_id) {
    return matrix.getChildWithName(PARAMETER_NAMES[param_id]);
}

float Matrix::modulatorValue(int mod_id, double ms_elapsed, double release_time) {
    // returns the current modulator value after a ms_elapsed with release_time
    return modulators[mod_id]->get(ms_elapsed, release_time); 
}

float Matrix::modulatedParamValue(int param_id, double ms_elapsed, double release_time) {
    // returns the param value modulated by currently active modulators
    float v = 0.0f;
    juce::ValueTree mods = matrix.getChildWithName(PARAMETER_NAMES[param_id]);
    for (int i = 0; i < mods.getNumChildren(); ++i) {
        auto mod_node = mods.getChild(i);
        float mod_depth = mod_node.getProperty("DEPTH");
        int mod_id = mod_node.getProperty("MOD_ID");

        v += mod_depth * modulatorValue(mod_id, ms_elapsed, release_time);
    }

    auto param_range = paramRange(param_id);
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
    // return &preset_tree;
    return apvts_ptr->state.getChildWithName("PRESET");
}

void Matrix::update_modulator_parameters(double ms_elapsed, double last_release_time) {
    // TODO: update modulators first and cache
    for (unsigned long m_id = 0; m_id < TotalNumberModulators; ++m_id) {
        modulators[m_id]->update_parameters(this, ms_elapsed, last_release_time);
    }
}

float Matrix::propertyValue(int property_id) {
    return property_tree_ptr->getRawParameterValue(PROPERTY_NAMES[property_id])->load();
}

juce::UndoManager* Matrix::getUndoManager() {
    return &undo_manager;
}

void Matrix::reload_state() {
    // must be called every time state is overriden
    // TODO: combine this with writing the XML
    matrix = apvts_ptr->state.getChildWithName("MATRIX"); //matrix
    property_tree_ptr->replaceState(apvts_ptr->state.getChildWithName("PROPERTIES")); //properties
    // preset_tree = apvts_ptr->state.getChildWithName("PRESET");
}

void Matrix::save_preset(juce::String preset_name) {
    set_preset_name(preset_name);
    auto file = presets_dir.getChildFile(preset_name).withFileExtension(".template_preset");
    if (!presets_dir.exists()) {
        // create directory if it doesn't exist
        presets_dir.createDirectory();
    }
    if (!file.existsAsFile()) {
        // create file
        file.create();
    }
    std::unique_ptr<juce::XmlElement> xml (apvts_ptr->copyState().createXml());
    auto temp = juce::File::createTempFile("template_preset_temp");
    xml->writeTo(temp);
    temp.replaceFileIn(file);
}

void Matrix::load_preset(juce::String preset_name) {
    auto file = presets_dir.getChildFile(preset_name).withFileExtension(".template_preset");
    if (file.existsAsFile()) {
        std::unique_ptr<juce::XmlElement> xmlState = juce::XmlDocument::parse(file);
 
        if (xmlState.get() != nullptr){
            apvts_ptr->replaceState(juce::ValueTree::fromXml(*xmlState)); //apvts
            reload_state();
            set_preset_name(preset_name);
        }
    } 
}

juce::String Matrix::get_preset_name() {
    return getPresetTree().getProperty("NAME");
}

void Matrix::set_preset_name(juce::String preset_name) {
    getPresetTree().setProperty("NAME", preset_name, &undo_manager);
    // getPresetTree()->sendPropertyChangeMessage("NAME");
}

// void Matrix::add_listener_to_preset_tree(juce::ValueTree::Listener* listener) {
//     getPresetTree()->addListener(listener);
// }

void Matrix::reset_parameter(int param_id) {
    auto parameter = getParamTree()->getParameter(PARAMETER_NAMES[param_id]);
    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost(PARAMETER_RANGES[param_id].convertTo0to1(PARAMETER_DEFAULTS[param_id]));
    parameter->endChangeGesture();
}

void Matrix::reset_property(int property_id) {
    auto property = getPropertyTree()->getParameter(PROPERTY_NAMES[property_id]);
    property->beginChangeGesture();
    property->setValueNotifyingHost(PROPERTY_RANGES[property_id].convertTo0to1(PROPERTY_DEFAULTS[property_id]));
    property->endChangeGesture();
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
}