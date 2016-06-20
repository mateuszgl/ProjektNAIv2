# ProjektNAIv2

Projekt na zaliczenie przedmiotu NAI.

Gra z zastosowaniem algorytmu A*. Klikajac wybiera się cel do którego zmierza postać,
droga którą idze postać obliczna jest przez A*. Celem gry jest odnalezienie skarbu.

Mapa tworzona jest z pliku .bmp o rozdzielczości 20x15 pikseli, użytkownik sam może stworzyć swoją własną.
Różne elementy na mapie zaznaczone są następującymi kolorami:
ściany - czarny (0,0,0),
podłoga - biały (255,255,255),
drzwi - czerwony (255,0,0),
klucz - zielony (0,255,0),
skarb - niebieski (0,0,255),
początkowa pozycja postaci - morski (0,255,255).

Aby móc przejść przez drzwi trzeba odnaleść klucz, po odnalezieniu skarbu gra automatycznie się zamknie.
