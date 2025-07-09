#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plotTrackWeightings() {

    vector<string> labels = {"ppref, loose", "ppref, nominal", "ppref, tight", "ppref, 2017", "uncorrected"};
  vector<string> inputFiles = {
      "output/output_20250708_Skim_ppref2024_all_Loose.root",
      "output/output_20250708_Skim_ppref2024_all_Nominal.root",
      "output/output_20250708_Skim_ppref2024_all_Tight.root",
      "output/output_20250708_Skim_ppref2024_all_2017.root",
      "output/output_20250708_Skim_ppref2024_all_noTrack.root"
  };
  const char* output = "plots/output_20250708_Skim_ppref2024_all_trackWeightings";

    /*
    vector<string> labels = {"ppref, loose", "ppref, nominal", "ppref, tight", "ppref, 2017", "uncorrected"};
    vector<string> inputFiles = {
        "output/20250708_Skim_ppref2024_debug_noTrackEventSelection_min04_Loose.root",
        "output/20250708_Skim_ppref2024_debug_noTrackEventSelection_min04_Nominal.root",
        "output/20250708_Skim_ppref2024_debug_noTrackEventSelection_min04_Tight.root",
        "output/20250708_Skim_ppref2024_debug_noTrackEventSelection_min04_2017.root",
        "output/20250708_Skim_ppref2024_debug_noTrackEventSelection_min04_noTrack.root"
    };
    const char* output = "plots/20250708_Skim_ppref2024_debug_noTrackEventSelection_min04_trackWeightingsOverlay";
    */

    vector<TH1*> hNEvtPassCuts, hNTrkPassCuts, hTrkPt, hTrkEta, hMult, hVZ_pf, hTrkWeight;
    vector<TH2*> hTrkPtEta;
    vector<TH1*> hVX, hVY, hVZ;
    vector<TH3*> hVXYZ;
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

        hTrkPtEta.push_back((TH2D*)fin->Get("hTrkPtEta"));

        // Divide by bin width
        divideByWidth(hTrkPt.back());
        divideByWidth(hTrkEta.back());
    }

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 2400);
    c1->Divide(2, 4);

    c1->cd(1);
    plotSimple(
        hNEvtPassCuts, "NEvtPassCuts", labels,
        "", -1, -1,
        "Counts", -1, -1,
        false, false,
        true
    );

    c1->cd(2);
    plotSimple(
        hNTrkPassCuts, "NTrkPassCuts", labels,
        "", -1, -1,
        "Counts", -1, -1,
        false, false,
        true
    );

    c1->cd(3);
    plotRatioLogy(
        hTrkPt, "", labels,
        "Track pT [GeV/c]", 0.5, 20,
        "dN/dp_{T}", -1, -1,
        "Ratio to nominal", 0.7, 1.3,
        1, true, false
    );
    
    c1->cd(4);
    plotRatioLogy(
        hTrkEta, "", labels,
        "Track #eta", -3, 3,
        "dN/d#eta", -1, -1,
        "Ratio to nominal", 0.7, 1.3,
        1, true, false
    );

    c1->cd(5);
    plotSimple(
        hMult, "hMult", labels,
        "Multiplicity", 0, 100,
        "Counts", -1, -1,
        false, true
    );

    c1->cd(6);
    plotSimple(
        hTrkWeight, "hTrkWeight", labels,
        "Track Weight", 1.05, 1.3,
        "Counts", -1, -1,
        false, false
    );

    c1->SaveAs(Form("%s.png", output));

}