grid 0 -1 0 -4e+09 -4e+09 -4e+09  4e+09 4e+09 4e+09  0 0 0  32 32 32
grid 1 0 0 -2e+09 -2e+09 -2e+09  2e+09 2e+09 2e+09  16 16 16  32 32 32
dimension 3
domain    3 -4e9 -4e9 -4e9  4e9 4e9 4e9
boundary  dirichlet
sphere    5.993985e27 6.378137e8 0.0 0.0 0.0
point     1e43   -6.25e+07 -6.25e+07 -6.25e+07   1
point     1e43   6.25e+07 -6.25e+07 -6.25e+07   1
point     1e43   -6.25e+07 6.25e+07 -6.25e+07   1
point     1e43   6.25e+07 6.25e+07 -6.25e+07   1
point     1e43   -6.25e+07 -6.25e+07 6.25e+07   1
point     1e43   6.25e+07 -6.25e+07 6.25e+07   1
point     1e43   -6.25e+07 6.25e+07 6.25e+07   1
point     1e43   6.25e+07 6.25e+07 6.25e+07   1
discret constant
solver fac
dump_x true
dump_a true
dump_b true
