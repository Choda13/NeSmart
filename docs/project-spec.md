# 2D specifikacije 2026/2027

## Opšte informacije (Odnosi se i na samostalne teme\!\!)

Odbrana 2D projekta biće realizovana u terminima vežbi u sedmici 16.11-20.11. Svaki student dolazi na odbranu u odgovarajućem terminu po grupi kojoj pripada, osim ako nije naglasio asistentu na vežbama da se zamenio sa drugim studentom.

Projekat treba da bude napisan u jeziku C++ sa osloncem na biblioteku OpenGL.  
**OpenGL verzija mora biti najmanje 3 i mora se koristiti programabilni pajplajn\!\! Projekti urađeni u fiksnom pajplajnu neće biti ocenjivani\!\!**

Projekat mora da sadrži sledeće tehnike pređene na vežbama: programiranje u šejderima, uniforme, providne boje i/ili teksture, unos s miša i/ili tastature i kursor sa nalepljenom slikom (za samostalne teme, kursor treba da nosi izgled nečega što je povezano sa temom, npr. kocka leda za frižider). Svako pisanje teksta ili brojeva se može realizovati teksturama ili korišćenjem biblioteke FreeType. Na svakom mestu gde je potrebno implementirati kretanje konstantnom brzinom, nije greška implementirati ubrzano kretanje, ako se studentu više dopadne (obrnuto ne važi\!\!).

**Projekat mora sadržati frame limiter sa 75 FPS, prekrivati ceo ekran (*full screen*) i u svakom trenutku mora biti moguće isključivanje na taster Escape. U proizvoljnom uglu ekrana potrebno je nalepiti poluprovidnu teksturu na kojoj je napisano ime, prezime i indeks studenta.**

Studenti koji su smislili samostalne teme potrebno je da kontaktiraju asistenta mejlom (ako već nisu) na [vasilijem@uns.ac.rs](mailto:vasilijem@uns.ac.rs) kako bi teme bile odobrene. Na [linku](https://docs.google.com/spreadsheets/d/1bkQsxFHKnOTYth4ZEpYlxrmebQHCfMnx5_37-du3srw/edit?usp=sharing) su prikazani studenti koji imaju samostalne teme, zajedno sa nazivom teme.

2D Projekat je potrebno uraditi do termina odbrane i položen je ako je ostvareno \>50% bodova.

U daljem tekstu navedeno je 10 predefinisanih tema. Svakom studentu je dodeljena tema obeležena cifrom kojom se završava broj studentovog indeksa ne računajući godinu upisa (npr. studentu SV 15/2021 bi bila zadata tema broj 5).

**Studenti koji brane projekat na računarima fakulteta moraju poslati radove u .zip formatu ili kao github linkove na imejl-adresu <vasilijem@uns.ac.rs>\!**

## 6 Autobus

Napraviti 2D grafičku aplikaciju za praćenje autobuske rute.

~~Na ekranu je prikazana putanja kojom se kreće autobus. Sačinjena je od krivih crvenih linija koje se nalaze između stanica i koje formiraju **nepravilan** oblik, koje su obeležene kao crveni krugovi sa belim brojem unutra. Stanica ima 10 i obeležene su od 0 do 9\.~~ ~~Putanja je zatvorena, odnosno, osim svake dve susedne, stanica 0 je takođe povezana sa stanicom 9\.~~

~~Autobus je prikazan sličicom autobusa i kreće se po putanji stanicama redom od 0 do 9. Autobus se od stanice do stanice kreće konstantnom brzinom. Kada dođe do stanice, on se zaustavi. Potrebno je na proizvoljnom uglu ekrana prikazati ikonicu vrata koja su zatvorena dok se autobus vozi, a otvorena su 10 sekundi dok je on na stanici (za ovo vreme se ikonica autobusa takođe zadrži na stanici).~~

~~U drugom proizvoljnom uglu ekrana pamti se broj putnika u autobusu, koji je na početku 0. Dok su vrata autobusa otvorena na stanici, levim klikom miša na ekran ulazi po jedan putnik, odnosno broj se povećava za 1; dok desnim klikom miša na ekran izlazi po jedan putnik, odnosno broj se smanji za 1. Opseg broja putnika se kreće od 0 do 50\.~~

~~Dok su vrata autobusa otvorena, pritiskom na taster K u autobus ulazi kontrola. Broj putnika se tada poveća za 1, i trećem proizvoljnom uglu ekrana se prikazuje oznaka kontrole (npr. slika saobraćajca u Monopolu) sve dok autobus ne dođe do sledeće stanice, kad kontrola izlazi i broj putnika se smanji za 1\. Na stanici na kojoj kontrola izlazi se generiše random vrednost između `0` i `broja putnika - 1` kojima je naplaćena kazna (- 1 jer je jedan od putnika kontrola) i sabira se sa ukupnim brojem naplaćenih kazni. Ovaj broj je inicijalno 0 i potrebno ga je uvek prikazivati pored broja trenutnih putnika. Moguće je da druga kontrola uđe na stanici na kojoj je prethodna kontrola izašla.~~

~~Kursor treba da ima izgled žutog rombastog saobraćajnog znaka (kao BUS STOP znak)  tako da je sam znak u gornjem levom uglu, a metalna šipka dijagonala slike.~~

## Scratchpad

- Autobus je prikazan sličicom autobusa i kreće se po putanji stanicama redom od 0 do 9. 
- Autobus se od stanice do stanice kreće konstantnom brzinom. Kada dođe do stanice, on se zadrzava na njoj 10 sekundi. Nakon 10 sekundi nastavlja kretanje na sledecu stanicu
- Potrebno je na proizvoljnom uglu ekrana prikazati ikonicu vrata koja su zatvorena dok se autobus vozi, a otvorena su dok je on na stanici.
- U drugom proizvoljnom uglu ekrana pamti se broj putnika u autobusu, koji je na početku 0.
- Dok su vrata autobusa otvorena na stanici, levim klikom miša na ekran ulazi po jedan putnik, odnosno broj se povećava za 1, dok desnim klikom miša na ekran izlazi po jedan putnik, odnosno broj se smanji za 1.
- Opseg broja putnika se kreće od 0 do 50.
- Dok su vrata autobusa otvorena, pritiskom na taster K u autobus ulazi kontrola. Broj putnika se tada poveća za 1, i trećem proizvoljnom uglu ekrana se prikazuje oznaka kontrole (npr. slika saobraćajca u Monopolu) sve dok autobus ne dođe do sledeće stanice, kad kontrola izlazi i broj putnika se smanji za 1. Moguće je da druga kontrola uđe na stanici na kojoj je prethodna kontrola izašla.
- Na stanici na kojoj kontrola izlazi se generiše random vrednost između `0` i `broja putnika - 1` kojima je naplaćena kazna (- 1 jer je jedan od putnika kontrola) i sabira se sa ukupnim brojem naplaćenih kazni. Ovaj broj je inicijalno 0 i potrebno ga je uvek prikazivati pored broja trenutnih putnika.
- Kursor treba da ima izgled žutog rombastog saobraćajnog znaka (kao BUS STOP znak)  tako da je sam znak u gornjem levom uglu, a metalna šipka dijagonala slike.