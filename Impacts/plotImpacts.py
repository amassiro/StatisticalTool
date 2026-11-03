#!/usr/bin/env python
#
# Impact plot with observed and expected results overlaid
#   - left panel:  pulls (theta_hat - theta_0)/Delta_theta, obs = black points, exp = grey band
#   - right panel: impacts on the POI, exp = filled boxes, obs = lines
#
# Input: two json files from "combineTool.py -M Impacts ... -o impacts.json"
#        (one on data, one on Asimov with -t -1)
#
# NB: written to be python2/python3 compatible (pyROOT)
#

from __future__ import print_function

import json
import re
import math
import argparse
import ROOT

# PDG rounding from combine, as in the original plotImpacts.py
# (old CMSSW: from CombineHarvester), simple fallback if combine is not available
try:
  import HiggsAnalysis.CombinedLimit.tool_base.rounding as rounding
except ImportError:
  try:
    import CombineHarvester.CombineTools.combine.rounding as rounding
  except ImportError:
    rounding = None
    print ("combine rounding not found, using a simple rounding")

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)


parser = argparse.ArgumentParser()
parser.add_argument("-i", "--obs",       default=None, help="json with observed impacts")
parser.add_argument("-e", "--exp",       default=None, help="json with expected impacts (Asimov)")
parser.add_argument("-o", "--output",    default="impacts", help="output name (without extension)")
parser.add_argument("-t", "--translate", default=None, help="json file to rename parameters, {name: label}")
parser.add_argument("-p", "--POI",       default=None, help="POI to draw (default: first one)")
parser.add_argument("-n", "--per-page",  type=int, default=20, help="number of parameters to show per page")
parser.add_argument("-m", "--max-pages", type=int, default=None, help="maximum number of pages to write")
parser.add_argument("-s", "--sortBy",    default="obs", choices=["obs", "exp"], help="sort parameters by obs or exp impact")
parser.add_argument("--showOnly",        default=None, choices=["constrained", "unconstrained", "mcstat"],
                    help="show only: constrained nuisances (MC stat excluded), unconstrained parameters (e.g. rateParam), MC stat nuisances (autoMCStats); default all")
parser.add_argument("--mcStatPattern",   default="^prop_bin.*_bin[0-9]+", help="regular expression for the MC stat nuisances (autoMCStats)")
parser.add_argument("-x", "--pullRange", type=float, default=2.7, help="x range of the pull panel")
parser.add_argument("-r", "--impactRange", type=float, default=None, help="x range of the impact panel (default: automatic)")
parser.add_argument("-l", "--poiLabel",  default="#hat{r}", help="label of the POI")
parser.add_argument("-c", "--cmsLabel",  default="", help="label next to CMS, e.g. Preliminary")
parser.add_argument("--obsLabel",        default="obs.", help="label for the observed json in the legend")
parser.add_argument("--expLabel",        default="exp.", help="label for the expected json in the legend, e.g. 'MC Asimov' or 'data Asimov'")
parser.add_argument("-b", "--blind",     action="store_true", help="do not print the observed best fit value")
parser.add_argument("--labelSize",       type=int, default=16, help="text size of the parameter names, in pixels")
parser.add_argument("--width",           type=int, default=650, help="canvas width")
parser.add_argument("--rowHeight",       type=int, default=25, help="height of one row in pixels")
parser.add_argument("--minHeight",       type=int, default=450, help="minimum canvas height in pixels (few parameters)")
args = parser.parse_args()

if args.obs is None and args.exp is None:
  print ("Give at least one of --obs or --exp")
  exit(1)


#
# rounding of the fitted value of unconstrained parameters
# copied from the original plotImpacts.py
#

def GetRounded(nom, e_hi, e_lo):
  if e_hi < 0.0:
    e_hi = 0.0
  if e_lo < 0.0:
    e_lo = 0.0
  if rounding is None:
    # fallback: 2 significant digits on the smallest uncertainty, same decimals for the value
    errs = [e for e in [e_hi, e_lo] if e > 0]
    nd = max(0, 1 - int(math.floor(math.log10(min(errs))))) if len(errs) > 0 else 2
    fmt = "{:." + str(nd) + "f}"
    return (fmt.format(nom), fmt.format(e_hi), fmt.format(e_lo))
  rounded = rounding.PDGRoundAsym(nom, e_hi if e_hi != 0.0 else 1.0, e_lo if e_lo != 0.0 else 1.0)
  s_nom = rounding.downgradePrec(rounded[0], rounded[2])
  s_hi = rounding.downgradePrec(rounded[1][0][0], rounded[2]) if e_hi != 0.0 else "0"
  s_lo = rounding.downgradePrec(rounded[1][0][1], rounded[2]) if e_lo != 0.0 else "0"
  return (s_nom, s_hi, s_lo)


