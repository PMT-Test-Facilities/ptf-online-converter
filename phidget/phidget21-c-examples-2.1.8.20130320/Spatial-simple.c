// - Spatial simple -
// This simple example creates an spatial handle, initializes it, hooks the event handlers and opens it.  It then waits
// for a spatial to be attached and waits for events to be fired. We preset the data rate to 16ms, but can be set higher (eg. 200)
// in order to slow down the events to make them more visible.
//
// Copyright 2010 Phidgets Inc.  All rights reserved.
// This work is licensed under the Creative Commons Attribution 2.5 Canada License. 
// view a copy of this license, visit http://creativecommons.org/licenses/by/2.5/ca/

#include <stdio.h> 
#include <phidget21.h>

//callback that will run if the Spatial is attached to the computer
int CCONV AttachHandler(CPhidgetHandle spatial, void *userptr)
{
	int serialNo;
	CPhidget_getSerialNumber(spatial, &serialNo);
	printf("Spatial %10d attached!", serialNo);

	return 0;
}

//callback that will run if the Spatial is detached from the computer
int CCONV DetachHandler(CPhidgetHandle spatial, void *userptr)
{
	int serialNo;
	CPhidget_getSerialNumber(spatial, &serialNo);
	printf("Spatial %10d detached! \n", serialNo);

	return 0;
}

//callback that will run if the Spatial generates an error
int CCONV ErrorHandler(CPhidgetHandle spatial, void *userptr, int ErrorCode, const char *unknown)
{
	printf("Error handled. %d - %s \n", ErrorCode, unknown);
	return 0;
}

//callback that will run at datarate
//data - array of spatial event data structures that holds the spatial data packets that were sent in this event
//count - the number of spatial data event packets included in this event
int CCONV SpatialDataHandler(CPhidgetSpatialHandle spatial, void *userptr, CPhidgetSpatial_SpatialEventDataHandle *data, int count)
{
	int i;
	printf("Number of Data Packets in this event: %d\n", count);
	for(i = 0; i < count; i++)
	{
		printf("=== Data Set: %d ===\n", i);
		printf("Acceleration> x: %6f  y: %6f  x: %6f\n", data[i]->acceleration[0], data[i]->acceleration[1], data[i]->acceleration[2]);
		printf("Angular Rate> x: %6f  y: %6f  x: %6f\n", data[i]->angularRate[0], data[i]->angularRate[1], data[i]->angularRate[2]);
		printf("Magnetic Field> x: %6f  y: %6f  x: %6f\n", data[i]->magneticField[0], data[i]->magneticField[1], data[i]->magneticField[2]);
		printf("Timestamp> seconds: %d -- microseconds: %d\n", data[i]->timestamp.seconds, data[i]->timestamp.microseconds);
	}

	printf("---------------------------------------------\n");

	return 0;
}

