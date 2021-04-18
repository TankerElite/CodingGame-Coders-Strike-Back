#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>

using namespace std;

//Define Magic Numbers
const int BOOSTMOD = 50;
const int MAXSPEED = 100;
const int MAGIC_HEURISTIC = 3;

//Use atan(1)*4 to obtain relatively precise value of PI (3.1415...)
const double PI = atan(1)*4;


double calculatedDistance(tuple<int, int> pos1, tuple <int, int> pos2) {
    //Calculate distance on 2D plane
    double yComponent = pow( get<1>(pos2) - get<1>(pos1) , 2);
    double xComponent = pow( get<0>(pos2) - get<0>(pos1) , 2);

    return sqrt( yComponent + xComponent);
}


class Pod {
    //Racing Pod Class
    private:
    //Internal values of the Pod class
        int id;

        tuple<int, int> pos;

        int speed_x;
        int speed_y;

        int checkpoint;
        int lap;

        int angle;

        int driver;
        int boosts;
        int lastLeader;
        vector<tuple<int, int, int>> checkpointVector;

        tuple<int, int> lastpos;
    
    public:
    //Public values for the Pod class
        Pod() :
            id(0)
            , pos(0, 0)
            , speed_x(0)
            , speed_y(0)
            , angle(0)
            , checkpoint(0)
            , lap(0)
            , driver(1)
            , boosts(1)
            , lastpos(0, 0)
        {
        }

        int getID() {
            //Get ID of Pod
            return this -> id;
        }
    
        void updatePos(int x, int y){
            //Update Position of pod
            get<0>(this->pos) = x;
            get<1>(this->pos) = y;
        }

        void setID(int idIn){
            //Update ID of pod
            this->id = idIn;
        }

        int getPos(char d){
            //Get pod position on X or Y axis, depending on input char d.
            if (d == 'x'){
                //requested position is x
                return get<0>(this->pos);
            } else if (d == 'y') {
                //requested position is y
                return get<1>(this->pos);
            }
            //We should never hit this
            cerr << "Something has gone critically wrong - check Pod class getPos function." << endl;
            return get<1>(this->pos);
        }

        void feedCheckpointVector(vector<tuple<int, int, int>> inputcheckpointVector) {
            //give Pod access to it's local copy of the checkpointVector for easier data manipulation within the Bot
            this -> checkpointVector = inputcheckpointVector;
        }

        void updateSpeed(int x, int y){
            //Update Position of Pod
            this->speed_x = x;
            this->speed_y = y;
        }

        void updateAngle(int absangle) {
            //Update Absolute angle of pod
            this->angle = absangle;
        }

        void updateDriverAIStatus(int aiNumber) {
            //Update Driver AI of Pod, 1 = Driver, 0 = Interceptor
            this->driver = aiNumber;
            //cerr << "Driver AI status updated." << endl;
        }

        int getSpeed(char d){
             //Get pod speed on X or Y axis, depending on input char d.
            if (d =='x'){
                //requested speed is x-speed
                return this -> speed_x;
            } else if (d == 'y') {
                //requested speed is y-speed
                return this -> speed_y;
            }
            //We should never hit this
            cerr << "Something has gone critically wrong - check Pod class getSpeed function." << endl;
            return this -> speed_y;
        }

        void updateCheckpoint(int checkpointID) {
            //If checkpoint changed, update local checkpoint
            if (this->checkpoint != checkpointID){
                this->checkpoint = checkpointID;
                //If checkpoint changed and has the ID of zero, we are on the next lap
                if (this->checkpoint == 0){
                    lap += 1;
                }
            }
            // cerr << "Checkpoint didn't update - making no changes within object. This is expected behaviour most of the time." << endl;
        }     

