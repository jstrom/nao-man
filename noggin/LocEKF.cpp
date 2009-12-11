#include "LocEKF.h"
#include <boost/numeric/ublas/io.hpp> // for cout
#include "FieldConstants.h"
//#define DEBUG_LOC_EKF_INPUTS
//#define DEBUG_STANDARD_ERROR
using namespace boost::numeric;
using namespace boost;
using namespace std;
using namespace NBMath;

// Parameters
// Measurement conversion form
const float LocEKF::USE_CARTESIAN_DIST = 50.0f;
// Uncertainty
const float LocEKF::BETA_LOC = 1.0f;
const float LocEKF::GAMMA_LOC = 0.1f;
const float LocEKF::BETA_ROT = M_PI_FLOAT/64.0f;
const float LocEKF::GAMMA_ROT = 0.1f;

// Default initialization values
const float LocEKF::INIT_LOC_X = CENTER_FIELD_X;
const float LocEKF::INIT_LOC_Y = CENTER_FIELD_Y;
const float LocEKF::INIT_LOC_H = 0.0f;
const float LocEKF::INIT_BLUE_GOALIE_LOC_X = (FIELD_WHITE_LEFT_SIDELINE_X +
                                         GOALBOX_DEPTH / 2.0f);
const float LocEKF::INIT_BLUE_GOALIE_LOC_Y = CENTER_FIELD_Y;
const float LocEKF::INIT_BLUE_GOALIE_LOC_H = 0.0f;
const float LocEKF::INIT_RED_GOALIE_LOC_X = (FIELD_WHITE_RIGHT_SIDELINE_X -
                                             GOALBOX_DEPTH / 2.0f);
const float LocEKF::INIT_RED_GOALIE_LOC_Y = CENTER_FIELD_Y;
const float LocEKF::INIT_RED_GOALIE_LOC_H = M_PI_FLOAT;
// Uncertainty limits
const float LocEKF::X_UNCERT_MAX = FIELD_WIDTH / 2.0f;
const float LocEKF::Y_UNCERT_MAX = FIELD_HEIGHT / 2.0f;
const float LocEKF::H_UNCERT_MAX = 4*M_PI_FLOAT;
const float LocEKF::X_UNCERT_MIN = 1.0e-6f;
const float LocEKF::Y_UNCERT_MIN = 1.0e-6f;
const float LocEKF::H_UNCERT_MIN = 1.0e-6f;
// Initial estimates
const float LocEKF::INIT_X_UNCERT = X_UNCERT_MAX / 2.0f;
const float LocEKF::INIT_Y_UNCERT = Y_UNCERT_MAX / 2.0f;
const float LocEKF::INIT_H_UNCERT = M_PI_FLOAT * 2.0f;
// Estimate limits
const float LocEKF::X_EST_MIN = 0.0f;
const float LocEKF::Y_EST_MIN = 0.0f;
const float LocEKF::X_EST_MAX = FIELD_GREEN_WIDTH;
const float LocEKF::Y_EST_MAX = FIELD_GREEN_HEIGHT;

//JHS: You should rewrite this LOC filter so the uncertainty update to
// the a priori (timestep) covariance P_k_bar depends on the size of
// the odometry step taken. or at least, most partly in this direction.
// This way, the covariance will stay lower when you aren't moving/moving fast

/**
 * Initialize the localization EKF class
 *
 * @param initX Initial x estimate
 * @param initY Initial y estimate
 * @param initH Initial heading estimate
 * @param initXUncert Initial x uncertainty
 * @param initYUncert Initial y uncertainty
 * @param initHUncert Initial heading uncertainty
 */
LocEKF::LocEKF(float initX, float initY, float initH,
               float initXUncert,float initYUncert, float initHUncert)
    : EKF<Observation, MotionModel, LOC_EKF_DIMENSION,
          LOC_MEASUREMENT_DIMENSION>(BETA_LOC,GAMMA_LOC), lastOdo(0,0,0),
      useAmbiguous(true)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;

    // Setup initial values
    setXEst(initX);
    setYEst(initY);
    setHEst(initH);
    setXUncert(initXUncert);
    setYUncert(initYUncert);
    setHUncert(initHUncert);

    betas(2) = BETA_ROT;
    gammas(2) = GAMMA_ROT;

