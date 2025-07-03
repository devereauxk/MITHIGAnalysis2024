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

void divideByWidth(TH1* hist) {
  if (!hist) {
    std::cerr << "Error: Null histogram pointer passed to divideByWidth function." << std::endl;
    return;
  }

  int nBins = hist->GetNbinsX();
  for (int i = 1; i <= nBins; ++i) {
    double binContent = hist->GetBinContent(i);
    double binError = hist->GetBinError(i);
    double binWidth = hist->GetBinWidth(i);

    if (binWidth != 0) {
      hist->SetBinContent(i, binContent / binWidth);
      hist->SetBinError(i, binError / binWidth);
    } else {
      std::cerr << "Warning: Bin width is zero for bin " << i << ". Skipping division for this bin." << std::endl;
    }
  }
}

void LabelBinContent(TH1* hist) {
  for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
    double x = hist->GetBinCenter(bin);
    double y = hist->GetBinContent(bin);
    double y_offset = (y > 0) ? y + 0.03 * (hist->GetMaximum() - hist->GetMinimum()) : 0.03 * (hist->GetMaximum() - hist->GetMinimum());
    TLatex latex;
    latex.SetTextAlign(22);
    latex.SetTextSize(0.027);
    latex.DrawLatex(x, y_offset, Form("%.0f", y));
  }
}

