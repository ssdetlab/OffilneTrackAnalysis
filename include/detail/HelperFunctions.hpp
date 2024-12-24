#pragma once

#include "include/Types/Track.hpp"
#include "include/Analysis/Cuts.hpp"
#include "include/Analysis/EventStats.hpp"  
#include "include/Analysis/TrackHistogramSet.hpp"

#include <algorithm>
#include <string>
#include <filesystem>
#include <ranges>
#include <utility>

#include "TFile.h"
#include "TTree.h"
#include "TGraphAsymmErrors.h"

// Extract the file paths from a directory
inline std::vector<std::string> prepareFilePaths(const std::string& path) {
    // Get the paths to the files in the directory
    std::vector<std::string> filePaths;
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::string pathToFile = entry.path();
        filePaths.push_back(pathToFile);
    }

    // Sort the file paths by the BX number
    std::sort(filePaths.begin(), filePaths.end(),
        [] (const std::string& a, const std::string& b) {
            std::size_t idxRootA = a.find_last_of('.');
            std::size_t idxEventA = a.find_last_of('t', idxRootA);
            std::string eventSubstrA = a.substr(idxEventA + 1, idxRootA - idxEventA);
            
            std::size_t idxRootB = b.find_last_of('.');
            std::size_t idxEventB = b.find_last_of('t', idxRootB);
            std::string eventSubstrB = b.substr(idxEventB + 1, idxRootB - idxEventB);

            return std::stoul(eventSubstrA) < std::stoul(eventSubstrB);
        }
    );

    return filePaths;
}

inline std::vector<double> getMatchingDegrees(const std::vector<Track>& tracks) {
    // Collect the matching degrees
    std::vector<double> matchingDegrees;
    for (auto track : tracks) {
        matchingDegrees.push_back(track.matchingDegree);
    }
    
    std::sort(matchingDegrees.begin(), matchingDegrees.end());

    auto last = std::unique(matchingDegrees.begin(), matchingDegrees.end());
    matchingDegrees.erase(last, matchingDegrees.end());

    return matchingDegrees;
}

inline bool processTrack(
    Track& track, 
    EventStats& evStat, 
    const Cuts& cuts) {
        for (auto cut : cuts.cuts) {
            if (cut.range.first > cut.getter(track) || 
                cut.range.second < cut.getter(track)) {
                    return false;
            }
            evStat.cutFlow.flow[cut.name]++;
        }
        return true;
};

inline void removeOverlaps(std::vector<Track>& tracks) {
    auto isOverlap = [](
        const std::vector<TVector3>& track1, 
        const std::vector<TVector3>& track2) {
            if (track1.size() != track2.size()) {
                return false;
            }
            for (int i = 0; i < track1.size(); i++) {
                if (track1.at(i) == track2.at(i)) {
                    return true;
                }
            }
            return false;
    };
    
    for (auto it = tracks.begin(); it != tracks.end(); it++) {
        for (auto jt = it + 1; jt != tracks.end(); jt++) {
            if (isOverlap(it->trackHits, jt->trackHits)) {
                if (it->chi2 / it->ndf < jt->chi2 / jt->ndf) {
                    jt->isOverlap = true;
                } else {
                    it->isOverlap = true;
                }
            }
        }
    }
} 

inline void removeMultiple(std::vector<Track>& tracks) {
    if (tracks.empty()) {
        return;
    }
    if (tracks.size() == 1) {
        tracks.front().isMultiple = false;
        return;
    }
    std::ranges::sort(tracks, 
        [](const auto& trackA, const auto& trackB) {
            return trackA.chi2 / trackA.ndf < trackB.chi2 / trackB.ndf;
        });

    for (auto& track : tracks) {
        track.isMultiple = true;
    }
    tracks.front().isMultiple = false;
} 

inline std::pair<TH1D*, TGraphAsymmErrors*> getCutFlow(
    const std::map<int, EventStats>& evStats,
    const std::string& suffix, 
    int nEvents = -1) {
        std::vector<std::string> cutNames;
        for (auto unit : units) {
            if (unit.range.has_value()) {
                cutNames.push_back(unit.name);
            }
        }

        int cutFlowN = cutNames.size();
        std::string cutFlowName = "cutFlow_" + suffix;
        TH1D* cutFlow = new TH1D(cutFlowName.c_str(), "", cutFlowN, 0, cutFlowN);

        EventStats avgStats;
        if (nEvents == -1) {
            nEvents = evStats.size();
            std::cout << "EVENTS " << nEvents << "\n";
        }
        for (auto [evN, evStat] : evStats) {
            for (auto [cutName, cutValue] : evStat.cutFlow.flow) {
                avgStats.cutFlow.flow.at(cutName) += 
                    cutValue / nEvents;
            }
        }
        for (int i = 0; i < cutFlowN; i++) {
            cutFlow->GetXaxis()->SetBinLabel(i + 1, cutNames.at(i).c_str());
        }

        auto getCutVector = [](
            const std::map<int, EventStats>& evStats, 
            const std::string& field) {
                std::vector<double> v;
                for (auto [evN, evStat] : evStats) {
                    v.push_back(evStat.cutFlow.flow.at(field));
                }
                return v;
        };

        auto likelihood = [](double p, int accept, int reject) {
            return accept * std::log(p) + reject * std::log(1 - p);
        };

        auto getErrors = [&](std::vector<double> v) 
            -> std::pair<double, double>{
                double reject = std::ranges::count(v, 0.0);
                double accept = v.size() - reject;
                double p = accept / v.size();
                
                double lMax = likelihood(p, accept, reject);
                
                double lSearch = lMax;
                int k = 0;
                while (likelihood(p - k * 1e-4, accept, reject) - lMax > -0.5) {
                    k++;
                }
                double leftErrBound = p - k * 1e-4;
            
                lSearch = lMax;
                k = 0;
                while (likelihood(p + k * 1e-4, accept, reject) - lMax > -0.5) {
                    k++;
                }
                double rightErrBound = p + k * 1e-4;
            
                return {p - leftErrBound, rightErrBound - p};
        };

        std::map<std::string,std::pair<double, double>> cutCIs;
        for (const auto& cutName : cutNames) {
            std::vector<double> dist = getCutVector(evStats, cutName);
            auto CI = getErrors(dist);
            cutCIs.insert({cutName,CI});
        }

        TGraphAsymmErrors* cutFlowGraph = new TGraphAsymmErrors();
        std::string cutFlowGraphName = "cutFlowGraph_" + suffix;
        cutFlowGraph->SetName(cutFlowGraphName.c_str());
        for (int i = 0; i < cutFlowN; i++) {
            auto point = avgStats.cutFlow.flow.at(cutNames.at(i));
            auto CI = cutCIs.at(cutNames.at(i));

            cutFlow->Fill(cutNames.at(i).c_str(), point);
            
            cutFlowGraph->SetPoint(i, cutFlow->GetXaxis()->GetBinCenter(i + 1), point);
            cutFlowGraph->SetPointError(
                i, 
                0, 0, 
            CI.first, 
            CI.second);
        }
    
        return {cutFlow, cutFlowGraph};
}

inline void storeTrackHistograms(TFile* file, TrackHistogramSet& histSet) {
    file->cd();

    for (auto& [hist, getter] : histSet.histograms()) {
        hist->Write();
    }
};
