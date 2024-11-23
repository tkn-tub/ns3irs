set terminal pdf
set output "irs-validation.pdf"

# set title 'Throughput with Channel conditions'
# set xlabel 'Columns'
set ylabel "Throughput (Mbps)" font "Helvetica,12" offset 1,0

# Set boxplot style
set style data boxplot
set style fill solid 0.5
set boxwidth 0.5
set pointsize 0.5
set yrange [-5:350]
set bmargin 3

set key off
set grid
# Set x-axis tics
set xtics ("LOS" 1, "IRS" 2, "IRS + LOS\nConstructive" 3, "Multiple\nIRS + LOS" 4, "IRS + LOS\nDestructive" 5)
set xtics font "Helvetica,12"

# Plot each column as a separate boxplot
plot 'irs-validation.dat' using (1):1 with boxplot title 'LOS', \
     '' using (2):2 with boxplot title 'IRS', \
     '' using (3):3 with boxplot title 'IRS + LOS (Constructive)', \
     '' using (4):5 with boxplot title 'Multiple IRS + LOS', \
     '' using (5):4 with boxplot title 'IRS + LOS (Destructive)'
