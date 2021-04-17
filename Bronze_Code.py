import sys
import math

# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.

class Pod():
    def __init__(self):
        self.pos = (0, 0)
    def update_pos(self, x, y):
        self.pos = (x, y)
    def get_pos(self, d):
        if d == "x":
            return self.pos[0]
        elif d == "y":
            return self.pos[1]
BOOSTUSE = 1
BOOSTMOD = 100
MAXSPEED = 100
MAGIC_HEURISTIC = 2
podmade = False

# game loop
while True:
    # next_checkpoint_x: x position of the next check point
    # next_checkpoint_y: y position of the next check point
    # next_checkpoint_dist: distance to the next checkpoint
    # next_checkpoint_angle: angle between your pod orientation and the direction of the next checkpoint
    x, y, next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, next_checkpoint_angle = [int(i) for i in input().split()]
    opponent_x, opponent_y = [int(i) for i in input().split()]

    if podmade == False:
        pod = Pod()
        pod.update_pos(x, y)
        podmade = True
    # Write an action using print
    # To debug: print("Debug messages...", file=sys.stderr, flush=True)

    # Speed Counter - Drift Management
    current_speed_X = x - pod.get_pos("x")
    current_speed_Y = y - pod.get_pos("y")

    pod.update_pos(x, y)
    #Speedup when enough distance
    if next_checkpoint_dist >= 6000 and next_checkpoint_angle ==0 and BOOSTUSE >=1:
        thrust = "BOOST"
        BOOSTUSE -=1

    elif abs(next_checkpoint_dist) <= 2500 and -10 <= next_checkpoint_angle <= 10:
        thrust = int(abs(next_checkpoint_dist * 0.0526316 ) - 31.5789)
    elif next_checkpoint_angle != 0:
        if -10 <= next_checkpoint_angle <= 10:
            thrust = 100
        elif 10 < next_checkpoint_angle <= 100:
            thrust = int(110.164 - 0.565574*(abs(next_checkpoint_angle)))
        elif -100100 <= next_checkpoint_angle < -10:
            thrust = int(110.164 - 0.565574*(abs(next_checkpoint_angle)))
        else:
            thurst = 2
    else:
        thrust = 100
    if thrust != "BOOST":
        if thrust > 100:
            thrust = 100
    # You have to output the target position
    # followed by the power (0 <= thrust <= 100)
    # i.e.: "x y thrust"
    #drift to correct spot
    x_target = next_checkpoint_x - (MAGIC_HEURISTIC * current_speed_X)
    y_target = next_checkpoint_y - (MAGIC_HEURISTIC * current_speed_Y)
    print(str(x_target) + " " + str(y_target) + f" {thrust}")
