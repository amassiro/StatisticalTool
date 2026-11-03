#
# fake impacts json (same format as combineTool.py -M Impacts) to test the plotting
#
from __future__ import print_function
import json, random

random.seed(3)
names = ["CMS_btag_HF_corr", "FSR", "WjetsBC_norm", "CMS_pileup", "CMS_JES_RelHF", "CMS_JES_RelPt_2022",
         "QCDscale_muF_tch", "lumi_2022_2023", "CMS_btag_HF_uncorr_2022", "MEPS_tch", "CMS_UE_2022",
         "CMS_eff_e_trig", "CMS_JES_PileUpPtBB", "CMS_eff_m_trig", "CMS_UE_2023", "prop_binch1_bin11"] + \
        ["minor_%d" % i for i in range(10)]

def make(asimov):
  params = []
  for i, n in enumerate(names):
    scale = 0.014 * (1. - 0.05 * i) if i < 16 else 0.002
    pull  = 0. if asimov else random.gauss(0, 0.6)
    constr = random.uniform(0.7, 1.0)
    fit = [pull - constr, pull, pull + constr]
    up = scale * random.uniform(0.6, 1.2) * random.choice([1, -1])
    dn = -up * random.uniform(0.7, 1.3)
    r0 = 1.0 if asimov else 0.97
    params.append({"name": n, "type": "Gaussian", "groups": [], "prefit": [-1., 0., 1.],
                   "fit": fit, "r": [r0 + dn, r0, r0 + up], "impact_r": max(abs(up), abs(dn))})
  # unconstrained parameter (e.g. rateParam), different values in data and Asimov
  fit = [0.89, 1.00, 1.12] if asimov else [0.95, 1.083, 1.21]
  params.append({"name": "rate_WjetsBC", "type": "Unconstrained", "groups": [], "prefit": [0., 1., 2.],
                 "fit": fit, "r": [0.983, 1.0, 1.014] if asimov else [0.952, 0.97, 0.985], "impact_r": 0.018})
  r0 = 1.0 if asimov else 0.97
  return {"POIs": [{"name": "r", "fit": [r0 - 0.04, r0, r0 + 0.04]}], "params": params, "method": "default"}

json.dump(make(False), open("impacts_data.json", "w"), indent=1)
json.dump(make(True),  open("impacts_asimov.json", "w"), indent=1)
print ("written impacts_data.json and impacts_asimov.json")
