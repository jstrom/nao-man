
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include "MotionInterface.h"
using boost::shared_ptr;

void MotionInterface::setNextWalkCommand(const WalkCommand *command){
    switchboard->sendMotionCommand(command);
}
void
MotionInterface::sendStepCommand(const shared_ptr<StepCommand> command){
    switchboard->sendMotionCommand(command);
}
void
MotionInterface::sendDistanceCommand(const shared_ptr<DistanceCommand> command){
    switchboard->sendMotionCommand(command);
}
void MotionInterface::enqueue(const BodyJointCommand *command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::enqueue(const HeadJointCommand *command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::setHead(const SetHeadCommand *command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::sendFreezeCommand(const shared_ptr<FreezeCommand> command){
    switchboard->sendMotionCommand(command);
}
void MotionInterface::sendFreezeCommand(const shared_ptr<UnfreezeCommand> command){
    switchboard->sendMotionCommand(command);
}


void MotionInterface::stopBodyMoves() {
	switchboard->stopBodyMoves();
}

void MotionInterface::stopHeadMoves() {
    switchboard->stopHeadMoves();
}

void MotionInterface::resetWalkProvider(){
    switchboard->resetWalkProvider();
}

void MotionInterface::resetScriptedProvider(){
    switchboard->resetScriptedProvider();
}

float MotionInterface::getHeadSpeed() {
    return DUMMY_F;
}

void MotionInterface::setWalkConfig ( float pMaxStepLength, float pMaxStepHeight,
				      float pMaxStepSide, float pMaxStepTurn,
				      float pZmpOffsetX, float pZmpOffsetY) {
}

void MotionInterface::setWalkArmsConfig ( float pShoulderMedian,
					  float pShoulderApmlitude,
					  float pElbowMedian,
					  float pElbowAmplitude) {
}

void MotionInterface::setWalkExtraConfig( float pLHipRollBacklashCompensator,
					  float pRHipRollBacklashCompensator,
					  float pHipHeight,
					  float pTorsoYOrientation) {
}

void MotionInterface::setGait(const shared_ptr<Gait> command){
    switchboard->sendMotionCommand(command);
}

void MotionInterface::setSupportMode(int pSupportMode) {
}

int MotionInterface::getSupportMode() {
    return DUMMY_I;
}


void MotionInterface::setBalanceMode(int pBalanceMode) {
}

int MotionInterface::getBalanceMode() {
    return DUMMY_I;
}
