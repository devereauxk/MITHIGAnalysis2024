#include <TCanvas.h>
#include <TCut.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TLegend.h>
#include <TTree.h>

#include <iostream>

using namespace std;
#include "CommandLine.h"
#include "Messenger.h"
#include "ProgressBar.h"
#include "helpMessage.h"
#include "parameter.h" // Parameters for the analysis
#include "utilities.h" // Utility functions for the analysis
#include "skimSelectionBits_OO_PP.h"


const Int_t nPtBins = 47;
const Double_t pTBins_fine[nPtBins + 1] = {
  0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 3.2, 4.0, 4.8, 5.6, 6.4, 7.2, 8.4, 9.6, 10.8, 12.0, 14.4, 15.8, 17.6, 19.2, 22.0, 24.0, 26.2, 28.8, 32.0, 35.2, 38.0, 41.6, 43.6, 48.0, 54.0, 60.8, 73.6, 86.4, 103.6, 120.8, 140.0, 165.0, 250.0, 400.0
};

const Int_t nPtBins_log = 26;
const Double_t pTBins_log[nPtBins_log + 1] = {
  0.5, 0.603, 0.728, 0.879, 1.062, 1.284, 1.553, 1.878, 2.272, 2.749, 3.327, 4.027, 4.872, 5.891, 7.117, 8.591, 10.36, 12.48, 15.03, 18.08, 21.73, 26.08, 31.28, 37.48, 44.89, 53.73, 64.31
};
// Note: The above values are generated using logarithmic spacing between 0.5 and 400 with 45 bins.
// If you want to generate them programmatically, you can use a loop with log10 spacing.

//============================================================//
// Function to check for configuration errors
//============================================================//
bool checkError(const Parameters &par) { return false; }


bool eventSelection(const ChargedHadronRAATreeMessenger *MChargedHadronRAA, TH1D* hNEvtPassCuts) {

  hNEvtPassCuts->Fill(1); // Total events

  //cout<<MChargedHadronRAA->HLT_OxyZeroBias_v1<<endl;
  if (!MChargedHadronRAA->HLT_OxyZeroBias_v1) return false;
  hNEvtPassCuts->Fill(2); // HLT trigger

  if (MChargedHadronRAA->ClusterCompatibilityFilter == false) return false;
  hNEvtPassCuts->Fill(3); // Centrality filter

  if (MChargedHadronRAA->PVFilter == false) return false;
  hNEvtPassCuts->Fill(4); // Primary vertex filter

  if (MChargedHadronRAA->isFakeVtx) return false;
  hNEvtPassCuts->Fill(5); // Not a fake vertex

  // using VZ as found from pTSumVtx method (skim) rather than particle flow method used in the forest
  if (fabs(MChargedHadronRAA->VZ) >= 15.0) return false;
  hNEvtPassCuts->Fill(6); // Vertex Z position within range

  if (MChargedHadronRAA->nTracksVtx < 0) return false;
  hNEvtPassCuts->Fill(7); // Number of tracks in vertex

  if (!checkHFORCondition(MChargedHadronRAA, 10, false)) return false;
  hNEvtPassCuts->Fill(8);

  return true;
}

bool trackSelection(const ChargedHadronRAATreeMessenger *MChargedHadronRAA, unsigned long j, TH1D* hNTrkPassCuts, bool doEtaCut = true) {

  if (j >= MChargedHadronRAA->trkPt->size()) return false;

  hNTrkPassCuts->Fill(1); // Total tracks

  if (fabs(MChargedHadronRAA->trkCharge->at(j)) != 1) return false;
  hNTrkPassCuts->Fill(2); // Charge = 1

  if (MChargedHadronRAA->highPurity->at(j) == false) return false;
  hNTrkPassCuts->Fill(3); // High purity

  if (doEtaCut && fabs(MChargedHadronRAA->trkEta->at(j)) > 1) return false;
  hNTrkPassCuts->Fill(4); // Eta < 1, changed from 2.4

  if (MChargedHadronRAA->trkPt->at(j) < 0.1) return false; // vipuls
  double RelativeUncertainty = MChargedHadronRAA->trkPtError->at(j) / MChargedHadronRAA->trkPt->at(j);
  if (MChargedHadronRAA->trkPt->at(j) > 10 && RelativeUncertainty > 0.1) return false;
  hNTrkPassCuts->Fill(5); // Relative uncertainty < 10%

  if (fabs(MChargedHadronRAA->trkDxyAssociatedVtx->at(j)) / MChargedHadronRAA->trkDxyErrAssociatedVtx->at(j) > 3) return false;
  hNTrkPassCuts->Fill(6); // Dxy < 3 sigma

  if(fabs(MChargedHadronRAA->trkDzAssociatedVtx->at(j)) / MChargedHadronRAA->trkDzErrAssociatedVtx->at(j) > 3) return false;
  hNTrkPassCuts->Fill(7); // Dz < 3 sigma

  if (MChargedHadronRAA->trkPt->at(j) > 500) return false;
  hNTrkPassCuts->Fill(8); // pT < 500 GeV/c

  return true;
}

