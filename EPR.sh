#!/bin/bash

EXE="x64/Release/GameOfLife.exe"
OUT_NAME="gs20m019"
INPUT="Data/in"

echo "Removing Folder 'EPR_Output'"
rm -r EPR_Output
echo "Creating Folder 'EPR_Output"
mkdir EPR_Output
mkdir EPR_Output/OutputFiles

for i in 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000
do
    echo "Running File '$i', Mode=SEQ"
    ./$EXE --load $INPUT/random"$i"_in.gol --save EPR_Output/OutputFiles/"$OUT_NAME"_"$i"_cpu_out.gol --generations 250 --measure --mode seq >> EPR_Output/"$OUT_NAME"_cpu_time.csv
    echo "Running File '$i', Mode=OMP"
    ./$EXE --load $INPUT/random"$i"_in.gol --save EPR_Output/OutputFiles/"$OUT_NAME"_"$i"_openmp_out.gol --generations 250 --measure --mode omp >> EPR_Output/"$OUT_NAME"_openmp_time.csv
    echo "Running File '$i', Mode=OCL"
    ./$EXE --load $INPUT/random"$i"_in.gol --save EPR_Output/OutputFiles/"$OUT_NAME"_"$i"_opencl_cpu_out.gol --generations 250 --measure --mode ocl >> EPR_Output/"$OUT_NAME"_opencl_cpu_time.csv
done

