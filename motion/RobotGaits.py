import man.motion as motion


"""
TODO: Note: It would probably be nice to store each separate gait in it's 
own file -- say motion/gaits/MyGait.py 
"""

WALKING = 1.0
NON_WALKING = 0.0

STANCE_CONFIG = (31.00, # CoM height
                 1.45,  # Forward displacement of CoM
                 10.0,  # Horizontal distance between feet
                 3.0,   # Body angle around y axis
                 0.0,   # Angle between feet
                 0.1)   # Time to transition to/from this stance


MARVIN_STANCE_CONFIG = (31.00, # CoM height
                        1.45,  # Forward displacement of CoM
                        10.0,  # Horizontal distance between feet
                        6.0,   # Body angle around y axis
                        0.0,   # Angle between feet
                        0.1)   # Time to transition to/from this stance

STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.9,  # stepHeight
               0.0,  # step lift
               7.0,  # max x speed
               -7.0,  # max x speed
               3.5,  # max y speed
               20.0, # max theta speed()
               7.0,  # max x accel
               7.0,  # max y accel
               20.0, # max theta speed()
               WALKING)  # walking gait = true


MARVIN_STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.9,  # stepHeight
               0.0,  # step lift
               7.0,  # max x speed
               -7.0,  # max x speed
               3.5,  # max y speed
               20.0, # max theta speed()
               7.0,  # max x accel
               7.0,  # max y accel
               20.0, # max theta speed()
               20.0,  # max theta speed()
                      WALKING)#walk gait = true

STATIONARY_STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.0,  # stepHeight
               0.0,  # step lift
               0.0,  # max x speed
               0.0,  # max x speed
               0.0,  # max y speed
               0.0, # max theta speed()
               0.0,  # max x acc
               0.0,  # max y acc
               0.0, # max theta acc()
               NON_WALKING) # walking gait = false



ZMP_CONFIG = (0.0,  # footCenterLocX
              0.4,  # zmp static percentage
              0.4,  # left zmp off
              0.4,  # right zmp off
              0.01,  # strafe zmp offse
              6.6)   # turn zmp offset


MARVIN_ZMP_CONFIG = (0.0,  # footCenterLocX
              0.3,  # zmp static percentage
              0.5,  # left zmp off
              0.5,  # right zmp off
              0.01,  # strafe zmp offse
              6.6)   # turn zmp offset


JOINT_HACK_CONFIG = (5.5, # left swing hip roll addition
                     5.5) # right swing hip roll addition

MARVIN_JOINT_HACK_CONFIG = (5.5, # left swing hip roll addition
                            5.5) # right swing hip roll addition

NEW_SENSOR_CONFIG = (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
                     0.1,  # angle X scale (gamma)
                     0.20,  # angle Y scale (gamma)
                     150.00,  # X spring constant k (kg/s^2)
                     250.00,  # Y spring constant k (kg/s^2)
                     15.0,   # max angle X (compensation)
                     15.0,   # max angle Y
                     75.0)   # max angle vel (change in compensation)


OLD_SENSOR_CONFIG = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                     0.5,  # angle X scale (gamma)
                     0.5,  # angle Y scale (gamma)
                     0.00,  # spring constant k (kg/s^2)
                     0.00,  # spring constant k (kg/s^2)
                     15.0,   # max angle X (compensation)
                     15.0,   # max angle Y
                     75.0)   # max angle vel (change in compensation)


MARVIN_SENSOR_CONFIG = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                        0.5,  # angle X scale (gamma)
                        0.3,  # angle Y scale (gamma)
                        0.00,  # spring constant k (kg/s^2)
                        0.00,  # spring constant k (kg/s^2)
                        7.0,   # max angle X (compensation)
                        3.0,   # max angle Y
                        45.0)   # max angle vel (change in compensation)


CUR_SENSOR_CONFIG = OLD_SENSOR_CONFIG

STIFFNESS_CONFIG = (0.85, #hipStiffness
                    0.3,  #kneeStiffness
                    0.4,  #anklePitchStiffness
                    0.3,  #ankleRollStiffness
                    0.1,  #armStiffness
                    0.1)  #arm pitch

