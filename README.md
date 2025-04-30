# Elsys_prosjekt_2025_ferdig
Koder til prosjektet. Arduino-kode til ESP-en, og python-kode til datamaskinen.

## Arduino-kode:
- Arduino-koden er utviklet for å kjøre på en ESP32 Wroom 32D. 
- Koden samler sensordata under opptak og sender disse til datamaskinen via WiFi-protokollen TCP etter opptaket er ferdig.

## Python-kode:
- Python-koden kjøres på datamaskinen og mottar sensordataene som sendes fra ESP32-en via WiFi-protokollen TCP. 
- Python-scriptet lagrer dataene i csv-filer, preprosesserer de, og bruker matplotlib.animation til simulering av opptaket.

### Konfigurasjon av kodene:
- Sett IP-adressen til datamaskinen i `host`-variabelen, i Arduino-koden (i filen `Arduino.ino`).
- Endre variablene `ssid` og `password` til hhv. navn og passord for det valgte WiFi-nettverket, i Arduino-koden (i filen `wifi_config.cpp`).
- Sett IP-adressen til ESP_en i `IP_ESP32`-variabelen, i Python-koden (i filen `tcpServer2.py`).

## Krav:
- ESP32 Wroom 32D for Arduino-koden. Andre ESP-er kan gjerne også gå fint, men har kun testet på ESP32 Wroom 32D.
- Python 3.11.4 for datamaskinen.
- Begge enhetene må være på samme WiFi-nettverk for å kunne kommunisere.

## Bruksanvisning:
1. Koble datamaskinen til et valgt WiFi-nettverk.
2. Endre `host`-variabelen i Arduino-koden (`Arduino.ino`) til datamaskinens IP-adresse
3. Endre variablene `ssid` og `password` til til det valgte WiFi-nettverkets hhv. navn og passord i Arduino-koden (i filen `wifi_config.cpp`).
4. Last opp Arduino-koden til ESP32.
5. Sett IP-adressen til ESP_en i `IP_ESP32`-variabelen i Python-koden (i filen `tcpServer2.py`).
6. Kjør Python-koden på datamaskinen. Sjekk i terminalen at det opprettes en forbindelse på alle portene (siste er port 6000).
7. Bruk knappene i animasjonsvinduet til å hente data fra ESP-en, starte/stoppe og restarte simulasjonen, og restarte ESP-en.
