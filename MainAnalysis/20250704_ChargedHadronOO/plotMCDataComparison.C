#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

#include "include/plotting.h" // Kyle's plotting utilities

void plotMCDataComparison() {

    vector<string> labels = {"OO Run 394153", "OO Run 394153, track correction", "OO HIJING"};
    vector<string> inputFiles = {
        "output/20250705_OO_394153_FIRSTLOOK_eventSel_min04_HFAND14_HFAND12.root",
        "output/20250705_OO_394153_FIRSTLOOK_eventSel_min04_HFAND14_HFAND12_trackWeighted.root",
        "output/skim_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518_eventSel_min04_HFAND14_HFAND12.root"
    };
    const char* output = "plots/394153_min04_HFAND14_HFAND12_trackWeighted";

    float MC_scale = 534170. / 892630.; // Scale factor for MC histograms

    vector<TH1*> hNEvtPassCuts, hNTrkPassCuts, hTrkPt, hTrkEta, hMult, hhiHF_pf, hVZ_pf;
    vector<TH2*> hTrkPtEta, hHFEMaxPlusMinus, hhiHFPlusMinus_pf, hZDCPlusMinus;
    vector<TH1*> hHFEMaxPlus, hHFEMaxMinus, hhiHFPlus_pf, hhiHFMinus_pf, hZDCPlus, hZDCMinus, hVX, hVY, hVZ, hHFEMaxMinus_clone, hHFEMaxPlus_clone;
    vector<TH3*> hVXYZ;
    for (int i = 0; i < inputFiles.size(); i++) {
        TFile* fin = TFile::Open(inputFiles[i].c_str());
        if (!fin || !fin->IsOpen()) {
            std::cerr << "Error: Could not open file " << inputFiles[i] << std::endl;
            return;
        }

        hNEvtPassCuts.push_back((TH1D*)fin->Get("hNEvtPassCuts"));
        //print the number of passed events, last bin
        std::cout << "Number of events passing cuts in " << inputFiles[i] << ": " << hNEvtPassCuts.back()->GetBinContent(hNEvtPassCuts.back()->GetNbinsX()) << std::endl;
        hNTrkPassCuts.push_back((TH1D*)fin->Get("hNTrkPassCuts"));
        hTrkPt.push_back((TH1D*)fin->Get("hTrkPt"));
        hTrkEta.push_back((TH1D*)fin->Get("hTrkEta"));
        hMult.push_back((TH1D*)fin->Get("hMult"));
        hhiHF_pf.push_back((TH1D*)fin->Get("hhiHF_pf"));

        hTrkPtEta.push_back((TH2D*)fin->Get("hTrkPtEta"));

        TH2D* hHFEMaxPlusMinus_this = (TH2D*)fin->Get("hHFEMaxPlusMinus");
        TH1D* hHFEMaxPlus_this = (TH1D*)hHFEMaxPlusMinus_this->ProjectionX(Form("hHFEMaxPlus_%d", i));
        TH1D* hHFEMaxMinus_this = (TH1D*)hHFEMaxPlusMinus_this->ProjectionY(Form("hHFEMaxMinus_%d", i));
        hHFEMaxPlusMinus.push_back(hHFEMaxPlusMinus_this);
        hHFEMaxPlus.push_back(hHFEMaxPlus_this);
        hHFEMaxMinus.push_back(hHFEMaxMinus_this);
        hHFEMaxMinus_clone.push_back((TH1D*)hHFEMaxMinus_this->Clone(Form("hHFEMaxMinus_clone_%d", i)));
        hHFEMaxPlus_clone.push_back((TH1D*)hHFEMaxPlus_this->Clone(Form("hHFEMaxPlus_clone_%d", i)));

        TH2D* hhiHFPlusMinus_pf_this = (TH2D*)fin->Get("hhiHFPlusMinus_pf");
        TH1D* hhiHFPlus_pf_this = (TH1D*)hhiHFPlusMinus_pf_this->ProjectionX(Form("hhiHFPlus_pf_%d", i));
        TH1D* hhiHFMinus_pf_this = (TH1D*)hhiHFPlusMinus_pf_this->ProjectionY(Form("hhiHFMinus_pf_%d", i));
        hhiHFPlusMinus_pf.push_back(hhiHFPlusMinus_pf_this);
        hhiHFPlus_pf.push_back(hhiHFPlus_pf_this);
        hhiHFMinus_pf.push_back(hhiHFMinus_pf_this);

        TH2D* hZDCPlusMinus_this = (TH2D*)fin->Get("hZDCPlusMinus");
        TH1D* hZDCPlus_this = (TH1D*)hZDCPlusMinus_this->ProjectionX(Form("hZDCPlus_%d", i));
        TH1D* hZDCMinus_this = (TH1D*)hZDCPlusMinus_this->ProjectionY(Form("hZDCMinus_%d", i));
        hZDCPlusMinus.push_back(hZDCPlusMinus_this);
        hZDCPlus.push_back(hZDCPlus_this);
        hZDCMinus.push_back(hZDCMinus_this);
        
        if (i == 0) hZDCPlusMinus.push_back((TH2D*)fin->Get("hZDCPlusMinus"));

        TH3D* hVXYZ_this = (TH3D*)fin->Get("hVXYZ");
        hVXYZ.push_back(hVXYZ_this);
        hVX.push_back(hVXYZ_this->ProjectionX(Form("hVX_%d", i)));
        hVY.push_back(hVXYZ_this->ProjectionY(Form("hVY_%d", i)));
        hVZ.push_back(hVXYZ_this->ProjectionZ(Form("hVZ_%d", i)));

        hVZ_pf.push_back((TH1D*)fin->Get("hVZ_pf"));

        // Divide by bin width
        divideByWidth(hTrkPt.back());
        divideByWidth(hTrkEta.back());
        divideByWidth(hHFEMaxMinus.back());
        divideByWidth(hHFEMaxPlus.back());
        divideByWidth(hHFEMaxMinus_clone.back());
        divideByWidth(hHFEMaxPlus_clone.back());

        // if MC, scale histograms
        if (i == 2) {
            hNEvtPassCuts.back()->Scale(MC_scale);
            hNTrkPassCuts.back()->Scale(MC_scale);
            hTrkPt.back()->Scale(MC_scale);
            hTrkEta.back()->Scale(MC_scale);
            hMult.back()->Scale(MC_scale);
            hhiHF_pf.back()->Scale(MC_scale);
            hHFEMaxPlus.back()->Scale(MC_scale);
            hHFEMaxMinus.back()->Scale(MC_scale);
            hhiHFPlus_pf.back()->Scale(MC_scale);
            hhiHFMinus_pf.back()->Scale(MC_scale);
            hZDCPlus.back()->Scale(MC_scale);
            hZDCMinus.back()->Scale(MC_scale);
            hVX.back()->Scale(MC_scale);
            hVY.back()->Scale(MC_scale);
            hVZ.back()->Scale(MC_scale);
            hVZ_pf.back()->Scale(MC_scale);
        }
    }

    // make canvas
    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 3200);
    c1->Divide(2, 5);

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
    plotSimple(
        hTrkPt, "hTrkPt", labels,
        "Track pT [GeV/c]", 0.5, 2,
        "dN/dp_{T}", -1, -1,
        false, false
    );
    
    c1->cd(4);
    plotSimple(
        hTrkEta, "hTrkEta", labels,
        "Track #eta", -3, 3,
        "dN/d#eta", -1, -1,
        false, false
    );

    c1->cd(5);
    plotSimple(
        hMult, "hMult", labels,
        "Multiplicity", 0, 100,
        "Counts", -1, -1,
        false, true
    );

    c1->cd(7);
    plotSimple(
        hZDCPlus, "hZDCPlus", labels,
        "ZDC Sum Plus [GeV]", -1, -1,
        "Counts", 1e2, 1e5,
        false, true
    );

    c1->cd(8);
    plotSimple(
        hZDCMinus, "hZDCMinus", labels,
        "ZDC Sum Minus [GeV]", -1, -1,
        "Counts", 1e2, 1e5,
        false, true
    );

    // Save as png
    c1->SaveAs(Form("%s-basic.png", output));

    // make canvas
    TCanvas* c2 = new TCanvas("c2", "c2", 1600, 3600);
    c2->Divide(2, 6);

    c2->cd(1);
    plot2D(
        hHFEMaxPlusMinus[0], "OO data hHFEMaxPlusMinus",
        "HF EMax Plus [GeV]", -1, -1,
        "HF EMax Minus [GeV]", -1, -1
    );

    c2->cd(2);
    plot2D(
        hHFEMaxPlusMinus[1], "OO HIJING hHFEMaxPlusMinus",
        "HF EMax Plus [GeV]", -1, -1,
        "HF EMax Minus [GeV]", -1, -1
    );

    c2->cd(3);
    plot2D(
        hhiHFPlusMinus_pf[0], "OO data hHFPlusMinus_pf",
        "HF ET Sum Plus [GeV]", -1, -1,
        "HF ET Sum Minus [GeV]", -1, -1
    );

    c2->cd(4);
    plot2D(
        hhiHFPlusMinus_pf[1], "OO HIJING hHFPlusMinus_pf_HIJING",
        "HF ET Sum Plus [GeV]", -1, -1,
        "HF ET Sum Minus [GeV]", -1, -1
    );

    c2->cd(5);
    plotSimple(
        hHFEMaxMinus, "hHFEMaxMinus", labels,
        "HF EMax Minus [GeV]", 0, 400,
        "Counts", -1, -1,
        false, true
    );

    c2->cd(6);
    plotSimple(
        hHFEMaxPlus, "hHFEMaxPlus", labels,
        "HF EMax Plus [GeV]", 0, 400,
        "Counts", -1, -1,
        false, true
    );

    c2->cd(7);
    plotSimple(
        hHFEMaxMinus_clone, "hHFEMaxMinus", labels,
        "HF EMax Minus [GeV]", 0, 40,
        "Counts", -1, -1,
        false, true
    );

    c2->cd(8);
    plotSimple(
        hHFEMaxPlus_clone, "hHFEMaxPlus", labels,
        "HF EMax Plus [GeV]", 0, 40,
        "Counts", -1, -1,
        false, true
    );

    c2->cd(9);
    plotSimple(
        hhiHFMinus_pf, "hhiHFMMinus_pf", labels,
        "HF ET Sum Minus [GeV]", -1, -1,
        "Counts", -1, -1,
        false, true
    );

    c2->cd(10);
    plotSimple(
        hhiHFPlus_pf, "hhiHFPlus_pf", labels,
        "HF ET Sum Plus [GeV]", -1, -1,
        "Counts", -1, -1,
        false, true
    );

    c2->cd(11);
    plotSimple(
        hhiHF_pf, "hhiHF_pf", labels,
        "HF ET Sum [GeV]", 0, 800,
        "Counts", -1, -1,
        false, true
    );

    TFile* fin = TFile::Open(inputFiles[1].c_str());
    TH1D* hTrkWeight = (TH1D*)fin->Get("hTrkWeight");

    c2->cd(12);
    plotSimple(
        {hTrkWeight}, "hTrkWeight", {"hTrkWeight"},
        "Track Weight", 1, 1.7,
        "Counts", -1, -1,
        false, false
    );

    // Save as png
    c2->SaveAs(Form("%s-ZDCHF.png", output));

}