#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "Initializing LocEKF with: " << *this << endl;
#endif
}

/**
 * Reset the EKF to a starting configuration
 */
void LocEKF::reset()
{
    setXEst(INIT_LOC_X);
    setYEst(INIT_LOC_Y);
    setHEst(INIT_LOC_H);
    setXUncert(INIT_X_UNCERT);
    setYUncert(INIT_Y_UNCERT);
    setHUncert(INIT_H_UNCERT);
}

/**
 * Reset the EKF to a blue goalie starting configuration
 */
void LocEKF::blueGoalieReset()
{
    setXEst(INIT_BLUE_GOALIE_LOC_X);
    setYEst(INIT_BLUE_GOALIE_LOC_Y);
    setHEst(INIT_BLUE_GOALIE_LOC_H);
    setXUncert(INIT_X_UNCERT);
    setYUncert(INIT_Y_UNCERT);
    setHUncert(INIT_H_UNCERT);
}

/**
 * Reset the EKF to a red goalie starting configuration
 */
void LocEKF::redGoalieReset()
{
    setXEst(INIT_RED_GOALIE_LOC_X);
    setYEst(INIT_RED_GOALIE_LOC_Y);
    setHEst(INIT_RED_GOALIE_LOC_H);
    setXUncert(INIT_X_UNCERT);
    setYUncert(INIT_Y_UNCERT);
    setHUncert(INIT_H_UNCERT);
}

/**
 * Method to deal with updating the entire loc model
 *
 * @param u The odometry since the last frame
 * @param Z The observations from the current frame
 */
void LocEKF::updateLocalization(MotionModel u, vector<Observation> Z)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "Loc update: " << endl;
    cout << "Before updates: " << *this << endl;
    cout << "\tOdometery is " << u <<endl;
    cout << "\tObservations are: " << endl;
    for(unsigned int i = 0; i < Z.size(); ++i) {
        cout << "\t\t" << Z[i] <<endl;
    }
#endif
    // Update expected position based on odometry
    timeUpdate(u);
    limitAPrioriUncert();
    lastOdo = u;

    if (! useAmbiguous) {
        // Remove ambiguous observations
        vector<Observation>::iterator iter = Z.begin();
        while( iter != Z.end() ) {
            if (iter->getNumPossibilities() > 1 ) {
                iter = Z.erase( iter );
            } else {
                ++iter;
            }
        }
    }

    // Correct step based on the observed stuff
    if (Z.size() > 0) {
        correctionStep(Z);
    } else {
        noCorrectionStep();
    }
    //limitPosteriorUncert();

    // Clip values if our estimate is off the field
    clipRobotPose();
    if (testForNaNReset()) {
        cout << "LocEKF reset to: "<< *this << endl;
        cout << "\tLast odo is: " << lastOdo << endl;
        cout << "\tObservations are: ";
        vector<Observation>::iterator iter = Z.begin();
        while( iter != Z.end() ) {
            cout << endl << "\t\t" << *iter;
            ++iter;
        }
        cout << endl;
    }

#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "After updates: " << *this << endl;
    cout << endl;
    cout << endl;
    cout << endl;
#endif
}

/**
 * Method incorporate the expected change in loc position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_k.
 *
 * @param u The motion model of the last frame.  Ignored for the loc.
 * @return The expected change in loc position (x,y, xVelocity, yVelocity)
 */
EKF<Observation, MotionModel,
    LOC_EKF_DIMENSION, LOC_MEASUREMENT_DIMENSION>::StateVector
LocEKF::associateTimeUpdate(MotionModel u)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tUpdating Odometry of " << u << endl;
#endif

    // Calculate the assumed change in loc position
    // Assume no decrease in loc velocity
    StateVector deltaLoc(LOC_EKF_DIMENSION);
    const float h = getHEst();
    float sinh, cosh;
    sincosf(h, &sinh, &cosh);

    deltaLoc(0) = u.deltaF * cosh - u.deltaL * sinh;
    deltaLoc(1) = u.deltaF * sinh + u.deltaL * cosh;
    deltaLoc(2) = u.deltaR;

    A_k(0,2) =  0;//-u.deltaF * sinh - u.deltaL * cosh;
    A_k(1,2) =  0;//u.deltaF * cosh - u.deltaL * sinh;

    return deltaLoc;
}