ODO_CONFIG = (1.0,   #xOdoScale
              1.0,   #yOdoScale
              1.0)   #thetaOdoScale

ARM_CONFIG = (0.0,)   #armAmplitude (degs)


#Put together all the parts to make a gait - ORDER MATTERS!
NEW_GAIT = motion.GaitCommand(MARVIN_STANCE_CONFIG,
            STEP_CONFIG,
            ZMP_CONFIG,
            JOINT_HACK_CONFIG,
            MARVIN_SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)
MARVIN_NEW_GAIT = motion.GaitCommand(MARVIN_STANCE_CONFIG,
                                     MARVIN_STEP_CONFIG,
                                     MARVIN_ZMP_CONFIG,
                                     MARVIN_JOINT_HACK_CONFIG,
                                     MARVIN_SENSOR_CONFIG,
                                     STIFFNESS_CONFIG,
                                     ODO_CONFIG,
                                     ARM_CONFIG)

DUCK_SENSOR =  (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
               0.08,  # angle X scale (gamma)
               0.08,  # angle Y scale (gamma)
               100.0,  # X spring constant k (kg/s^2)
               100.0,  # Y spring constant k (kg/s^2)
               7.0,   # max angle X (compensation)
               7.0,   # max angle Y
               45.0)   # max angle vel (change in compensation)
DUCK_STANCE = (31.00, # CoM height
               -0.5,  # Forward displacement of CoM
               10.0,  # Horizontal distance between feet
               10.0,   # Body angle around y axis
               80.0,   # Angle between feet
               1.0)   # Time to transition to/from this stance
DUCK_ZMP = (0.0,  # footCenterLocX
            0.4,  # zmp static percentage
            0.4,  # left zmp off
            0.4,  # right zmp off
            0.01,  # strafe zmp offse
            6.6)   # turn zmp offset
DUCK_STEP = (0.4, # step duration
             0.25,  # fraction in double support
             1.3,  # stepHeight
             -5.0,  # step lift
             7.0,  # max x speed
             -7.0,  # min x speed
             3.5,  # max y speed
             5.0, # max theta speed()
             7.0,  # max x accel
             7.0,  # max y accel
             20.0, # max theta speed()
             WALKING)  # walking gait = true

DUCK_GAIT = motion.GaitCommand(DUCK_STANCE,
            STEP_CONFIG,
            ZMP_CONFIG,
            JOINT_HACK_CONFIG,
            DUCK_SENSOR,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)



WEBOTS_STEP_CONFIG = (0.4, # step duration
                      0.25,  # fraction in double support
                      1.1,  # stepHeight
                      0.0,  # step lift
                      10.0,  # max x speed
                      -6.0,  # max x speed
                      5.0,  # max y speed
                      30.0,  # max theta speed()
                      5.0,  # max x acc
                      5.0,  # max y acc
                      20.0,  # max theta acc()
                      WALKING)

WEBOTS_ZMP_CONFIG =(0.0,  # footCenterLocX
                    0.4,  # zmp static percentage
                    0.0,  # left zmp off
                    0.0,  # right zmp off
                    0.01,  # strafe zmp offset (no units)
                    6.6)   # turn zmp off    ""
WEBOTS_HACK_CONFIG = (0.0, # left swing hip roll addition
                      0.0) # right swing hip roll addition

WEBOTS_GAIT=motion.GaitCommand(STANCE_CONFIG,
            WEBOTS_STEP_CONFIG,
            WEBOTS_ZMP_CONFIG,
            WEBOTS_HACK_CONFIG,
            CUR_SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)

WEBOTS_GAIT2=motion.GaitCommand(STANCE_CONFIG,
            STATIONARY_STEP_CONFIG,
            WEBOTS_ZMP_CONFIG,
            WEBOTS_HACK_CONFIG,
            CUR_SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)


########## MEDIUM GAIT #################
MARVIN_MED_STANCE = (31.00, # CoM height
              1.45,  # Forward displacement of CoM
              10.0,  # Horizontal distance between feet
              6.0,   # Body angle around y axis
              0.0,   # Angle between feet
              0.1)   # Time to transition to/from this stance

