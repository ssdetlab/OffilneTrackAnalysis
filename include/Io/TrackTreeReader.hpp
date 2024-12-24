#pragma once

#include "include/Types/Track.hpp"

#include <unordered_map>

#include "TFile.h"  
#include "TTree.h"
#include "TVector3.h"

// Chain manager that performs the setup of the TTree
struct TrackTreeReader {
    public:
        struct Config {
            /// File path to get the tree from
            std::string filePath;
            /// Name of the tree
            std::string treeName = "fitted-tracks";
        };

        TrackTreeReader(const Config& cfg) : m_cfg(cfg) {
            prepareTree(m_cfg.filePath);
        }

        // Get the list of events
        std::vector<std::uint32_t> getEvents() const {
            std::vector<std::uint32_t> events;
            for (auto& event : m_eventMap) {
                events.push_back(std::get<0>(event));
            }
            return events;
        }

        // Extract all tracks
        std::vector<Track> getTracks() {
            std::vector<Track> tracks;
            for (auto& event : m_eventMap) {
                auto eventTracks = getTracksForEvent(std::get<0>(event));
                tracks.insert(tracks.end(), eventTracks.begin(), eventTracks.end());
            }
            return tracks;
        }

        // Extract tracks for a specific event
        std::vector<Track> getTracksForEvent(std::uint32_t eventN) {
            std::vector<Track> tracks;
            auto it = std::find_if(m_eventMap.begin(), m_eventMap.end(),
                [eventN] (const auto& a) {
                    return std::get<0>(a) == eventN;
                }
            );
            if (it == m_eventMap.end() || eventN == 0) {
                return tracks;
            }
            auto start = std::get<1>(*it);
            auto end = std::get<2>(*it);
            
            for (auto i = start; i < end; ++i) {
                m_tree->GetEntry(i);
                Track track;
                
                track.trueTrackHits = *m_vVector3Columns.at("trueTrackHits");
                track.trackHits = *m_vVector3Columns.at("trackHits");
                
                track.predictedTrackHits = *m_vVector3Columns.at("predictedTrackHits"); 
                track.filteredTrackHits = *m_vVector3Columns.at("filteredTrackHits"); 
                track.smoothedTrackHits = *m_vVector3Columns.at("smoothedTrackHits"); 
                
                track.truePredictedResiduals = *m_vVector3Columns.at("truePredictedResiduals"); 
                track.trueFilteredResiduals = *m_vVector3Columns.at("trueFilteredResiduals"); 
                track.trueSmoothedResiduals = *m_vVector3Columns.at("trueSmoothedResiduals"); 
                
                track.predictedResiduals = *m_vVector3Columns.at("predictedResiduals"); 
                track.filteredResiduals = *m_vVector3Columns.at("filteredResiduals"); 
                track.smoothedResiduals = *m_vVector3Columns.at("smoothedResiduals"); 
                
                track.truePredictedPulls = *m_vVector3Columns.at("truePredictedPulls"); 
                track.trueFilteredPulls = *m_vVector3Columns.at("trueFilteredPulls"); 
                track.trueSmoothedPulls = *m_vVector3Columns.at("trueSmoothedPulls"); 
                
                track.predictedPulls = *m_vVector3Columns.at("predictedPulls"); 
                track.filteredPulls = *m_vVector3Columns.at("filteredPulls"); 
                track.smoothedPulls = *m_vVector3Columns.at("smoothedPulls"); 
                
                track.chi2 = m_doubleColumns.at("chi2");
                track.matchingDegree = m_doubleColumns.at("matchingDegree");
                
                track.ndf = m_intColumns.at("ndf");
                track.trackId = m_intColumns.at("trackId");
                track.eventId = m_intColumns.at("eventId");
                
                track.ipMomentum = *m_lorentzColumns.at("ipMomentum");
                track.ipMomentumTruth = *m_lorentzColumns.at("ipMomentumTruth");

                track.vertex = *m_vector3Columns.at("vertex"); 
                track.vertexTruth = *m_vector3Columns.at("vertexTruth"); 

                track.ipMomentumError = *m_vector3Columns.at("ipMomentumError"); 
                track.vertexError = *m_vector3Columns.at("vertexError"); 

                tracks.push_back(track);
            }
            return tracks;
        }

