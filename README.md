## Feedreader - čítačka feedov vo formáte RSS / Atom

---

### Popis

Program slúži k stručnému výpisu obsahov jednotlivých noviniek z feedu zadaného adresou URL.
Podporuje zabezpečenú aj nezabezpečenú komunikáciu (SSL, TLS).
Pre spracovanie viacerých URL slúži špeciálny súbor **_feedfile_**, ktorý môže užívateľ vytvoriť a zadať do programu.
Užívateľ má možnosť poskytnúť vlastné certifikáty, pomocou ktorých sa overuje dôveryhodnosť serveru.

### Spustenie

```cpp
./feedreader <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u] [-h]
```

#### Možnosti

`-h`

> zobrazí pomocný manuál a ukončí program

`-f <feedfile>`

> definuje cestu k súboru s odkazmi na stránky s RSS/Atom feedom

`-c <certfile>`

> Definuje súbor s certifikátmi, ktorý sa použije pre overenie platnosti certifikátu SSL/TLS predloženého serverom

`-C <certaddr> `

> Určuje adresár, v ktorom sa majú vyhľadávať certifikáty, ktoré sa použijú pre overenie platnosti certifikátu SSL/TLS predloženého serverom

`-T`

> pre každý záznam zobrazí informácie o čase zmeny záznamu, či vytvorenia záznamu

`-a`

> pre každý záznam zobrazí meno autora, či jeho e-mailovoú adresu

`-u`

> pre každý záznam zobrazí asociované URL

#### Príklady spustenia

```
./feedreader -h

./feedreader https://www.fit.vut.cz/fit/news-rss/ -C certdirectory -T

./feedreader https://www.fit.vut.cz/fit/news-rss/ -c certfile.pem -a -u

./feedreader https://www.fit.vut.cz/fit/news-rss/ -Tau

./feedreader -f /cesta_k_feedfile/feedfile -a -uT
```

### Obsluha Makefile

| Príkaz          | Dopad                                   |
| --------------- | --------------------------------------- |
| `make`          | Vytvorí spustiteľný súbor feedreader    |
| `make all`      | Rovnaký ako `make`                      |
| `make clean`    | Odstráni feedreader a nadbytočné súbory |
| `make test`     | Spustí testy                            |
| `make tar`      | Vytvorí z projektu .tar súbor           |
| `make cleantar` | Vymaže .tar súbory                      |

---

### Formát výpisu

**\* **Hlavný titulok** \*** \
**Titulok vstupu** \
**URL:** `url` \
**Aktualizace:** `dátum aktualizácie` \
**Autor:** `meno autora (email autora)*`

**Titulok vstupu 2** \
`...`

**\* **Hlavný titulok** 2 \*** \
`...`

_\*_ ak je uvedený email, nachádza sa v zátvorke za menom autora, ak nie je uvedený autor, formát je nasledovný: \
Autor: `email autora`
