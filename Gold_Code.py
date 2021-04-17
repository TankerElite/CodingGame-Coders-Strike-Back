import sys
import math

# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.

class CheckPointData():
    def __init__(self, checkpointlist = []):
        self.checkpoint_list = checkpointlist
        

class Pod():
    #Pod class for storing Data
    def __init__(self):
        self.pos = (0, 0)
        self.speed_x = 0
        self.speed_y = 0
        self.checkpoint = 0
        self.lap = 0
        self.angle = 0
        self.driver = 0
        self.boosts = 1
        self.last_leader = self

    def update_pos(self, x, y):
        self.pos = (x, y)
    
    def get_pos(self, d):
        if d == "x":
            return self.pos[0]
        elif d == "y":
            return self.pos[1]

    def update_speed(self, x, y):
        self.speed_x = x
        self.speed_y = y

    def update_angle(self, angle):
        self.angle = angle

    def get_speed(self, d):
        if d == "x":
            return self.speed_x
        elif d == "y":
            return self.speed_y

    def update_checkpoint(self, checkpoint):
        if self.checkpoint != checkpoint:
            self.checkpoint = checkpoint
            if checkpoint == 0:
                self.lap += 1

    def find_leading_pod(self, pod1, pod2):
        if pod1.lap > pod2.lap:
            self.last_leader = pod1
            return pod1
        elif pod2.lap > pod1.lap:
            self.last_leader = pod2
            return pod2
        else:
            #laps are equal
            if pod1.checkpoint > pod2.checkpoint:
                self.last_leader = pod1
                return pod1
            elif pod2.checkpoint > pod1.checkpoint:
                self.last_leader = pod2
                return pod2
            else:
                return self.last_leader

    def __repr__(self):
        print(f"I am the pod")


    def abs_to_relative(self, checkpoint_data):
        #Normalize vector to obtain unit vector aka ühikvektor
        
        """Normalizing a Vector:
        Vector vector = (x, y)
        const float norm = sqrt(x*x+y*y);
        return Vector(x/norm, y/norm);
        """

        """Utilizing a normalized vector to obtain correct angle between two points:
        {
            Vector direction = (point1_vector - point2_vector).normalized();
            float res = acos(direction[0]) * 180 / math.PI;
            if (direction[1]< 0)
                return 360.f - res;
            else
                return res;
        }
        """
        
        direction_prenorm = (checkpoint_data[1] - self.pos[0], checkpoint_data[2] - self.pos[1])
        
        direction_norm = math.sqrt(direction_prenorm[0]**2 + direction_prenorm[1]**2);
        direction = (direction_prenorm[0]/direction_norm, direction_prenorm[1]/direction_norm);

        result = math.acos(direction[0]) * 180.0 / math.pi
        if direction[1] < 0:
            return 360 - result
        else:
            return result
            
       


    def action(self):
        if self.driver == 1:
            next_checkpoint = check_data.checkpoint_list[self.checkpoint]
            thrust = 50

            next_checkpoint_dist = calculatedistance(self.pos, (next_checkpoint[1], next_checkpoint[2]))
            #raise Exception(f"distance is {next_checkpoint_dist}")
            

            #compute actual angle 

            old_angle = self.abs_to_relative(next_checkpoint) - self.angle
            #raise Exception(f"angle is {old_angle}")
            if next_checkpoint_dist >= 5000 and -5 <= old_angle <= 5 and self.boosts >= 1:
                thrust = "BOOST"
                self.boosts -=1
            
            elif abs(next_checkpoint_dist) <= 2500 and -10 <= old_angle <= 10:
                #thrust = int(abs(next_checkpoint_dist * 0.0526316 ) - 31.5789)
                thrust = int(abs(next_checkpoint_dist * 0.0327253 ) + 22.8433)
            elif old_angle != 0:
                if -10 <= old_angle <= 10:
                    thrust = 100
                elif 10 < old_angle <= 100:
                    thrust = int(110.164 - 0.565574*(abs(old_angle)))
                elif -100 <= old_angle < -10:
                    thrust = int(110.164 - 0.565574*(abs(old_angle)))
                else:
                    thurst = 5
            else:
                thrust = 100

            if thrust != "BOOST":
                if thrust > 100:
                    thrust = 100

            #use target drifting
            x_target = next_checkpoint[1] - (MAGIC_HEURISTIC * self.speed_x)
            y_target = next_checkpoint[2] - (MAGIC_HEURISTIC * self.speed_y)
            self.driver = 0
            return str(x_target) + " " + str(y_target) + f" {thrust} I am the racer"
        else:
            #is interceptor
            thrust = 100
            x_target = opponent_pod_to_kill.pos[0] - (MAGIC_HEURISTIC * self.speed_x)
            y_target = opponent_pod_to_kill.pos[1] - (MAGIC_HEURISTIC * self.speed_y)

            old_angle = self.abs_to_relative((1, opponent_pod_to_kill.pos[0], opponent_pod_to_kill.pos[1])) - self.angle

            if calculatedistance(self.pos, opponent_pod_to_kill.pos) < 1000 and -4 <= old_angle <= 4:
                thrust = "BOOST"
            if calculatedistance(self.pos, opponent_pod_to_kill.pos) < 600 and -5 <= old_angle <= 5:
                thrust = "SHIELD"
            return str(x_target) + " " + str(y_target) + f" {thrust} I am the rammer"


