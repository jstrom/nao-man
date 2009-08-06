#ifndef DistanceCommand_h
#define DistanceCommand_h

class DistanceCommand : public MotionCommand {
public:
	DistanceCommand(const float _x_mm, const float _y_mm, const float _theta_rad)
		: MotionCommand(MotionConstants::DIST),
		  x_mm(_x_mm),y_mm(_y_mm),theta_rad(_theta_rad)
		{ setChainList(); }
public:
	const float x_mm, y_mm, theta_rad; // distance in mm, angle in rad

protected:
	virtual void setChainList() {
        chainList.assign(MotionConstants::DIST_CHAINS,
                         MotionConstants::DIST_CHAINS +
                         MotionConstants::DIST_NUM_CHAINS);
    }
public:
    friend std::ostream& operator<< (std::ostream &o, const DistanceCommand &w)
        {
            return o << "DistCommand("
                     << w.x_mm << "," << w.y_mm << ","
                     << w.theta_rad << ") ";
        }
};

#endif
