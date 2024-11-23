set term pdf
set output "optimal-irs-placement.pdf"
# Load the data from the CSV file
set datafile separator ","
set xlabel "x pos (m)" font "Helvetica,12"
set ylabel "SNR (dB)" font "Helvetica,12"
set tics font "Helvetica,10"
set xrange [0:15]
set key Left
set key right top
set key outside
set key width -2
set key reverse
set key font "Helvetica,12"
set key spacing 1.5
set grid

plot "optimal-irs-placement.csv" using 1:3 with lines lw 2 lc rgb "#7570b3" dt 2 title "LOS", \
	 "optimal-irs-placement.csv" using 1:2 with lines lw 2 lc rgb "#1b9e77" dt 1 title "IRS + LOS (MATLAB)", \
     "optimal-irs-placement.csv" using 1:4 with lines lw 2 lc rgb "#1f78b4" dt 1 title "IRS (MATLAB)", \
     "optimal-irs-placement.csv" using 1:5 with lines lw 2 lc rgb "#a6cee3" dt 2 title "IRS (ETSI)", \
     "optimal-irs-placement.csv" using 6:7 with lines lw 1 lc rgb "#e31a1c" dt 1 title "IRS (ns3)", \
     "optimal-irs-placement.csv" using 6:8 with lines lw 1 lc rgb "#d95f02" dt 1 title "IRS + LOS (ns3)"