/**
 * Method to deal with incorporating a loc measurement into the EKF
 *
 * @param z the measurement to be incorporated
 * @param H_k the jacobian associated with the measurement, to be filled out
 * @param R_k the covariance matrix of the measurement, to be filled out
 * @param V_k the measurement invariance
 *
 * @return the measurement invariance
 */
void LocEKF::incorporateMeasurement(Observation z,
                                    StateMeasurementMatrix &H_k,
                                    MeasurementMatrix &R_k,
                                    MeasurementVector &V_k)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tIncorporating measurement " << z << endl;
#endif
    int obsIndex;

    // Get the best fit for ambigious data
    // NOTE: this is only done, if useAmbiguous is turned to true
    if (z.getNumPossibilities() > 1) {
        obsIndex = findBestLandmark(&z);
        // No landmark is close enough, don't attempt to use one
        if (obsIndex < 0) {
            R_k(0,0) = DONT_PROCESS_KEY;
            return;
        }
    } else {
        obsIndex = 0;
    }

    if (z.getVisDistance() < USE_CARTESIAN_DIST) {

#ifdef DEBUG_LOC_EKF_INPUTS
        cout << "\t\t\tUsing cartesian " << endl;
#endif

        // Convert our sighting to cartesian coordinates
        MeasurementVector z_x(2);
        z_x(0) = z.getVisDistance() * cos(z.getVisBearing());
        z_x(1) = z.getVisDistance() * sin(z.getVisBearing());

        // Get expected values of the post
        const float x_b = z.getPointPossibilities()[obsIndex].x;
        const float y_b = z.getPointPossibilities()[obsIndex].y;
        MeasurementVector d_x(2);

        const float x = xhat_k_bar(0);
        const float y = xhat_k_bar(1);
        const float h = xhat_k_bar(2);

        float sinh, cosh;
        sincosf(h, &sinh, &cosh);

        d_x(0) = (x_b - x) * cosh + (y_b - y) * sinh;
        d_x(1) = -(x_b - x) * sinh + (y_b - y) * cosh;

        // Calculate invariance
        V_k = z_x - d_x;

        // Calculate jacobians
        H_k(0,0) = -cosh;
        H_k(0,1) = -sinh;
        H_k(0,2) = -(x_b - x) * sinh + (y_b - y) * cosh;

        H_k(1,0) = sinh;
        H_k(1,1) = -cosh;
        H_k(1,2) = -(x_b - x) * cosh - (y_b - y) * sinh;

        // Update the measurement covariance matrix
        R_k(0,0) = z.getDistanceSD();
        R_k(1,1) = z.getDistanceSD();

    } else {

#ifdef DEBUG_LOC_EKF_INPUTS
        cout << "\t\t\tUsing polar " << endl;
#endif

        // Get the observed range and bearing
        MeasurementVector z_x(2);
        z_x(0) = z.getVisDistance();
        z_x(1) = z.getVisBearing();

        // Get expected values of the post
        const float x_b = z.getPointPossibilities()[obsIndex].x;
        const float y_b = z.getPointPossibilities()[obsIndex].y;
        MeasurementVector d_x(2);

        const float x = xhat_k_bar(0);
        const float y = xhat_k_bar(1);
        const float h = xhat_k_bar(2);

        d_x(0) = static_cast<float>(hypot(x - x_b, y - y_b));
        d_x(1) = safe_atan2(y_b - y,
                            x_b - x) - h;
        d_x(1) = NBMath::subPIAngle(d_x(1));

        // Calculate invariance
        V_k = z_x - d_x;
        V_k(1) = NBMath::subPIAngle(V_k(1));

	//JHS: This is the measurement jacobian
	// it should be d(measure)/d(state)

	// e.g. H[0][0] is d(d_x(0))/d(x)
	// this is d/dx[Sqrt(x-x_b )^2+ (y - y_b)^2)] = 2*(x-x_b)/d(0)

	// e.g. H[1][0] is d(d_x(1))/d(x)
	// this is d/dx[atan(y_b-y / x_b-x)] = 2*(y-y_b)/(d(0)**2)
	// Just seems like some twos are missing... hmm

        // Calculate jacobians
        H_k(0,0) = (x - x_b) / d_x(0);
        H_k(0,1) = (y - y_b) / d_x(0);
        H_k(0,2) = 0;

        H_k(1,0) = (y_b - y) / (d_x(0)*d_x(0));
        H_k(1,1) = (x - x_b) / (d_x(0)*d_x(0));
        H_k(1,2) = -1;

        // Update the measurement covariance matrix
        R_k(0,0) = z.getDistanceSD();
        R_k(1,1) = z.getBearingSD();

#ifdef DEBUG_LOC_EKF_INPUTS
        cout << "\t\t\tR vector is" << R_k << endl;
        cout << "\t\t\tH vector is" << H_k << endl;
        cout << "\t\t\tV vector is" << V_k << endl;
        cout << "\t\t\t\td vector is" << d_x << endl;
        cout << "\t\t\t\t\tx est is " << x << endl;
        cout << "\t\t\t\t\ty est is " << y << endl;
        cout << "\t\t\t\t\th est is " << h << endl;
        cout << "\t\t\t\t\tx_b est is " << x_b << endl;
        cout << "\t\t\t\t\ty_b est is " << y_b << endl;
#endif
    }

    // Calculate the standard error of the measurement
    StateMeasurementMatrix newP = prod(P_k, trans(H_k));
    MeasurementMatrix se = prod(H_k, newP) + R_k;
    se(0,0) = sqrt(se(0,0));
    se(1,1) = sqrt(se(1,1));

    // Ignore observations based on standard error
    if ( se(0,0)*6.0f < abs(V_k(0))) {
#ifdef DEBUG_STANDARD_ERROR
        cout << "\t Ignoring measurement " << endl;
        cout << "\t Standard error is " << se << endl;
        cout << "\t Invariance is " << abs(V_k(0))*5 << endl;
#endif
        R_k(0,0) = DONT_PROCESS_KEY;
    }

}

