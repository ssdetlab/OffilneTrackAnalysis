#pragma once

#include "include/Types/Track.hpp"

#include <optional>
#include <utility>

using Range = std::pair<double, double>;

struct AnalysisUnit {
    // General unit identifier
    std::string name;

    //--------------------------------
    // Cut parameters

    // Range of the cut
    std::optional<Range> range;

    //--------------------------------
    // Histogram parameters

    // Number of bins
    int nBins;

    // Lower bound
    double low;

    // Upper bound
    double high;

    //--------------------------------
    // Getter function
    Track::Getter getter;
};


static std::vector<AnalysisUnit> units{
    /// ---------------------------------------------
    /// D.o.F. performance

    // Matching degree
    {"matchingDegree", 
        std::optional<Range>({0, 1}), 
        // std::nullopt,
        100, 0, 1, 
        TrackGetters::matchingDegree},

    // Number of degrees of freedom
    {"ndf", 
        std::optional<Range>({8, 8}), 
        // std::nullopt,
        100, 0, 100, 
        TrackGetters::ndf},

    /// ---------------------------------------------
    /// Inter-track performance

    // Overlap flag
    {"isOverlap", 
        std::optional<Range>({0, 0}), 
        // std::nullopt,
        2, 0, 1, 
        TrackGetters::isOverlap},

    // Multiple tracks in event flag
    {"isMultiple", 
        std::optional<Range>({0, 0}), 
        // std::nullopt,
        2, 0, 1, 
        TrackGetters::isMultiple},

    /// ---------------------------------------------
    /// KF fit performance

    // Chi2/ndf
    {"chi2ndf", 
        std::optional<Range>({0, 2.2}), 
        // std::nullopt,
        100, 0, 3.0, 
        TrackGetters::chi2ndf},

    /// ---------------------------------------------
    /// KF-estimated kinematics

    // Momentum in x
    {"ipPx", 
        // {-0.08, 0.08}, 
        std::nullopt,
        100, -0.1, 0.1, 
        TrackGetters::ipPx},

    // Momentum in y
    {"ipPy", 
        // {-0.5, 0.5}, 
        std::nullopt,
        100, 1, 4.5, 
        TrackGetters::ipPy},

    // Momentum in z
    {"ipPz", 
        // {-0.5, 0.5}, 
        std::nullopt,
        100, -0.5, 0.5, 
        TrackGetters::ipPz},

    // Energy
    {"E", 
        // {2, 4}, 
        std::nullopt,
        100, 1, 4.5, 
        TrackGetters::E},

    /// ---------------------------------------------
    /// Truth kinematics

    // Momentum in x
    {"ipPxTruth", 
        // {-0.08, 0.08}, 
        std::nullopt,
        100, -0.02, 0.02, 
        TrackGetters::ipPxTruth},

    // Momentum in y
    {"ipPyTruth", 
        // {-0.5, 0.5}, 
        std::nullopt,
        100, 1, 4.5, 
        TrackGetters::ipPyTruth},

    // Momentum in z
    {"ipPzTruth", 
        // {-0.5, 0.5}, 
        std::nullopt,
        100, -0.01, 0.01, 
        TrackGetters::ipPzTruth},

    // Energy
    {"ETruth", 
        // {2, 4}, 
        std::nullopt,
        100, 1, 4.5, 
        TrackGetters::ETruth},

    /// ---------------------------------------------
    /// Kinematics errors

    // Momentum in x
    {"ipPxErr", 
        // {-0.08, 0.08}, 
        std::nullopt,
        1000, -1000, 1000, 
        TrackGetters::ipPxErr},

    // Momentum in y
    {"ipPyErr", 
        // {-0.5, 0.5}, 
        std::nullopt,
        100, -0.2, 0.2, 
        TrackGetters::ipPyErr},

    // Momentum in z
    {"ipPzErr", 
        // {-0.5, 0.5}, 
        std::nullopt,
        1000, -1000, 1000, 
        TrackGetters::ipPzErr},

    // Energy
    {"EErr", 
        // {2, 4}, 
        std::nullopt,
        100, -0.2, 0.2, 
        TrackGetters::EErr},

    /// ---------------------------------------------
    /// Significances

    // Vertex x significance
    {"vertexXSignificance", 
        // {-3, 3}, 
        std::nullopt,
        100, -10, 10, 
        TrackGetters::vertexXSignificance},

    // Vertex z significance
    {"vertexZSignificance", 
        // {-27, 27}, 
        std::nullopt,
        100, -30, 30, 
        TrackGetters::vertexZSignificance},

    // Momentum phi significance
    {"ipMomentumPhiSignificance", 
        // {-1, 1}, 
        std::nullopt,
        100, -40, 40, 
        TrackGetters::ipMomentumPhiSignificance},

    // Momentum theta significance
    {"ipMomentumThetaSignificance", 
        // {-10, 20}, 
        std::nullopt,
        200, -60, 80, 
        TrackGetters::ipMomentumThetaSignificance}
};
