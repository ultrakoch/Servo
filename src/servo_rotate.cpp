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

/**
  * This function turns the Dynamixel to a fix position
  * @param id      0-253                    id of Dynamixel
  * @param goal    0-1023 = 0-300° CCW      position
  * @param speed   0-1023 = 0-114rpm        to reach goal
  *
  */
void turn2posDynamixel(int id, int goal, int speed)
{
  dxl_write_word(id, P_MOVING_SPEED_L , speed );
  dxl_write_word( id, P_GOAL_POSITION_L, goal );
}

/**
  * This function turns the Dynamixel continious
  * @param id      0-253                    id of Dynamixel
  * @param speed   0-1023 = 0-114rpm        to reach goal
  *
  * Put these function in a while loop
  *
  */
void rotateDynamixel(int id, int speed)
{
  dxl_write_word(id, P_GOAL_POSITION_L , 0 );
  dxl_write_word(id, P_GOAL_POSITION_H , 0 );
  dxl_write_word(id, P_MOVING_SPEED_L , speed );
}

/**
  * This funktion setup the Dynamixel
  * @param dev     0-253                    device id of Dynamixel
  * @param baudnum 0-253                    Baudrate =>  Baudrate (BPS) = 2000000/(baudnum+1)
  * @param id      0-253                    id of Dynamixel
  * @param goal    0-1023 = 0-100%          torque limit
  * @param speed   0-1023 = 0-114rpm        to reach goal
  * @return        0,1 = error, success
  *
  */
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
    dev = 0;
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
    cout << "Step " << goal_pos << endl;
    turn2posDynamixel(id, goal_pos, speed);
    sleep(2);
  }

  sleep(10);

  int low_byte;
  int high_byte;
  // Rotate motor
  cout << "Start rotating" << endl;
  for(int i= 0; i < 1000; i++)
  {
    rotateDynamixel(id, 200);
    low_byte = dxl_read_word(id, P_PRESENT_POSITION_L);
    high_byte = dxl_read_word(id, P_PRESENT_POSITION_H);
    cout << "H: " << high_byte << "     L: " << low_byte << endl;

  }


  cout << "End of program" << endl;
  // stop motor
  dxl_write_word( id, P_MOVING_SPEED_L, 0 );
  dxl_write_word( id, P_LED_STATUS, 0 );

  // Close device
  dxl_terminate();

  return 0;
}