MED_STANCE = (31.00, # CoM height
              1.45,  # Forward displacement of CoM
              10.0,  # Horizontal distance between feet
              5.0,   # Body angle around y axis
              0.0,   # Angle between feet
              0.1)   # Time to transition to/from this stance


MARVIN_MED_STEP = (0.4, # step duration
            0.25,  # fraction in double support
            0.9,  # stepHeight
            -5.0,  # step lift
            10.0,  # max x speed
            -5.0,  # max x speed
            5.0,  # max y speed
            30.0, # max theta speed()
            7.0,  # max x accel
            7.0,  # max y accel
            20.0, # max theta speed()
            WALKING)#walk gait = true

MED_STEP = (0.4, # step duration
            0.25,  # fraction in double support
            1.1,  # stepHeight
            -5.0,  # step lift
            10.0,  # max x speed
            -5.0,  # max x speed
            5.0,  # max y speed
            30.0, # max theta speed()
            7.0,  # max x accel
            7.0,  # max y accel
            20.0, # max theta speed()
            20.0,  # max theta speed()
            WALKING)#walk gait = true

MARVIN_MED_ZMP = (0.0,  # footCenterLocX
           0.3,  # zmp static percentage
           0.4,  # left zmp off
           0.4,  # right zmp off
           0.01,  # strafe zmp offse
           7.6)   # turn zmp offset

MED_ZMP = (0.0,  # footCenterLocX
           0.3,  # zmp static percentage
           0.5,  # left zmp off
           0.5,  # right zmp off
           0.01,  # strafe zmp offse
           7.6)   # turn zmp offset


MED_SENSOR =  (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
               0.08,  # angle X scale (gamma)
               0.08,  # angle Y scale (gamma)
               100.0,  # X spring constant k (kg/s^2)
               100.0,  # Y spring constant k (kg/s^2)
               7.0,   # max angle X (compensation)
               7.0,   # max angle Y
               45.0)   # max angle vel (change in compensation)

MED_HACK = JOINT_HACK_CONFIG
MED_STIFFNESS = STIFFNESS_CONFIG
MED_ODO= ODO_CONFIG
MED_ARM=ARM_CONFIG

MARVIN_MEDIUM_GAIT = motion.GaitCommand(MARVIN_MED_STANCE,
                                        MARVIN_MED_STEP,
                                        MARVIN_MED_ZMP,
                                        MED_HACK,
                                        MED_SENSOR,
                                        MED_STIFFNESS,
                                        MED_ODO,
                                        MED_ARM)
MEDIUM_GAIT = motion.GaitCommand(MED_STANCE,
                                 MED_STEP,
                                 MED_ZMP,
                                 MED_HACK,
                                 MED_SENSOR,
                                 MED_STIFFNESS,
                                 MED_ODO,
                                 MED_ARM)
##### END MEDIUM GAIT ####



########## FAST GAIT #################
FAST_STANCE = (31.00, # CoM height
              1.45,  # Forward displacement of CoM
              10.0,  # Horizontal distance between feet
              3.0,   # Body angle around y axis
              0.0,   # Angle between feet
              0.1)   # Time to transition to/from this stance
FAST_STEP = (0.5, # step duration
            0.2,  # fraction in double support
            1.5,  # stepHeight
            -5.0,  # step lift
            15.0,  # max x speed
            -5.0,  # max x speed
            7.5,  # max y speed
            45.0, # max theta speed()
            7.0,  # max x accel
            7.0,  # max y accel
            20.0, # max theta speed()
            WALKING)#walk gait = true

FAST_ZMP = (0.0,  # footCenterLocX
           0.3,  # zmp static percentage
           0.45,  # left zmp off
           0.45,  # right zmp off
            0.01,  # strafe zmp offse
           6.6)   # turn zmp offset


FAST_SENSOR =  (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
               0.06,  # angle X scale (gamma)
               0.08,  # angle Y scale (gamma)
               250.0,  # X spring constant k (kg/s^2)
               100.0,  # Y spring constant k (kg/s^2)
               7.0,   # max angle X (compensation)
               7.0,   # max angle Y
               45.0)   # max angle vel (change in compensation)