def calculatedistance(pos1, pos2):
    return math.sqrt((pos2[1]-pos1[1])**2 + (pos2[0]-pos1[0])**2)

BOOSTUSE = 1
BOOSTMOD = 100
MAXSPEED = 100
MAGIC_HEURISTIC = 3

own_podlist = []
opponent_podlist = []
podsmade = 0
enemypodsmade = 0

check_data = CheckPointData()

killing_has_begun = 0

laps = int(input())
checkpoint_count = int(input())
for i in range(checkpoint_count):
    checkpoint_x, checkpoint_y = [int(j) for j in input().split()]
    check_data.checkpoint_list.append((i, checkpoint_x, checkpoint_y))

# game loop
while True:
    for i in range(2):
        # x: x position of your pod
        # y: y position of your pod
        # vx: x speed of your pod
        # vy: y speed of your pod
        # angle: angle of your pod
        # next_check_point_id: next check point id of your pod
        x, y, vx, vy, angle, next_check_point_id = [int(j) for j in input().split()]


        if podsmade != 2:
            pod = Pod()
            own_podlist.append(pod)
            podsmade += 1
        
        own_podlist[i].update_pos(x, y)
        own_podlist[i].update_speed(vx, vy)
        own_podlist[i].update_angle(angle)
        own_podlist[i].update_checkpoint(next_check_point_id)
            
    
    for i in range(2):
        # x_2: x position of the opponent's pod
        # y_2: y position of the opponent's pod
        # vx_2: x speed of the opponent's pod
        # vy_2: y speed of the opponent's pod
        # angle_2: angle of the opponent's pod
        # next_check_point_id_2: next check point id of the opponent's pod
        x_2, y_2, vx_2, vy_2, angle_2, next_check_point_id_2 = [int(j) for j in input().split()]

        if enemypodsmade != 2:
            enemy_pod = Pod()
            opponent_podlist.append(enemy_pod)
            enemypodsmade += 1
        
        opponent_podlist[i].update_pos(x_2, y_2)
        opponent_podlist[i].update_speed(vx_2, vy_2)
        opponent_podlist[i].update_angle(angle_2)
        opponent_podlist[i].update_checkpoint(next_check_point_id_2)

   ############### DOUBLE BOTS MF
   
   #who is rammer, who is driver

    
    leading_pod = own_podlist[0].find_leading_pod(own_podlist[0], own_podlist[1])
    if killing_has_begun == 0:
        opponent_pod_to_kill = opponent_podlist[0].find_leading_pod(opponent_podlist[0], opponent_podlist[1])
    else:
        if opponent.podlist[0].find_leading_podfind_leading_pod(opponent_podlist[0], opponent_podlist[1]) != opponent_pod_to_kill:
            opponent_pod_to_kill = opponent.podlist[0].find_leading_podfind_leading_pod(opponent_podlist[0], opponent_podlist[1])
    leading_pod.driver = 1
   
    # Write an action using print
    # To debug: print("Debug messages...", file=sys.stderr, flush=True)
    #Driving print

    for i in range(len(own_podlist)):
        print(own_podlist[i].action())

    #Ramming Print
    
    # You have to output the target position
    # followed by the power (0 <= thrust <= 100)
    # i.e.: "x y thrust"
    
