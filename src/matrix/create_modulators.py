import os 

def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))

    with open(os.path.join(dir_path, "modulators.csv")) as f:
        lines = [[x.strip() for x in line.split(",")] for line in f.readlines()[1:]]
    modulators = [l[0] for l in lines]
    types = [l[1] for l in lines]
    params = [l[2] for l in lines]

    formatted = ["#pragma once", '#include "JuceHeader.h"', '#include "ParameterDefines.h"']

    mod_enum = ["enum MOD {"]
    mod_enum += [f"\t{m}," for m in modulators]
    mod_enum += ["\tTOTAL_NUMBER_MODULATORS"]
    mod_enum += ["};"]

    mod_types_enum = ["enum MOD_TYPES {"]
    mod_types_enum += [f"\t{t}," for t in set(types)]
    mod_types_enum += ["};"]

    mod_ids = ["static const std::array<juce::Identifier, TOTAL_NUMBER_MODULATORS> MODULATOR_IDS{"]
    mod_ids += [f'\t"{m}",' for m in modulators]
    mod_ids += ["};"]

    mod_names = ["static const juce::StringArray MODULATOR_NAMES{"]
    mod_names += [f'\t"{m}",' for m in modulators]
    mod_names += ["};"]

    mod_types = ["static const std::vector<int> MODULATOR_TYPES {"]
    mod_types += [f"\tMOD_TYPES::{t}," for t in types]
    mod_types += ["};"]

    mod_params = ["static const std::vector<std::vector<int>> MODULATOR_PARAMS {"]
    mod_params += ["\t{" + "".join([f"PARAM::{p}, " for p in ps.split(" ")]) + "}," for ps in params]
    mod_params += ["};"]

    formatted += mod_enum + mod_types_enum + mod_ids + mod_names + mod_types + mod_params

    with open(os.path.join(dir_path, "ModulatorDefines.h"), "w") as f:
        f.write('\n'.join(formatted))

if __name__ == "__main__":
    main()