# ***Flash Sim***

* Oborový projekt - Simulátor Flash paměti.

## Popis implementace

* Jádro programu je třída Flash_Memory, která představuje paměť typu FLASH (NAND, NOR).
Nad pamětí je možné provádět dva typy operací (COMMAND), a to funkční (řídící), jak například
čtení, zápis stránky nebo vyresetování paměti, a statistické jako čas podledního čtení.

### Sector

* Minimální datová jednotka. Je dána napevno na 512 bytů.

### Stránka

* Minimální jednotka pro zápis. Její velikost může být změněna při spuštení simulace. Základní
hodnota je 512 bytů.

#### Spare Data

* Jako velikost spare dat jedné stránky bylo zvoleno 24 bitů, tady 3 Byty.
  * 16 bitů pro ECC - zabezpečení dat stránky.
  * 8 bitů pro bitové data 
    * BAD bit - určuje, zda je stránka poškozená, tedy zdaj jde vůbec použít
    * VALID bit - určuje, zda jsou data stránky validní nebo zda je dostupná pro zápis
    * ERROR bit - určuje, zda došlo při programování nebo mazání k chybě (zatím není implementováno)
    * LOCKED bit - určuje, zda je stránka uzamčená, pokud je uzamčená, nemůže být smazána nebo naprogramována (zatím není implementováno)
    * SECURITY bit - určuje, zda jsou data ve stránce šifrována nebo jinak zabezpečena (zatím není implementováno)

| Číslo bitu | 0   | 1     | 2     | 3      | 4        | 5 | 6 | 7 |
|------------|-----|-------|-------|--------|----------|---|---|---|
| Význam     | BAD | VALID | ERROR | LOCKED | SECURITY | - | - | - |

### Blok

* Minimální mazací jednotka. Obsahuje několik stránek. Její velikost může být změněna při
spuštění simulace.

### Status registr

| Číslo bitu | 0   | 1     | 2   | 3  | 4  | 5  | 6   | 7 |
|------------|-----|-------|-----|----|----|----|-----|---|
| Význam     | WP | BP | EPE | RB | ES | PT | ECC | - |

#### Význam jednotlivých bitů

* WP - Write Protect: Indikuje ochranu paměti před zápisem.
* BP - Block Protect: Indikuje ochranu daného bloku před zápisem.
* EPE - Erase Program Error: Indikuje chybu při poslední operaci čtení nebo zápis.
* RB - Ready/Busy: Indikuje, že pamět právě provádí operaci nebo že je připravena k použití.
* ES - Erase Suspend: Indikuje pozastavení možnosti mazání bloku např. pokud má paměť malý přísun energie.
* PT - Programming Time-out: Indikuje timeout u poslední operace zápisu.
* ECC - ECC/Checksum: Indikuje chybu ECC nebo checksumy.

### ECC

Error Correction Code - slouží k detekci chyby stránky nebo bloku.

### Adresování

* S adresou je pracováno jako s 16-bitovým celým číslem ve formátu: 0.-7. bit - adresa stránky, 8.-15. bit - adresa bloku.

### Ukládání stavu paměti

* Ukládání je implementováno pomocí zápisu do souboru typu JSON.
* K tomu je využívána knihovna nlohmann/json.hpp viz [zde](https://github.com/nlohmann/json). Jedná se o hojně 
  užívanou knihovnu jazyka C++ pro práci s JSON soubory. Knihovna slouží k volnému použití, proto může být zvolena
  pro tuto aplikaci.

### Funkčí příkazy


### Statistické příkazy


### TODO
*[x] Zápis a čtení dat do souboru.
*[x] Status registr.
*[ ] Ukládání a čtení cache před a po save_state.
*[ ] Logika ECC a ostatních spare dat.
*[ ] Histogram na všech úrovních.
*[ ] Přidat jednotlivé unit testy a function testy.
*[ ] Přidat tutorial na přidání unittestů.

https://www.youtube.com/watch?v=6dqFqh3UbPY
