Filtrowanie obrazów 

Jedną z najprostszych operacji jaką można wykonać na obrazie jest operacja filtrowania (splotu). Operacja ta przyjmuje na wejściu dwie macierze:

Macierz I reprezentującą obraz. Dla uproszczenia rozważamy jedynie obrazy w 256 odcieniach szarości. Każdy element macierzy I jest więc liczbą całkowitą z zakresu 0 do 255.
Macierz K reprezentującą filtr. Elementami tej macierzy są liczby zmiennoprzecinkowe. Dla uproszczenia zakładamy, że elementy macierzy K sumują się do jedności.


Operacja filtrowania tworzy nowy obraz J, którego piksele mają wartość:

J[x][y]=round(S[x][y])

S[x][y] = ∑i ∑j I[max{1,x−ceil(c/2)+i}][max{1,y−ceil(c/2)+j] ∗ K[i][j].

Operacja  round()  oznacza zaokrąglenie do najbliższej liczby całkowitej a ceil() zaokrąglenie w górę do najbliższej liczby całkowitej.

Napisz program, który wykonuje wielowątkową operację filtrowania obrazu. Program przyjmuje w argumentach wywołania:
 - liczbę wątków,
 - nazwę pliku z wejściowym obrazem,
 - nazwę pliku z definicją filtru,
 - nazwę pliku wynikowego.
 
Po wczytaniu danych (wejściowy obraz i definicja filtru) wątek główny tworzy tyle nowych wątków, ile zażądano w argumencie wywołania. Utworzone wątki równolegle generują wyjściowy (filtrowany) obraz. 

Każdy wątek odpowiada za wygenerowanie fragmentu wyniku. 
Wątek główny czeka na zakończenie pracy przez wątki wykonujące operację filtrowania. 
Następnie zapisuje powstały obraz do pliku wynikowego.

Format wejścia-wyjścia
Program powinien odczytywać i zapisywać obrazy w formacie ASCII PGM (Portable Gray Map). Pliki w tym formacie mają nagłówek postaci:

P2
W H
M
...

gdzie: W to szerokość obrazu w pikselach, H to wysokość obrazu w pikselach a M to maksymalna wartość piksela. Zakładamy, że obsługujemy jedynie obrazy w 256 odcieniach szarości: od 0 do 255 (a więc M=255). Po nagłówku, w pliku powinno być zapisanych W*H liczb całkowitych reprezentujących wartości kolejnych pikseli. Liczby rozdzielone są białymi znakami (np. spacją). Piksele odczytywane są wierszami, w kolejności od lewego górnego do prawego dolnego rogu obrazu.

Przykładowe obrazy w formacie ASCII PGM (jak również opis formatu) można znaleźć pod adresem: http://people.sc.fsu.edu/~jburkardt/data/pgma/pgma.html

W pierwszej linii pliku z definicją filtru powinna znajdować się liczba całkowita c określająca rozmiar filtru. Dalej, plik powinien zawierać c2 liczb zmiennoprzecinkowych określających wartości elementów filtru (w kolejności wierszy, od elementu K[1,1] do elementu K[c,c]).