//Display the properties of the attached phidget to the screen.  
//We will be displaying the name, serial number, version of the attached device, the number of accelerometer, gyro, and compass Axes, and the current data rate
// of the attached Spatial.
int display_properties(CPhidgetHandle phid)
{
	int serialNo, version;
	const char* ptr;
	int numAccelAxes, numGyroAxes, numCompassAxes, dataRateMax, dataRateMin;

	CPhidget_getDeviceType(phid, &ptr);
	CPhidget_getSerialNumber(phid, &serialNo);
	CPhidget_getDeviceVersion(phid, &version);
	int status = CPhidgetSpatial_getAccelerationAxisCount((CPhidgetSpatialHandle)phid, &numAccelAxes);
	printf("Status: %i\n", status);
	 status = CPhidgetSpatial_getGyroAxisCount((CPhidgetSpatialHandle)phid, &numGyroAxes);
	printf("Status: %i\n", status);
	 status = CPhidgetSpatial_getCompassAxisCount((CPhidgetSpatialHandle)phid, &numCompassAxes);
	printf("Status: %i\n", status);
	 status = CPhidgetSpatial_getDataRateMax((CPhidgetSpatialHandle)phid, &dataRateMax);
	printf("Status: %i\n", status);
	 status = CPhidgetSpatial_getDataRateMin((CPhidgetSpatialHandle)phid, &dataRateMin);
	printf("Status: %i\n", status);

        int deviceStatus;
        int i;
        for( i = 0; i < 100; i++ ){
        status = CPhidget_getDeviceStatus(phid, &deviceStatus);
	printf("Sstatus: %i, attached %i\n", status,deviceStatus);
        usleep(20);
        }

        double accelerationX;
        double accelerationY;
        double accelerationZ;


	 status = CPhidgetSpatial_setDataRate((CPhidgetSpatialHandle)phid, 16);
	printf("Status: %i\n", status);

        //sleep(1);
        //        getMagneticField
        status = CPhidgetSpatial_getMagneticField((CPhidgetSpatialHandle)phid, 0, &accelerationX);
	printf("Status1: %i\n", status);
        status = CPhidgetSpatial_getMagneticField((CPhidgetSpatialHandle)phid, 1, &accelerationY);
	printf("Status2: %i\n", status);
        status = CPhidgetSpatial_getMagneticField((CPhidgetSpatialHandle)phid, 2, &accelerationZ);
	printf("Status3: %i\n", status);
        //CPhidgetSpatial_getAcceleration((CPhidgetSpatialHandle)phid, 0, &accelerationX);
        // CPhidgetSpatial_getAcceleration((CPhidgetSpatialHandle)phid, 1, &accelerationY);
        //CPhidgetSpatial_getAcceleration((CPhidgetSpatialHandle)phid, 2, &accelerationZ);
        //        CPhidgetSpatial_getAcceleration((CPhidgetSpatialHandle)phid, 1, acceleration);
        //CPhidgetSpatial_getAcceleration((CPhidgetSpatialHandle)phid, 2, acceleration);
	

	printf("%s\n", ptr);
	printf("Serial Number: %10d\nVersion: %8d\n", serialNo, version);
	printf("Number of Accel Axes: %i\n", numAccelAxes);
	printf("Number of Gyro Axes: %i\n", numGyroAxes);
	printf("Number of Compass Axes: %i\n", numCompassAxes);
	printf("datarate> Max: %d  Min: %d\n", dataRateMax, dataRateMin);
	printf("acceleration %d \n", accelerationX);
	printf("acceleration %d \n", accelerationY);
	printf("acceleration %d \n", accelerationZ);
        status = CPhidget_getDeviceStatus(phid, &deviceStatus);
	printf("Sstatus: %i, attached %i\n", status,deviceStatus);

	return 0;
}

int spatial_simple()
{

  CPhidget_enableLogging(PHIDGET_LOG_INFO,NULL);
	int result;
	const char *err;

	//Declare a spatial handle
	CPhidgetSpatialHandle spatial = 0;

	//create the spatial object
	CPhidgetSpatial_create(&spatial);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)spatial, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)spatial, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)spatial, ErrorHandler, NULL);

	//Registers a callback that will run according to the set data rate that will return the spatial data changes
	//Requires the handle for the Spatial, the callback handler function that will be called, 
	//and an arbitrary pointer that will be supplied to the callback function (may be NULL)
	CPhidgetSpatial_set_OnSpatialData_Handler(spatial, SpatialDataHandler, NULL);

	//open the spatial object for device connections
	CPhidget_open((CPhidgetHandle)spatial, -1);

	//get the program to wait for a spatial device to be attached
	printf("Waiting for spatial to be attached.... \n");
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)spatial, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 0;
	}

	//Display the properties of the attached spatial device
	display_properties((CPhidgetHandle)spatial);

	//read spatial event data
	printf("Reading.....\n");
	
	//Set the data rate for the spatial events
	CPhidgetSpatial_setDataRate(spatial, 160);

        int i;

	//run until user input is read
	printf("Press any key to end\n");
	getchar();

	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)spatial);
	CPhidget_delete((CPhidgetHandle)spatial);

	return 0;
}

//main entry point to the program
int main(int argc, char* argv[])
{
	//all done, exit
	spatial_simple();
	return 0;
}