class DataAnalyzer {
public:
  TFile *inf, *outf;
  TH1D *hTrkPt, *hTrkEta, *hZDCPlus, *hZDCMinus, 
        *hZDCPlus_noEvtSel, *hZDCMinus_noEvtSel;
  TH2D *hTrkPtEta;
  TH1D *hNEvtPassCuts, *hNTrkPassCuts;
  TH1D *hVZ, *hVZ_pf;
  ChargedHadronRAATreeMessenger *MChargedHadronRAA;
  string title;

  DataAnalyzer(const char *filename, const char *outFilename, const char *mytitle = "")
      : inf(new TFile(filename)), 
        MChargedHadronRAA(new ChargedHadronRAATreeMessenger(*inf, string("Tree"), false, false, 2)),
        title(mytitle), outf(new TFile(outFilename, "recreate")) {
    outf->cd();
  }

  ~DataAnalyzer() {
    deleteHistograms();
    inf->Close();
    outf->Close();
    delete MChargedHadronRAA;
  }

  void analyze(Parameters &par) {
    outf->cd();

    hTrkPt = new TH1D(Form("hTrkPt%s", title.c_str()), "", nPtBins_log, pTBins_log);
    hTrkPt->Sumw2();
    hTrkEta = new TH1D(Form("hTrkEta%s", title.c_str()), "", 50, -3.0, 3.0);
    hTrkEta->Sumw2();
    hTrkPtEta = new TH2D(Form("hTrkPtEta%s", title.c_str()), "", nPtBins, pTBins_fine, 50, -4.0, 4.0);
    hTrkPtEta->Sumw2();
    hZDCPlus = new TH1D(Form("hZDCPlus%s", title.c_str()), "ZDC Plus Energy", 50, 0.0, 10000);
    hZDCPlus->Sumw2();
    hZDCPlus_noEvtSel = new TH1D(Form("hZDCPlus_noEvtSel%s", title.c_str()), "ZDC Plus Energy (no event selection)", 50, 0.0, 10000);
    hZDCPlus_noEvtSel->Sumw2();
    hZDCMinus = new TH1D(Form("hZDCMinus%s", title.c_str()), "ZDC Minus Energy", 50, 0.0, 10000);
    hZDCMinus->Sumw2();
    hZDCMinus_noEvtSel = new TH1D(Form("hZDCMinus_noEvtSel%s", title.c_str()), "ZDC Minus Energy (no event selection)", 50, 0.0, 10000);
    hZDCMinus_noEvtSel->Sumw2();

    hVZ = new TH1D(Form("hVZ%s", title.c_str()), "Vertex Z position", 100, -30.0, 30.0);
    hVZ_pf = new TH1D(Form("hVZ_pf%s", title.c_str()), "Vertex Z position (PF)", 100, -30.0, 30.0);

    hNEvtPassCuts = new TH1D("hNEvtPassCuts", "Number of events passing cuts", 8, 0.5, 8.5);
    hNEvtPassCuts->GetXaxis()->SetBinLabel(1, "Total Events");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(2, "+ HLT_OxyZeroBias_v1");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(3, "+ CC");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(4, "+ PV");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(5, "+ !isFakeVtx");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(6, "+ abs(VZ)<15");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(7, "+ nTrk>=0");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(8, "+ HF 10 OR Offline");
    hNEvtPassCuts->Sumw2();

    hNTrkPassCuts = new TH1D("hNTrkPassCuts", "Number of tracks passing cuts", 8, 0.5, 8.5);
    hNTrkPassCuts->GetXaxis()->SetBinLabel(1, "Total Tracks");
    hNTrkPassCuts->GetXaxis()->SetBinLabel(2, "+ abs(charge)=1");
    hNTrkPassCuts->GetXaxis()->SetBinLabel(3, "+ High Purity");
    hNTrkPassCuts->GetXaxis()->SetBinLabel(4, "+ Eta < 1");
    hNTrkPassCuts->GetXaxis()->SetBinLabel(5, "+ pT > 10 && Rel pT Error < 10%");
    hNTrkPassCuts->GetXaxis()->SetBinLabel(6, "+ Dxy < 3 sigma");
    hNTrkPassCuts->GetXaxis()->SetBinLabel(7, "+ Dz < 3 sigma");
    hNTrkPassCuts->GetXaxis()->SetBinLabel(8, "+ pT < 500 GeV/c");
    hNTrkPassCuts->Sumw2();

    par.printParameters();
    unsigned long nEntry = MChargedHadronRAA->GetEntries() * par.scaleFactor;
    ProgressBar Bar(cout, nEntry);
    Bar.SetStyle(1);

    int eventsRejected = 0;
    for (unsigned long i = 0; i < nEntry; i++) {
      MChargedHadronRAA->GetEntry(i);
      if (i % 1000 == 0) {
        Bar.Update(i);
        Bar.Print();
      }

      // fill hists with no event selection
      hZDCPlus_noEvtSel->Fill(MChargedHadronRAA->ZDCsumPlus);
      hZDCMinus_noEvtSel->Fill(MChargedHadronRAA->ZDCsumMinus);

      // event-level
      // event selection criteria
      if (!eventSelection(MChargedHadronRAA, hNEvtPassCuts)) {
        eventsRejected++;
        continue;
      }

      // event-level histograms
      hZDCPlus->Fill(MChargedHadronRAA->ZDCsumPlus);
      hZDCMinus->Fill(MChargedHadronRAA->ZDCsumMinus);

      // fill vertex histograms
      if (fabs(MChargedHadronRAA->VZ) < 15) hVZ->Fill(MChargedHadronRAA->VZ);
      if (fabs(MChargedHadronRAA->VZ_pf) < 15) hVZ_pf->Fill(MChargedHadronRAA->VZ_pf);

      // track-level
      for (unsigned long j = 0; j < MChargedHadronRAA->trkPt->size(); j++) {

        // apply track efficiency correction
        double trkWeight = 1.0;
        if (par.UseTrackWeight) {
          trkWeight *= MChargedHadronRAA->trackWeight->at(j);
        }

        // track selection w/o eta cut
        if (!trackSelection(MChargedHadronRAA, j, hNTrkPassCuts, false)) continue;

        // eta hist
        hTrkEta->Fill(MChargedHadronRAA->trkEta->at(j), trkWeight);

        // apply eta cut (last track selection)
        if (fabs(MChargedHadronRAA->trkEta->at(j)) > 1.0) continue;

        // fill histograms
        hTrkPt->Fill(MChargedHadronRAA->trkPt->at(j), trkWeight);
        hTrkPtEta->Fill(MChargedHadronRAA->trkPt->at(j), MChargedHadronRAA->trkEta->at(j), trkWeight);
        
      } // end of track loop
    } // end of event loop

    cout << "Total events: " << nEntry << endl;
    cout << "Events rejected: " << eventsRejected << endl;
    cout << "Events passing cuts: " << nEntry - eventsRejected << endl;

  } // end of analyze

