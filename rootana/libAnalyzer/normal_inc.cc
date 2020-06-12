#include <Math/IFunction.h>
#include "TMath.h"

//calculates the required gantry parameters for observation of a particular incident angle at a user defined offset angle from the cathode
void normal_incident(){
  //determined by circle fit at -90 degree rotation
  double angle_rot = -90*TMath::Pi()/180; // -90 rotation used to find center
  double center_x = 49.7;
  double center_y = 37.0;
  double center_z = 30.0;
  //from Hamamatsu -- radius of curvature
  double radius = 13.1; //cm
  //Center of box to laser
  double laser_box = 11.4; // height in cm from tilt axis to laser
  //offsets determined by user
  double offset_x = 3;
  double offset_y = 3;
  //coordinate offset due to gantry arm position on optical box
  double arm_x = 4.9;
  double arm_y = 4.7;
  center_x = center_x-arm_x;
  center_y = center_y-arm_y;

  double azimuthal_ang = 45;
  azimuthal_ang = azimuthal_ang*TMath::Pi()/180;
std::cout << "User Chosen Offsets: (" << offset_x << ", " << offset_y << ")" << std::endl;
std::cout << "DPMT Point of Observation: (" << offset_x+center_x << ", " << offset_y+center_y << ", " << center_z << ")" << std::endl;


  //incident angle determined by user
  double incidence_angle = 10;
  double tilt = incidence_angle*TMath::Pi()/180;
std::cout << "User chosen incident angle of: " << incidence_angle << std::endl;
  
  //distance from laser at tilt -90 to PMT focusing position
  double z_0 = 47.5+radius; //47.5 +/- 0.1 for SKPMT //32.1 from laser to top of DPMT
  
  //corrected x & y intensity offsets for normal incidence
  double rel_x_norm = (offset_x)/(sqrt(offset_x*offset_x+offset_y*offset_y));
  double rel_y_norm = (offset_y)/(sqrt(offset_x*offset_x+offset_y*offset_y));

  //corrected x & y intensity offsets including azimuthal correction
  double rel_x = TMath::Abs((offset_x*cos(azimuthal_ang)-offset_y*sin(azimuthal_ang))/(sqrt(offset_x*offset_x+offset_y*offset_y)));
  double rel_y = TMath::Abs((offset_y*cos(azimuthal_ang)+offset_x*sin(azimuthal_ang))/(sqrt(offset_x*offset_x+offset_y*offset_y)));
std::cout << "rel:  " << rel_x << "  " << rel_y << std::endl;
  
  double theta;
  double psi;

  //psi initially determined by angle from focus to cathode position
  double ang1 = sin(sqrt(offset_x*offset_x+offset_y*offset_y)/radius);

std::cout << "Normal Angle to PMT:  " << ang1*180/3.1415926535 << std::endl;
  double az = (sqrt(sin(azimuthal_ang)*tilt*sin(azimuthal_ang)*tilt+ang1*ang1)-cos(azimuthal_ang)*tilt); //azimuthally corrected combined tilt and psi angle
std::cout << az*180/3.14159 << std::endl;  
double change_z = laser_box*(1-cos(az)); //delta z due to tilt of laser box
  double z_point_pmt = sqrt((radius*radius)-(offset_x*offset_x+offset_y*offset_y)); //cathode change in z from top of PMT
  double dif_z = change_z+z_0-z_point_pmt; //total height from cathode position to tilted laser
  std::cout << dif_z << std::endl;
  //offset at normal incident
  double delta_z = laser_box*(1-cos(ang1));
  double normal_z = delta_z+z_0; //total height at normal incidence
  //double x_val = normal_z*tan(ang1)*(rel_x_norm); //x addition due to azimuthal rotation (to account for normal offset)
  //double y_val = normal_z*tan(ang1)*(rel_y_norm); //y addition due to azimuthal rotation (to account for normal offset)

  //offsets due to az (combined tilt/ azimuth/ psi)
  double change_x_box = laser_box*sin(az)*rel_x; //delta x due to tilt in laser box
  double change_y_box = laser_box*sin(az)*rel_y; //delta y due to tilt in laser box
  double change_x = dif_z*tan(az)*rel_x+change_x_box; //delta x from DPMT analysis point zenith to untilted laser
  double change_y = dif_z*tan(az)*rel_y+change_y_box; //delta y from DPMT analysis point zenith to untilted laser
  double x_position_coord = center_x+offset_x+change_x;
  double y_position_coord = center_y+offset_y+change_y;
std::cout << y_position_coord-(center_y+offset_y) << "  " << x_position_coord - (center_x+offset_x) << " httt" << std::endl;
  theta = atan((y_position_coord-(center_y+offset_y))/(x_position_coord - (center_x+offset_x)));
  x_position_coord = x_position_coord + (cos(angle_rot-theta)*arm_x - sin(angle_rot-theta)*arm_y);
  y_position_coord = y_position_coord + (cos(angle_rot-theta)*arm_y + sin(angle_rot-theta)*arm_x);

std::cout << "Final x-position:  " << x_position_coord << std::endl;
std::cout << "Final y-position:  " << y_position_coord << std::endl;
std::cout << "Final z-position:  " << center_z << std::endl;
  theta=theta*180/TMath::Pi();
std::cout << theta << std::endl;
  psi = (-90+az*180/TMath::Pi());
  if(psi<-90) {
    theta = theta+180;
  }
  if(theta>180) theta = 360-theta;

std::cout << "Final theta angle:  " << theta<< std::endl;
std::cout << "Final tilt angle:  " << psi << std::endl;

}