        int findLeadingPodID(Pod& pod1, Pod& pod2) {
            /** Leading pod has 1) the highest Lap count, 2) The Highest Checkpoint count, 3) The smallest distance to the next checkpoint 
            Using those parameters in that order to compare the lead status of two given Pods.**/
            if (pod1.lap > pod2.lap) {
                //pod 1 is leading in laps
                this->lastLeader = pod1.getID();
                return pod1.getID();
            } else if (pod2.lap > pod1.lap) {
                //pod 2 is leading in laps
                this->lastLeader = pod2.getID();
                return pod2.getID();
            }
            else {
                //Impossible to determine who is leader using only laps
                //Try using checkpoints
                if (pod1.checkpoint > pod2.checkpoint) {
                    //pod1 is ahead in checkpoints
                    this->lastLeader = pod1.getID();
                    return pod1.getID();
                } else if (pod2.checkpoint > pod1.checkpoint) {
                    //pod2 is ahead in checkpoints
                    this->lastLeader = pod2.getID();
                    return pod2.getID();
                } else {
                    //Impossible to determine who is leader using checkpoints and laps
                    //Try using distance:
                    
                    /** Dev cerr-s 
                    cerr << "Start of Distance." << endl;
                    cerr << to_string(pod1.checkpoint) << endl;
                    cerr << to_string(get<0>(pod1.checkpointVector.at(0))) << endl; **/
                    
                    tuple <int, int, int> nextCheckpoint = pod1.checkpointVector.at(pod1.checkpoint);
                    //cerr << "Got nextCheckpoint successfully." << endl;
                    double nextCheckpointDistancePod1 = calculatedDistance(pod1.pos, tuple<int, int> (get<1>(nextCheckpoint), get<2>(nextCheckpoint) ) );
                    double nextCheckpointDistancePod2 = calculatedDistance(pod2.pos, tuple<int, int> (get<1>(nextCheckpoint), get<2>(nextCheckpoint) ) );
                    //cerr << "Start of Distance IFs." << endl;
                    if  (nextCheckpointDistancePod1 < nextCheckpointDistancePod2) {
                        //Pod1 has a smaller distance to next checkpoint.
                        this->lastLeader = pod1.getID();
                        return pod1.getID();
                    } else if (nextCheckpointDistancePod2 < nextCheckpointDistancePod1) {
                        //Pod2 has a smaller distance to next checkpoint.
                        this->lastLeader = pod2.getID();
                        return pod2.getID();
                    } else {
                        //Arbitrarily decide first bot is leading if both pods have exactly the same distance.
                        this->lastLeader = pod1.getID();
                        return pod1.getID();
                    }
                }
            }
            cerr << "Something has gone critically wrong - check Pod class findLeadingPod function." << endl; 
        }

        double abs_to_relative(tuple<int, int, int> checkpointData) {
            //Normalize vector to obtain unit vector aka ühikvektor
            //Input -> nth infoTuple of the checkpointVector holding all values of checkpoints, defined in main()
            //Can also input any other tuple representing a point where the 2nd and 3rd value contain the X and Y values respectively.
            
            //get<1>(checkpointData) -> X coord. value of checkpoint
            //get<2>(checkpointData) -> Y coord. value of checkpoint

            /**Normalizing a Vector:
            func normalized():
            Vector vector = (x, y)
            const float norm = sqrt(x*x+y*y);
            return Vector(x/norm, y/norm);
            
            Utilizing a normalized vector to obtain correct angle between two points:
            {
                Vector direction = (point1_points - point2_points).normalized();
                float res = acos(direction[0]) * 180 / math.PI;
                if (direction[1]< 0)
                    return 360.f - res;
                else
                    return res;
            }
            **/
            
            //Perform vector subtraction
            tuple <int, int> directionPreNormalization (get<1>(checkpointData) - get<0>(this->pos), get<2>(checkpointData) - get<1>(this->pos));
            //Obtain the normalization double
            double normalization = sqrt(pow(get<0>(directionPreNormalization), 2) + pow(get<1>(directionPreNormalization), 2));
            //Normalize the vector
            tuple <double, double> realDirection (get<0>(directionPreNormalization) / normalization, get<1>(directionPreNormalization) / normalization);

            //Use arc-cos to obtain angle between two point, utilzing a normalized unit vector
            double result = acos(get<0>(realDirection)) * 180.f / PI;
            
            if (get<1>(realDirection) < 0) {
                //Depending on our orientation, we need to adjust how we calculate/display the result.
                return 360.f - result;
            } else {
                return result;
            }
        }

