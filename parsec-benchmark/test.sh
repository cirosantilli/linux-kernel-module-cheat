#!/bin/sh

# Since Parsec does not stop on errors and we need some raw commands for gem5,
# let's do some of our own unit tests here. Thses should be run on QEMU
# to be reasonably fast.
#
# Must be run with BR2_PACKAGE_PARSEC_BENCHMARK_INPUT_SIZE=test

set -ex

ncpus="${1:-1}"

## apps

# TODO segfaulting.
#cd /parsec/ext/splash2x/apps/barnes/run
#../inst/*/bin/barnes 1 < input_1

cd /parsec/ext/splash2x/apps/fmm/run
../inst/*/bin/fmm "$ncpus" < input_1

cd /parsec/ext/splash2x/apps/ocean_cp/run
../inst/*/bin/ocean_cp -n258 -p"$ncpus" -e1e-07 -r20000 -t28800

# TODO segfaulting.
# cd /parsec/ext/splash2x/apps/ocean_ncp/run
# ../inst/*/bin/ocean_ncp -n258 -p"$ncpus" -e1e-07 -r20000 -t28800

cd /parsec/ext/splash2x/apps/radiosity/run
../inst/*/bin/radiosity -bf 1.5e-1 -batch -room -p "$ncpus"

cd /parsec/ext/splash2x/apps/raytrace/run
../inst/*/bin/raytrace -s -p"$ncpus" -a4 teapot.env

cd /parsec/ext/splash2x/apps/volrend/run
../inst/*/bin/volrend "$ncpus" head-scaleddown4 4

cd /parsec/ext/splash2x/apps/water_nsquared/run
../inst/*/bin/water_nsquared "$ncpus" < input_1

cd /parsec/ext/splash2x/apps/water_spatial/run
../inst/*/bin/water_spatial "$ncpus" < input_1

## kernels

cd /parsec/ext/splash2x/kernels/cholesky/run
../inst/*/bin/cholesky -p"$ncpus" < tk14.O

cd /parsec/ext/splash2x/kernels/cholesky/run
../inst/*/bin/cholesky "$ncpus" test

cd /parsec/ext/splash2x/kernels/fft/run
../inst/*/bin/fft -m18 -p"$ncpus"

cd /parsec/ext/splash2x/kernels/lu_cb/run
../inst/*/bin/lu_cb -p"$ncpus" -n512 -b16

cd /parsec/ext/splash2x/kernels/lu_ncb/run
../inst/*/bin/lu_ncb -p"$ncpus" -n512 -b16

# TODO ERROR: Cannot malloc enough memory for global
# cd /parsec/ext/splash2x/kernels/radix/run
# ../inst/*/bin/radix -p"$ncpus" -r4096 -n262144 -m524288
