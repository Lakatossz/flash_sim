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

### Blok

* Minimální mazací jednotka. Obsahuje několik stránek. Její velikost může být změněna při
spuštění simulace.

### ECC

Error Correction Code - slouží k detekci chyby stránky nebo bloku.

### Adresování

* S adresou je pracováno jako s 16-bitovým celým číslem ve formátu: 0.-7. bit - adresa stránky, 8.-15. bit - adresa bloku.

### Funkčí příkazy


### Statistické příkazy

### TODO
* Přidat tutorial na přidání unittestů.

https://www.youtube.com/watch?v=6dqFqh3UbPY