/**
 * Given an observation with multiple possibilities, we return the observation
 * with the best possibility as the first element of the vector
 *
 * @param z The observation to be fixed
 */
int LocEKF::findBestLandmark(Observation *z)
{
    vector<PointLandmark> possiblePoints = z->getPointPossibilities();
    float minDivergence = 250.0f;
    int minIndex = -1;
    for (unsigned int i = 0; i < possiblePoints.size(); ++i) {
        float divergence = getDivergence(z, possiblePoints[i]);

        if (divergence < minDivergence) {
            minDivergence = divergence;
            minIndex = i;
        }
    }
    return minIndex;
}

/**
 * Get the divergence between an observation and a possible point landmark
 *
 * @param z The observation measurement to be examined
 * @param pt The possible point to check against
 *
 * @return The divergence value
 */
float LocEKF::getDivergence(Observation * z, PointLandmark pt)
{
    const float x = xhat_k_bar(0);
    const float y = xhat_k_bar(1);
    const float h = xhat_k_bar(2);
    float x_b_r = z->getVisDistance() * cos(z->getVisBearing());
    float y_b_r = z->getVisDistance() * sin(z->getVisBearing());

    float sinh, cosh;
    sincosf(h, &sinh, &cosh);

    float x_b  = (pt.x - x) * cosh + (pt.y - y) * sinh;
    float y_b = -(pt.x - x) * sinh + (pt.y - y) * cosh;

    //JHS: This line uses euclidean distance.
    //    you could include the uncert of the measurement (S_k = HPHt + R)
    //    and use mahalanobis distance that way
    return static_cast<float>(hypot(x_b_r - x_b, y_b_r - y_b));
}

/**
 * Method to ensure that uncertainty does not grow without bound
 */
