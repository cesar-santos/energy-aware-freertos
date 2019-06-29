# Created by César Augusto Marcelino dos Santos (cesaraugustounifei@gmail.com)
# Converts Keithley raw CSV data to time x current PS chart files

#!/bin/bash

# Directories to be accessed by this script
declare -a folders=("ARM" "IB64" "IB64_Clones/Sample1" "IB64_Clones/Sample2" "IB64_Clones/Sample3" "IB66/Sample1" "IB66/Sample2" "IB66/Sample3" "IB66/Sample4" "MSP430")
base_folder=~/Downloads/Measurements\ -\ 2016/Source\ Meter/

# Access each measurement folder
echo -n "Progress: "
for i in "${folders[@]}"; do
	cd "$base_folder/$i"
	for j in $(ls -d */); do
	(
		cd "$j"
		# Treat the Keithley output files, then create GNUPLOT PS files
		# Time is in seconds, convert to milliseconds
		# Current is in multiples of 10mA, convert to uA (divided by 10 due to average to be calculated on the next step)
		for file in $(echo *.csv); do
			# Create a temporary file with arbitrary extension
			outfile="${file}.csp"

			# Filters the data with correct values for the desired units
	        	awk -F "," '{ $1=$1*1000; $3=$3*100; print }' OFS="," $file > $outfile
		done

		# Generate average of FreeRTOS execution
		awk -F "," 'BEGIN {OFS=",";} {line[FNR] = line[FNR] $3 OFS} END {for (i=1; i<=FNR; i++) print line[i]}' *.csp >> 00.csv
		awk -F "," '{x=0; for (i=1; i<=NF; i++) x+=($i/10); print x}' 00.csv > 00.csv.csp
		cut -d, -f1-2 01.csv.csp > avg.csp
		paste -d, avg.csp <(cut -d, -f1 00.csv.csp) > avg.csv

		# Let's plot it!
		gnuplot <<EOF
			set terminal postscript eps enhanced color dashed "Helvetica" 14
			set output "avg.ps"
			set datafile separator ','
			set xlabel 'Tempo [ms]'
			set ylabel 'Corrente [mA]'
			set grid
			plot "avg.csv" u 1:3 with linespoints linetype 3 notitle
EOF
# EOF or other end-of-file delimiter character CANNOT BE INDENTED!!! So needs to be on column 1 of the text file
# http://stackoverflow.com/questions/18660798/here-document-delimited-by-end-of-file

		# Remove temporary filtered files
		rm 00.*
		rm *.csp
		cd "$base_folder/$i"
		echo -n .
	)
	done
	echo -n !
done

echo " Done!"
