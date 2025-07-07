#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plotCrosscheck() {

    const char* input =   "output/20250706_Skim_ppref2024_debug_noTrackEventSelection_min01.root";
    const char* input_TrackWeighted = "output/20250706_Skim_ppref2024_debug_noTrackEventSelection_min01_trackWeighted.root";
    const char* output =  "plots/crosscheck";
    bool doCrossCheck = true;
    const char* crossCheckInput = "ppzerobias_crosscheck.root";

    TFile* fin = TFile::Open(input, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error: Unable to open file " << fin << std::endl;
        return;
    }

    TFile* fin_corr = TFile::Open(input_TrackWeighted, "READ");
    if (!fin_corr || fin_corr->IsZombie()) {
        std::cerr << "Error: Unable to open file " << input_TrackWeighted << std::endl;
        return;
    }

    TH1D* hNEvtPassCuts = (TH1D*)fin->Get("hNEvtPassCuts");
    TH1D* hNTrkPassCuts = (TH1D*)fin->Get("hNTrkPassCuts");

    TH2D* hTrkPtEta = (TH2D*)fin->Get("hTrkPtEta");
    TH1D* hTrkPt = (TH1D*) hTrkPtEta->ProjectionX("hTrkPt");
    //TH1D* hTrkPt = (TH1D*)fin->Get("hTrkPt");
    TH1D* hTrkEta = (TH1D*) hTrkPtEta->ProjectionY("hTrkEta");

    TH2D* hTrkPtEta_corr = (TH2D*)fin_corr->Get("hTrkPtEta");
    hTrkPtEta_corr->SetName("hTrkPtEta_corr");
    TH1D* hTrkPt_corr = (TH1D*) hTrkPtEta_corr->ProjectionX("hTrkPt_corr");
    TH1D* hTrkEta_corr = (TH1D*) hTrkPtEta_corr->ProjectionY("hTrkEta_corr");

    TH3D* hVXYZ = (TH3D*)fin->Get("hVXYZ");
    TH1D* hVZ = (TH1D*)hVXYZ->ProjectionZ("hVZ");
    TH1D* hVZ_pf = (TH1D*)fin->Get("hVZ_pf");

    // Divide by bin width
    divideByWidth(hTrkPt);
    divideByWidth(hTrkEta);
    divideByWidth(hTrkPt_corr);
    divideByWidth(hTrkEta_corr);

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 2400);
    c1->Divide(2, 4);

    c1->cd(1);
    plotSimple(
        {hNEvtPassCuts}, "NEvtPassCuts", {"NEvtPassCuts"},
        "", -1, -1,
        "Counts", 0, 1e5,
        false, false,
        true
    );

    c1->cd(2);
    plotSimple(
        {hNTrkPassCuts}, "NTrkPassCuts", {"NTrkPassCuts"},
        "", -1, -1,
        "Counts", -1, -1,
        false, false,
        true
    );
    // print bin contents without scientific notation
    std::cout<< "NTrkPassCuts bin contents: "<<endl;
    std::cout << std::fixed;
    for (int i = 1; i <= hNTrkPassCuts->GetNbinsX(); ++i) {
        std::cout << hNTrkPassCuts->GetBinContent(i) <<endl;
    }
    std::cout << std::endl;

    c1->cd(3);
    plotSimple(
        {hTrkPt, hTrkPt_corr}, "hTrkPt", {"hTrkPt", "hTrkPt w trkCorr"},
        "Track pT [GeV/c]", 1, 20,
        "dN/dp_{T}", 1, 1e7,
        false, true
    );
    
    c1->cd(4);
    plotSimple(
        {hTrkEta, hTrkEta_corr}, "hTrkEta", {"hTrkEta", "hTrkEta w trkCorr"},
        "Track #eta", -3, 3,
        "dN/d#eta", 1, 1e7,
        false, true
    );

    // Save as png
    if (!doCrossCheck) {
        c1->SaveAs(Form("%s.png", output));
        return;
    }

    // Load in cross-check histogram
    // Vipul's cross-check direct from their forest output
    TFile* fin_crossCheck = TFile::Open(crossCheckInput, "READ");
    TH1F* hTrkPt_crossCheck = (TH1F*)fin_crossCheck->Get("htrkpt_split");
    divideByWidth(hTrkPt_crossCheck);

    c1->cd(5);
    plotRatioLogy(
        {hTrkPt, hTrkPt_crossCheck}, "Track p_{T}", {"Skim [Kyle]", "Forest [Vipul]"},
        "Track pT [GeV/c]", 1, 20,
        "dN/dp_{T}", 1, 1e7,
        "Forest/Skim", 0.95, 1.05,
        0, true, false
    );

    c1->cd(7);
    plotSimple(
        {hVZ, hVZ_pf}, "Vertex Z", {"VZ", "VZ_pf"},
        "counts", -20, 20,
        "VZ", 20, 1e4,
        false, true
    );

    // Print hVZ - hVZ_pf for each bin
    std::cout << std::endl;
    std::cout << "hVZ - hVZ_pf bin differences:" << std::endl;
    int nBins = std::min(hVZ->GetNbinsX(), hVZ_pf->GetNbinsX());
    float sum = 0;
    for (int i = 1; i <= nBins; ++i) {
        double diff = hVZ->GetBinContent(i) - hVZ_pf->GetBinContent(i);
        sum += diff;
        std::cout << "Bin " << i << ": " << diff << std::endl;
    }
    std::cout << "Total difference: " << sum << std::endl;

    c1->SaveAs(Form("%s.png", output));

}