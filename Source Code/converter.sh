# Created by César Augusto Marcelino dos Santos (cesaraugustounifei@gmail.com)
# Converts Keithley raw CSV data to time x current PS chart files

#!/bin/bash

# Treat the Keithley outpu files, then create GNUPLOT PS files
# Time is in seconds, convert to milliseconds
# Current is in multiples of 10mA, convert to uA
for file in $(echo *.csv); do

	#Create a temporary file with arbitrary extension
	outfile="${file}.csp"

	# Filters the data with correct values for the desired units
        awk -F "," '{ $1=$1*1000; $3=$3*100; print }' OFS="," $file > $outfile

	# Plot overall data: from POR to 110ms
	gnuplot <<EOF
	set terminal postscript eps enhanced color dashed "Helvetica" 14
	set output "${file}.ps"
	set datafile separator ','
	set xlabel 'Tempo [ms]'
	set ylabel 'Corrente [mA]'
	set grid
	plot "$outfile" u 1:3 with linespoints linetype 3 notitle
EOF
done

# Plot only FreeRTOS execution
# Stall-like measurements do not run FreeRTOS
#filelist=$(find *.csp -type f -print | grep -v "Stall")
filelist=$(find *.csp -type f -print)
for var in $filelist; do
	tempfile="${var}_FreeRTOS.csp"

	# ARM, MSP430 and old models have different timings to start executing the FreeRTOS
	if [[ $var == *"ARM"* ]]; then
		awk -F "," '{if($1 >= 10){print}}' OFS="," $var > $tempfile
	elif [[ $var == *"MSP430"* ]]; then
		awk -F "," '{if($1 >= 50){print}}' OFS="," $var > $tempfile
	else
		awk -F "," '{if($1 >= 70){print}}' OFS="," $var > $tempfile
	fi

	# Let's plot it!
	gnuplot <<EOF
	set terminal postscript eps enhanced color dashed "Helvetica" 14
	set output "${tempfile}.ps"
	set datafile separator ','
	set xlabel 'Tempo [ms]'
	set ylabel 'Corrente [mA]'
	set grid
	plot "$tempfile" u 1:3 with linespoints linetype 3 notitle
EOF
done

# Remove temporary filtered files
rm *.csp
