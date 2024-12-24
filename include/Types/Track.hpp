#pragma once

#include <vector>

#include "TVector3.h"
#include "TLorentzVector.h"

struct Track {
    using Getter = std::function<double(const Track&)>;
    
    /// Track hits from the true information
    std::vector<TVector3> trueTrackHits;

    /// Track hits from the measurements
    std::vector<TVector3> trackHits;

    /// KF predicted track hits
    std::vector<TVector3> predictedTrackHits;
    std::vector<TVector3> filteredTrackHits;
    std::vector<TVector3> smoothedTrackHits;

    /// KF residuals with respect to the true hits
    std::vector<TVector3> truePredictedResiduals;
    std::vector<TVector3> trueFilteredResiduals;
    std::vector<TVector3> trueSmoothedResiduals;

    /// KF residuals with respect to the measurements
    std::vector<TVector3> predictedResiduals;
    std::vector<TVector3> filteredResiduals;
    std::vector<TVector3> smoothedResiduals;

    /// KF pulls with respect to the true hits
    std::vector<TVector3> truePredictedPulls;
    std::vector<TVector3> trueFilteredPulls;
    std::vector<TVector3> trueSmoothedPulls;

    /// KF pulls with respect to the measurements
    std::vector<TVector3> predictedPulls;
    std::vector<TVector3> filteredPulls;
    std::vector<TVector3> smoothedPulls;
    
    /// Flag indicating how many hits are matched
    /// between the true and the fitted track
    double matchingDegree;

    /// Chi2 of the track
    double chi2;

    /// Number of degrees of freedom
    int ndf;

    /// TrackId
    int trackId;

    /// EventId
    int eventId;

    /// True momentum at the IP
    TLorentzVector ipMomentumTruth;

    /// True vertex position
    TVector3 vertexTruth;

    /// KF predicted momentum at the IP
    TLorentzVector ipMomentum;
    TVector3 ipMomentumError;
    
    /// KF predicted vertex position
    TVector3 vertex;

    /// KF predicted vertex error
    TVector3 vertexError;

    /// Overlap flag
    bool isOverlap = false;

    /// Multiple tracks in events flag
    bool isMultiple = false;
};

namespace TrackGetters {

    /// ---------------------------------------------
    /// D.o.F. performance

    static auto matchingDegree = [] (const Track& track) {
        return track.matchingDegree;
    };

    static auto ndf = [] (const Track& track) {
        return track.ndf;
    };

    /// ---------------------------------------------
    /// Inter-track performance

    static auto isOverlap = [] (const Track& track) {
        return track.isOverlap;
    };

    static auto isMultiple = [] (const Track& track) {
        return track.isMultiple;
    };

    /// ---------------------------------------------
    /// KF fit performance

    static auto chi2ndf = [] (const Track& track) {
        return track.chi2/track.ndf;
    };

    // static auto smoothedResidualsX = [] (const Track& track) {
        // return track.smoothedResiduals;
    // };

    /// ---------------------------------------------
    /// KF-estimated kinematics

    static auto ipPx = [] (const Track& track) {
        return track.ipMomentum.Px();
    };

    static auto ipPy = [] (const Track& track) {
        return track.ipMomentum.Py();
    };

    static auto ipPz = [] (const Track& track) {
        return track.ipMomentum.Pz();
    };

    static auto E = [] (const Track& track) {
        return track.ipMomentum.E();
    };

    // static auto vertexX = [] (const Track& track) {
        // return track.vertex.X();
    // };
    // static auto vertexY = [] (const Track& track) {
        // return track.vertex.Y();
    // };
    // static auto vertexZ = [] (const Track& track) {
        // return track.vertex.Z();
    // };

    /// ---------------------------------------------
    /// Truth kinematics

    static auto ipPxTruth = [] (const Track& track) {
        return track.ipMomentumTruth.Px();
    };

    static auto ipPyTruth = [] (const Track& track) {
        return track.ipMomentumTruth.Py();
    };

    static auto ipPzTruth = [] (const Track& track) {
        return track.ipMomentumTruth.Pz();
    };

    static auto ETruth = [] (const Track& track) {
        return track.ipMomentumTruth.E();
    };


    /// ---------------------------------------------
    /// Kinematics errors

    static auto ipPxErr = [] (const Track& track) {
        return (track.ipMomentumTruth.Px() - track.ipMomentum.Px()) / 
            track.ipMomentumTruth.Px();
    };
    static auto ipPyErr = [] (const Track& track) {
        return (track.ipMomentumTruth.Py() - track.ipMomentum.Py()) / 
            track.ipMomentumTruth.Py();
    };
    static auto ipPzErr = [] (const Track& track) {
        return (track.ipMomentumTruth.Pz() - track.ipMomentum.Pz()) / 
            track.ipMomentumTruth.Pz();
    };
    static auto EErr = [] (const Track& track) {
        return (track.ipMomentumTruth.E() - track.ipMomentum.E()) / 
            track.ipMomentumTruth.E();
    };

    /// ---------------------------------------------
    /// Significances

    static auto vertexXSignificance = [] (const Track& track) {
        return track.vertex.X()/track.vertexError.X();
    };

    static auto vertexZSignificance = [] (const Track& track) {
        return track.vertex.Z()/track.vertexError.Z();
    };

    static auto ipMomentumPhiSignificance = [] (const Track& track) {
        return (track.ipMomentum.Phi() - M_PI_2)/track.ipMomentumError.X();
    };

    static auto ipMomentumThetaSignificance = [] (const Track& track) {
        return (track.ipMomentum.Theta() - M_PI_2)/track.ipMomentumError.Y();
    };

} // namespace TrackGetters
