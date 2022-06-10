set terminal gif
set output "trial.gif"
set terminal gif animate delay 10
set xlabel '{y}' font ",13"
set ylabel '{x}' font ",13"



set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi 1 ps 1.5
set pointintervalbox 3

do for [i=1:75] {
	tl = system(sprintf("cat TSP_result_%d.txt | awk 'NR==1{print $0}'", i))
	#tl = system(sprintf("cat TSP_result_%d.txt | awk 'NR==1{print $0}'", i))+0    // number
	set title sprintf("%s", tl)
	plot sprintf("<(cat TSP_result_%d.txt | awk 'NR>1{print $1,$2}')", i) with linespoints ls 1 notitle
	pause 0.2
	reread
}