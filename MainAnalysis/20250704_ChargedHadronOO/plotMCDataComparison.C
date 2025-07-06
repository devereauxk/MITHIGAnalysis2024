#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

const int ccolors[14] = {
    kBlue+2,     // deep blue
    kAzure+7,    // cyan
    kTeal+3,     // teal
    kGreen+2,    // green
    kSpring+7,   // turquoise-green
    kOrange+7,   // orange (avoiding yellow)
    kRed,        // red
    kPink+7,     // pink
    kMagenta+2,  // magenta
    kViolet+7,   // purple
    kGray+2,     // gray
    kBlack,      // black
    kOrange+2,   // extra orange
    kCyan+2      // extra cyan
};

const int scolors[4] = {
    kAzure+7,    // cyan
    kOrange+7,   // orange (avoiding yellow)
    kTeal+3,     // teal
    kPink+7,     // pink
};

void divideByWidth(TH1* input) {
    if (!input) {
        std::cerr << "Error: Null histogram pointer passed to divideByWidth function." << std::endl;
        return;
    }

    int nBins = input->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
        double binContent = input->GetBinContent(i);
        double binError = input->GetBinError(i);
        double binWidth = input->GetBinWidth(i);

        if (binWidth != 0) {
            input->SetBinContent(i, binContent / binWidth);
            input->SetBinError(i, binError / binWidth);
        } else {
            std::cerr << "Warning: Bin width is zero for bin " << i << ". Skipping division for this bin." << std::endl;
        }
    }
}

void setErrors(TH1* hRatio, TH1* num, TH1* denom){
    for (int bin = 1; bin <= hRatio->GetNbinsX(); bin++) {
        double content1 = num->GetBinContent(bin);
        double error1 = num->GetBinError(bin);
        double content2 = denom->GetBinContent(bin);
        double error2 = denom->GetBinError(bin);

        double ratioError = 0;
        if (content2 > 0) {
            ratioError = sqrt((error1 * error1) / (content2 * content2) +
                              (content1 * content1 * error2 * error2) / (content2 * content2 * content2 * content2));
        }
        hRatio->SetBinError(bin, ratioError);
    }
}

