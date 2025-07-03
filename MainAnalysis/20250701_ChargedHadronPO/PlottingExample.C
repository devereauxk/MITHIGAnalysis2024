// run with
// root -l -b -q PlottingExample.C
#include <filesystem>
#include <iostream>
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "MITHIG_CMSStyle.h"

// This is only to create sample hists for plotting
void FillExampleHists(
  TH1D* h1,
  TH2D* h2
) {
  h1->Sumw2();
  h2->Sumw2();
  TRandom rand;
  for (int i = 0; i < 5000; ++i) {
    double x = rand.Gaus(0, 1);
    double y = rand.Gaus(0, 1);
    h1->Fill(x);
    h2->Fill(x, y);
  }
}

void PlottingExample() {
  // Make output directory and simple hists for examples
  system("mkdir -p ./PlottingExamplePlots");
  TH1D *h1 = new TH1D("h1", "1D Gaussian; X; Counts", 6, -3, 3);
  TH2D *h2 = new TH2D("h2", "2D Gaussian; X; Y; Counts", 5, -3, 3, 5, -3, 3);
  FillExampleHists(h1, h2);
  
  
  // ===========================================================================
  // EXAMPLE 1: Styling a Single Canvas ----------------------------------------
  TCanvas* ex1Canvas = new TCanvas("ex1Canvas", "", 600, 600);
  
  // Get the canvas pad to pass to other functions
  TPad* ex1Pad = (TPad*) ex1Canvas->GetPad(0);
  ex1Pad->cd();
  
  // >>> Apply the CMS TDR style <<<
  SetTDRStyle();
  
  // Draw the TH1 as normal.
  h1->SetMarkerColor(cmsBlue);      // You can easily use official CMS colors
  h1->SetMarkerStyle(mCircleFill);  // And easily choose markers
  h1->SetLineColor(cmsRed);
  h1->SetLineWidth(3);
  h1->Draw();
  
  // >>> Add the CMS header and the UPC collisions energy header <<<
  AddCMSHeader(ex1Pad);
  AddUPCHeader(ex1Pad);
  ex1Pad->Update();
  
  // That's it!
  ex1Canvas->SaveAs("PlottingExamplePlots/plot_example1.pdf");
  
  
  // ===========================================================================
  // EXAMPLE 2: Plotting a TH1 and a TH2 ---------------------------------------
  TCanvas* ex2Canvas = new TCanvas("ex2Canvas", "", 1320, 600);
  TPad* ex2Pad1 = new TPad("ex2Pad1", "", 0., 0., 0.4545, 1.);
  TPad* ex2Pad2 = new TPad("ex2Pad2", "", 0.4545, 0., 1., 1.);
  ex2Pad1->Draw();
  ex2Pad2->Draw();
  // 1. LEFT PAD (TH1) -----
  ex2Pad1->cd();

  // >>> You can auto style a TH1 and draw it to the pad <<<
  StyleTH1(
    ex2Pad1,      // Provide the TPad
    h1,           // Provide the TH1
    "-3 < x < 3", // (optional) Add a plot label
    cmsPurple,    // (optional) Choose a color (default is black)
    mSquareLine   // (optional) Choose the marker ("-Line" uses outlined shape)
  );
  
  // Feel free to edit the hist after this!
  h1->SetLineWidth(2);
  h1->SetMaximum(1.25*h1->GetMaximum());

  // Add the headers at the end
  AddCMSHeader(
    ex2Pad1,      // Provide the TPad
    "Preliminary" // (optional) Add a subheader to the CMS header
  );
  AddUPCHeader(ex2Pad1);
  ex2Pad1->Update();
  
  // 2. RIGHT PAD (TH2) -----
  ex2Pad2->cd();
  
  // >>> You can also auto style a TH2 and draw it to the pad <<<
  BuildPalettes(); // Call this to use custom palettes
  StyleTH2(
    ex2Pad2,    // Provide the TPad
    h2,         // Provide the TH2
    "Internal", // (optional) Use empty string to skip plot label
    true,       // (optional) If true, draws text on bins
    true,       // (optional) If true,
    0          // (optional) choose a color palette:
  );            //    0: cmsSunset, 1:cmsRedGrad, 2:cmsBlueGrad

  // Add the headers
  AddCMSHeader(
    ex2Pad2,      // Provide the TPad
    "", // (optional) Add a subheader to the CMS header
    false         // (optional) Set false to draw CMS header outside of frame
  );
  AddUPCHeader(ex2Pad2);
  ex2Pad2->Update();

  // That's it!
  ex2Canvas->SaveAs("PlottingExamplePlots/plot_example2.pdf");
}