  void writeHistograms(TFile *outf) {
    outf->cd();
    smartWrite(hTrkPt);
    smartWrite(hTrkPtEta);
    smartWrite(hTrkEta);
    smartWrite(hZDCPlus);
    smartWrite(hZDCMinus);
    smartWrite(hZDCPlus_noEvtSel);
    smartWrite(hZDCMinus_noEvtSel);
    smartWrite(hNEvtPassCuts);
    smartWrite(hNTrkPassCuts);
    smartWrite(hVZ);
    smartWrite(hVZ_pf);
  }

private:
  void deleteHistograms() {
    delete hTrkPt;
    delete hTrkPtEta;
    delete hTrkEta;
    delete hZDCPlus;
    delete hZDCMinus;
    delete hZDCPlus_noEvtSel;
    delete hZDCMinus_noEvtSel;
    delete hNEvtPassCuts;
    delete hNTrkPassCuts;
    delete hVZ;
    delete hVZ_pf;
  }
};

//============================================================//
// Main analysis
//============================================================//
int main(int argc, char *argv[]) {
  // if (printHelpMessage(argc, argv))
  //   return 0;
  CommandLine CL(argc, argv);
  float MinTrackPt = CL.GetDouble("MinTrackPt", 1.0); // Minimum track pT selection
  bool IsData = CL.GetBool("IsData", 0);              // Data or MC
  float scaleFactor = CL.GetDouble("ScaleFactor", 1.0);
  int TriggerChoice = CL.GetInt("TriggerChoice", 0);

  Parameters par(MinTrackPt, TriggerChoice, IsData, scaleFactor);
  par.input = CL.Get("Input", "input.root");    // Input file
  par.output = CL.Get("Output", "output.root"); // Output file
  par.UseTrackWeight = CL.GetBool("UseTrackWeight", false); // Path to track efficiency corrections
  if (checkError(par))
    return -1;
  std::cout << "Parameters are set" << std::endl;

  // Analyze Data
  DataAnalyzer analyzer(par.input.c_str(), par.output.c_str(), "");
  analyzer.analyze(par);
  analyzer.writeHistograms(analyzer.outf);
  saveParametersToHistograms(par, analyzer.outf);
  cout << "done!" << analyzer.outf->GetName() << endl;

  return 0;
}
