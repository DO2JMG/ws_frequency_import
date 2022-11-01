# Frequenz Import von wettersonde.net für dxlAPRS.

Es werden alle Frequenzen importiert, aus einem angegebenen Bereich, den man in den Einstellungen festlegt. Danach wird diese Liste abgearbeitet und alle 20 Sekunden eine Frequenz nach der anderen in die Frequenzliste geschrieben. So braucht man nur einen SDR-Stick am PI.

<b>Installation</b>

Vor ab muss das folgende Paket installiert werden :

<code>sudo apt-get install libcurl4-openssl-dev</code>

<b>Kopieren und Compilieren</b>

<code>
git clone https://github.com/DO2JMG/ws_frequency_import.git
cd ws_frequency_import
make

cp fqimport /home/pi/ws/bin/
cp dxlfqimport.sh /home/pi/ws/
cp ws-import.conf /home/pi/ws/

sudo chmod +x /home/pi/ws/dxlfqimport.sh
</code>

<b>Cronjob einstellen</b>

<code>*/1 * * * * /home/pi/ws/dxlfqimport.sh</code>

<b>Beispiel ws-import.conf</b>

<code>
latitude="52.1234"                          
longitude="8.1234"
range="300"                                     # Reichweite Kilometer (Radius)
squelch="85"
channelfile="/home/pi/ws/sdrcfg-rtl0.txt"
</code>
