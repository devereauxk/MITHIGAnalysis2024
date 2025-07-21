// run with
// root -l -b -q PlottingExample.C
#include <filesystem>
#include <iostream>
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "include/plotting.h"

void plotFancy() {
  // Make output directory and simple hists for examples
  system("mkdir -p ./plots");

  vector<string> labels = {"OO Run 394270", "tracks uncorrected", "OO HIJING MC Reco"};
  vector<string> inputFiles = {
      "output/20250709_NeNe_394270_PhysicsIonPhysics0_250708_201011.root",
      "output/20250709_NeNe_394270_PhysicsIonPhysics0_250708_201011_noTrack.root",
      "output/skim_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518_eventSel_min04_HFAND14_HFAND12.root"
  };
  const char* output = "plots/20250709_NeNe_394270_PhysicsIonPhysics0_250708_201011";

  float MC_scale = 96727./892630; // Scale factor for MC histograms

  vector<TH1*> hNEvtPassCuts, hNTrkPassCuts, hTrkPt, hTrkEta, hMult, hVZ_pf, hTrkWeight;
  vector<TH2*> hTrkPtEta;
  vector<TH1*> hVX, hVY, hVZ;
  vector<TH3*> hVXYZ;
  vector<TH2D*> hTrkWeightPt;
  for (int i = 0; i < inputFiles.size(); i++) {
      TFile* fin = TFile::Open(inputFiles[i].c_str());
      if (!fin || !fin->IsOpen()) {
          std::cerr << "Error: Could not open file " << inputFiles[i] << std::endl;
          return;
      }

      hNEvtPassCuts.push_back((TH1D*)fin->Get("hNEvtPassCuts"));
      hNTrkPassCuts.push_back((TH1D*)fin->Get("hNTrkPassCuts"));
      hTrkPt.push_back((TH1D*)fin->Get("hTrkPt"));
      hTrkEta.push_back((TH1D*)fin->Get("hTrkEta"));
      hMult.push_back((TH1D*)fin->Get("hMult"));
      hTrkWeight.push_back((TH1D*)fin->Get("hTrkWeight"));
      hTrkWeightPt.push_back((TH2D*)fin->Get("hTrkWeightPt"));

      hTrkPtEta.push_back((TH2D*)fin->Get("hTrkPtEta"));

      // Divide by bin width
      divideByWidth(hTrkPt.back());
      divideByWidth(hTrkEta.back());

      if (i == 2) {
          // Scale MC histograms
          hTrkPt.back()->Scale(MC_scale);
          hTrkEta.back()->Scale(MC_scale);
          hMult.back()->Scale(MC_scale);
      }
  }
  
  // ===========================================================================
  // EXAMPLE 1: Styling a Single Canvas ----------------------------------------
  TCanvas* ex1Canvas = new TCanvas("ex1Canvas", "", 800, 600);

  TPad* pad1 = (TPad*) plotCMSSimple(
    ex1Canvas, hTrkPt, "", labels,
    {cmsBlue, cmsGray, cmsYellow}, {0, 1, 1},
    {cmsRed, cmsRed, cmsRed, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
    "Track pT [GeV/c]", 0.5, 25,
    "dN/dp_{T}", -1, -1,
    false, true
  );

  // >>> Add the CMS header
  AddCMSHeader(
      pad1,      // Provide the TPad
      "Internal", // (optional) Add a subheader to the CMS header
      false
  );
  AddUPCHeader(pad1, "5.36 TeV", "OO Run 394270");
  pad1->Update();

  /*
  AddPlotLabel(
    ex1Pad,      // Provide the TPad
    "-1 < #eta < 1" , // (optional) Add a label to the plot
    0.04,        // (optional) Set the text size
    0.02,        // (optional) Set the text offset
    0.75,        // (optional) Set the X position of the label (move right)
    0.85         // (optional) Set the Y position of the label (top)
  );
  */
  
  // That's it!
  ex1Canvas->SaveAs(Form("%s_pT.pdf", output));

  // ===========================================================================
  // EXAMPLE 1: Styling a Single Canvas ----------------------------------------
  TCanvas* c_ratio = new TCanvas("c_ratio", "", 800, 600);

  TPad* pad_ratio = (TPad*) plotCMSRatio(
    hTrkPt, "", labels,
    {cmsBlue, cmsGray, cmsYellow}, {0, 1, 1},
    {cmsRed, cmsRed, cmsRed, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
    "Track pT [GeV/c]", 3, 50,
    "dN/dp_{T}", -1, -1,
    "Ratio over corrected      ", 0.5, 1.5,
    0, true
  );

  // >>> Add the CMS header
  AddCMSHeader(
      pad_ratio,      // Provide the TPad
      "Internal", // (optional) Add a subheader to the CMS header
      false
  );
  AddUPCHeader(pad_ratio, "5.36 TeV", "OO Run 394270");
  pad_ratio->Update();

  /*
  AddPlotLabel(
    ex1Pad,      // Provide the TPad
    "-1 < #eta < 1" , // (optional) Add a label to the plot
    0.04,        // (optional) Set the text size
    0.02,        // (optional) Set the text offset
    0.75,        // (optional) Set the X position of the label (move right)
    0.85         // (optional) Set the Y position of the label (top)
  );
  */
  
  // That's it!
  c_ratio->SaveAs(Form("%s_pTratio.pdf", output));

  // ===========================================================================
  TCanvas* ex2Canvas = new TCanvas("ex2Canvas", "", 800, 600);

  TPad* pad_Nevt = (TPad*) plotCMSSimple(
    ex2Canvas, {hNEvtPassCuts[0]}, "", {labels[0]},
    {cmsRed}, {0}, {cmsBlack}, {0},
    "", -1, -1,
    "Events passed", -1, -1,
    false, false, true
  );

  AddCMSHeader(
    pad_Nevt,      // Provide the TPad
    "Internal", // (optional) Add a subheader to the CMS header
    true
  );
  AddUPCHeader(pad_Nevt, "5.36 TeV", "OO Run 394270");
  pad_Nevt->Update();

  // That's it!
  ex2Canvas->SaveAs(Form("%s_Nevt.pdf", output));

  // ===========================================================================
  TCanvas* ex3Canvas = new TCanvas("ex3Canvas", "", 800, 600);
  
  TPad* pad_eta = (TPad*) plotCMSSimple(
    ex3Canvas, hTrkEta, "", labels,
    {cmsBlue, cmsGray, cmsYellow}, {0, 1, 1},
    {cmsRed, cmsRed, cmsRed, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
    "#eta", -2.4, 2.4,
    "dN/d#eta", -1, -1,
    false, false
  );

  AddCMSHeader(
    pad_eta,      // Provide the TPad
    "Internal", // (optional) Add a subheader to the CMS header
    true
  );
  AddUPCHeader(pad_eta, "5.36 TeV", "OO Run 394270");
  pad_eta->Update();

  // That's it!
  ex3Canvas->SaveAs(Form("%s_eta.pdf", output));

  // ===========================================================================
  // EXAMPLE 4: Plot TProfile of hTrkWeightPt (average weight vs trackPt) ------
  TCanvas* ex4Canvas = new TCanvas("ex4Canvas", "", 800, 600);

  // Make TProfiles from hTrkWeightPt and convert to TH1Ds
  vector<TH1*> profTrkWeightPt;
  for (int i = 0; i < hTrkWeightPt.size(); i++) {
    TH2D* h2 = hTrkWeightPt[i];
    TProfile* prof = h2->ProfileX();
    profTrkWeightPt.push_back((TH1D*)prof->ProjectionX(Form("prof_%i", i)));
  }

  TPad* pad_weights = (TPad*) plotCMSSimple(
    ex4Canvas, profTrkWeightPt, "", labels,
    {cmsBlue, cmsGray, cmsYellow}, {0, 1, 1},
    {cmsRed, cmsRed, cmsRed, cmsRed, cmsRed}, {mCircleFill, mCircleFill, mCircleFill, mCircleFill, mCircleFill},
    "Track p_{T} [GeV/c]", 0.5, 50,
    "Average track weight", 0.8, 1.5,
    false, false
  );

  AddCMSHeader(
    pad_weights,      // Provide the TPad
    "Internal", // (optional) Add a subheader to the CMS header
    true
  );
  AddUPCHeader(pad_weights, "5.36 TeV", "OO Run 394270");

  ex4Canvas->SaveAs(Form("%s_trkWeight_vs_pt.pdf", output));

}
