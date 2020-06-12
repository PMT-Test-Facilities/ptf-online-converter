{

  {

#include <fstream>
    std::ifstream infile("wave0_lots.txt");
    int sample;
    int total = 0;

    TH1F *baseline = new TH1F("baseline","baseline",100,7800,8000);
    TH1F *all = new TH1F("all","all",100,7800,8000);
    while (infile >> sample)
      {
	
	if(total < 70)
	  std::cout << sample << std::endl;

	total++;

	all->Fill(sample);
	if(total%70 < 20)
	  baseline->Fill(sample);
	
      }

    //    all->Draw();
    //all->SetLineColor(2);
    baseline->Draw();
  }


}