#
# read json and compute pulls and impacts
#

def readImpacts(fileName, poiName):
  with open(fileName) as jsonfile:
    data = json.load(jsonfile)
  pois = [ele["name"] for ele in data["POIs"]]
  if poiName is None:
    poiName = pois[0]
  poi_fit = None
  for ele in data["POIs"]:
    if ele["name"] == poiName:
      poi_fit = ele["fit"]
  results = {}
  for ele in data["params"]:
    res = {}
    # signed impacts: POI value when the nuisance is moved to +1/-1 sigma (post-fit)
    res["impact_hi"] = ele[poiName][2] - ele[poiName][1]
    res["impact_lo"] = ele[poiName][0] - ele[poiName][1]
    res["impact"] = max(abs(res["impact_hi"]), abs(res["impact_lo"]))
    res["constrained"] = (ele["type"] != "Unconstrained")
    res["fit"] = ele["fit"]
    if res["constrained"]:
      # same definition as the default one in combine plotImpacts.py
      pre = ele["prefit"]
      fit = ele["fit"]
      pre_err_hi = pre[2] - pre[1]
      pre_err_lo = pre[1] - pre[0]
      def scale(x):
        return (x / pre_err_hi) if x >= 0 else (x / pre_err_lo)
      sc_fit    = scale(fit[1] - pre[1])
      sc_fit_hi = scale(fit[2] - pre[1]) - sc_fit
      sc_fit_lo = sc_fit - scale(fit[0] - pre[1])
      res["pull"]    = sc_fit
      res["pull_hi"] = sc_fit_hi
      res["pull_lo"] = sc_fit_lo
    results[ele["name"]] = res
  return poiName, poi_fit, results


poi = args.POI
obs = {}
exp = {}
poi_fit_obs = None
poi_fit_exp = None
if args.obs is not None:
  poi, poi_fit_obs, obs = readImpacts(args.obs, poi)
if args.exp is not None:
  poi, poi_fit_exp, exp = readImpacts(args.exp, poi)

translate = {}
if args.translate is not None:
  with open(args.translate) as jsonfile:
    translate = json.load(jsonfile)

#
# sorting: by obs (or exp) impact, the other one is used if a parameter is missing
#
main  = obs if (args.sortBy == "obs" and len(obs) > 0) or len(exp) == 0 else exp
other = exp if main is obs else obs
names = list(set(list(main.keys()) + list(other.keys())))

def sortKey(name):
  if name in main:
    return main[name]["impact"]
  return other[name]["impact"]

#
# selection of the parameters to show (--showOnly), three disjoint categories:
#   mcstat        = names matching --mcStatPattern (autoMCStats: prop_bin<channel>_bin<N>[_<process>])
#   unconstrained = type "Unconstrained" in the json (e.g. rateParam)
#   constrained   = all the others
#
def category(name):
  if re.search(args.mcStatPattern, name):
    return "mcstat"
  dd = main if name in main else other
  return "constrained" if dd[name]["constrained"] else "unconstrained"

if args.showOnly is not None:
  names = [name for name in names if category(name) == args.showOnly]
  print ("show only", args.showOnly, ":", len(names), "parameters")
  if len(names) == 0:
    print ("Nothing to plot")
    exit(0)

names.sort(key=sortKey, reverse=True)

print ("POI = ", poi, " sorted by", "obs" if main is obs else "exp", "impact")
for i, name in enumerate(names):
  print ("  {:3d} {:40s} obs = {:10s} exp = {:10s}".format(
    i + 1, name,
    "{:.4f}".format(obs[name]["impact"]) if name in obs else "-",
    "{:.4f}".format(exp[name]["impact"]) if name in exp else "-"))


#
# pages, as in the original plotImpacts.py: per_page parameters per page,
# all the pages in a single pdf
#
pages = [names[i:i + args.per_page] for i in range(0, len(names), args.per_page)]
if args.max_pages is not None and args.max_pages > 0:
  pages = pages[:args.max_pages]

