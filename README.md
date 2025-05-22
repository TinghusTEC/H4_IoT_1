# Smiley Feedback Panel
En opgave for IoT3 på TEC

## Udstyr
* en esp32
* et breadboard
* fire led'er i forskellige farver
* 8 220Ω resistorer
* en bunke jumper kabler

### Diagram
![billed](/documentation/files/diagram.png)

## Brugervejledning
Upload koden til din esp32 og vent til den er klar, du kan se den er klar og forbundet til wifi'et når onboard LED'en ikke længere blinker hurtigt. \
Når den er klar kan du trykke på en af knapperne og se at den tilhørende LED tænder. Du er låst ude i syv sekunder mens esp'en sender MQTT beksederne til severen. \
 Onboard LED'en vil blinke hurtigt mens esp'en opretter forbindelse til MQTT serveren. \ 
 Efter LED'en slukker kan du klikke på en anden knap. \
Esp'en går i deep sleep efter et minut uden aktivitet. Når man trykker på en knap vil esp'en vågne igen.

## Dokumentation
* [logbog](./documentation/logbog.md)
* [opgavefodeling](./documentation/opgaveCheckliste.md)

