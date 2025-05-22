# Logbog

## Tirsdag d. 20-05-2025

### Plan

* Opsætning af Repo
* Opsætning af IDE
* Opsætning af Hardware

### Faktisk

* Vi fik sat Repo op på github og sat rettigheder op.
* Vi fik sat hardware op med LED og knapper.
* Vi fik testet at vores device kunne registrere input og aktivere led.
* Vi fik fordelt de relevante arbejdsopgaver.
* Sikrede os at vi begge 2 kunne builde og deploye til ESP32
* Gennemgik dokumentation om ESP32 sleep mode.
* Fået et kort overblik over krav til kode og arkitektur
* Påbegyndte kodning

### Noter

Vi skal have sikret os at de pins vi bruger til vores knapper også understøtter wakeup.

Rent designmæssigt vil det give mest mening at håndtere sleep timer gennem en service som
vi kan kalde ind i hver gang der bliver foretaget en handling der skal udsætte sleep eller
vække enheden.

Det vil så også være den service der har ansvaret for vække ESP32 periodisk hvor den så kan verificere
den har en wifi forbindelse og afstemme vores interne ur.

Vi skal have undersøgt præcis hvilken kode der bliver kørt når enheden blive vækket. Samt hvilke moduler vi
har adgang til

I forhold til batteri kan det muligvis være relevant at logge hvornår device går i sleep mode,
hvonår den forlader sleep mode samt årsagen til den har forladt sleep mode. Da det vil tillade os
at samle noget lidt mere meningsfuldt data sammen. Det vil også kunne bruges til debugging.

## Onsdag d. 21-05-2025

### Plan

* Få bygget en service til at håndtere sleep
* Få forbundet til WIFI og hent tidsstempel
* Få forbundet til MQTT
* Få ombygget device så knapper sidder på de korrekte pins for at wake-up fungerer
* Få tilføjet lokal logning på device.

### Faktisk
* Vi fik sat en EventService up der understøtter deep sleep og callbacks
* WiFi delen blev kodet.
* Vi fik sat MQTT op med TSL men rendte ind i nogle udfordringer med hensyn til at få den til at verifisere  certifikater. Certifikaterne blev genereret med openssl og de virker på andre enheder. men ikke på ESP32.
* Vi fik bygget device om så knapperne kom over på pins der understøttede at vække devicen.
* Vi droppede lokal logning

### Noter

Hvordan skal logning håndteres når vi ikke har adgang til et netværk hvor vi kan få et timestamp.
Hvis vi stoler på det interne risikerer og ikke markerer hvor timestamp kommer fra. Så risikerer
vi at logning ikke vil være til at være præcis. Umiddelbart vil det nok give mest mening at gemme logs hvor vi ikke har fået bekræftet tiden uden timestamp. Så de nemt kan ignoreres.

Skal have kigget ind i forskellen på main memory chippen og RTC.

Havde et hul i vores logik for callback funktionen i event servicen. Så den ikke blev triggered i sleep mode.
Tilføjede

Rendte ind 2 seperate problemer med at vække enheden fra deep sleep.
Første problem var grundet den måde vi havde sat vores kredsløb op.
Det andet problem kom sig af at man ikke både kunne have wakeup sat på onboard knappen
og de almindelige input samtidig

## Torsdag d. 22-05-2025

### Plan

* Få tilføjet LEDs.
* Lav Map til knapper og LEDs.
* Merge netværks relateret kode med input relateret kode.
* Forbind til fælles MQTT

### Faktisk
* LEDs sat op
* Fik tilføjet mapping mellem knapper/leds/vote values
* Fik merged vores 2 arbejdsbranches
* Forbandt til fælles MQTT Server
* Tilføjede timestamps
* Tilføjede diagram til filer

### Noter

Deep Sleep fungerede ikke helt som forventet. Mens vi kan vække enheden fra deep sleep og den kan gå i deep sleep. Så kan man ikke lytte på flere en én knap når man har sat det op som vores, i pull up mode, så alle knapper sender HIGH når de ikke er trykket ned. En mask skulle sørge for at lytte til alle knapper, men triggeren til wake up findes kun som en der tjekker ALLE knapper for LOW. 

Det vil sige at den aldrig kan vækkes i vore setup med pull up. Dog *burde* man kunne tage signalet fra alle knapperne, og gennem dioder samle knappernes signaler til én pin, Den står så i pullup mode men skulle blive LOW når en af knapperne trykkes ned. 

Problemet er at de dioder vi havde første for meget strøm igennem, så signalet blev aldrig lavt nok til at flippe pinnen til LOW. Der var én enkel diode til rådighed der havde lav nok forward voltage til at det kunne lade sig gøre. Så det blev testet på en enkel knap og det virkede -og i teorien burde vi kunne samle alle knapperne på den måde, uden at de ville interferere med hinanden.  