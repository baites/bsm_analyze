# from http://hep.pha.jhu.edu:8080/boostedtop/1456

# use width=1.2 or 3.0 for the high-mass all-hadronic
# comparison
def get_zp(widthZp, kFactorZP=1.3):
    x, y = [0.0] * 14, [0.0] * 14
    x[0], y[0] = 700.0/1000. ,  0.3744E+01* kFactorZP * widthZp
    x[1], y[1] = 800.0/1000. ,  0.2128E+01* kFactorZP * widthZp
    x[2], y[2] = 900.0/1000. ,  0.1265E+01* kFactorZP * widthZp
    x[3], y[3] = 1000.0/1000. ,  0.7784E+00* kFactorZP * widthZp
    x[4], y[4] = 1100.0/1000. ,  0.4919E+00* kFactorZP * widthZp
    x[5], y[5] = 1200.0/1000. ,  0.3174E+00* kFactorZP * widthZp
    x[6], y[6] = 1300.0/1000. ,  0.2081E+00* kFactorZP * widthZp
    x[7], y[7] = 1400.0/1000. ,  0.1382E+00* kFactorZP * widthZp
    x[8], y[8] = 1500.0/1000. ,  0.9276E-01* kFactorZP * widthZp
    x[9], y[9] = 1600.0/1000. ,  0.6275E-01* kFactorZP * widthZp
    x[10], y[10] = 1700.0/1000. ,  0.4272E-01* kFactorZP * widthZp
    x[11], y[11] = 1800.0/1000. ,  0.2923E-01* kFactorZP * widthZp
    x[12], y[12] = 1900.0/1000. ,  0.2008E-01* kFactorZP * widthZp
    x[13], y[13] = 2000.0/1000. ,  0.1383E-01* kFactorZP * widthZp
    return [1000*xx for xx in x], y

def get_kkg():
    x_mass_fixed = array.array('d', [ 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 
                                      2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9,
                                      3.0
                                      ] )
    y_kkg = array.array('d', [ 4.5, 2.9, 1.9, 1.3, 0.9, 0.59, 0.41, 0.28, 0.21, 0.14,
                               0.10,0.07, 0.055, 0.045, 0.04, 0.031, 0.025, 0.02, 0.018,
                               0.014
                               ])
    #TODO: something is wrong; they have not the same length!
    return [1000 * xx for xx in x_mass_fixed[:20]], y_kkg[:20]

