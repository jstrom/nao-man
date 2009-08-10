//TODO: look into replacing DCM->getTime with local time tracking

#include "NaoEnactor.h"
#include <iostream>
using namespace std;
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;
#include "NBMath.h"

#include <boost/bind.hpp>
using namespace boost;

#include "BasicWorldConstants.h"
#include "ALNames.h"
using namespace ALNames;

#include "motionconfig.h"

#include "Kinematics.h"
using Kinematics::jointsMaxVelNoLoad;

void staticPostSensors(NaoEnactor * n) {
    n->postSensors();
}
void staticSendCommands(NaoEnactor * n) {
    n->sendCommands();
}

NaoEnactor::NaoEnactor(boost::shared_ptr<Sensors> s,
                       boost::shared_ptr<Transcriber> t,
                       AL::ALPtr<AL::ALBroker> _pbroker)
    : MotionEnactor(), broker(_pbroker), sensors(s),
      transcriber(t),
      jointValues(Kinematics::NUM_JOINTS,0.0f),  // current values of joints
      motionValues(Kinematics::NUM_JOINTS,0.0f),  // commands sent to joints
      lastMotionCommandAngles(Kinematics::NUM_JOINTS,0.0f),
      lastMotionHardness(Kinematics::NUM_JOINTS,0.0f)

{
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }


    initDCMAliases();
    initDCMCommands();

    //Assumes the constructor of the Transcriber is updating these
    //before this constructor is called
    lastMotionCommandAngles = sensors->getMotionBodyAngles();

    // connect to dcm using the static methods declared above
    broker->getProxy("DCM")->getModule()->onPostProcess()
        .connect(bind(staticPostSensors,this));
    broker->getProxy("DCM")->getModule()->onPreProcess()
        .connect(bind(staticSendCommands,this));
}

const int NaoEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float NaoEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f /
                                                NaoEnactor::MOTION_FRAME_RATE;
const float NaoEnactor::MOTION_FRAME_LENGTH_S = 1.0f /
                                                NaoEnactor::MOTION_FRAME_RATE;

void NaoEnactor::sendCommands(){

    if(!switchboard){
        if(switchboardSet)
            cout<< "Caution!! Switchboard is null, skipping NaoEnactor"<<endl;
        return;
    }

    sendJoints();
    sendHardness();
    sendUltraSound();

}

void NaoEnactor::sendJoints() {
    // Get the angles we want to go to this frame from the switchboard
    motionValues = switchboard->getNextJoints();

    // Get most current joint values possible for performing checks
    jointValues = sensors->getBodyAngles();//Need these for velocity checks

    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
#ifdef DEBUG_ENACTOR_JOINTS
        cout << "result of joint " << i << " is " << motionValues[i] << endl;
#endif
        //returns the fastest safe value if requested movement is too fast
        const float clipped_angle  = SafetyCheck(jointValues[i],
                                                 motionValues[i],
                                                 lastMotionCommandAngles[i],
                                                 i);
        //save the clipped value for next time
        motionValues[i] =  clipped_angle;
        joint_command[5][i][0] = clipped_angle;

        //Save the values we sent this cycle for next cycle
        lastMotionCommandAngles[i] = clipped_angle;
    }

    //sendHardness();

    // Send the array with a 25 ms delay. This delay removes the jitter.
    // Note: I tried 20 ms and it didn't work quite as well. Maybe there is
    // a value in between that works though. Will look into it.
    joint_command[4][0] = dcmProxy->getTime(20);
#ifndef NO_ACTUAL_MOTION
    try {
        dcmProxy->setAlias(joint_command);
    } catch(AL::ALError& a) {
        std::cout << "dcm value set error " << a.toString() << std::endl;
    }
#endif
}