void plotRatioLogy(vector<TH1*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    const char* rTitle, double rmin, double rmax,
    int baseline = 0, bool logy = true,
    bool errorBars = true) {

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);
    leg->SetBorderSize(0); // Remove legend box
    leg->SetTextSize(0.04); // Reduce font size

    TPad *pad1 = new TPad(title, title, 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    pad1->Draw();
    TPad *pad2 = new TPad(title, title, 0, 0, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();

    for (int i = 0; i < hists.size(); i++) {
        pad1->cd();

        TH1* hist = hists[i];

        hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetRangeUser(xmin, xmax);
        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetRangeUser(ymin, ymax);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->SetLineColor(ccolors[i]);

        if (i == baseline) {
            hist->Draw("E SAME");
        } else {
            hist->Draw("HIST SAME");
        }
        leg->AddEntry(hist, Form("%s", labels[i].c_str()), "l");

        pad2->cd();
        if (i != baseline) {
            TH1* hRatio = (TH1*)hist->Clone(Form("ratio_TrkPt_%d", i));
            hRatio->Divide(hists[baseline]);
            hRatio->SetTitle("");
            hRatio->SetStats(0);
            hRatio->GetXaxis()->SetTitle(xTitle);
            hRatio->GetXaxis()->SetTitleSize(0.1);
            hRatio->GetXaxis()->SetLabelSize(0.08);
            hRatio->GetXaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetTitle(rTitle);
            hRatio->GetYaxis()->SetTitleSize(0.1);
            hRatio->GetYaxis()->SetLabelSize(0.08);
            hRatio->GetYaxis()->SetTitleOffset(0.4);
            hRatio->GetYaxis()->SetRangeUser(rmin, rmax);
            hRatio->SetLineColor(ccolors[i]);

            // Calculate statistical errors for the ratio
            if (errorBars) {
                setErrors(hRatio, hist, hists.at(baseline));
                hRatio->Draw("E SAME");
            } else {
                hRatio->Draw("HIST SAME");
            }

            cout << title << " " << labels[i] << " " << hRatio->Integral() << endl;

            // Print (ratio y value, ratio y value * bin width) for each bin
            for (int bin = 1; bin <= hRatio->GetNbinsX(); ++bin) {
                double y = (1 - hRatio->GetBinContent(bin)) * hists[baseline]->GetBinContent(bin);
                double w = hRatio->GetBinWidth(bin);
                cout << y << " " << y * w << endl;
            }

            TLine *line = new TLine(xmin, 1, xmax, 1);
            line->SetLineColor(kGray+2);
            line->SetLineStyle(2);
            line->Draw("SAME");
        }
    }
    pad1->cd();
    leg->Draw("SAME");
}

void plot2D(TH2* hist, const char* title,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    pad1->SetLogx(0);
    pad1->SetLogy(0);
    pad1->SetLogz(1);
    pad1->Draw();
    pad1->cd();

    hist->SetStats(0);
    hist->SetTitle(title);
    hist->SetTitleSize(0.03, "XYZ"); // Decrease title size
    hist->GetXaxis()->SetTitle(xTitle);
    hist->GetXaxis()->SetTitleSize(0.05);
    hist->GetXaxis()->SetLabelSize(0.045);
    hist->GetXaxis()->SetRangeUser(xmin, xmax);
    hist->GetYaxis()->SetTitle(yTitle);
    hist->GetYaxis()->SetTitleSize(0.05);
    hist->GetYaxis()->SetLabelSize(0.045);
    hist->GetYaxis()->SetRangeUser(ymin, ymax);

    hist->Draw("COLZ");
}

void plotSimple(vector<TH1*> hists, const char* title, vector<string> labels,
    const char* xTitle, double xmin, double xmax,
    const char* yTitle, double ymin, double ymax,
    bool logx = false, bool logy = false, bool binnums = false) {

    TPad *pad1 = new TPad(title, title, 0, 0, 1, 1);
    pad1->SetBottomMargin(0.18); // Increase bottom margin for x-axis labels/ticks
    logx ? pad1->SetLogx() : pad1->SetLogx(0);
    logy ? pad1->SetLogy() : pad1->SetLogy(0);
    pad1->Draw();
    pad1->cd();

    TLegend *leg = new TLegend(0.68, 0.75, 0.58, 0.85); // smaller, upper right
    leg->SetBorderSize(0);
    leg->SetTextSize(0.035);

    double global_min = 1e30, global_max = -1e30;

    // Find global min/max for all hists in the visible x range
    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
        double x1 = (xmin < xmax) ? xmin : hist->GetXaxis()->GetXmin();
        double x2 = (xmin < xmax) ? xmax : hist->GetXaxis()->GetXmax();
        int binmin = hist->GetXaxis()->FindBin(x1);
        int binmax = hist->GetXaxis()->FindBin(x2);
        for (int i = binmin; i <= binmax; ++i) {
            double val = hist->GetBinContent(i);
            if (val < global_min) global_min = val;
            if (val > global_max) global_max = val;
        }
    }
    // Add some margin
    double margin = 0.2 * (global_max - global_min);

    for (size_t ih = 0; ih < hists.size(); ++ih) {
        TH1* hist = hists[ih];
        hist->SetStats(0);
        hist->SetTitle(title);
        hist->GetXaxis()->SetTitle(xTitle);
        hist->GetXaxis()->SetTitleSize(0.05);
        hist->GetXaxis()->SetLabelSize(0.045);

        // Auto-scale x axis if xmin >= xmax
        if (xmin < xmax) {
            hist->GetXaxis()->SetRangeUser(xmin, xmax);
        } else {
            hist->GetXaxis()->SetRangeUser(hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
        }

        hist->GetYaxis()->SetTitle(yTitle);
        hist->GetYaxis()->SetTitleSize(0.05);
        hist->GetYaxis()->SetLabelSize(0.03);
        hist->GetYaxis()->SetTitleOffset(1);

        // Auto-scale y axis if ymin < ymax, otherwise use global min/max
        if (ymin < ymax) {
            hist->GetYaxis()->SetRangeUser(ymin, ymax);
        } else {
            hist->GetYaxis()->SetRangeUser(global_min - margin, global_max + margin);
        }

        hist->SetLineColor(scolors[ih]);
        hist->SetLineWidth(2);

        // Set dashed line style for the second curve (ih == 1)
        if (ih == 1) {
            hist->SetLineStyle(2); // dashed
        } else {
            hist->SetLineStyle(1); // solid
        }

        // Draw histogram as usual
        if (ih == 0) {
            hist->Draw("HIST");
        } else {
            hist->Draw("HIST SAME");
        }

        // Old way: draw bin content above each bin using "TEXT0" option
        if (binnums) {
            hist->Draw("SAME TEXT0");
        }

        /*
        if (binnums) {
            for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
            double x = hist->GetBinCenter(bin);
            double y = hist->GetBinContent(bin);
            // Place 'a' slightly above the bin content
            double y_offset = (y > 0) ? y + 0.03 * (hist->GetMaximum() - hist->GetMinimum()) : 0.03 * (hist->GetMaximum() - hist->GetMinimum());
            TLatex latex;
            latex.SetTextAlign(22);
            latex.SetTextSize(0.027);
            latex.DrawLatex(x, y_offset, Form("%.0f", y));
            }
        }
        */

        if (labels.size() > ih)
            leg->AddEntry(hist, labels[ih].c_str(), "l");
    }

    leg->Draw("SAME");
}

void plotMCDataComparison(const char* output = "plots/394153_min04_HFAND14_HFAND12") {

    vector<string> labels = {"OO Run 394153", "OO HIJING"};
    vector<string> inputFiles = {
        "output/20250705_OO_394153_FIRSTLOOK_only25_eventSel_min04_HFAND14_HFAND12.root",
        "output/skim_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518_eventSel_min04_HFAND14_HFAND12.root"
    };

    float MC_scale = 30887./892630;

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
        if (i == 1) {
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
        "Counts", 1, 1e3,
        false, true
    );

    c2->cd(6);
    plotSimple(
        hHFEMaxPlus, "hHFEMaxPlus", labels,
        "HF EMax Plus [GeV]", 0, 400,
        "Counts", 1, 1e3,
        false, true
    );

    c2->cd(7);
    plotSimple(
        hHFEMaxMinus_clone, "hHFEMaxMinus", labels,
        "HF EMax Minus [GeV]", 0, 40,
        "Counts", 1, 1e6,
        false, true
    );

    c2->cd(8);
    plotSimple(
        hHFEMaxPlus_clone, "hHFEMaxPlus", labels,
        "HF EMax Plus [GeV]", 0, 40,
        "Counts", 1, 1e6,
        false, true
    );

    c2->cd(9);
    plotSimple(
        hhiHFMinus_pf, "hhiHFMMinus_pf", labels,
        "HF ET Sum Minus [GeV]", -1, -1,
        "Counts", 1, 2e4,
        false, true
    );

    c2->cd(10);
    plotSimple(
        hhiHFPlus_pf, "hhiHFPlus_pf", labels,
        "HF ET Sum Plus [GeV]", -1, -1,
        "Counts", 1, 2e4,
        false, true
    );

    c2->cd(11);
    plotSimple(
        hhiHF_pf, "hhiHF_pf", labels,
        "HF ET Sum [GeV]", 0, 800,
        "Counts", 1, 2e4,
        false, true
    );

    // Save as png
    c2->SaveAs(Form("%s-ZDCHF.png", output));

}
