rm -r 8091
mkdir 8091 && cd 8091
mkdir Shaklein && cd Shaklein
date > Seva.txt
date --date "next Mon" > filedate.txt
cat Seva.txt filedate.txt > result.txt
cat result.txt