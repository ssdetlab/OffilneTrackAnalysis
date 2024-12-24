#pragma once

// Filter that classifies clusters as signal or background
// based on their position in the x-y plane
struct HourglassFilter {
    
    /// Diagonal hourglass lines
    /// slopes
    double a1 = 5.4;
    double a2 = -6.0;

    /// Tunnel-bounded vertical 
    /// shifts
    double b1 = 110.0;
    double b2 = 110.0;

    /// Center-bounded vertical
    /// shift
    double b0 = 190.0;

    // Tunnel in the center
    double tunnel = 1.5;

    // Filter operator
    //
    // @par x: x-coordinate of the point
    // @par y: y-coordinate of the point
    // 
    // @return: true if the point is inside the 
    // hourglass shape, false otherwise
    bool operator()(double x, double y) const{
        // Filter out the top part of the tracking plane
        bool cond0 = (x < 0) ? (y < a1 * x + b0) : (y < a2 * x + b0);

        // Conditions alternate between the two sides of
        // the hourglass shape
        if (x < -tunnel) {
            bool cond1 = y < a1 * x + b1;
            bool cond2 = y > a2 * x + b2;

            return cond0 && (cond1 || cond2);
        }
        else if (x > tunnel) {
            bool cond1 = y > a1 * x + b1;
            bool cond2 = y < a2 * x + b2;

            return cond0 && (cond1 || cond2);
        }
        else {
            // Tunnel in the center
            return cond0;
        }
    }
};
