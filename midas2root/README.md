# Midas2Root: The PTF Version 

This is a utility for converting a midas event stream into a root file for the PTF analyses. 
It's a modification of the baseline midas2root code with a few fixes/improvements
    - For scans which span multiple files, this code keeps track of whether or not events are being kept as we switch from one file to another
    - No more segfaults! The TTree is always instantiated, so when a scan spans multiple files we don't try to TTree->Fill a null pointer 

These fixes required pairing the c++ executable with a bash script to make sure the call is made properly. 

The bash script keeps track of whether or not data is being kept (ie, if the gantry was moving) and tells 

## Why did those segfaults happen? 

**The Problem:** originally, the TTree was only made when at a "run start" midas event. 
As a consequence, in files after the first, the TTree would never be made. 
And so when the fill command was called we'd get a segfault.
As an extra consequence, later files had no way to communicate the run number to midas2root.

**The Fix:** now we now explicilty make the TTree if it's a null pointer.
The run number is now also provided, by the bash script, as an argument to midas2root_ptf. 

## How does it keep track of gantry motion? 

**The Problem:** the midas2root event stream is interrupted at the end of a file and resumed at the next one. 
Midas2root processes each file's event stream individually, after processing all files it then concatenates the individual files' root files into one big root file. 
Midas2root can, upon starting to process a file's event stream, default to either *accept* or *reject* events (essentially this is assuming the gantry is starting *stationary* or *mid-move*). 

Either assumption cannot be accurate in all cases. 
If it defaults to accept, when midas switches between files mid-move then some scanpoint will end up with an excessive number of noisy non-sense waveforms. 
If it instead defaults to reject, then when midas switches between files mid-scan then that scanpoint will be interrupted. 

**The Fix:** now, when midas2root reaches the end of a file, the acceptance status is returned by the `main` method. The controlling bash script records this return value, and then passes it on as an argument to the next midas2root instance. 


### Possible Better Fix:
The current fix is hacky and leads to midas2root searching for the run number and the movement status as other files. 
If your run number is "5660" and you have a similarly named file, this will lead to unexpected behavior and almost certainly a crash. 

I don't know how easy this would be to implement, but there certainly are more robust fixes possible!
But if midas could include a header event for each new file including the run number, that would be tremendously helpful. 
If these header events could include more meta-information, that would be event better.

An alternative, probably better fix, would be if the midas2root_ptf script could be fundamentally modified to process a series of files as a single event stream. 
This functionality *does* appear to be in the event stream code, but it isn't incorporated with midas2root.

# Usage 

Create a symbolic link in your data folder that points to the `run_convert_to_root.sh` script. 
In the data folder (ie `/home/midptf/online/data`), call 
```
   ./run_convert_to_root.sh <run number> 
```

Another template script for reprocessing multiple runs is included, `reprocess_data.sh`