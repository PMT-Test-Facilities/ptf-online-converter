#include <Math/IFunction.h>
#include "TMath.h"
// I don't have time but there are some critical issues that need to be addressed:
// 1) The script as written only works for gantry 1. Should probably have two scripts (one for gantry 1 move, 1 for gantry 0 move) and then decide in advance which gantry to use (based on cathode position) 
//calculates the required gantry parameters for observation of a particular incident angle at a user defined offset angle from the cathode

void normal_incident(){
	
	//determined by circle fit at -90 degree rotation
  	double angle_rot = -90*TMath::Pi()/180; // -90 rotation used to find center
  	double center_x = 32.6; //32.6 center of acrylic cover using tape scan// 30.6 Center using CircleFit
  	double center_y = 40.0; //40.0 center of acrylic cover using tape scan// 40.2 Center using CircleFit
  	double center_z = 30.0; //0.0
  
	//from Hamamatsu -- radius of curvature
	double radius = 27.0; //cm

	//Center of box to laser
	double laser_box = 11.4; // height in cm from tilt axis to laser

	//offsets determined by user
	double offset_x = 10;
	double offset_y = 10; //-7.7

	//coordinate offset due to gantry arm position on optical box
	double arm_x = 4.9; // value for gantry 1 only
	double arm_y = 4.7; // value for gantry 1 only
	center_x = center_x-arm_x;
	center_y = center_y-arm_y;

	//azimuthal angle
	double azimuthal_ang = 0;
	azimuthal_ang = azimuthal_ang*TMath::Pi()/180;

	//incident angle determined by user
	double incidence_angle = 0;
	double tilt = incidence_angle*TMath::Pi()/180;

	//z distance from top of PMT to gantry laser
	double z_0 = 47.5; //47.5 +/- 0.1 for SKPMT //32.1 from laser to top of DPMT
	std::cout << "User Chosen Offsets: (" << offset_x << ", " << offset_y << ")" << std::endl;
	std::cout << "DPMT Point of Observation: (" << offset_x+center_x << ", " << offset_y+center_y << ", " << center_z << ")" << std::endl;
	std::cout << "User chosen incident angle of: " << incidence_angle << std::endl;

	//distance from laser at tilt -90 to PMT focusing position
	double z_0 = z_0-center_z+radius; //17.5 is distance from top of acrylic cover to center of PMT
	
	//corrected x & y intensity offsets for normal incidence
	double rel_x_norm = (offset_x)/(sqrt(offset_x*offset_x+offset_y*offset_y));
	double rel_y_norm = (offset_y)/(sqrt(offset_x*offset_x+offset_y*offset_y));
	
	// Correct For  direction of offset --- needs revision
	if(offset_x < 0) rel_x_norm = -rel_x_norm;
	if(offset_y < 0) rel_y_norm = -rel_y_norm;
	
	//corrected x & y intensity offsets including azimuthal correction
	double rel_x = TMath::Abs((offset_x*cos(azimuthal_ang)-offset_y*sin(azimuthal_ang))/(sqrt(offset_x*offset_x+offset_y*offset_y)));
	double rel_y = TMath::Abs((offset_y*cos(azimuthal_ang)+offset_x*sin(azimuthal_ang))/(sqrt(offset_x*offset_x+offset_y*offset_y)));
	
	// std::cout << "rel:  " << rel_x << "  " << rel_y << std::endl; //DEBUG
	if(offset_x < 0) rel_x = -rel_x;
	if(offset_y < 0) rel_y = -rel_y;
	double theta;
	double psi;

	//psi initially determined by angle from focus to cathode position
	double ang1 = sin(sqrt(offset_x*offset_x+offset_y*offset_y)/radius);

	// std::cout << "Normal Angle to PMT:  " << ang1*180/3.1415926535 << std::endl; // DEBUG
	double az = (sqrt(sin(azimuthal_ang)*tilt*sin(azimuthal_ang)*tilt+ang1*ang1)-cos(azimuthal_ang)*tilt); //azimuthally corrected combined tilt and psi angle
	std::cout << az*180/3.14159 << std::endl; //DEBUG
	double change_z = laser_box*(1-cos(az)); //delta z due to tilt of laser box
	double z_point_pmt = sqrt((radius*radius)-(offset_x*offset_x+offset_y*offset_y)); //cathode change in z from top of PMT
	double dif_z = change_z+z_0-z_point_pmt; //total height from cathode position to tilted laser
	// std::cout << dif_z << std::endl; //DEBUG
	
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
	std::cout << dif_z*tan(az)*rel_y << " RTE" << std::endl;
	double x_position_coord = center_x+offset_x+change_x; // the sign of change_x will depend on which gantry
	double y_position_coord = center_y+offset_y+change_y; // the sign of change_y will depend on which gantry
	theta = atan((y_position_coord-(center_y+offset_y))/(x_position_coord - (center_x+offset_x)));
	
	// requires revision depending on which gantry is moved...
	double x_position_cor = -(cos(angle_rot-theta)*arm_x + sin(angle_rot-theta)*arm_y); //need to play around with these a bit --they transform the coordinates back from laser position to gantry arm position will be different for gantry 0
	double y_position_cor = -(cos(angle_rot-theta)*arm_y - sin(angle_rot-theta)*arm_x); // same as above -- for gantry 0 theta will be positive not negative in these equations 
	
	// correct for direction of offset -- needs revision
	if(rel_x + rel_y < 0) x_position_cor = -x_position_cor;
	if(rel_x + rel_y < 0) y_position_cor = -y_position_cor;

	x_position_coord = x_position_coord + x_position_cor; //position put back into gantry arm coordinate
	y_position_coord = y_position_coord + y_position_cor; //position put back into gantry arm coordinate
	std::cout << cos(angle_rot-theta)*arm_y << "  " << sin(angle_rot-theta)*arm_x <<" XGba" << std::endl;
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
