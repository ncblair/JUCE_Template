import os 

def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))

    with open(os.path.join(dir_path, "parameters.csv")) as f:
        lines = [[x.strip() for x in line.split(",")] for line in f.readlines()[1:]]
    parameters = [l[0] for l in lines]
    ranges = [l[1:5] for l in lines]

    for i, r in enumerate(ranges):
        for j, f in enumerate(r):
            if not ("." in f):
                ranges[i][j] += ".0f"
            else:
                ranges[i][j] += "f"
                
    defaults = [l[5] for l in lines]

    for i, d in enumerate(defaults):
        if not ("." in d):
            defaults[i] += ".0f"
        else:
            defaults[i] += "f"

    automatable = [l[6] for l in lines]

    formatted = ["#pragma once", '#include "JuceHeader.h"']

    param_enum = ["enum PARAM {"]
    param_enum += ["\t" + p + "," for p in parameters]
    param_enum += ["\t" + "TotalNumberParameters"]
    param_enum += ["};"]

    param_ids = ["static const std::array<juce::Identifier, TotalNumberParameters> PARAMETER_IDS{"]
    param_ids += ['\t"' + p + '",' for p in parameters]
    param_ids += ["};"]

    param_names = ["static const juce::StringArray PARAMETER_NAMES{"]
    param_names += ['\t"' + p + '",' for p in parameters]
    param_names += ["};"]

    param_ranges = ["static const juce::Array<juce::NormalisableRange<float>> PARAMETER_RANGES {"]
    param_ranges += [f"\tjuce::NormalisableRange<float>({r[0]}, {r[1]}, {r[2]}, {r[3]})," for r in ranges]
    param_ranges += ["};"]

    param_defaults = ["static const juce::Array<float> PARAMETER_DEFAULTS {"]
    param_defaults += [f"\t{d}," for d in defaults]
    param_defaults += ["};"]

    param_automatable = ["static const juce::Array<bool> PARAMETER_AUTOMATABLE {"]
    param_automatable += [f'\t{"true" if a == "1" else "false"},' for a in automatable]
    param_automatable += ["};"]

    formatted += param_enum + param_ids + param_names + param_ranges + param_defaults + param_automatable

    with open(os.path.join(dir_path, "ParameterDefines.h"), "w") as f:
        f.write('\n'.join(formatted))

if __name__ == "__main__":
    main()