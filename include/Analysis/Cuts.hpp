#pragma once

#include "include/Analysis/AnalysisUnit.hpp"
#include "include/Types/Track.hpp"

#include <map>
#include <utility>

struct Cut {
    std::string name;
    
    std::pair<double, double> range;

    Track::Getter getter;
};

struct Cuts {
    std::vector<Cut> cuts;

    Cuts() {
        for (auto unit : units) {
            if (unit.range.has_value()) {
                cuts.push_back(
                    {unit.name, unit.range.value(), unit.getter});
            }
        }
    }
};

struct CutFlow {
    std::map<std::string, double> flow;
    
    CutFlow() {
        for (auto unit : units) {
            flow[unit.name] = 0;
        }
    }
};
