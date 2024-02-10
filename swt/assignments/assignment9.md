# Assignment 9

## Aufgabe 1 

*kein bock gehabt mir die ganzen konventionen durchzulese, deswegen wahrscheinlich nicht 'richtig'  
habe hier nur die Indentation geändert*  
### a

```java
static sort <E extends Comparable<? super E>> void shellSort(E[] sammlung, int[]x ) {
    for (int SCHRITTWEITE : x) {    // straight insertion mit Schrittweite
        for (int index = SCHRITTWEITE; index < sammlung.length; index++) {
            E eze = sammlung[index];
            int platzHalter = index;
            
            while (platzHalter - SCHRITTWEITE >= 0 
                && eze.compareTo(sammlung[SCHRITTWEITE]) < 0) {
                    sammlung[platzHalter] = sammlung[platzHalter - SCHRITTWEITE]; 
                    platzhalter -= SCHRITTWEITE;    // Sprung um Schrittweite
                }
                
                sammlung[platzHalter] = eze;
        }
    }
}

```

### b

allg. ist variable naming schlimm. SCHRITTWEITE ist keine konstante. eze ist zu ungenau  im vergleich zu platzHalter und sammlung. Mix aus deutsch und englisch in comments (deutsch sollte im allg. eh vermieden werden). Indentation war an vielen stellen dramatisch schlecht. bspw. zeile 4 - 6. öffnende curly braces sollten nach der schlißeneden runden klammer kommen. (zeile 7). Niemals linebreak wie in zeile 10. 
nach schließender curly brace kommt line break. (zeile 17-18)

### c

sieh oben  
*wie gesagt habe nicht wirklich die konvention gelesen, kann also noch falsch sein. habe auch nicht die identifier geändert.*

## Aufgabe 2 

### a

Continuous Integration (CI) bedeutet, dass Code Änderungen regelmäßig und häufig in ein zentrales Repository übernommen werden (merged). Danach werden automatisch Tests und Builds ausgeführt, sodass bei jedem merge direkt geprüft werden kann, ob bestimmte Funktionen nicht mehr richtig funktionieren, daher ob die Änderungen etwas 'kaputt' gemacht haben. Git 'verpackt' Ändrungen in einzelne commits, welche dann in das zentrale Repo gemerged werden können. Wenn Fehler auftreten können einzelne commits näher untersucht werden. Es ist auch möglich jederzeit einen früheren Stand herzustellen und so zu identifizieren wo ein Problem das erste mal aufgetreten ist. 

### b

trivial 

### c

trivial