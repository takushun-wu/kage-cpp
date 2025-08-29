#include "fitcurve.h"

namespace Kage {

    void SplineAppend(std::vector<CubicSpline>& pp, Point* bezCurve) {
        pp.push_back({bezCurve[0], bezCurve[1], bezCurve[2], bezCurve[3]});
    }

    // ChordLengthParameterize :
    // Assign parameter values to digitized points using relative distances
    // between points.
    inline double* ChordLengthParameterize(
        std::vector<Point> d, size_t first, size_t last) {
        double* u = new double[last - first + 1]; // Parameterization
        u[0]      = 0.0;
        for(auto i = first + 1; i <= last; i++)
            u[i - first] = u[i - first - 1] + (d[i] - d[i - 1]).GetLength();
        for(auto i = first + 1; i <= last; i++)
            u[i - first] = u[i - first] / u[last - first];
        return u;
    }

    // BezierII :
    // Evaluate a Bezier curve at a particular parameter value
    Point BezierII(int degree, Point* V, double t) {
        // Copy array
        Point* Vtemp = new Point[degree + 1]; // Local copy of control points
        for(int i = 0; i <= degree; i++)
            Vtemp[i] = V[i];
        // Triangle computation
        for(int i = 1; i <= degree; i++)
            for(int j = 0; j <= degree - i; j++)
                Vtemp[j] = (1.0 - t) * Vtemp[j] + t * Vtemp[j + 1];
        Point Q = Vtemp[0]; // Point on curve at parameter t
        delete[] Vtemp;
        return Q;
    }

    // ComputeMaxError :
    // Find the maximum squared distance of digitized points to fitted curve.
    double ComputeMaxError(std::vector<Point> d, size_t first, size_t last,
        Point* bezCurve, double* u, int* splitPoint) {
        double maxDist = 0.0; // Maximum error
        double dist;          // Current error
        *splitPoint = (last - first + 1) / 2;
        for(auto i = first + 1; i < last; i++) {
            dist = std::pow(
                (BezierII(3, bezCurve, u[i - first]) - d[i]).GetLength(), 2);
            if(dist >= maxDist) maxDist = dist, *splitPoint = i;
        }
        return maxDist;
    }

    inline double B0(double u) {
        return ((1.0 - u) * (1.0 - u) * (1.0 - u));
    }

    inline double B1(double u) {
        return (3 * u * ((1.0 - u) * (1.0 - u)));
    }

    inline double B2(double u) {
        return (3 * u * u * (1.0 - u));
    }

    inline double B3(double u) {
        return (u * u * u);
    }

    // GenerateBezier :
    // Use least-squares method to find Bezier control points for region.
    void GenerateBezier(std::vector<Point> d, size_t first, size_t last,
        double* uPrime, Point tHat1, Point tHat2, Point* bezCurve) {
        Point** A;                             // Precomputed rhs for eqn
        int     nPts    = last - first + 1;    // Number of pts in sub-curve
        double  C[2][2] = {0.0};               // Matrix C
        double  X[2]    = {0.0};               // Matrix X
        double  det_C0_C1, det_C0_X, det_X_C1; // Determinants of matrices
        double  alpha_l, alpha_r;              // Alpha values, left and right
        Point   tmp;                           // Utility variable
        // Compute the A's
        A = new Point*[nPts];
        for(auto i = 0; i < nPts; i++) {
            A[i]     = new Point[2];
            A[i][0]  = tHat1.toLength(B1(uPrime[i])),
            A[i][1]  = tHat2.toLength(B2(uPrime[i]));
            C[0][0] += A[i][0].Dot(A[i][0]), C[0][1] += A[i][0].Dot(A[i][1]),
                C[1][0] = C[0][1], C[1][1] += A[i][1].Dot(A[i][1]);
            tmp = d[first + i] - d[first] * B0(uPrime[i]) -
                d[first] * B1(uPrime[i]) - d[last] * B2(uPrime[i]) -
                d[last] * B3(uPrime[i]);
            X[0] += A[i][0].Dot(tmp), X[1] += A[i][1].Dot(tmp);
            delete[] A[i];
        }
        delete[] A;
        // Compute the determinants of C and X
        det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
        det_C0_X  = C[0][0] * X[1] - C[0][1] * X[0];
        det_X_C1  = X[0] * C[1][1] - X[1] * C[0][1];
        // Finally, derive alpha values
        if(det_C0_C1 == 0.0) det_C0_C1 = (C[0][0] * C[1][1]) * 10e-12;
        alpha_l = det_X_C1 / det_C0_C1, alpha_r = det_C0_X / det_C0_C1;
        // If alpha negative, use the Wu/Barsky heuristic (see text)
        // (if alpha is 0, you get coincident control points that lead to divide
        // by zero in any subsequent NewtonRaphsonRootFind() call.
        bezCurve[0] = d[first], bezCurve[3] = d[last];
        if(alpha_l < 1.0e-6 || alpha_r < 1.0e-6) {
            double dist = (d[last] - d[first]).GetLength() / 3.0;
            bezCurve[1] = bezCurve[0] + tHat1.toLength(dist);
            bezCurve[2] = bezCurve[3] + tHat2.toLength(dist);
        } else
            // First and last control points of the Bezier curve are positioned
            // exactly at the first and last data points Control points 1 and 2
            // are positioned an alpha distance out on the tangent vectors, left
            // and right, respectively
            bezCurve[1] = bezCurve[0] + tHat1.toLength(alpha_l),
            bezCurve[2] = bezCurve[3] + tHat2.toLength(alpha_r);
    }