void NaoEnactor::sendHardness(){
    motionHardness = switchboard->getNextStiffness();

    bool diffStiff = false;
    //TODO!!! ONLY ONCE PER CHANGE!sends the hardness command to the DCM
    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
        static float hardness =0.0f;
        if (hardness != -1.0f) //-1: yet to be implemented by AL decoupled mode
            hardness = NBMath::clip(motionHardness[i],0.0f,1.0f);

        //sets the value for hardness
        //hardness_command[5][i].arraySetSize(1);
        if(lastMotionHardness[i] != hardness)
            diffStiff = true;
        hardness_command[5][i][0] = hardness;

        //store for next time
        lastMotionHardness[i] = hardness;
    }
    hardness_command[4][0] = dcmProxy->getTime(0);

    if(!diffStiff)
        return;

 // #ifdef ROBOT_NAME_zaphod
 //     // turn off broken neck
 //    hardness_command[5][Kinematics::HEAD_YAW][0] = -1.0f;
 //    hardness_command[5][Kinematics::HEAD_PITCH][0] = -1.0f;
 // #endif

#ifndef NO_ACTUAL_MOTION
    try {
        dcmProxy->setAlias(hardness_command);
    } catch(AL::ALError& a) {
        std::cout << "DCM Hardness set error" << a.toString() << "    "
                  << hardness_command.toString() << std::endl;
    }
#endif
}

void NaoEnactor::sendUltraSound(){
    //The US sensors only resond on a 250 ms cycle -- TODO/HACK is this right??
    static const int US_FRAME_RATE = 4;
    //We need to skip approximately 12.5 (13) motion frames before sending
    //another command
    static const int US_IDLE_SKIP = MOTION_FRAME_RATE /  US_FRAME_RATE + 1;

    static int counter = 0;
    static int mode = 0;
    try {
        if (counter == US_IDLE_SKIP){
            // This is testing code which sends a new value to the actuator
            // every 13 motion frames (250ms). It also cycles the
            //ultrasound mode between the four possibilities. See docs.
            mode = mode % 4;

            // the current mode - changes every 5 frames
            us_command[2][0][0] = static_cast<float>(mode);
            us_command[2][0][1] = dcmProxy->getTime(250);

            // set the mode only once every 4 frames because it responds slowly anyway
            // but this rate needs to change if we don't run vision at 15 fps
            dcmProxy->set(us_command);

            //Reset the counter after each command is sent
            counter = 0;
            mode+=1;

        }else
            counter++;

    } catch(AL::ALError &e) {
        cout << "Failed to set ultrasound mode. Reason: "
             << e.toString() << endl;
    }

}

//makes sure that we don't tell the motors to move faster than they can
//the DCM takes care of trimming too large/ too small of values
float NaoEnactor::SafetyCheck(float currentVal, float toCheck, float motionAngle, int i){

    //We need to clip angles twice. Why? Because the sensor values are between
    //20 and 40 ms old, so we can't strictly use the sensor reports to clip
    // the velocity.
    //We also can't just use the internaly held motion angles because these
    // could be out of sync with reality, and thus allow us to send bad
    // commands.
    //As a balance, we clip both with respect to sensor readings which we
    //ASSUME are 40 ms old (even if they are newer), AND we clip with respect
    //to the internally held motion command angles, which ensures that we'
    //arent sending commands which are in general too fast for the motors.
    //For the sensor angles, we clip with TWICE the max speed.

    const float absDiffInRad = fabs(currentVal - toCheck);
    const float allowedMotionDiffInRad = jointsMaxVelNoLoad[i];
    const float allowedSensorDiffInRad = allowedMotionDiffInRad*6.0f;
    const float clippedMotionVal =
        NBMath::clip(toCheck, motionAngle - allowedMotionDiffInRad,
                              motionAngle + allowedMotionDiffInRad);

    const float clippedSensorVal =
        NBMath::clip(clippedMotionVal, currentVal - allowedSensorDiffInRad,
                                       currentVal + allowedSensorDiffInRad);

#ifdef DEBUG_ENACTOR_CLIPPING
    const float difference = abs(currentVal - toCheck);
    const float motionDiff = abs(currentVal - motionAngle);
    if ( difference > allowedSensorDiffInRad )
        cout << "Clipped " << Kinematics::JOINT_STRINGS[i]
             << ". Difference due to SENSORS was " << difference << endl
             << "  Reduction limitation is "<<jointsMaxVelNoLoad[i]<<"/20ms"<<endl;
    if( motionDiff > allowedMotionDiffInRad ){
        cout << "Clipped " << Kinematics::JOINT_STRINGS[i]
             << "  Difference due to MOTION was " <<motionDiff <<endl
             << "  Reduction limitation is "<<jointsMaxVelNoLoad[i]<<"/20ms"<<endl;
    }
#endif

    return clippedSensorVal;
}

