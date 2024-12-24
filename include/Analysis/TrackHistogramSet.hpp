#pragma once

#include "include/Analysis/AnalysisUnit.hpp"
#include "include/Types/Track.hpp"

#include <string>
#include <map>

#include "TH1.h"

class TrackHistogramSet {
    public:
        TrackHistogramSet(std::string suffix) : m_suffix(suffix) {
            // Initialize histograms
            for (auto unit : units) {
                m_histograms.insert({new TH1D(
                    (unit.name + "_" + m_suffix).c_str(), "", 
                    unit.nBins, unit.low, unit.high), unit.getter});
            }

            // Suffix
            m_suffix = suffix;
        }

        std::string suffix() const {
            return m_suffix;
        }

        const std::map<TH1D*, Track::Getter>& histograms() const {
            return m_histograms;
        }

        void fill(const Track& track) {
            for (auto& [hist, getter] : m_histograms) {
                hist->Fill(getter(track));
            }
        }

    private:
        std::string m_suffix;

        std::map<TH1D*, Track::Getter> m_histograms;
};
