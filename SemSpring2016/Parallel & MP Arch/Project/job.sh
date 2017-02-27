#!/bin/bash
#SBATCH - J sobel_OMP.cpp
#SBATCH -o sobel.o%j
#SBATCH -n 16
#SBATCH -p normal
#SBATCH -t 01:00:00
#SBATCH--mail-user=NagaGanesh_Kurapati@student.uml.edu
#SBATCH--mail-type=begin
#SBATCH--mail-type=end
#SBATCH -A TG-CIE160001
./sobel map_3.ppm