void NaoEnactor::postSensors(){
    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    sensors->setMotionBodyAngles(motionValues);
    transcriber->postMotionSensors();

    if(!switchboard){
        return;
    }
    //We only want the switchboard to start calculating new joints once we've
    //updated the latest sensor information into Sensors
    switchboard->signalNextFrame();
}

/**
 * Creates the appropriate aliases with the DCM
 */
void NaoEnactor::initDCMAliases(){
    ALValue positionCommandsAlias;
    positionCommandsAlias.arraySetSize(3);
    positionCommandsAlias[0] = string("AllActuatorPosition");
    positionCommandsAlias[1].arraySetSize(Kinematics::NUM_JOINTS);

    ALValue hardCommandsAlias;
    hardCommandsAlias.arraySetSize(3);
    hardCommandsAlias[0] = string("AllActuatorHardness");
    hardCommandsAlias[1].arraySetSize(Kinematics::NUM_JOINTS);

    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++){
        positionCommandsAlias[1][i] = jointsP[i];
        hardCommandsAlias[1][i] = jointsH[i];
    }

    dcmProxy->createAlias(positionCommandsAlias);
    dcmProxy->createAlias(hardCommandsAlias);
}


void NaoEnactor::initDCMCommands(){
    //set-up the array for sending hardness commands to DCM
    //ALValue hardness_command;
    hardness_command.arraySetSize(6);
    hardness_command[1] = string("ClearAll");
    hardness_command[2] = string("time-separate");
    hardness_command[3] = 0; //importance level
    hardness_command[4].arraySetSize(1); //list of time to send commands
    hardness_command[5].arraySetSize(Kinematics::NUM_JOINTS);

    //sets the hardness for all the joints
    hardness_command[0] = string("AllActuatorHardness");
    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
        //sets default hardness for each joint, which will never be sent
        hardness_command[5][i].arraySetSize(1);
        hardness_command[5][i][0] = 0.0;
    }

    //set-up the array for sending commands to DCM
    joint_command.arraySetSize(6);
    joint_command[1] = string("ClearAll");
    joint_command[2] = string("time-separate");
    joint_command[3] = 0; //importance level
    joint_command[4].arraySetSize(1); //list of time to send commands
    joint_command[5].arraySetSize(Kinematics::NUM_JOINTS);

    //sets the hardness for all the joints
    joint_command[0] = string("AllActuatorPosition");
    for (unsigned int i = 0; i<Kinematics::NUM_JOINTS; i++) {
        //sets default value for each joint, which will never be sent
        joint_command[5][i].arraySetSize(1);
        joint_command[5][i][0] = 0.0;
    }

    us_command.arraySetSize(3);
    us_command[0] = string("US/Actuator/Value");
    us_command[1] = string("Merge");
    us_command[2].arraySetSize(1);
    us_command[2][0].arraySetSize(2);
//      // the current mode - changes every 5 frames
//     us_command[2][0][0] = 0.0f; //static_cast<float>(setMode);
//     us_command[2][0][1] = 0.0f; //dcm->getTime(250);


}

