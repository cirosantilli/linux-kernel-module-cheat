#!/bin/sh

# Since Parsec does not stop on errors and we need some raw commands for gem5,
# let's do some of our own unit tests here. Thses should be run on QEMU
# to be reasonably fast.
#
# Must be run with BR2_PACKAGE_PARSEC_BENCHMARK_INPUT_SIZE=test

set -ex

ncpus=1

cd /parsec/ext/splash2x/apps/fmm/run
../inst/*/bin/fmm "$ncpus" < input_1

cd /parsec/ext/splash2x/apps/raytrace/run
../inst/*/bin/raytrace -s -p"$ncpus" -a4 teapot.env

cd /parsec/ext/splash2x/apps/volrend/run
../inst/*/bin/volrend "$ncpus" head-scaleddown4 4 < input_1

cd /parsec/ext/splash2x/apps/water_nsquared/run
../inst/*/bin/water_nsquared "$ncpus" < input_1

cd /parsec/ext/splash2x/apps/water_spatial/run
../inst/*/bin/water_spatial "$ncpus" < input_1