    // NewtonRaphsonRootFind :
    // Use Newton-Raphson iteration to find better root.
    inline double NewtonRaphsonRootFind(Point* Q, Point P, double u) {
        Point Q1[3], Q2[2]; // Q' and Q''
        // Compute Q(u)
        Point Q_u = BezierII(3, Q, u); // u evaluated at Q, Q', & Q''
        // Generate control vertices for Q'
        for(auto i = 0; i <= 2; i++)
            Q1[i] = (Q[i + 1] - Q[i]) * 3.0;
        // Generate control vertices for Q''
        for(auto i = 0; i <= 1; i++)
            Q2[i] = (Q1[i + 1] - Q1[i]) * 2.0;
        // Compute Q'(u) and Q''(u)
        Point Q1_u = BezierII(2, Q1, u), Q2_u = BezierII(1, Q2, u);
        // Compute f(u)/f'(u)
        double numerator   = (Q_u.x - P.x) * Q1_u.x + (Q_u.y - P.y) * Q1_u.y,
               denominator = Q1_u.x * Q1_u.x + Q1_u.y * Q1_u.y +
            (Q_u.x - P.x) * Q2_u.x + (Q_u.y - P.y) * Q2_u.y;
        // u = u - f(u)/f'(u)
        return u - numerator / denominator; // Return improved u
    }

    // Reparameterize:
    // Given set of points and their parameterization, try to find a better
    // parameterization.
    inline double* Reparameterize(std::vector<Point> d, size_t first,
        size_t last, double* u, Point* bezCurve) {
        double* uPrime = new double[last - first + 1]; // New parameter values
        for(auto i = first; i <= last; i++)
            uPrime[i - first] =
                NewtonRaphsonRootFind(bezCurve, d[i], u[i - first]);
        return uPrime;
    }

    inline Point ComputeCenterTangent(std::vector<Point> d, int center) {
        return (0.5 * (d[center - 1] - d[center] + d[center] - d[center + 1]))
            .Normalize();
    }

    // FitCubic :
    // Fit a Bezier curve to a (sub)set of digitized points
    void FitCubic(std::vector<Point> d, size_t first, size_t last, Point tHat1,
        Point tHat2, double error, std::vector<CubicSpline>& pp) {
        int   splitPoint;         // Point to split point set at
        int   maxIterations = 20; // Max times to try iterating
        Point bezCurve[4];
        // Use heuristic if region only has two points in it
        if(last - first + 1 == 2) { // Number of points in subset
            double dist = (d[last] - d[first]).GetLength() / 3.0;
            bezCurve[0] = d[first], bezCurve[3] = d[last];
            bezCurve[1] = bezCurve[0] + tHat1.toLength(dist),
            bezCurve[2] = bezCurve[3] + tHat2.toLength(dist);
            SplineAppend(pp, bezCurve);
            return;
        }
        // Parameterize points, and attempt to fit curve
        double* u = ChordLengthParameterize(
            d, first, last); // Parameter values for point
        GenerateBezier(d, first, last, u, tHat1, tHat2, bezCurve);
        // Find max deviation of points to fitted curve
        double maxError = ComputeMaxError(
            d, first, last, bezCurve, u, &splitPoint); // Maximum fitting error
        if(maxError < error) {
            delete[] u;
            SplineAppend(pp, bezCurve);
            return;
        }
        // If error not too large, try some reparameterization and iteration
        if(maxError < error * error) { // Error below which you try iterating
            for(auto i = 0; i < maxIterations; i++) {
                double* uPrime = Reparameterize(
                    d, first, last, u, bezCurve); // Improved parameter values
                GenerateBezier(d, first, last, uPrime, tHat1, tHat2, bezCurve);
                maxError = ComputeMaxError(
                    d, first, last, bezCurve, uPrime, &splitPoint);
                if(maxError < error) {
                    delete[] u;
                    SplineAppend(pp, bezCurve);
                    return;
                }
                delete[] u;
                u = uPrime;
            }
        }
        // Fitting failed -- split at max error point and fit recursively
        delete[] u;
        Point tHatCenter = ComputeCenterTangent(
            d, splitPoint); // Unit tangent vector at splitPoint
        FitCubic(d, first, splitPoint, tHat1, tHatCenter, error, pp);
        FitCubic(d, splitPoint, last, -1 * tHatCenter, tHat2, error, pp);
    }

    // FitCurve :
    // Fit a Bezier curve to a set of digitized points
    std::vector<CubicSpline> FitCurve(
        std::vector<Point> points, double maxError) {
        std::vector<CubicSpline> pp;
        FitCubic(points, 0, points.size() - 1,
            (points[1] - points[0])
                .Normalize(), // Unit tangent vectors at endpoints
            (points[points.size() - 2] - points[points.size() - 1]).Normalize(),
            maxError, pp);
        return pp;
    }

} // namespace Kage
