# Specifikacija

## Opšti zahtevi  

Očekuje se da projekat ispuni opšta očekivanja i upotrebu naučenih tehnika.

### Upotreba tehnika  

- Programiranje u šejderima  
- Uniforme
- Providne boje i/ili teksture
- Unos s miša i/ili tastature
- Kursor treba da ima izgled žutog rombastog saobraćajnog znaka (kao BUS STOP znak)  tako da je sam znak u gornjem levom uglu, a metalna šipka dijagonala slike.

### Opšti zahtevi prema projektu  

- OpenGL verzija mora biti najmanje 3 i mora se koristiti programabilni pajplajn
- Projekat mora sadržati frame limiter sa 75 FPS
- Prekrivati ceo ekran (full screen)
- Moguće isključivanje na taster Escape u svakom trenutku
- U proizvoljnom uglu ekrana potrebno je nalepiti poluprovidnu teksturu na kojoj je napisano ime, prezime i indeks studenta.


## Putanja i stanice  

Na ekranu je prikazana putanja kojom se kreće autobus.

- Putanja je sačinjena od krivih crvenih linija koje povezuju stanice i formiraju nepravilan oblik.

- Stanica ima 10 i obeležene su crvenim krugovima sa belim brojem unutra, od 0 do 9.

- Putanja je zatvorena: pored svake dve susedne stanice, stanica 0 je povezana i sa stanicom 9.

## Kretanje autobusa

- Autobus je prikazan sličicom autobusa i kreće se po putanji stanicama redom od 0 do 9.

- Autobus se od stanice do stanice kreće konstantnom brzinom. Kada dođe do stanice, on se zadrzava na njoj 10 sekundi. Nakon 10 sekundi nastavlja kretanje na sledecu stanicu.
 
- Potrebno je na proizvoljnom uglu ekrana prikazati ikonicu vrata koja su zatvorena dok se autobus vozi, a otvorena su dok je on na stanici.

## Putnici

- U proizvoljnom uglu ekrana pamti se broj putnika u autobusu, koji je na početku 0.

- Dok su vrata autobusa otvorena na stanici, levim klikom miša na ekran ulazi po jedan putnik, odnosno broj se povećava za 1, dok desnim klikom miša na ekran izlazi po jedan putnik, odnosno broj se smanji za 1.

- Opseg broja putnika se kreće od 0 do 50.

## Kontrola  

- Dok su vrata autobusa otvorena, pritiskom na taster K u autobus ulazi kontrola. Broj putnika se tada poveća za 1, i trećem proizvoljnom uglu ekrana se prikazuje oznaka kontrole (npr. slika saobraćajca u Monopolu)
- Kontrola izlazi i broj putnika se smanji za 1 kada autobus stane na sledecoj stanici
- Moguće je da druga kontrola uđe na stanici na kojoj je prethodna kontrola izašla.

## Kazne  

- Na stanici na kojoj kontrola izlazi se generiše random vrednost između `0` i `broja putnika - 1` kojima je naplaćena kazna (- 1 jer je jedan od putnika kontrola) i sabira se sa ukupnim brojem naplaćenih kazni.
- Ovaj broj je inicijalno 0 i potrebno ga je uvek prikazivati pored broja trenutnih putnika.