def isUnconstrained(dd, name):
  return name in dd and not dd[name]["constrained"]

# unconstrained parameters with both obs and exp values written need two text lines,
# they get a double height row
def rowHeights(page_names):
  row_h = []
  for name in page_names:
    nlines = int(isUnconstrained(obs, name)) + int(isUnconstrained(exp, name))
    row_h.append(2 if nlines == 2 else 1)
  return row_h

#
# canvas layout (NDC on the canvas)
#
top_margin    = 0.17   # room for legend + header
bottom_margin = 0.10
x_label_end   = 0.355  # parameter names are right aligned here
x_pull_end    = 0.705  # pull panel goes from x_label_end to x_pull_end
x_right_end   = 0.97   # impact panel goes from x_pull_end to x_right_end

# same canvas size for all the pages, rows on the last page are taller (as in the original)
max_rows = max([sum(rowHeights(page_names)) for page_names in pages])
height = max(args.minHeight, int(max_rows * args.rowHeight / (1. - top_margin - bottom_margin)))

# text sizes are relative to the smallest pad dimension: convert pull pad sizes to canvas ones
size_pull_to_canvas = min(x_pull_end * args.width, height) / float(min(args.width, height))

shade_color = ROOT.TColor.GetColor("#ebebeb")
def isShaded(i):
  return i % 2 == 0

color_up   = ROOT.TColor.GetColor("#d62728")
color_down = ROOT.TColor.GetColor("#1f5fbf")


