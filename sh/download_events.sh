#!/bin/sh
clear

# Főoldal letöltése.
rm -rf html
mkdir html
echo Downloading main page...
wget http://www.tipplap.hu/kalkulator/ -O ./html/_dates_labdarugas.html -q

# Dátumok kinyerése.
echo Parsing dates...
hxnormalize -l 2400 -d -x ./html/_dates_labdarugas.html | hxselect -s'\n' -c '#filterDay' | sed -n -e "s/^.*value=['\"]\(.*\)['\"].*/\1/p" | sed -e 's/\(^.*\)/http:\/\/www.tipplap.hu\/kalkulator\/\1\/1\/labdarugas/' > ./html/_url_labdarugas.txt

# Dátumokhoz tartozó weblapok letöltése.
echo Downloading pages by dates...
cd html
wget -i _url_labdarugas.txt -q
cd ..

# Fájlok átszámozása (feltöltés szám eleji nullákkal a megfelelő sorrend miatt).
echo Renaming files...
mv ./html/labdarugas ./html/labdarugas.0
rename 's/\d+/sprintf("%02d",$&)/e' ./html/labdarugas.*

# Adatok kinyerése az eseményeket tartalmazó fájlokból.
echo Parsing events...
> ./html/no_labdarugas.txt
> ./html/min_labdarugas.txt
> ./html/title_labdarugas.txt
> ./html/odds_labdarugas.txt
> ./html/day_labdarugas.txt
> ./html/time_labdarugas.txt

for f in ./html/labdarugas*
do
	echo $f
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line1' | hxselect -s'\n' -c '.no' >> ./html/no_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line2' | hxselect -s'\n' -c '.no' >> ./html/no_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line1' | hxselect -s'\n' -c '.min'  >> ./html/min_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line2' | hxselect -s'\n' -c '.min'  >> ./html/min_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line1' | hxselect -s'\n' -c '.title' >> ./html/title_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line2' | hxselect -s'\n' -c '.title' >> ./html/title_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line1' | hxselect -s'\n' -c '.odds' | hxselect -s'\n' -c 'a' >> ./html/odds_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line2' | hxselect -s'\n' -c '.odds' | hxselect -s'\n' -c 'a' >> ./html/odds_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line1' | hxselect -s'\n' -c '.day' >> ./html/day_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line2' | hxselect -s'\n' -c '.day' >> ./html/day_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line1' | hxselect -s'\n' -c '.time' | sed -n -e 's/^\([0-9][0-9]:[0-9][0-9]\)/\1/p' >> ./html/time_labdarugas.txt
	hxnormalize -l 2400 -d -x $f | hxselect -s'\n' -c '.line2' | hxselect -s'\n' -c '.time' | sed -n -e 's/^\([0-9][0-9]:[0-9][0-9]\)/\1/p' >> ./html/time_labdarugas.txt

done

# Fájlok egyesítése.
echo Merging files...

paste -d "\t" - - - < ./html/odds_labdarugas.txt > ./html/odds_labdarugas_columns.txt
paste ./html/no_labdarugas.txt ./html/min_labdarugas.txt ./html/title_labdarugas.txt ./html/odds_labdarugas_columns.txt ./html/day_labdarugas.txt ./html/time_labdarugas.txt > ./html/data_labdarugas.txt

# Adatok megfelelő helyre másolása.
echo Copy to bin/data...
mkdir ../bin
mkdir ../bin/data
cp ./html/data_labdarugas.txt ../bin/data/

# Kész.
echo Finished.
