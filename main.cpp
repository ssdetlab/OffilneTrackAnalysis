#include <iostream>

#include "include/Io/TrackTreeReader.hpp"
#include "include/Analysis/EventStats.hpp"
#include "include/Analysis/TrackHistogramSet.hpp"
#include "include/detail/HelperFunctions.hpp"

int processTracks() {
    // Input directory
    std::string filePath = 
        "/home/romanurmanov/lab/LUXE/acts_tracking/E320Pipeline_analysis/data/background_rejection/merged/fitted-tracks-bkg-full-merged.root";

    // Output directory
    std::string outPath = 
        "/home/romanurmanov/lab/LUXE/acts_tracking/E320Pipeline_analysis/analysisScript/processed/root/fitted-tracks-bkg-full-processed.root";

    TrackTreeReader::Config trackTreeReaderCfg;
    trackTreeReaderCfg.filePath = filePath;

    TrackTreeReader trackTreeReader(trackTreeReaderCfg);

    // Initialize cuts
    Cuts cuts; 
    
    // Process events
    TFile* outFile = new TFile(outPath.c_str(), "RECREATE");
    
    auto events = trackTreeReader.getEvents();

    auto allTracks = trackTreeReader.getTracks();
    auto matchingDegrees = getMatchingDegrees(allTracks); 

    double temp = 0;

    std::map<double, std::map<int, EventStats>> eventStats;
    for (auto matchingDegree : matchingDegrees) {
        TrackHistogramSet histSet(std::to_string(matchingDegree));

        std::map<int, EventStats> evStat;
        for (auto id : events) {
            evStat[id] = EventStats();

            auto tracks = trackTreeReader.getTracksForEvent(id);

            removeOverlaps(tracks);
            removeMultiple(tracks);

            cuts.cuts.at(0).range = {matchingDegree, matchingDegree};
            for (auto track : tracks) {
                if (!processTrack(track, evStat.at(id), cuts)) {
                    continue;
                }
                temp++;

                histSet.fill(track);
            }
        }
        storeTrackHistograms(outFile, histSet);

        auto [cutFlow, cutFlowErrs] = 
            getCutFlow(
                evStat, 
                std::to_string(matchingDegree));
        cutFlow->Write();
        cutFlowErrs->Write();

        eventStats[matchingDegree] = evStat;
    }

    std::cout << "Total number of tracks: " << temp << std::endl;
    std::cout << "Tracks per event: " << temp/events.size() << std::endl;

    outFile->Close();

    return 0;
}

int main() {
    processTracks();
    return 0;
}