    private:
        Config m_cfg;

        // File pointer
        TFile* m_file = nullptr;

        // Tree pointer
        TTree* m_tree = nullptr;

        // Column containers
        std::unordered_map<std::string_view, 
            std::int32_t> m_intColumns;
        
        std::unordered_map<std::string_view, 
            double> m_doubleColumns;

        std::unordered_map<std::string_view, 
            TVector3*> m_vector3Columns;
        std::unordered_map<std::string_view, 
            std::vector<TVector3>*> m_vVector3Columns;

        std::unordered_map<std::string_view,
            TLorentzVector*> m_lorentzColumns;
    
        // Event map
        std::vector<std::tuple<
            std::uint32_t, std::size_t, std::size_t>> m_eventMap;

        // Columns of the chain
        std::vector<const char*> m_intKeys = {
            "trackId", "eventId", "ndf"};
        std::vector<const char*> m_doubleKeys = {
            "chi2", "matchingDegree"};
        std::vector<const char*> m_vector3Keys = {
            "ipMomentumError",
            "vertex",
            "vertexError",
            "vertexTruth"};
        std::vector<const char*> m_vVector3Keys = {
            "trueTrackHits",
            "trackHits",
            "predictedTrackHits",
            "filteredTrackHits",
            "smoothedTrackHits",
            "truePredictedResiduals",
            "trueFilteredResiduals",
            "trueSmoothedResiduals",
            "predictedResiduals",
            "filteredResiduals",
            "smoothedResiduals",
            "truePredictedPulls",
            "trueFilteredPulls",
            "trueSmoothedPulls",
            "predictedPulls",
            "filteredPulls",
            "smoothedPulls"};
        std::vector<const char*> m_lorentzKeys = {
            "ipMomentum",
            "ipMomentumTruth"};

        // Prepare the tree for reading
        void prepareTree(std::string& filePath) {
            // Open the file and get the tree
            m_file = new TFile(filePath.c_str(), "READ");
            m_tree = (TTree*)m_file->Get(m_cfg.treeName.c_str());
    
            // Set the branches
            setBranches(m_tree, m_intKeys, m_intColumns);
            setBranches(m_tree, m_doubleKeys, m_doubleColumns);
    
            setBranches(m_tree, m_vector3Keys, m_vector3Columns);
            setBranches(m_tree, m_vVector3Keys, m_vVector3Columns);
    
            setBranches(m_tree, m_lorentzKeys, m_lorentzColumns);
    
            // Disable all branches and only enable event-id for a first scan of the file
            m_tree->SetBranchStatus("*", false);
            if(!m_tree->GetBranch("eventId")) {
                throw std::invalid_argument("Missing eventId branch");
            }
            m_tree->SetBranchStatus("eventId", true);
                        
            auto nEntries = static_cast<std::size_t>(m_tree->GetEntries());
        
            // Add the first entry
            m_tree->GetEntry(0);
            m_eventMap.push_back({m_intColumns.at("eventId"), 0ul, 0ul});
        
            // Go through all entries and store the position of the events
            for (auto i = 1ul; i < nEntries; ++i) {
                m_tree->GetEntry(i);
                const auto evtId = m_intColumns.at("eventId");
        
                if (evtId != std::get<0>(m_eventMap.back())) {
                    std::get<2>(m_eventMap.back()) = i;
                    m_eventMap.push_back({evtId, i, i});
                }
            }
            // Sort by event id
            std::sort(m_eventMap.begin(), m_eventMap.end(),
                [] (const auto& a, const auto& b) {
                    return std::get<0>(a) < std::get<0>(b);
                }
            );
    
            std::get<2>(m_eventMap.back()) = nEntries;
    
            // Re-Enable all branches
            m_tree->SetBranchStatus("*", true);
        }
    
        // Set the branches of the TTree
        template <typename K, typename T>
        inline void setBranches(
            TTree* tree,
            const K& keys, 
            std::unordered_map<std::string_view, T>& columns) {
            T value = 0;
            for (auto key : keys) {
                columns.insert({key, value});
            }
            for (auto key : keys) {
                tree->SetBranchAddress(key, &columns.at(key));
            }
        }
};
