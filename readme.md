# Frequenz Import von wettersonde.net für dxlAPRS.

Es werden alle Frequenzen importiert, aus einem angegebenen Bereich, den man in den Einstellungen festlegt. Danach wird diese Liste abgearbeitet und alle 20 Sekunden eine Frequenz nach der anderen in die Frequenzliste geschrieben. So braucht man nur einen SDR-Stick am PI.

<b>Installation</b>

Vor ab muss das folgende Paket installiert werden :

<code>sudo apt-get install libcurl4-openssl-dev</code>

<b>Kopieren und Compilieren</b>

<code>
git clone https://github.com/DO2JMG/ws_frequency_import.git<br>
cd ws_frequency_import<br>
make<br>
<br>
cp fqimport /home/pi/ws/bin/<br>
cp dxlfqimport.sh /home/pi/ws/<br>
cp ws-import.conf /home/pi/ws/<br>
<br>
sudo chmod +x /home/pi/ws/dxlfqimport.sh
</code>

<b>Cronjob einstellen</b>

<code>*/1 * * * * /home/pi/ws/dxlfqimport.sh</code>

<b>Beispiel ws-import.conf</b>

<code>
latitude="52.1234"<br>                          
longitude="8.1234"<br>
range="300"<br>                                     # Reichweite Kilometer (Radius)<br>
squelch="85"<br>
channelfile="/home/pi/ws/sdrcfg-rtl0.txt"
</code>