FAST_HACK = (6.5,6.5)
FAST_STIFFNESS = (0.85, #hipStiffness
                  0.3,  #kneeStiffness
                  0.4,  #anklePitchStiffness
                  0.3,  #ankleRollStiffness
                  0.1,  #armStiffness
                  0.5)  #arm pitch
FAST_ODO= ODO_CONFIG
FAST_ARM= (10,)

FAST_GAIT = motion.GaitCommand(FAST_STANCE,
                                 FAST_STEP,
                                 FAST_ZMP,
                                 FAST_HACK,
                                 FAST_SENSOR,
                                 FAST_STIFFNESS,
                                 FAST_ODO,
                                 FAST_ARM)
##### END FAST GAIT ####

########## COM GAIT #################
COM_STANCE = (31.00, # CoM height
              0.0,  # Forward displacement of CoM
              10.0,  # Horizontal distance between feet
              3.0,   # Body angle around y axis
              0.0,   # Angle between feet
              0.1)   # Time to transition to/from this stance


COM_STEP = (0.5, # step duration
            0.2,  # fraction in double support
            2.0,  # stepHeight
            0.0,  # step lift
            20.0,  # max x speed
            -4.0,  # max x speed
            5.0,  # max y speed
            30.0, # max theta speed()
            5.0,  # max x accel
            5.0,  # max y accel
            20.0, # max theta speed()
            WALKING)#walk gait = true


COM_ZMP = (2.0,  # footCenterLocX
           0.2,  # zmp static percentage
           0.1,  # left zmp off
           0.1,  # right zmp off
           0.01,  # strafe zmp offse
           7.6)   # turn zmp offset


COM_SENSOR =  (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
               0.00,  # angle X scale (gamma)
               0.00,  # angle Y scale (gamma)
               100.0,  # X spring constant k (kg/s^2)
               100.0,  # Y spring constant k (kg/s^2)
               7.0,   # max angle X (compensation)
               7.0,   # max angle Y
               45.0)   # max angle vel (change in compensation)

COM_HACK = (5.5,5.5,)#JOINT_HACK_CONFIG
COM_STIFFNESS = (0.85, #hipStiffness
                 0.6,  #kneeStiffness
                 0.6,  #anklePitchStiffness
                 0.6,  #ankleRollStiffness
                 0.1,  #armStiffness
                 0.5)  #arm pitch
COM_ODO=ODO_CONFIG
COM_ARM=ARM_CONFIG

# COM_GAIT = motion.GaitCommand(COM_STANCE,
#                               COM_STEP,
#                               COM_ZMP,
#                               COM_HACK,
#                               COM_SENSOR,
#                               COM_STIFFNESS,
#                               COM_ODO,
#                               COM_ARM)
##### END COM GAIT ####



############# DEFAULT GAIT ASSIGNMENTS ##################


CUR_GAIT = MEDIUM_GAIT
CUR_DRIBBLE_GAIT = DUCK_GAIT
CUR_SLOW_GAIT = NEW_GAIT

MARVIN_CUR_GAIT = MARVIN_MEDIUM_GAIT
MARVIN_CUR_SLOW_GAIT = MARVIN_NEW_GAIT


TRILLIAN_GAIT = CUR_GAIT
ZAPHOD_GAIT   = CUR_GAIT
SLARTI_GAIT   = CUR_GAIT
MARVIN_GAIT   = CUR_GAIT

TRILLIAN_DRIBBLE_GAIT = CUR_DRIBBLE_GAIT
ZAPHOD_DRIBBLE_GAIT   = CUR_DRIBBLE_GAIT
SLARTI_DRIBBLE_GAIT   = CUR_DRIBBLE_GAIT
MARVIN_DRIBBLE_GAIT   = CUR_DRIBBLE_GAIT

TRILLIAN_SLOW_GAIT = CUR_SLOW_GAIT
ZAPHOD_SLOW_GAIT   = CUR_SLOW_GAIT
SLARTI_SLOW_GAIT   = CUR_SLOW_GAIT
MARVIN_SLOW_GAIT   = CUR_SLOW_GAIT