//calculates the required gantry parameters for observation of a particular incident angle at a user defined offset angle from the cathode
//void normal_incident(){
//  //determined by circle fit
//    double center_x = 37;
//      double center_y = 37.0;
//        //from Hamamatsu -- radius of curvature
//          double radius = 13.1;
//            //Center of box to laser
//              double laser_box = 11.4;
//                //offsets determined by user
//                  double offset_x = -3.00;
//                    double offset_y = 0;
//
//                    std::cout << "User chosen x offset of: " << offset_x << ", and y offset of: " << offset_y << std::endl;
//
//                      //incident angle determined by user
//                        double incidence_angle = 20;
//                          double tilt = incidence_angle*3.1415926535/180;
//
//                          std::cout << "User chosen incident angle of: " << incidence_angle << std::endl;
//
//                            //distance from laser at tilt 90 to PMT focusing position
//                              double z_0 = 32.1+radius; //32.1 from laser to top of DPMT 
//
//                                double theta;
//                                  double psi;
//
//                                  //std::cout << "Height to datum:  " << z_0 << std::endl;
//
//                                    double ang1 = sin(offset_x/radius);
//
//                                    std::cout << "Normal Angle to PMT:  " << ang1*180/3.1415926535 << std::endl;
//
//                                      double delta_z = laser_box*(1-cos(ang1));
//                                        double normal_z = delta_z+z_0;
//                                        //std::cout << "Height at normal:  " << normal_z << std::endl;
//
//
//                                          double change_z = laser_box*(1-cos(ang1-tilt));
//                                            double angle_z = change_z+z_0;
//
//                                            //std::cout << "Needed z at angle:  " << angle_z << std::endl;
//
//                                              double x_val = normal_z*tan(ang1);
//
//                                              //std::cout << "X-distance to normal:  " << x_val << std::endl;
//
//                                                double z_point_pmt = sqrt((radius*radius)-(offset_x*offset_x));
//                                                  double dif_z = angle_z-z_point_pmt;
//
//                                                  std::cout << "Distance from focus to laser height:  " << dif_z+radius << std::endl;
//
//                                                    double change_x = dif_z*tan(ang1-tilt);
//                                                      psi = (-90-ang1*180/3.1415926535)+(tilt*180/3.1415926535);
//                                                        double x_position_coord = change_x+offset_x+center_x;
//
//                                                        std::cout << "Final x-position:  " << x_position_coord << std::endl;
//                                                        std::cout << "Final tilt angle:  " << psi << std::endl;
//                                                        }
//                                                        ~                                                                                                                                                             
//                                                        "normal_incident.cc" 61L, 1984C                
