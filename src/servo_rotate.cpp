/**
* @file   ${servo_rotate.cpp}
* @author Rainer Koch
* @date   ${2013/11/04}
*
* Dynamixel-Servo control
*/

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <dynamixel.h>

// Control table addresses
#define P_GOAL_POSITION_L     30
#define P_GOAL_POSITION_H     31
#define P_MOVING_SPEED_L      32
#define P_MOVING_SPEED_H      33
#define P_TORQUE_LIMIT_L      34
#define P_TORQUE_LIMIT_H      35
#define P_PRESENT_POSITION_L  36
#define P_PRESENT_POSITION_H  37
#define P_LED_STATUS          25

using namespace std;


void turn2posDynamixel(int id, int goal, int speed)
{
  dxl_write_word(id, P_MOVING_SPEED_L , speed );
  dxl_write_word( id, P_GOAL_POSITION_L, goal );
}


int setupDynamixel(int dev, int baudnum, int id, int speed, int torque)
{
  if(dxl_initialize(dev, baudnum) == 1)
  {
    dxl_write_word(id, P_LED_STATUS, 1 );
    if(!(dxl_get_result( ) == COMM_RXSUCCESS))
    {
        cout << "Wrong Dynamixel-ID!" << endl;
        return 0;
    }
    dxl_write_word(id, P_TORQUE_LIMIT_L, torque );
    dxl_write_word(id, P_MOVING_SPEED_L , speed );
    cout << "Succeed to open USB2Dynamixel!" << endl;
    sleep(5);
    return 1;

  }
  else
  {
    cout << "Failed to open USB2Dynamixel!" << endl;
    cout << "Wrong device or baudnumber!" << endl << endl;
    cout << "Press Enter key to terminate..." << endl;
    getchar();
    return 0;
  }
}

int main(int argc, char* argv[]) {
  // Variables for Dynamixel
  int baudnum;
  int dev;
  int id;
  int speed;
  int torque;
  float goal_pos;

  // Variables for process
  int angle;

  // Load configuration
  if (argc > 1)
  {
    id = atoi(argv[1]);
    baudnum = 34;
    dev = 1;
    speed = 250;
    torque = 500;
    angle = 10;
    cout << "id: " << argv[1] << " baudnum: " << baudnum << " dev: " << dev << " speed: " << speed << endl;

  }
  else
  {
    cout << "usage: " << argv[0] << " <id>" << endl;
    exit(1);
  }

  // Setup servo
  if(!setupDynamixel(dev, baudnum, id, speed, torque))
  {
    exit(0);
  }

  // Step to positions
  cout << "Starting Step Sequence" << endl;

  for(goal_pos = 0.0; goal_pos <= 1023; (goal_pos = goal_pos+102.3))
  {
    turn2posDynamixel(id, goal_pos, speed);
    sleep(2);
    cout << "Step " << goal_pos << endl;
  }


  cout << "End of program" << endl;
  // stop motor
  dxl_write_word( id, P_MOVING_SPEED_L, 0 );
  dxl_write_word( id, P_LED_STATUS, 0 );

  // Close device
  dxl_terminate();

  return 0;
}
