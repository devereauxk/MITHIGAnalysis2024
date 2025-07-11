#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plotCrosscheck() {

    vector<string> labels = {"Forest-level", "Skim-level"};
    vector<string> inputFiles = {
        "ppzerobias_crosscheck.root",
        "output/20250708_Skim_ppref2024_debug_noTrackEventSelection_min01_noTrack.root"
    };
    const char* output = "plots/ppzerobias_crosscheck";

    vector<TH1*> hNEvtPassCuts, hNTrkPassCuts, hTrkPt, hTrkEta, hMult, hVZ_pf, hTrkWeight;
    vector<TH2*> hTrkPtEta;
    vector<TH1*> hVX, hVY, hVZ;
    vector<TH3*> hVXYZ;
    vector<TH2D*> hTrkWeightPt;
    for (int i = 0; i < 2; i++) {
        TFile* fin = TFile::Open(inputFiles[i].c_str());
        if (!fin || !fin->IsOpen()) {
            std::cerr << "Error: Could not open file " << inputFiles[i] << std::endl;
            return;
        }

        // skim
        if (i == 1) {
            hNEvtPassCuts.push_back((TH1D*)fin->Get("hNEvtPassCuts"));
            hNTrkPassCuts.push_back((TH1D*)fin->Get("hNTrkPassCuts"));
            
            hTrkEta.push_back((TH1D*)fin->Get("hTrkEta"));
            hMult.push_back((TH1D*)fin->Get("hMult"));
            hTrkWeight.push_back((TH1D*)fin->Get("hTrkWeight"));
            hTrkWeightPt.push_back((TH2D*)fin->Get("hTrkWeightPt"));

            TH2D* hTrkPtEta_this = (TH2D*)fin->Get("hTrkPtEta");
            hTrkPtEta.push_back(hTrkPtEta_this);
            hTrkPt.push_back((TH1D*)hTrkPtEta_this->ProjectionX("hTrkPt"));

            // Divide by bin width
            divideByWidth(hTrkPt.back());
            divideByWidth(hTrkEta.back());
        
        // forest
        } else {
            hTrkPt.push_back((TH1D*)fin->Get("htrkpt"));
            divideByWidth(hTrkPt.back());
        }

        // Print comma-delimited bin edges for hTrkPt (skim and forest)
        TH1D* hTrkPt_hist = (TH1D*) hTrkPt[i];
        int nBinEdges = hTrkPt_hist->GetNbinsX() + 1;
        std::cout << "hTrkPt bin edges (" << labels[i] << ") [count: " << nBinEdges << "]: ";
        for (int b = 1; b <= nBinEdges; ++b) {
            std::cout << hTrkPt_hist->GetBinLowEdge(b);
            if (b < nBinEdges) std::cout << ",";
        }
        std::cout << std::endl;
    }


    // =====================================================================
    TCanvas* c_ratio = new TCanvas("c_ratio", "", 800, 600);

    TPad* pad_ratio = (TPad*) plotCMSRatio(
        hTrkPt, "", labels,
        {cmsTealL1, cmsBlue}, {1, 2},
        {cmsRed, cmsRed}, {mCircleFill, mCircleFill},
        "Track pT [GeV/c]", 1, 18,
        "dN/dp_{T}", 1, 1e7,
        "Skim/Forest      ", 0.95, 1.05,
        0, true
    );

    // >>> Add the CMS header
    AddCMSHeader(
        pad_ratio,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        false
    );
    AddUPCHeader(pad_ratio, "5.26 TeV", "pp ref");
    pad_ratio->Update();
    
    // That's it!
    c_ratio->SaveAs(Form("%s_pTratio.pdf", output));


    // ===========================================================================
    TCanvas* ex2Canvas = new TCanvas("ex2Canvas", "", 800, 600);

    TPad* pad_Nevt = (TPad*) plotCMSSimple(
        ex2Canvas, {hNEvtPassCuts[0]}, "", labels,
        {cmsTealL1, cmsBlue}, {1, 1},
        {cmsRed, cmsRed}, {mCircleFill, mCircleFill},
        "", -1, -1,
        "Events passed", -1, -1,
        false, false, true
    );

    AddCMSHeader(
        pad_Nevt,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );
    AddUPCHeader(pad_Nevt, "5.26 TeV", "pp ref");
    pad_Nevt->Update();

    // That's it!
    ex2Canvas->SaveAs(Form("%s_Nevt.pdf", output));


    // ===========================================================================
    TCanvas* ex3Canvas = new TCanvas("ex3Canvas", "", 800, 600);

    TPad* pad_Ntrk = (TPad*) plotCMSSimple(
        ex3Canvas, {hNTrkPassCuts[0]}, "", labels,
        {cmsTealL1, cmsBlue}, {1, 1},
        {cmsRed, cmsRed}, {mCircleFill, mCircleFill},
        "", -1, -1,
        "Tracks passed", -1, -1,
        false, false, true
    );

    AddCMSHeader(
        pad_Ntrk,      // Provide the TPad
        "Internal", // (optional) Add a subheader to the CMS header
        true
    );
    AddUPCHeader(pad_Ntrk, "5.26 TeV", "pp ref");
    pad_Ntrk->Update();

    // That's it!
    ex3Canvas->SaveAs(Form("%s_Ntrk.pdf", output));

}