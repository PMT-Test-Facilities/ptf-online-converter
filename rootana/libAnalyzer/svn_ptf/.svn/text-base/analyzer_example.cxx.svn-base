// Simple program to print out the values from a magnetic field scan.


#include <stdio.h>
#include <iostream>

#include "TRootanaEventLoop.hxx"
#include "TH1D.h"
#include "TV1190Data.hxx"
#include "TV792Data.hxx"

class MagneticFieldScan: public TRootanaEventLoop {

  int nnn ;

public:

  MagneticFieldScan() {
    nnn = 0;
  };

  virtual ~MagneticFieldScan() {};

  void BeginRun(int transition,int run,int time){
    std::cout << "Custom: begin run " << run << std::endl;
  }

  void EndRun(int transition,int run,int time){
    std::cout << "Custom end run " << run <<std::endl;
  }

  bool ProcessMidasEvent(TDataContainer& dataContainer){


    for(int i = 0; i < 6;i++){

      char name[100];
      sprintf(name,"CYC%i",i);
      //std::cout << name << std::endl;
      TGenericData *bank = dataContainer.GetEventData<TGenericData>(name);
      if(bank){ 
      
	double counter = ((double*)bank->GetData64())[0];
	
	double x_pos = ((double*)bank->GetData64())[1];
	double y_pos = ((double*)bank->GetData64())[2];
	double z_pos = ((double*)bank->GetData64())[3];
	double x_meas = ((double*)bank->GetData64())[4];
	double y_meas = ((double*)bank->GetData64())[5];
	double z_meas = ((double*)bank->GetData64())[6];
	int time = (int)((double*)bank->GetData64())[6];
	std::cout << counter << " " << i << " " 
		  <<  x_pos << " " << y_pos << " " << z_pos << " " 
		  <<  x_meas << " " << y_meas << " " << z_meas << " " 
		  << time << " " 
		  << std::endl;
	
      }
    }

 
    return true;
  }


  // Describe some other command line argument
  void Usage(void){
    std::cout << "\t-D option: this is a fun new option " << std::endl;
  }

  // Define some other command line argument
  bool CheckOption(std::string option){
    const char* arg = option.c_str();
    if (strncmp(arg,"-D",2)==0){
      std::cout << arg+2 << std::endl;
      std::cout << "I'm happy with this flag!" << std::endl;
      return true;
    }

    return false;
  }

}; 






int main(int argc, char *argv[])
{

  MagneticFieldScan::CreateSingleton<MagneticFieldScan>();
  return MagneticFieldScan::Get().ExecuteLoop(argc, argv);

}

