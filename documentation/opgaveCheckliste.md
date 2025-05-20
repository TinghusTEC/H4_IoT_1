# Opgave checkliste

- [ ] Anordningen har 4 knapper, med tilhørende smileys.
- [ ] Der er respons på LED ved hver knap.
- [ ] På kapperne implementeres debounce funktionalitet
- [ ] Efter tryk på en knap, er anordningen låst i 7 sekunder, før en ny feedback modtages
- [ ] lysdioden ud for den trykkede knap lyser i alle 7 sekunder
- [ ] Anordningen tilsluttes internet, via wi-fi access-punkt i undervisningslokalet
- [ ] Anordningens ur synkroniseres løbende med NTP, og korrekt tidszone
- [ ] Knaptryk-data (værdi og timestamp), overføres via MQTT
- [ ] der bruges brugernavn og password, og hele kommunikationen er sikret ved brug af TLS-kryptering
- [ ] Anordningen er designet strømbesparende, ved brug af DeepSleep og batteridrift
- [ ] anordningen skal spare strøm ved at skifte til deep-sleep, når den er ubeskæftiget iet stykke tid
- [ ] eksperimenter og analyser skal vise de bedste tidsgrænser for hvor længeanordningen er ubeskæftiget, men aktiv
- [ ] samt, hvordan skal opvågnen fungere, så anordningen opdage hvad der sker, ogfalder i søvn igen
- [ ] en god analyse og overvejelse danner grundlag for hvor længe anordningen sover,inden den vågner rutinemæssigt.

## Proceskrav

- [ ] Opgaven løses i grupper på 2 (to).
- [ ] Under hele projektforløbet skal versionsstyringssystemet git bruges til at håndtere og registrere de filer som indgår i projektet.
- [ ] Alle gruppemedlemmer skal dagligt lave comits til repo'et.
- [ ] Gruppen fører løbende en logbog, hvor alle comit'ter, mindst en gang om dagen
- [ ] Gruppen opbygger også et arbejds-portfolio hvor alle ideer og beslutninger, som førertil løsning af projektets elementer beskrives, fra ide, via skitse til færdig løsning

## Søren

- [ ] Anordningen tilsluttes internet, via wi-fi access-punkt i undervisningslokalet
- [ ] Anordningens ur synkroniseres løbende med NTP, og korrekt tidszone
- [ ] Knaptryk-data (værdi og timestamp), overføres via MQTT
- [ ] der bruges brugernavn og password, og hele kommunikationen er sikret ved brug af TLS-kryptering

## Tinghus

- [ ] Anordningen er designet strømbesparende, ved brug af DeepSleep og batteridrift
- [ ] anordningen skal spare strøm ved at skifte til deep-sleep, når den er ubeskæftiget iet stykke tid
- [ ] eksperimenter og analyser skal vise de bedste tidsgrænser for hvor længeanordningen er ubeskæftiget, men aktiv
- [ ] samt, hvordan skal opvågnen fungere, så anordningen opdage hvad der sker, ogfalder i søvn igen
- [ ] en god analyse og overvejelse danner grundlag for hvor længe anordningen sover,inden den vågner rutinemæssigt.