#
# draw one page
#
def drawPage(names, page, npages, first_rank):
  nparams = len(names)
  row_h = rowHeights(names)
  total_h = sum(row_h)
  # first parameter (largest impact) on top
  row_low = [total_h - sum(row_h[:i + 1]) for i in range(nparams)]
  row_center = [row_low[i] + 0.5 * row_h[i] for i in range(nparams)]


  cc = ROOT.TCanvas("cc_" + str(page), "", args.width, height)
  cc.SetFillColor(0)

  def rowToNDC(y):
    return bottom_margin + y / float(total_h) * (1. - top_margin - bottom_margin)

  # ---- alternating row shading, over labels and both panels
  shading = []
  for i in range(nparams):
    if not isShaded(i):
      continue
    box = ROOT.TBox(0.0, rowToNDC(row_low[i]), x_right_end, rowToNDC(row_low[i] + row_h[i]))
    box.SetFillColor(shade_color)
    box.Draw()
    shading.append(box)

  # ---- parameter names
  latex = ROOT.TLatex()
  latex.SetNDC()
  latex.SetTextFont(43)  # size in pixels, independent of the canvas height
  latex.SetTextSize(args.labelSize)
  latex.SetTextAlign(32)
  for i, name in enumerate(names):
    latex.DrawLatex(x_label_end, rowToNDC(row_center[i]), translate.get(name, name))

  # ---- rank of the parameter (as in the original)
  rank = ROOT.TLatex()
  rank.SetNDC()
  rank.SetTextFont(43)
  rank.SetTextSize(int(0.75 * args.labelSize))
  rank.SetTextAlign(12)
  rank.SetTextColor(ROOT.kGray + 2)
  for i, name in enumerate(names):
    rank.DrawLatex(0.008, rowToNDC(row_center[i]), str(first_rank + i))


  #
  # left panel: pulls
  #
  pad_pull = ROOT.TPad("pad_pull", "", 0., 0., x_pull_end, 1.)
  pad_pull.SetFillStyle(4000)
  pad_pull.SetFrameFillStyle(4000)
  pad_pull.SetLeftMargin(x_label_end / x_pull_end)
  pad_pull.SetRightMargin(0.)
  pad_pull.SetTopMargin(top_margin)
  pad_pull.SetBottomMargin(bottom_margin)
  pad_pull.SetTicks(1, 0)
  pad_pull.Draw()
  pad_pull.cd()

  frame_pull = ROOT.TH2F("frame_pull_" + str(page), "", 1, -args.pullRange, args.pullRange, 1, 0, total_h)
  frame_pull.GetYaxis().SetLabelSize(0)
  frame_pull.GetYaxis().SetTickLength(0)
  frame_pull.GetXaxis().SetTitle("(#hat{#theta}-#theta_{0})/#Delta#theta")
  frame_pull.GetXaxis().SetTitleSize(0.045)
  frame_pull.GetXaxis().SetTitleOffset(0.95)
  frame_pull.GetXaxis().CenterTitle()
  frame_pull.GetXaxis().SetLabelSize(0.040)
  frame_pull.GetXaxis().SetNdivisions(505)
  frame_pull.Draw("AXIS")

  objects = []
  for xline in [-2, -1, 1, 2]:
    if abs(xline) >= args.pullRange:
      continue
    line = ROOT.TLine(xline, 0, xline, total_h)
    line.SetLineStyle(3)
    line.SetLineColor(ROOT.kBlack)
    line.Draw()
    objects.append(line)

  gr_pull_obs = ROOT.TGraphAsymmErrors()
  for i, name in enumerate(names):
    y = row_center[i]
    if name in exp and exp[name]["constrained"]:
      ee = exp[name]
      box = ROOT.TBox(max(ee["pull"] - ee["pull_lo"], -args.pullRange), y - 0.4, min(ee["pull"] + ee["pull_hi"], args.pullRange), y + 0.4)
      box.SetFillColor(ROOT.TColor.GetColor("#b0b0b0"))
      box.Draw()
      objects.append(box)
    if name in obs and obs[name]["constrained"]:
      oo = obs[name]
      ipoint = gr_pull_obs.GetN()
      gr_pull_obs.SetPoint(ipoint, oo["pull"], y)
      gr_pull_obs.SetPointError(ipoint, oo["pull_lo"], oo["pull_hi"], 0, 0)

  #
  # unconstrained parameters (e.g. rateParam): no pull, write the fitted value as in
  # the original plotImpacts.py, observed on the upper line and expected on the lower one.
  # Values taken from the "fit" field of each json
  #
  unc_text = ROOT.TLatex()
  unc_text.SetTextFont(43)
  unc_text.SetTextSize(args.labelSize - 2)
  unc_text.SetTextAlign(22)
  for i, name in enumerate(names):
    y = row_center[i]
    lines = []
    for dd, label, color in [(obs, args.obsLabel, ROOT.kBlack), (exp, args.expLabel, ROOT.kGray + 2)]:
      if name in dd and not dd[name]["constrained"]:
        fit = dd[name]["fit"]
        s_nom, s_hi, s_lo = GetRounded(fit[1], fit[2] - fit[1], fit[1] - fit[0])
        lines.append(("{}^{{#plus{}}}_{{#minus{}}} ({})".format(s_nom, s_hi, s_lo, label), color))
    if len(lines) == 0:
      continue
    # hide the dotted lines behind the text, keeping the row shading
    box = ROOT.TBox(-args.pullRange, row_low[i] + 0.02, args.pullRange, row_low[i] + row_h[i] - 0.02)
    box.SetFillColor(shade_color if isShaded(i) else ROOT.kWhite)
    box.Draw()
    objects.append(box)
    dy = [0.] if len(lines) == 1 else [+0.5, -0.5]
    for (text, color), ddy in zip(lines, dy):
      unc_text.SetTextColor(color)
      unc_text.DrawLatex(0., y + ddy, text)

  gr_pull_obs.SetMarkerStyle(20)
  gr_pull_obs.SetMarkerSize(0.8)
  gr_pull_obs.SetLineWidth(2)
  ROOT.gStyle.SetEndErrorSize(4)
  if gr_pull_obs.GetN() > 0:
    gr_pull_obs.Draw("P same")

  frame_pull.Draw("AXIS same")

  # ---- CMS label, inside the pull panel
  cms = ROOT.TLatex()
  cms.SetNDC()
  cms.SetTextFont(61)
  cms.SetTextSize(0.050)
  cms.SetTextAlign(13)
  x_cms = pad_pull.GetLeftMargin() + 0.03
  y_cms = 1. - top_margin + 0.045
  cms.DrawLatex(x_cms, y_cms, "CMS")
  if args.cmsLabel != "":
    cms.SetTextFont(52)
    cms.SetTextSize(0.038)
    cms.DrawLatex(x_cms + 0.085, y_cms - 0.008, args.cmsLabel)

  cc.cd()


  #
  # right panel: impacts
  #
  max_impact = 0.
  for name in names:
    for dd in [obs, exp]:
      if name in dd:
        max_impact = max(max_impact, abs(dd[name]["impact_hi"]), abs(dd[name]["impact_lo"]))
  impact_range = args.impactRange if args.impactRange is not None else 1.15 * max_impact
  # the panel is narrow: labels only at 0 and +/- step, with step = largest 1,2,5 x 10^n
  # below 0.75 x range, so that no label is at the edges of the panel
  base = 10 ** math.floor(math.log10(0.75 * impact_range))
  step = base
  for mult in [2, 5]:
    if mult * base <= 0.75 * impact_range:
      step = mult * base

  pad_impact = ROOT.TPad("pad_impact", "", x_pull_end, 0., 1., 1.)
  pad_impact.SetFillStyle(4000)
  pad_impact.SetFrameFillStyle(4000)
  pad_impact.SetLeftMargin(0.)
  pad_impact.SetRightMargin((1. - x_right_end) / (1. - x_pull_end))
  pad_impact.SetTopMargin(top_margin)
  pad_impact.SetBottomMargin(bottom_margin)
  pad_impact.SetTicks(1, 0)
  pad_impact.Draw()
  pad_impact.cd()

  frame_impact = ROOT.TH2F("frame_impact_" + str(page), "", 1, -impact_range, impact_range, 1, 0, total_h)
  frame_impact.GetYaxis().SetLabelSize(0)
  frame_impact.GetYaxis().SetTickLength(0)
  # axis title drawn on the canvas later (the pad is too narrow)
  frame_impact.GetXaxis().SetLabelSize(0)
  frame_impact.GetXaxis().SetTickLength(0)
  frame_impact.Draw("AXIS")

  # dotted vertical lines at the major ticks, solid one at 0
  xline = step
  while xline < impact_range:
    for xx in [-xline, xline]:
      line = ROOT.TLine(xx, 0, xx, total_h)
      line.SetLineStyle(3)
      line.SetLineColor(ROOT.kBlack)
      line.Draw()
      objects.append(line)
    xline += step

  line0 = ROOT.TLine(0, 0, 0, total_h)
  line0.SetLineColor(ROOT.kBlack)
  line0.SetLineWidth(2)

  for i, name in enumerate(names):
    y = row_center[i]
    # expected: filled boxes, +1 sigma (red) and -1 sigma (blue)
    if name in exp:
      ee = exp[name]
      for value, color in [(ee["impact_hi"], color_up), (ee["impact_lo"], color_down)]:
        box = ROOT.TBox(0, y - 0.4, value, y + 0.4)
        box.SetFillColorAlpha(color, 0.45)
        box.SetLineWidth(0)
        box.Draw()
        objects.append(box)
    # observed: lines with end tick, red slightly above, blue slightly below
    if name in obs:
      oo = obs[name]
      for value, color, dy in [(oo["impact_hi"], color_up, +0.08), (oo["impact_lo"], color_down, -0.08)]:
        line = ROOT.TLine(0, y + dy, value, y + dy)
        line.SetLineColor(color)
        line.SetLineWidth(2)
        line.Draw()
        tick = ROOT.TLine(value, y + dy - 0.2, value, y + dy + 0.2)
        tick.SetLineColor(color)
        tick.SetLineWidth(2)
        tick.Draw()
        objects.append(line)
        objects.append(tick)

  line0.Draw()
  frame_impact.Draw("AXIS same")

  # ticks at multiples of step, labels only at 0 and +/- step
  tick_length = 0.015 * total_h / (1. - top_margin - bottom_margin)
  xline = 0.
  while xline < impact_range:
    for xx in set([-xline, xline]):
      for y1, y2 in [(0, tick_length), (total_h - tick_length, total_h)]:
        tick = ROOT.TLine(xx, y1, xx, y2)
        tick.Draw()
        objects.append(tick)
    xline += step

  cc.cd()


  #
  # legend and POI value
  #
  leg_size = 0.030
  dummy_obs_pull = ROOT.TGraphAsymmErrors(1)
  dummy_obs_pull.SetMarkerStyle(20)
  dummy_obs_pull.SetMarkerSize(0.8)
  dummy_obs_pull.SetLineWidth(2)
  dummy_exp_pull = ROOT.TH1F("dummy_exp_pull_" + str(page), "", 1, 0, 1)
  dummy_exp_pull.SetFillColor(ROOT.TColor.GetColor("#b0b0b0"))
  dummy_exp_pull.SetLineWidth(0)

  dummy_obs_up = ROOT.TH1F("dummy_obs_up_" + str(page), "", 1, 0, 1)
  dummy_obs_up.SetLineColor(color_up)
  dummy_obs_up.SetLineWidth(2)
  dummy_obs_down = ROOT.TH1F("dummy_obs_down_" + str(page), "", 1, 0, 1)
  dummy_obs_down.SetLineColor(color_down)
  dummy_obs_down.SetLineWidth(2)
  dummy_exp_up = ROOT.TH1F("dummy_exp_up_" + str(page), "", 1, 0, 1)
  dummy_exp_up.SetFillColorAlpha(color_up, 0.45)
  dummy_exp_up.SetLineWidth(0)
  dummy_exp_down = ROOT.TH1F("dummy_exp_down_" + str(page), "", 1, 0, 1)
  dummy_exp_down.SetFillColorAlpha(color_down, 0.45)
  dummy_exp_down.SetLineWidth(0)

  legend = ROOT.TLegend(0.03, 0.905, 0.97, 0.995)
  legend.SetNColumns(3)
  legend.SetBorderSize(0)
  legend.SetFillStyle(0)
  legend.SetTextFont(42)
  legend.SetTextSize(leg_size)
  if len(obs) > 0:
    legend.AddEntry(dummy_obs_pull, "Fit constraint (" + args.obsLabel + ")", "lp")
    legend.AddEntry(dummy_obs_up,   "+1#sigma impact (" + args.obsLabel + ")", "l")
    legend.AddEntry(dummy_obs_down, "-1#sigma impact (" + args.obsLabel + ")", "l")
  if len(exp) > 0:
    legend.AddEntry(dummy_exp_pull, "Fit constraint (" + args.expLabel + ")", "f")
    legend.AddEntry(dummy_exp_up,   "+1#sigma impact (" + args.expLabel + ")", "f")
    legend.AddEntry(dummy_exp_down, "-1#sigma impact (" + args.expLabel + ")", "f")
  legend.Draw()

  poi_fit = poi_fit_obs if (poi_fit_obs is not None and not args.blind) else poi_fit_exp
  if poi_fit is not None:
    poi_text = ROOT.TLatex()
    poi_text.SetNDC()
    poi_text.SetTextFont(42)
    poi_text.SetTextSize(0.042)
    poi_text.SetTextAlign(31)
    text = "{} = {:.2f}^{{+{:.2f}}}_{{-{:.2f}}}".format(args.poiLabel, poi_fit[1], poi_fit[2] - poi_fit[1], poi_fit[1] - poi_fit[0])
    if poi_fit is poi_fit_exp and len(obs) > 0:
      text += " (" + args.expLabel + ")"
    poi_text.DrawLatex(x_right_end, 1. - top_margin + 0.02, text)

  impact_labels = ROOT.TLatex()
  impact_labels.SetNDC()
  impact_labels.SetTextFont(42)
  impact_labels.SetTextSize(0.040 * size_pull_to_canvas)
  impact_labels.SetTextAlign(23)
  for xx in [-step, 0., step]:
    x_ndc = x_pull_end + (xx + impact_range) / (2. * impact_range) * (x_right_end - x_pull_end)
    label = "0" if xx == 0 else ("#minus" if xx < 0 else "") + "{:g}".format(abs(xx))
    impact_labels.DrawLatex(x_ndc, bottom_margin - 0.012, label)

  impact_title = ROOT.TLatex()
  impact_title.SetNDC()
  impact_title.SetTextFont(42)
  impact_title.SetTextSize(0.045 * size_pull_to_canvas)
  impact_title.SetTextAlign(33)
  impact_title.DrawLatex(x_right_end + 0.02, bottom_margin - 0.045, "#Delta" + args.poiLabel)

  cc.Update()

  # multi-page pdf as in the original, one png per page
  extra = ""
  if page == 0:
    extra = "("
  if page == npages - 1:
    extra = ")"
  if npages == 1:
    extra = ""
  cc.Print(args.output + ".pdf" + extra, "pdf")
  suffix = "" if npages == 1 else "_page" + str(page + 1)
  cc.SaveAs(args.output + suffix + ".png")
  cc.SaveAs(args.output + suffix + ".root")


npages = len(pages)
first_rank = 1
for page, page_names in enumerate(pages):
  drawPage(page_names, page, npages, first_rank)
  first_rank += len(page_names)
print ("written", args.output + ".pdf", "with", npages, "page(s)")
