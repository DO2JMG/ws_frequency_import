# Frequenz Import von wettersonde.net für dxlAPRS.

Es werden alle Frequenzen importiert, aus einem angegebenen Bereich, den man in den Einstellungen festlegt. Danach wird diese Liste abgearbeitet und alle 20 Sekunden eine Frequenz nach der anderen in die Frequenzliste geschrieben. So braucht man nur einen SDR-Stick am PI.

<b>Installation</b>

Vor ab muss das folgende Paket installiert werden :

<code>sudo apt-get install libcurl4-openssl-dev</code>

<b>Kopieren und Compilieren</b>

<code>git clone https://github.com/DO2JMG/ws_frequency_import.git</code><br>
<code>cd ws_frequency_import</code><br>
<code>make</code><br>
<br>
<code>cp fqimport /home/pi/ws/bin/</code><br>
<code>cp dxlfqimport.sh /home/pi/ws/</code><br>
<code>cp ws-import.conf /home/pi/ws/</code><br>
<br>
<code>sudo chmod +x /home/pi/ws/dxlfqimport.sh</code><br>
</code>

<b>Cronjob einstellen</b>

<code>*/1 * * * * /home/pi/ws/dxlfqimport.sh</code>

<b>Beispiel ws-import.conf</b>

<code>latitude="52.1234"</code><br>                  
<code>longitude="8.1234"</code><br>
<code>range="300"                                     # Reichweite Kilometer (Radius)</code><br>
<code>squelch="85"</code><br>
<code>channelfile="/home/pi/ws/sdrcfg-rtl0.txt"</code><br>

