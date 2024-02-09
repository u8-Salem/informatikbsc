# Assignment 6

## Aufgabe 1

Das nur an der Sytanx nicht aber an der Semantik der Funktion gearbeitet wird. Daher sie tut danch exakt das gleiche wie vorher.
Prüfbar mit unit tests. 

## Aufgabe 2

- Push-down: Funktion einer super Klasse wird an Kinder weiter "nach unten gedrückt"
- Pull-up: Funktionen die in Kinder Klassen bestehen werden in die super Klasse "rauf gezogen".

## Aufgabe 3

1. zu ungenaue Abkürzungen für Funktionsparameter und Funktionsnamen.
2. Main Funktion gehört nicht in die Student klasse

## Aufgabe 4

### a

Kohäsion und Kopplung sind allg. Entwurfsregeln für Software. 
Kohäsion beschreibt die innere "verbundenheit/abhängigkeit" von klassen. Kohäsion wird besser mit logischer Ähnlichkeit, zeitliche Zusammengehörigkeit, sequentielle Folge, Gruppierung im wichtige Funktionen und Daten. Kohäsion soll möglichst hoch sein.  
Kopplung beschreibt Abhängigkeit zwischen Klassen. Sie wird besser mit indem Beeinflussung lokaler Daten oder Kontrollfluss Anderer und globale Daten minimiert werden. Kopplung soll möglichst gering sein.

### b

hohes Maß. wenn ähnliche Funktionen gruppiert sind gibt es nur einen Ort an dem diese zu finden sind. Werden nun neue hinzugefügt, fällt besser auf ob diese bereits existieren. dies reduziert code Dopplung

### c

niedriges Maß. geringer Abhängigkeiten zwischen Klassen reduzieren Komplexität.

### d

1. Anwendungsebene: Schnittstelle zwischen User und Geschäftslogik. Beispielsweise eine grafische Oberfläche
2. Geschäftslogik: Logik zur Transformation der Daten der 3. Ebene
3. Datenbanken Infrastruktur: Verwalten von Userdaten und Geschäftsdaten. 

### e

1. Durch die Teilung sind die einzelnen Ebenen leichter zu warten und "auszutauschen"
2. Einfachere Aufteile auf mehrere Teams. 