        string action( tuple<int, int, int, int> opponentPod) {
            /**
            Main robot AI

            driver value 1 = AI acts as driver, trying to reach checkpoints as fast as possible
            driver value 0 = AI acts as a simple interceptor, trying to collide with the leading opposing pod

            **/
            if (this -> driver == 1) {
                //cerr << "We have a driver." << endl;
                //AI is driver
                tuple <int, int, int> nextCheckpoint = this->checkpointVector.at(this->checkpoint);
                //Throwaway default thrust value
                string thrust = to_string(BOOSTMOD);

                double nextCheckpointDistance = calculatedDistance(pos, tuple<int, int> (get<1>(nextCheckpoint), get<2>(nextCheckpoint) ) );
                //throw (f"distance is {next_checkpoint_dist}") <-Needs converting  to c++ syntax if needed
                

                //Subtract angle given by simulation from our actual angle to preserve bronze and silver league angle logic
                double oldAngle = abs_to_relative(nextCheckpoint) - this->angle;
                //throw (f"angle is {oldAngle}") <-Needs converting  to c++ syntax if needed

                cerr << "Driver stats are" << endl;
                cerr << to_string(nextCheckpointDistance) << endl;
                cerr << to_string(oldAngle) << endl;

                if ( (nextCheckpointDistance >= 4000) and (-5 <= oldAngle) and (oldAngle <= 5) and (boosts >= 1) ){
                    //Boost - direction is good, distance is (arbitrarily) long enough for the boost to be useful and we have a boost
                    thrust = "BOOST";
                    this->boosts -=1;
                }
                else if ( (abs(nextCheckpointDistance) <= 2500) and (-10 <= oldAngle) and (oldAngle <= 10) ) {
                    /**
                    Two promising heuristics old: thrust = to_string(int(abs(nextCheckpointDistance * 0.0526316 ) - 31.5789));
                    and new: thrust = to_string(int(abs(nextCheckpointDistance * 0.0327253 ) + 22.8433));
                    Potentially improved heuristic - faster traverse near edges of checkpoints, 
                    compromised by more agressive drift cancellation via larger MAGIC_HEURISTIC value **/

                    //Slow down when approaching the checkpoint given the distance to checkpoint, based off of an arbitrary heuristic function
                    //thrust = to_string(int(abs(nextCheckpointDistance * 0.0526316 ) - 31.5789));

                    thrust = to_string(int(abs(nextCheckpointDistance * 0.0327253 ) + 22.8433));
                } else {
                    if ( (-30 <= oldAngle) and  (oldAngle <= 30) and ( abs(nextCheckpointDistance) <= 800) ) {
                        //We are within 30 degrees to the checkpoint and close enough that we should just maxthrust,
                        //we were most likely crashed into right near the checkpoint and can just thrust into it
                        thrust = to_string(100);
                    } else if ( (-10 <= oldAngle) and  (oldAngle <= 10)) {
                        //We are within 9 degrees, or half of a turn's pivot value, to be directed straight at the checkpoint
                        //We should use maxthrust since we will be moving towards the checkpoint for atleast half the given turn
                        thrust = to_string(100);
                    } else if ( (10 < oldAngle) and (oldAngle <= 100) ) {
                        /**We are between 10 and 100 degrees to be directed straight at the checkpoint,
                        determine our speed using an arbitrary heuristic function
                        Useful if we wish to have differing heuristic functions depending on the side the pod is angled towards**/
                        thrust = to_string(int(110.164 - 0.565574*(abs(oldAngle))));
                    } else if ( (-100 <= oldAngle) and (oldAngle < -10)) {
                        /**We are between -10 and -100 degrees to be directed straight at the checkpoint,
                        determine our speed using an arbitrary heuristic function
                        Useful if we wish to have differing heuristic functions depending on the side the pod is angled towards**/

                        //Currently the same as adjusting within positive angle values
                        thrust = to_string(int(110.164 - 0.565574*(abs(oldAngle))));
                    } else if ( ( (oldAngle >= 250) or (oldAngle <= -250) ) and (abs(nextCheckpointDistance) >= 4000 ))  {
                        //Getting true direction, while it should be clamped -180 - 180, sometimes gives values beyond that clamp, which is very strange,
                        //this happens when the pod is nearly facing a checkpoint at a weird angle, try thrusting at it as a solution.
                        //This could be a python->c++ implementation anomaly
                        //A rewrite of the oldAngle computation or a rewrite of the whole angle handling is required

                        //This is the far away option - just charge at the target
                        thrust = to_string(100);
                    
                    } else if ( ( (oldAngle >= 250) or (oldAngle <= -250)) and (abs(nextCheckpointDistance) <= 2500 ))  {
                        //Getting true direction, while it should be clamped -180 - 180, sometimes gives values beyond that clamp, which is very strange,
                        //this happens when the pod is nearly facing a checkpoint at a weird angle, try thrusting at it as a solution.
                        //This could be a python->c++ implementation anomaly
                        //A rewrite of the oldAngle computation or a rewrite of the whole angle handling is required

                        //This is the close option - use the same heuristic function to determine speed as you otherwise would
                        thrust = to_string(int(abs(nextCheckpointDistance * 0.0526316 ) - 31.5789));
                    } else {
                        //We are completely turned around, we do not want to go fast as that will increase our travel-time to hit the checkpoint 
                        thrust = to_string(5);
                    }
                }

                if (thrust != "BOOST") {
                    /** In case a calculation gives us a higher top speed than what is neccessary due to rounding issues, 
                    heuristic function mishaps (edge cases giving us a value of 101 ) or other combinations of known and unknown issues,
                    cap the thrust to 100 so it does not result in automatic disqualification.
                    
                    This has not yet been neccessary for thrust speeds of values smaller than 0 due to the way the heuristic functions were
                    chosen, and results of boosts being lower than 0 are attainable much later than passing the checkpoint and obtaining a new
                    distance to the next checkpoint.**/
                    if (stoi(thrust) > 100) {
                        thrust = to_string(100);
                    }
                }
                cerr << "Non-interceptor  thrust is" + thrust << endl;
                //Use drifting cancellation
                int xTarget = get<1>(nextCheckpoint) - (MAGIC_HEURISTIC * this->speed_x);
                int yTarget = get<2>(nextCheckpoint) - (MAGIC_HEURISTIC * this->speed_y);
                
                /**Reset the AI back to the default interceptor behaviour, handle re-enabling driver behaviour when deciding
                which pod is leading during  main() function.**/

                this -> driver = 0;
                //Construct the outgoing string that the action() function returns
                return to_string(xTarget) + " " + to_string(yTarget) + " " + thrust + " I am the racer";
            
            
            } else {
                //AI is interceptor

                //Interceptor should move at full speed
                string thrust = to_string(100);

                //Opponent location X + Y, opponent speed X + Y
                tuple <int, int, int, int> opponentData;
                opponentData = opponentPod;

                //Required for opponent distance finding
                tuple <int, int> opponentLocation ( get<0>(opponentData), get<1>(opponentData) );
                

                //Drive ahead of opponent based on THEIR speed
                int xTarget = (get<0>(opponentData) + MAGIC_HEURISTIC * get<2>(opponentData))  - (MAGIC_HEURISTIC * this->speed_x);
                int yTarget = (get<1>(opponentData) + MAGIC_HEURISTIC *  get<3>(opponentData)) - (MAGIC_HEURISTIC * this->speed_y);

                /** Show interceptor data
                cerr << "Attacker is attacking" << endl;
                cerr << to_string(xTarget) << endl;
                cerr << to_string(yTarget) << endl;

                cerr << "While opponent is at " << endl;
                cerr << to_string(get<0>(opponentData)) << endl;
                cerr << to_string(get<1>(opponentData)) << endl;
                **/
                
                //Subtract angle given by simulation from our actual angle to preserve bronze and silver league angle logic
                double oldAngle = abs_to_relative(tuple<int, int, int> (1, xTarget, yTarget)) - this->angle;


                double opponentDistance = calculatedDistance(pos, opponentLocation);

                cerr << "Interceptor stats are" << endl;
                cerr << to_string(opponentDistance) << endl;
                cerr << "angle:" + to_string(oldAngle) << endl;

                if ( (opponentDistance < 2500) and (-3 <= oldAngle) and (oldAngle <= 3) and (boosts >= 1) ){
                    //Opponent is close, we are looking towards him and we have a boost - use boost
                    thrust = "BOOST";
                    boosts -=1;
                } else if ( (opponentDistance < 1000) and (-36 <= oldAngle) and (oldAngle <= 36) ) {
                    //We are looking towards the opponent, we are very close and likely to impact soon - use shield
                    thrust = "SHIELD";
                } else if ( ((-100 >= oldAngle) or (oldAngle >= 100)) and (opponentDistance < 1000 )) {
                    //We are turned around but still likely to hit opponent, shield anyway
                    thrust = "SHIELD";

                } else if ( (10 < oldAngle) and (oldAngle <= 100) ) {
                    /**We are between 10 and 100 degrees to be directed straight at the checkpoint,
                    determine our speed using an arbitrary heuristic function
                    Useful if we wish to have differing heuristic functions depending on the side the pod is angled towards**/
                     thrust = to_string(int(110.164 - 0.565574*(abs(oldAngle))));
                } else if ( (-100 <= oldAngle) and (oldAngle < -10)) {
                    /**We are between -10 and -100 degrees to be directed straight at the checkpoint,
                    determine our speed using an arbitrary heuristic function
                    Useful if we wish to have differing heuristic functions depending on the side the pod is angled towards**/

                    //Currently the same as adjusting within positive angle values
                    thrust = to_string(int(110.164 - 0.565574*(abs(oldAngle))));
                } else if ( ((-100 >= oldAngle) or (oldAngle >= 100)) and (opponentDistance > 2000 )) {
                    //We are turned around, set thust to 10 until we re-aim.
                    thrust = to_string(10);
                }


                if ( (thrust != "BOOST") and (thrust != "SHIELD")) {
                    /** In case a calculation gives us a higher top speed than what is neccessary due to rounding issues, 
                    heuristic function mishaps (edge cases giving us a value of 101 ) or other combinations of known and unknown issues,
                    cap the thrust to 100 so it does not result in automatic disqualification.
                    
                    This has not yet been neccessary for thrust speeds of values smaller than 0 due to the way the heuristic functions were
                    chosen, and results of boosts being lower than 0 are attainable much later than passing the checkpoint and obtaining a new
                    distance to the next checkpoint.**/
                    if (stoi(thrust) > 100) {
                        thrust = to_string(100);
                    }
                }

                cerr << "Interceptor thrust is" + thrust << endl;
                //Construct the outgoing string that the action() function returns
                return to_string(xTarget) + " " + to_string(yTarget) + " " + thrust + " I am the rammer";
            }
        }
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    //Declare Useful integers, Magic Numbers and Podlists
    int laps;
    int checkpointCount;
    int podsMade = 0;
    int oppPodsMade = 0;
    int killingHasBegun = 0;
    int IDCounter = 0;
    int opponentPodToKillID;
    int leaderID;

