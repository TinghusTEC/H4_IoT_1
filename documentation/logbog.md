# Logbog

## Tirsdag d. 20-05-2025

### Plan

Opsætning af Repo
Opsætning af IDE
Opsætning af Hardware

### Faktisk

Vi fik sat Repo op på github og sat rettigheder op.
Vi fik sat hardware op med LED og knapper.
Vi fik testet at vores device kunne registrere input og aktivere led.
Vi fik fordelt de relevante arbejdsopgaver.
Sikrede os at vi begge 2 kunne builde og deploye til ESP32
Gennemgik dokumentation om ESP32 sleep mode.
Fået et kort overblik over krav til kode og arkitektur
Påbegyndte kodning

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
