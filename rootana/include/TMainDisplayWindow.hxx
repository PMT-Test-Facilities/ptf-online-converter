#ifndef _WaveformDisplayWindowRUNWINDOW_H_
#define _WaveformDisplayWindowRUNWINDOW_H_

#include <TGClient.h>
#include <TCanvas.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGObject.h>
#include <TGTab.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <TFile.h>
#include <TApplication.h>
//#include <TGTextButton.h>


class TMainDisplayWindow {
 
  private:

  
  TGMainFrame *fMain;
  TGTab *fTab;
  
  // We have option to have a whole vector of subtabs; this is list of sub-tabs that hang off the main tab...
  TGFrame *fFrame;
  TGHorizontalFrame *fHframe;

  /// Variables to keep track of state of display
  /// Are we processing offline?
  bool fIsOffline;
  /// Is processing paused?
  bool fProcessingPaused;

  /// Buttons to make plots of current pad/canvas
  TGTextButton  *fSavePadButton;
  TGTextButton  *fSaveCanvasButton;

  // Button to reset histograms.
  TGTextButton  *fNextButton;

  // Button to reset histograms.
  TGTextButton  *fResetButton;

  // Button to quit
  TGTextButton  *fQuitButton;
  
  // Button to pause updating (online)
  TGTextButton *fPauseButton;

  // Button to set how many events to skip before plotting
  TGNumberEntry *fNumberSkipEventButton;


  // Add a couple methods for dealing with sub-tabs

  // Helper: get TGTab for a particular tab index.
  TGTab* GetTab(std::pair<int,int> tabindex);

  // Helper: get the current tab for particular tab group
  // If index is unset, get name for the current tab.
  std::string GetTabName(TGTab *tab, int index = -1);

  // Returns the number of sub-tabs for the given tab index.
  // If there are no sub-tabs (ie, this tab actually has a 
  // display), then returns 0.
  int GetNumberSubTabs(int i);

  // Helper : get a particular embedded canvas, based on name.
  TRootEmbeddedCanvas* GetEmbeddedCanvas(const char *name);

 public:

  TGTextButton* GetResetButton(){ return fResetButton;}
  
  TGTextButton* GetNextButton(){ return fNextButton;}
  
  TGTextButton* GetQuitButton(){ return fQuitButton;}

  TGNumberEntry* GetSkipEventButton(){ return fNumberSkipEventButton;}

  TMainDisplayWindow(const TGWindow *p,UInt_t w,UInt_t h, bool isOffline);
  virtual ~TMainDisplayWindow();

  
  /// Method to call when 'save pad' button is pressed.
  void SavePadButtonAction();

  /// Method to call when 'save canvas' button is pressed.
  void SaveCanvasButtonAction();

  /// Method to call when 'pause/resume' button is pressed.
  void PauseResumeButtonAction();

  // Is the display paused?
  bool IsDisplayPaused(){
    return fProcessingPaused;
  }

  std::pair<int,int> AddSingleTab(std::string name, TGTab * tab= 0, int mainTabIndex = -1);
  
  std::pair<int,int> AddCanvas(std::string subtabname, std::string tabname=std::string(""));
  
  /// This method should be called just once, after you have added all 
  /// the canvases and tabs that you want.
  void BuildWindow();

  void ResetSize();
  

  /// Get a particular canvas based on canvas name.
  TCanvas* GetCanvas(const char *name);
  
 
  /// Return the current embedded canvas.
  TRootEmbeddedCanvas* GetCurrentEmbeddedCanvas();

  // REturn the current composite frame.
  TGCompositeFrame* GetCurrentCompositeFrame();

  // Return the composite frame for a particular tab index.
  TGCompositeFrame* GetCompositeFrame(std::pair<int,int> index);

  /// Return an pair<int> index for current tab/sub-tab;
  std::pair<int,int> GetCurrentTabIndex();

  /// Return the canvas name for the current tab.
  std::string GetCurrentTabName();

  // Return the sub-tab-group for a particular tab.
  // Returns 0 if that tab doesn't have a sub-tab group.
  TGTab* GetSubTab(int index);

  // Get the main window.
  TGMainFrame* GetMain(){return fMain;};
  TGTab* GetTab(){return fTab;};



  ClassDef(TMainDisplayWindow,1)
};


#endif