    tuple<int, int, int, int> enemyPosTuple;

    vector<Pod> ownPodlist (2);
    vector<Pod> opponentPodlist (2);

    //Read Lap count and checkpoint count
    cin >> laps; cin.ignore();
    cin >> checkpointCount; cin.ignore();

    //Create the checkpointVector
    vector<tuple<int, int, int>> checkpointVector (checkpointCount);

    //Populate checkpointVector with data
    for (int i = 0; i < checkpointCount; i++) {
        int checkpointX;
        int checkpointY;
        
        //Read checkpoint data
        cin >> checkpointX >> checkpointY; cin.ignore();
        
        tuple<int, int, int> infoTuple (i, checkpointX, checkpointY);
        checkpointVector[i] = infoTuple;

    }

    // Game Loop
    while (1) {

        for (int i = 0; i < 2; i++) {
            int x; // x position of your pod
            int y; // y position of your pod
            int vx; // x speed of your pod
            int vy; // y speed of your pod
            int angle; // angle of your pod
            int nextCheckPointId; // next check point id of your pod
            cin >> x >> y >> vx >> vy >> angle >> nextCheckPointId; cin.ignore();

            
            if (podsMade != 2) {
                //If pods are not yet created, create Pods
                IDCounter += 1;
                Pod pod;
                pod.setID(IDCounter);
                pod.feedCheckpointVector(checkpointVector);
                ownPodlist[i] = pod;
                podsMade += 1;
                //cerr << "Made Friendly Pod" << endl;
                //cerr << "ID counter is " << endl;
                //cerr << to_string(IDCounter) << endl;
            }
            
            //Update Pod Values from input
            ownPodlist[i].updatePos(x, y);
            ownPodlist[i].updateSpeed(vx, vy);
            ownPodlist[i].updateAngle(angle);
            ownPodlist[i].updateCheckpoint(nextCheckPointId);

            /** cerrs to make sure information is being read
            cerr << to_string(ownPodlist[i].getPos('x')) << endl;
            cerr << to_string(ownPodlist[i].getSpeed('x')) << endl;
            **/

        }



        for (int i = 0; i < 2; i++) {
            int x2; // x position of the opponent's pod
            int y2; // y position of the opponent's pod
            int vx2; // x speed of the opponent's pod
            int vy2; // y speed of the opponent's pod
            int angle2; // angle of the opponent's pod
            int nextCheckPointId2; // next check point id of the opponent's pod
            cin >> x2 >> y2 >> vx2 >> vy2 >> angle2 >> nextCheckPointId2; cin.ignore();

            if (oppPodsMade != 2) {
                IDCounter += 1;
                Pod pod;
                pod.setID(IDCounter);
                pod.feedCheckpointVector(checkpointVector);
                opponentPodlist[i] = pod;
                oppPodsMade += 1;
                //cerr << "Made Opposing Pod" << endl;
                //cerr << "ID counter is " << endl;
                //cerr << to_string(IDCounter) << endl;
            }

            //Update Pod Values from input
            opponentPodlist[i].updatePos(x2, y2);
            opponentPodlist[i].updateSpeed(vx2, vy2);
            opponentPodlist[i].updateAngle(angle2);
            opponentPodlist[i].updateCheckpoint(nextCheckPointId2);
        
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
        
        //Find our pod that is currently leading, and update his AI to be the driver AI for this turn.
        leaderID = ownPodlist[0].findLeadingPodID(ownPodlist[0], ownPodlist[1]);
        cerr << "Leader ID is "   + to_string(leaderID) << endl;

        if (killingHasBegun == 0) {
            //We have not yet chosen a pod to target, so choose a leading pod using the findLeadingPod function.
            opponentPodToKillID = opponentPodlist[0].findLeadingPodID(opponentPodlist[0], opponentPodlist[1]);
            killingHasBegun = 1;
            cerr << "Killing has begun" << endl;
        } else {
            //We have already chosen a pod to target.
            //cerr << "Killing same target" << endl;
            if (opponentPodlist[0].findLeadingPodID(opponentPodlist[0], opponentPodlist[1]) != opponentPodToKillID) {
                /**The Pod we were not targeting is ahead of the pod we are targeting, making it a bigger threat,
                start targetting the other pod instead.**/
                opponentPodToKillID = opponentPodlist[0].findLeadingPodID(opponentPodlist[0], opponentPodlist[1]);
                cerr << "Killing has begun anew" << endl;
            }
        }

        //cerr << "Target ID is " << endl;
        //cerr << to_string(opponentPodToKillID) << endl;


        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"
        
        //For every pod we have, output their action we get from the action() function.

        //First find the information our interceptor needs by reading new information from the original Pod class.
        for (int i = 0; i < 2; i++) {
            if (opponentPodlist[i].getID() == opponentPodToKillID) {
                get<0>(enemyPosTuple) = opponentPodlist[i].getPos('x');
                get<1>(enemyPosTuple) = opponentPodlist[i].getPos('y');
                get<2>(enemyPosTuple) = opponentPodlist[i].getSpeed('x');
                get<3>(enemyPosTuple) = opponentPodlist[i].getSpeed('y');

                /** Show targeted enemy location
                cerr << "enemypostuple is " << endl;
                cerr << to_string(get<0>(enemyPosTuple)) << endl;
                cerr << to_string(get<1>(enemyPosTuple)) << endl;
                **/
            }
        }

        for (int i = 0; i < 2; i++) {
            //Set our leading bot to be the racer
            if (ownPodlist[i].getID() == leaderID) {
                ownPodlist[i].updateDriverAIStatus(1);
            }
            //Call the Action function, passing requried parameters for our AI to drive the bot.
            string podAction = ownPodlist[i].action(enemyPosTuple);
            cout << podAction << endl;
        }
    }
}
