#!/bin/bash

pushd ../../
	./run.sh 240D/hw3/src/hw3.cpp
popd

mkdir -p data

mv ../../trajectory.csv data
mv ../../gains.csv data

# Plotting

mkdir -p plots

python src/generate_plots.py

mv *.png plots