void plotFancy() {
  // Make output directory and simple hists for examples
  system("mkdir -p ./plots");

  TFile* fin = TFile::Open("output/output.root", "READ");

  TH1D *hTrkPt = (TH1D*)fin->Get("hTrkPt");
  TH1D *hNEvtPassCuts = (TH1D*)fin->Get("hNEvtPassCuts");
  TH1D *hTrkEta = (TH1D*)fin->Get("hTrkEta");

  TH1D *hZDCPlus = (TH1D*)fin->Get("hZDCPlus");
  TH1D *hZDCMinus = (TH1D*)fin->Get("hZDCMinus");
  TH1D *hZDCPlus_noEvtSel = (TH1D*)fin->Get("hZDCPlus_noEvtSel");
  TH1D *hZDCMinus_noEvtSel = (TH1D*)fin->Get("hZDCMinus_noEvtSel");
  
  // ===========================================================================
  // EXAMPLE 1: Styling a Single Canvas ----------------------------------------
  TCanvas* ex1Canvas = new TCanvas("ex1Canvas", "", 800, 600);
  
  // Get the canvas pad to pass to other functions
  TPad* ex1Pad = (TPad*) ex1Canvas->GetPad(0);
  ex1Pad->cd();
  ex1Pad->SetLogy();
  
  // >>> Apply the CMS TDR style <<<
  SetTDRStyle();
  
  // Draw the TH1 as normal.
  hTrkPt->GetXaxis()->SetTitle("p_{T} [GeV/c]");
  hTrkPt->GetYaxis()->SetTitle("dN/dp_{T}");
  hTrkPt->GetXaxis()->SetRangeUser(0.5, 25);
  hTrkPt->GetYaxis()->SetRangeUser(1, 2e7);

  divideByWidth(hTrkPt);

  hTrkPt->SetMarkerColor(cmsBlue);      // You can easily use official CMS colors
  hTrkPt->SetMarkerStyle(mCircleFill);  // And easily choose markers
  hTrkPt->SetLineColor(cmsRed);
  hTrkPt->SetLineWidth(3);
  hTrkPt->Draw();

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
  
  // >>> Add the CMS header
  AddCMSHeader(
    ex1Pad,      // Provide the TPad
    "Internal", // (optional) Add a subheader to the CMS header
    false
  );
  AddUPCHeader(ex1Pad, "9.6 TeV", "Run 393952 pO");
  ex1Pad->Update();
  
  // That's it!
  ex1Canvas->SaveAs("plots/fancy_pT.pdf");


  // ===========================================================================
  TCanvas* ex2Canvas = new TCanvas("ex2Canvas", "", 800, 600);

  TPad* ex2Pad = (TPad*) ex2Canvas->GetPad(0);
  ex2Pad->cd();

  //LabelBinContent(hNEvtPassCuts);

  SetTDRStyle();

  hNEvtPassCuts->GetYaxis()->SetTitle("Counts");
  hNEvtPassCuts->GetYaxis()->SetRangeUser(0, 22e6);
  hNEvtPassCuts->SetLineColor(cmsRed);
  hNEvtPassCuts->SetLineWidth(3);
  hNEvtPassCuts->Draw("SAME TEXT0");

  AddCMSHeader(
    ex2Pad,      // Provide the TPad
    "Internal", // (optional) Add a subheader to the CMS header
    true
  );
  AddUPCHeader(ex2Pad, "9.6 TeV", "Run 393952 pO");
  ex2Pad->Update();

  // That's it!
  ex2Canvas->SaveAs("plots/fancy_NEvtPass.pdf");

  // ===========================================================================
  TCanvas* ex3Canvas = new TCanvas("ex3Canvas", "", 800, 600);
  TPad* ex3Pad = (TPad*) ex3Canvas->GetPad(0);
  ex3Pad->cd();

  SetTDRStyle();

  // Draw the TH1 as normal.
  hTrkEta->GetXaxis()->SetTitle("#eta");
  hTrkEta->GetYaxis()->SetTitle("dN/d#eta");
  hTrkEta->GetXaxis()->SetRangeUser(-2.4, 2.4);
  //hTrkEta->GetYaxis()->SetRangeUser(1, 2e7);

  divideByWidth(hTrkEta);

  hTrkEta->SetMarkerColor(cmsRed);      // You can easily use official CMS colors
  hTrkEta->SetMarkerStyle(mCircleFill);  // And easily choose markers
  hTrkEta->SetLineColor(cmsBlue);
  hTrkEta->SetLineWidth(3);
  hTrkEta->Draw();

  AddCMSHeader(
    ex3Pad,      // Provide the TPad
    "Internal", // (optional) Add a subheader to the CMS header
    true
  );
  AddUPCHeader(ex3Pad, "9.6 TeV", "Run 393952 pO");
  ex3Pad->Update();

  // That's it!
  ex3Canvas->SaveAs("plots/fancy_eta.pdf");


  // ===========================================================================
  TCanvas* ex4Canvas = new TCanvas("ex4Canvas", "", 800, 600);
  TPad* ex4Pad = (TPad*) ex4Canvas->GetPad(0);
  ex4Pad->cd(); 
  ex4Pad->SetLogy();

  SetTDRStyle();

  // Draw the TH1 as normal.
  hZDCPlus->GetXaxis()->SetTitle("ZDC Plus Energy [GeV]");
  hZDCPlus->GetYaxis()->SetTitle("Counts");
  hZDCPlus->GetXaxis()->SetRangeUser(0, 10000);
  hZDCPlus->GetYaxis()->SetRangeUser(1e1, 1e8);
  hZDCPlus->SetLineColor(cmsRed);
  hZDCPlus->SetLineWidth(2);
  hZDCPlus->Draw("HIST");

  hZDCPlus_noEvtSel->SetLineColor(cmsRed);
  hZDCPlus_noEvtSel->SetLineWidth(2);
  hZDCPlus_noEvtSel->SetLineStyle(2);
  hZDCPlus_noEvtSel->Draw("HIST SAME");

  TLegend* leg = new TLegend(0.45, 0.60, 0.68, 0.72);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.035);
  leg->AddEntry(hZDCPlus_noEvtSel, "ZDC Plus", "l");
  leg->AddEntry(hZDCPlus, "ZDC Plus with event selection", "l");
  leg->Draw();

  AddPlotLabel(
    ex4Pad,      // Provide the TPad
    "Oxygen-Going ZDC", // (optional) Add a label to the plot
    0.04,        // (optional) Set the text size
    0.02,        // (optional) Set the text offset
    0.75,        // (optional) Set the X position of the label (move right)
    0.85         // (optional) Set the Y position of the label (top)
  );

  AddCMSHeader(
    ex4Pad,      // Provide the TPad
    "Internal" // (optional) Add a subheader to the CMS header
  );
  AddUPCHeader(ex4Pad, "9.6 TeV", "Run 393952 pO");
  ex4Pad->Update();

  // That's it!
  ex4Canvas->SaveAs("plots/fancy_ZDCPlus.pdf");

  // ===========================================================================
  TCanvas* ex5Canvas = new TCanvas("ex5Canvas", "", 800, 600);
  TPad* ex5Pad = (TPad*) ex5Canvas->GetPad(0);
  ex5Pad->cd(); 
  ex5Pad->SetLogy();

  SetTDRStyle();

  // Draw the TH1 as normal.
  hZDCMinus->GetXaxis()->SetTitle("ZDC Minus Energy [GeV]");
  hZDCMinus->GetYaxis()->SetTitle("Counts");
  hZDCMinus->GetXaxis()->SetRangeUser(0, 10000);
  hZDCMinus->GetYaxis()->SetRangeUser(1e1, 1e8);
  hZDCMinus->SetLineColor(cmsBlue);
  hZDCMinus->SetLineWidth(2);
  hZDCMinus->Draw("HIST");

  hZDCMinus_noEvtSel->SetLineColor(cmsBlue);
  hZDCMinus_noEvtSel->SetLineWidth(2);
  hZDCMinus_noEvtSel->SetLineStyle(2);
  hZDCMinus_noEvtSel->Draw("HIST SAME");

  TLegend* leg2 = new TLegend(0.45, 0.60, 0.68, 0.72);
  leg2->SetBorderSize(0);
  leg2->SetFillStyle(0);
  leg2->SetTextFont(42);
  leg2->SetTextSize(0.035);
  leg2->AddEntry(hZDCMinus_noEvtSel, "ZDC Minus", "l");
  leg2->AddEntry(hZDCMinus, "ZDC Minus with event selection", "l");
  leg2->Draw();

  AddPlotLabel(
    ex5Pad,      // Provide the TPad
    "Proton-Going ZDC", // (optional) Add a label to the plot
    0.04,        // (optional) Set the text size
    0.02,        // (optional) Set the text offset
    0.75,        // (optional) Set the X position of the label (move right)
    0.85         // (optional) Set the Y position of the label (top)
  );

  AddCMSHeader(
    ex5Pad,      // Provide the TPad
    "Internal" // (optional) Add a subheader to the CMS header
  );
  AddUPCHeader(ex5Pad, "9.6 TeV", "Run 393952 pO");
  ex5Pad->Update();

  // That's it!
  ex5Canvas->SaveAs("plots/fancy_ZDCMinus.pdf");

}
