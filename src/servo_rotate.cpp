#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <dynamixel.h>

// Control table addresses
#define P_MOVING_SPEED_L        32
#define P_MOVING_SPEED_H        33
#define P_PRESENT_POSITION_L  36
#define P_PRESENT_POSITION_H  37

using namespace std;

unsigned int _seq = 0;
unsigned int _fragmentSize = 0;
double _angularVelocity = 0.0;
bool _newDataAvailable = false;
bool _running = true;


void approachPosition(int id, int goal)
{
  bool approaching = true;
  dxl_write_word( id, P_MOVING_SPEED_L, 5);
  double approach_speed = dxl_read_word( id, P_MOVING_SPEED_L );
  while(approaching)
  {
    double Kp = 2.0;
    double pos = dxl_read_word( id, P_PRESENT_POSITION_L );
    double goal_tmp = goal-pos;
    if(goal_tmp<0) goal_tmp += 4096.0;
    double new_speed = (goal_tmp)/4096.0 * 300.0 * Kp;
    if(new_speed > 250.0) new_speed = 250;
    if(new_speed < 3.0)   new_speed = 3;
    if((new_speed-approach_speed)>5.0) new_speed = approach_speed + 5.0;

    approach_speed = new_speed;
    dxl_write_word( id, P_MOVING_SPEED_L, approach_speed);
    if(pos==goal) approaching = false;
  }
}

int main(int argc, char* argv[]) {

  // Default parameters
  double      d_fragmentSize = 0.0;
  double      speed_rpm = 10.0;
  int         baudnum = 34;          // Baudnum 1 = 1000000, 34 = 57142
  int         deviceIndex = 0;      // /dev/ttyUSBx => x = deviceIndex

   if(argc>2)
   {
      if(argc!=3)
      {
         cout << "usage: " << argv[0] << " [speed] [fragmentSize]" << endl;
         return 0;
      }
      speed_rpm         = atof(argv[1]);
      d_fragmentSize    = atof(argv[2]);
   }
  cout << "  speed: " << speed_rpm << "  d_fragmetSize: " << d_fragmentSize << endl;


  _fragmentSize = (unsigned int)(d_fragmentSize);

  double timeForHalfRevolution = 60.0 / speed_rpm / 2.0;
  _angularVelocity = 180.0 / timeForHalfRevolution;

  if(_fragmentSize==0)
  {
    // scans should not be fragmented
    // ensure fragmentSize to be large enough for a full scan (half revolution)
    _fragmentSize = timeForHalfRevolution / 0.025 + 100;
  }

  // Set up servo
  if( dxl_initialize(deviceIndex, baudnum) == 0 )
  {
    printf( "Failed to open USB2Dynamixel!\n" );
    printf( "Press Enter key to terminate...\n" );
    getchar();
    return 0;
  }
  else
    printf( "Succeed to open USB2Dynamixel!\n" );
  int id = 1;

   // Move to reference position
  approachPosition(id, 0);

  // Start scanning
  int speed = 1023.0 / 114.0 * speed_rpm;
  dxl_write_word( id, P_MOVING_SPEED_L, (int)speed);

  cout << "Entering main loop" << endl;
  _seq = 0;
  int prev_pos = 0;
  while(1)
  {
    dxl_write_word( id, P_MOVING_SPEED_L, (int)speed);

    unsigned int f;
    bool rtrig = false;

    for (f = 0; f < _fragmentSize; f++)
    {
      int current_pos = dxl_read_word( id, P_PRESENT_POSITION_L );

      rtrig = (current_pos%2048 < prev_pos%2048);
      prev_pos = current_pos;

      if(rtrig)
      {
    //    _nextIsNewScan = true;
        break;
      }


    }

   // cout << "Scanning elapsed: " << t.reset() << " ms for " << f << " scans" << endl;

  }
  cout << "Shutting down" << endl;
  _running = false;


  dxl_write_word( id, P_MOVING_SPEED_L, 0 ); // stop motor
  dxl_terminate();                           // Close device

  return 0;
}



