import os

execfile("histogram_filter.py")

# rebin the mttbar histogram. 
# Can be removed eventually, once the input root file is adapted accordingly.
def transform_histo(h):
    name, xmin, xmax, data = h
    if name.startswith('el_mttbar'):
        rebin_hlist(data, 100)
    else:
        assert False

    return name, xmin, xmax, data

def build_model_ele():
    model = build_model_from_rootfile('theta_input.root', histogram_filter = histogram_filter, transform_histo = transform_histo)
    model.set_signal_processes('zp*')
    model.fill_histogram_zerobins()

    model.add_lognormal_uncertainty('top_rate', math.log(1.15), 'ttbar')
    model.add_lognormal_uncertainty('stop_rate', math.log(1.15), 'singletop')
    for p in ('wjets', 'zjets'):
        model.add_lognormal_uncertainty('vjets_rate', math.log(1.30), p)

    model.add_lognormal_uncertainty('qcd_rate', math.log(1.5), 'eleqcd')

    for p in model.processes:
        if not p.startswith("eleqcd"):
            model.add_lognormal_uncertainty('lumi', math.log(1.045), p)
            model.add_lognormal_uncertainty('trigger', math.log(1.040), p)
            #model.add_asymmetric_lognormal_uncertainty('trigger', 4.0 / 96.0, 4.0 / 96.0, p)

    return model


def limits_ele(model):
   plot_exp, plot_obs = bayesian_limits(model)
   #plot_exp, plot_obs = cls_limits(model)

   # zprimes are normalised to 0.1 pb, so multiply all y-values with 0.1 to get it in pb:
   #plot_exp.scale_y(10)
   #plot_obs.scale_y(10)

   plot_exp.write_txt('exp_limit.txt')
   plot_obs.write_txt('obs_limit.txt')
   
   # NOTE: the rest of this function is just for the pdf plot
   plot_obs.legend = 'observed limit (95% C.L.)'
   plot_exp.legend = 'expected limit (95% C.L.)'
   dp1 = plotutil.plotdata()
   dp1.x = [1000]
   dp1.y = [0]
   dp1.color = plot_exp.bands[1][2]
   dp1.legend = 'central $1\\sigma$ expected limit'
   dp2 = plotutil.plotdata()
   dp2.x = [1000]
   dp2.y = [0]
   dp2.color = plot_exp.bands[0][2]
   dp2.legend = 'central $2\\sigma$ expected limit'
   zp12 = plotutil.plotdata()
   zp12.x, zp12.y = get_zp(1.2)
   zp12.legend = 'Topcolor $\\rm Z^{\\prime}$, 1.2% width, Harris et al.'
   zp12.color = '#ff00ff'
   zp12.fmt = '--'
   zp3 = plotutil.plotdata()
   zp3.x, zp3.y = get_zp(3.0)
   zp3.legend = 'Topcolor $\\rm Z^{\\prime}$, 3.0% width, Harris et al.'
   zp3.color = '#0000ff'
   zp3.fmt = '--'
   kkg = plotutil.plotdata()
   kkg.x, kkg.y = get_kkg()
   kkg.legend = 'KK Gluon, Agashe et al.'
   kkg.color = '#aaaa00'
   kkg.fmt = '--'
   kkg.as_function=True
   zp12.as_function=True
   zp3.as_function=True
   plots = [plot_exp, dp1, dp2, zp3, zp12]
   plotutil.plot(plots, "$M_{\\rm Z^{\\prime}}$ [GeV/$c^{2}$]",
    "limit on $\sigma(\\rm pp\\rightarrow Z^{\\prime} \\rightarrow t\\bar{t})$ [pb]",
    "limits.pdf", title_ul = '$L = 3.4$ fb$^{-1}$', xmin=1000, ymin=0.0, ymax=15.0,
    title_ur = 'electron+jets, $\\sqrt{s} = 7$ TeV')
   plotutil.plot(plots, "$M_{\\rm Z^{\\prime}}$ [GeV/$c^{2}$]",
    "limit on $\sigma(\\rm pp\\rightarrow Z^{\\prime} \\rightarrow t\\bar{t})$ [pb]",
    "limits-log.pdf", title_ul = '$L = 3.4$ fb$^{-1}$', logy = True, xmin=1000, ymin=0.05, ymax=15.0,
    title_ur = 'electron+jets, $\\sqrt{s} = 7$ TeV')

execfile("theory-xsecs.py")
model = build_model_ele()
model_summary(model)
res = ml_fit_coefficients(model, signal_processes = [''], nuisance_constraint = "")
for p in res['']['el_mttbar']:
    print('%s: %f' % (p, res['']['el_mttbar'][p]))

with open('weights.txt', 'w') as output:
    for p in res['']['el_mttbar']:
        output.write("{0}: {1}\n".format(p, res['']['el_mttbar'][p]))

res = ml_fit(model, signal_prior = 'fix:0', nuisance_constraint = '')

with open('nuisance_params.txt', 'w') as output:
    output.write(str(res))

limits_ele(model)

if not os.path.exists("report"):
    os.mkdir("report", 0755)

report.write_html('./report/')
