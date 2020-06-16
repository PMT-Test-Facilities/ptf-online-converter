#include "TTreeMaker.h"



//MakeTree


TTree* MakeTree(){

    TTree *tree = new TTree("scan_tree","Scan Tree");

 
    return tree;
    
}