void LocEKF::limitAPrioriUncert()
{
    // Check x uncertainty
    if(P_k_bar(0,0) > X_UNCERT_MAX) {
        P_k_bar(0,0) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k_bar(1,1) > Y_UNCERT_MAX) {
        P_k_bar(1,1) = Y_UNCERT_MAX;
    }
    // Check h uncertainty
    if(P_k_bar(2,2) > H_UNCERT_MAX) {
        P_k_bar(2,2) = H_UNCERT_MAX;
    }

    // We don't want any covariance values getting too large
    for (unsigned int i = 0; i < numStates; ++i) {
        for (unsigned int j = 0; j < numStates; ++j) {
            if(P_k(i,j) > X_UNCERT_MAX) {
                P_k(i,j) = X_UNCERT_MAX;
            }
        }
    }

}

/**
 * Method to ensure that uncertainty does not grow or shrink without bound
 */
void LocEKF::limitPosteriorUncert()
{
    // Check x uncertainty
    if(P_k(0,0) < X_UNCERT_MIN) {
        P_k(0,0) = X_UNCERT_MIN;
        P_k_bar(0,0) = X_UNCERT_MIN;
    }
    // Check y uncertainty
    if(P_k(1,1) < Y_UNCERT_MIN) {
        P_k(1,1) = Y_UNCERT_MIN;
        P_k_bar(1,1) = Y_UNCERT_MIN;

    }
    // Check h uncertainty
    if(P_k(2,2) < H_UNCERT_MIN) {
        P_k(2,2) = H_UNCERT_MIN;
        P_k_bar(2,2) = H_UNCERT_MIN;

    }
    // Check x uncertainty
    if(P_k(0,0) > X_UNCERT_MAX) {
        P_k(0,0) = X_UNCERT_MAX;
        P_k_bar(0,0) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k(1,1) > Y_UNCERT_MAX) {
        P_k(1,1) = Y_UNCERT_MAX;
        P_k_bar(1,1) = Y_UNCERT_MAX;
    }
    // Check h uncertainty
    if(P_k(2,2) > H_UNCERT_MAX) {
        P_k(2,2) = H_UNCERT_MAX;
        P_k_bar(2,2) = H_UNCERT_MAX;
    }
}

/**
 * Method to use the estimate ellipse to intelligently clip the pose estimate
 * of the robot using the information of the uncertainty ellipse.
 */
void LocEKF::clipRobotPose()
{
    // Limit our X estimate
    if (xhat_k(0) > X_EST_MAX) {
        StateVector v(numStates);
        v(0) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - X_EST_MAX) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(0) < X_EST_MIN) {
        StateVector v(numStates);
        v(0) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }

    // Limit our Y estimate
    if (xhat_k(1) < Y_EST_MIN) {
        StateVector v(numStates);
        v(1) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(1) > Y_EST_MAX) {
        StateVector v(numStates);
        v(1) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - Y_EST_MAX) /
            inner_prod(v, prod(P_k,v));

    }
}



// THIS METHOD IS NOT CURRENTLY BEING USED NOR HAS IT BEEN ADEQUATELY TESTED
// CONSULT NUbots Team Report 2006 FOR MORE INFORMATION
/**
 * Detect if we are in a deadzone and apply the correct changes to keep the pose
 * estimate from drifting
 *
 * @param R The measurement noise covariance matrix
 * @param innovation The measurement divergence
 * @param CPC Predicted measurement variance
 * @param EPS Size of the deadzone
 */
void LocEKF::deadzone(float &R, float &innovation,
                      float CPC, float eps)
{
    float invR = 0.0;
    // Not in a deadzone
    if ((eps < 1.0e-08) || (CPC < 1.0e-08) || (R < 1e-08)) {
        return;
    }

    if ( abs(innovation) > eps) {
        // Decrease the covariance, so that it effects the estimate more
        invR=( abs(innovation) / eps-1) / CPC;
    } else {
        // Decrease the innovations, so that it doesn't effect the estimate
        innovation=0.0;
        invR = 0.25 / (eps*eps) - 1.0/ CPC;
    }

    // Limit the min Covariance value
    if (invR<1.0e-08) {
        invR=1e-08;
    }
    // Set the covariance to be the adjusted value
    if ( R < 1.0/invR ) {
        R=1.0/invR;
    }
}
