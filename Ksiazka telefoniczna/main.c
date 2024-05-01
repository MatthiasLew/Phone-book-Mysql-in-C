#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <ctype.h>
#include <windows.h>
// Funkcja s³u¿¹ca do logowania w strukturze MYSQL
void logowanie(MYSQL** con)
{
// U¿ytkownik musi podac haslo i login by zalogoac sie do srodowiska MYSQL
    char login[100], haslo[100];
    printf("Witaj, prosze podaj login i haslo\n");
    printf("Login: ");
    scanf_s("%99s", login, (unsigned int)sizeof(login));
    printf("Haslo: ");
    scanf_s("%99s", haslo, (unsigned int)sizeof(haslo));

    *con = mysql_init(NULL);
// Sprawdza czy jest polaczenie z struktura MYSQL
    if (*con == NULL)
    {
        fprintf(stderr, "Blad inicjalizacji: %s\n", mysql_error(*con));
        Sleep(5000);
        exit(1);

    }
// Sprawdza czy wpisane haslo i login jest poprawne
    if (mysql_real_connect(*con, "localhost", login, haslo,
        NULL, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "Blad polaczenia: %s\n", mysql_error(*con));
        mysql_close(*con);
        Sleep(5000);
        exit(1);
    }
    else
    {
        system("cls");
        printf("Udalo sie zalogowac\n");
    }
}
// Funkcja sluzy do sprawdzenia czy baza danych istnieje a jesli nie to ja tworzy
void sprawdzaniebazydanych(MYSQL** con)
{// Tworzy baze jesli nie istnieje
    if (mysql_query(*con, "CREATE DATABASE IF NOT EXISTS KSIAZKATELEFONICZNA"))
    {
        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
        mysql_close(*con);
        Sleep(5000);
        exit(1);
    }
// Informuje uzytkownika czy baza zostala stworzona czy juz istnieje
    printf("Baza danych KSIAZKATELEFONICZNA zostala utworzona lub juz istnieje.\n");
// Wybiera baze danych o nazwie KSIAZKATELEFONICZNA
    if (mysql_select_db(*con, "KSIAZKATELEFONICZNA") != 0)
    {
        fprintf(stderr, "%s\n", mysql_error(*con));
        mysql_close(*con);
        exit(1);
    }
// Wybiera w bazie danych tabele Dane do wczytywania 
    if (mysql_query(*con, "SHOW TABLES LIKE 'Dane'"))
    {
        fprintf(stderr, "%s\n", mysql_error(*con));
        exit(1);
    }
// Umozliwa wyswietlanie informacji ktore wyswietla MYSQL 
    MYSQL_RES* result = mysql_store_result(*con);
    if (result == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(*con));
        exit(1);
    }
// Tworzy tabele w bazie danych
    if (mysql_num_rows(result) == 0)
    {
        char* createtable = "CREATE TABLE Dane(ID int AUTO_INCREMENT PRIMARY KEY,imie varchar(255) not null,nazwisko varchar(255) not null,ulica varchar(255) not null,nr_dom int not null,kod_pocztowy varchar(255) not null,miasto varchar(255) not null,telefon int not null)";
        if (mysql_query(*con, createtable))
        {
            fprintf(stderr, "%s\n", mysql_error(*con));
            Sleep(5000);
            exit(1);
        }
    }
// Informuje czy tabela zostala stworzona lub juz istniela
    printf("Tabela zostala utworzona lub juz istniela\n");
    printf("\n");
}
// Funkcja umozliwa dodanie danych do bazy danych
void dodajdane(MYSQL** con)
{// Sprawdza czy polaczenie z baza dana istnieje
    if (*con == NULL)
    {
        fprintf(stderr, "B³¹d: Po³¹czenie z baz¹ danych nie zosta³o poprawnie zainicjowane.\n");
        Sleep(5000);
        return;
    }
// Wymaga od uzytkownika podanie danych by moc je dodac do bazy
    char imie[100], nazwisko[100], ulica[100], miasto[100], kod_pocztowy[100];
    int nr_dom, telefon;
    printf("Witaj, zaraz dodamy nowe dane do bazy danych\n");
    printf("Imie: ");
    scanf_s("%100s", imie, (unsigned int)sizeof(imie));
    printf("Nazwisko: ");
    scanf_s("%100ss", nazwisko, (unsigned int)sizeof(nazwisko));
    printf("Ulica: ");
    scanf_s("%100ss", ulica, (unsigned int)sizeof(ulica));
    printf("Nr domu/bloku: ");
    scanf_s("%d", &nr_dom);
    printf("Kod pocztowy: ");
    scanf_s("%100ss", kod_pocztowy, (unsigned int)sizeof(kod_pocztowy));
    printf("Miasto: ");
    scanf_s("%100ss", miasto, (unsigned int)sizeof(miasto));
    printf("Telefon: ");
    scanf_s("%d", &telefon);
// Wprowadzanie podanych danych bo bazy MYSQL
    char insert_query[500];
    sprintf_s(insert_query, sizeof(insert_query), "INSERT INTO Dane(imie, nazwisko, ulica, nr_dom, kod_pocztowy, miasto, telefon) VALUES ('%s', '%s', '%s', %d, '%s', '%s', %d)", imie, nazwisko, ulica, nr_dom, kod_pocztowy, miasto, telefon);
    if (mysql_query(*con, insert_query))
    {
        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
        Sleep(5000);
        exit(1);
    }
// Informacja ze dane zostaly dodane
    system("cls");
    printf("Nowe dane zostaly dodane do tabeli Dane.\n");
}
// Funkcja sluzaca do wyswietlania bazy danych 
void wyswietlBazeDanych(MYSQL** con) 
{
    int wybor=0;
    MYSQL_RES* result;
    MYSQL_ROW row;
// Sprawedza czy polaczenie z baza danych istnieje
    if (*con == NULL) 
    {
        fprintf(stderr, "Blad: Obiekt MYSQL nie zostal poprawnie zainicjowany.\n");
        Sleep(5000);
        exit(1);
    }
menu:
    printf("Witaj w wyswietlaniu bazy danych, prosze wybierz, ktore dane chcesz wyswietlic rosnaco lub malejaco:\n");
    printf("1. Imie\n");
    printf("2. Nazwisko\n");
    printf("3. Ulica\n");
    printf("4. Nr domu/bloku\n");
    printf("5. Kod pocztowy\n");
    printf("6. Miasto\n");
    printf("7. Telefon\n");
    printf("8. Przez ID\n");
    printf("Prosze wpisz numer przy opcji, ktora chcesz edytowac: ");
    char opcja[2];
    scanf_s("%2s", opcja, (unsigned int)sizeof(opcja));
// Zaleznosci od wyboru wyswietli sie tablica oraz umozliwia wyswietlenie danych rosnaco lub malejaco
    if (isdigit(opcja[0]))
    {
        wybor = atoi(opcja);
        switch (wybor)
        {
            case 1:
                system("cls");
                printf("Chcesz wyswietlic rosnaco czy malejaco: \n");
                printf("1- Rosnaco           2- Malejaco\n");
                scanf_s("%d", &wybor);
                if (wybor == 1)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY imie ASC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else if (wybor == 2)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY imie DESC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else
                {
                    printf("blad, prosze wybrac 1 lub 2.\n");
                    Sleep(5000);
                    goto menu;
                }
                result = mysql_store_result(*con);
                if (result == NULL) {
                    fprintf(stderr, "Blad pobierania wynikow: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                printf("Wyniki zapytania:\n");
                while ((row = mysql_fetch_row(result)) != NULL) {
                    printf("ID: %s, Imie: %s, Nazwisko: %s, Ulica: %s, Nr domu: %s, Kod pocztowy: %s, Miasto: %s, Telefon: %s\n",
                        row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
                }
                mysql_free_result(result);
                break;
            case 2:
                system("cls");
                printf("Chcesz wyswietlic rosnaco czy malejaco: \n");
                printf("1- Rosnaco           2- Malejaco\n");
                scanf_s("%d", &wybor);
                if (wybor == 1)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY nazwisko ASC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else if (wybor == 2)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY nazwisko DESC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else
                {
                    printf("blad, prosze wybrac 1 lub 2.\n");
                    Sleep(5000);
                    goto menu;
                }
                result = mysql_store_result(*con);
                if (result == NULL) {
                    fprintf(stderr, "Blad pobierania wynikow: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                printf("Wyniki zapytania:\n");
                while ((row = mysql_fetch_row(result)) != NULL) {
                    printf("ID: %s, Imie: %s, Nazwisko: %s, Ulica: %s, Nr domu: %s, Kod pocztowy: %s, Miasto: %s, Telefon: %s\n",
                        row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
                }
                mysql_free_result(result);
                break;
            case 3:
                system("cls");
                printf("Chcesz wyswietlic rosnaco czy malejaco: \n");
                printf("1- Rosnaco           2- Malejaco\n");
                scanf_s("%d", &wybor);
                if (wybor == 1)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY ulica ASC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else if (wybor == 2)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY ulica DESC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else
                {
                    printf("blad, prosze wybrac 1 lub 2.\n");
                    Sleep(5000);
                    goto menu;
                }
                result = mysql_store_result(*con);
                if (result == NULL) {
                    fprintf(stderr, "Blad pobierania wynikow: %s\n", mysql_error(*con));
                    exit(1);
                }
                printf("Wyniki zapytania:\n");
                while ((row = mysql_fetch_row(result)) != NULL) {
                    printf("ID: %s, Imie: %s, Nazwisko: %s, Ulica: %s, Nr domu: %s, Kod pocztowy: %s, Miasto: %s, Telefon: %s\n",
                        row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
                }
                mysql_free_result(result);
                break;
            case 4:
                system("cls");
                printf("Chcesz wyswietlic rosnaco czy malejaco: \n");
                printf("1- Rosnaco           2- Malejaco\n");
                scanf_s("%d", &wybor);
                if (wybor == 1)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY nr_dom ASC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else if (wybor == 2)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY nr_dom DESC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else
                {
                    printf("blad, prosze wybrac 1 lub 2.\n");
                    Sleep(5000);
                    goto menu;
                }
                result = mysql_store_result(*con);
                if (result == NULL) {
                    fprintf(stderr, "Blad pobierania wynikow: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                printf("Wyniki zapytania:\n");
                while ((row = mysql_fetch_row(result)) != NULL) {
                    printf("ID: %s, Imie: %s, Nazwisko: %s, Ulica: %s, Nr domu: %s, Kod pocztowy: %s, Miasto: %s, Telefon: %s\n",
                        row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
                }
                mysql_free_result(result);
                break;
            case 5:
                system("cls");
                printf("Chcesz wyswietlic rosnaco czy malejaco: \n");
                printf("1- Rosnaco           2- Malejaco\n");
                scanf_s("%d", &wybor);
                if (wybor == 1)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY kod_pocztowy ASC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else if (wybor == 2)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY kod_pocztowy DESC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else
                {
                    printf("blad, prosze wybrac 1 lub 2.\n");
                    Sleep(5000);
                    goto menu;
                }
                result = mysql_store_result(*con);
                if (result == NULL) {
                    fprintf(stderr, "Blad pobierania wynikow: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                printf("Wyniki zapytania:\n");
                while ((row = mysql_fetch_row(result)) != NULL) {
                    printf("ID: %s, Imie: %s, Nazwisko: %s, Ulica: %s, Nr domu: %s, Kod pocztowy: %s, Miasto: %s, Telefon: %s\n",
                        row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
                }
                mysql_free_result(result);
                break;
            case 6:
                system("cls");
                printf("Chcesz wyswietlic rosnaco czy malejaco: \n");
                printf("1- Rosnaco           2- Malejaco\n");
                scanf_s("%d", &wybor);
                if (wybor == 1)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY miasto ASC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else if (wybor == 2)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY miasto DESC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else
                {
                    printf("blad, prosze wybrac 1 lub 2.\n");
                    Sleep(5000);
                    goto menu;
                }
                result = mysql_store_result(*con);
                if (result == NULL) {
                    fprintf(stderr, "Blad pobierania wynikow: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                printf("Wyniki zapytania:\n");
                while ((row = mysql_fetch_row(result)) != NULL) {
                    printf("ID: %s, Imie: %s, Nazwisko: %s, Ulica: %s, Nr domu: %s, Kod pocztowy: %s, Miasto: %s, Telefon: %s\n",
                        row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
                }
                mysql_free_result(result);
                break;
            case 7:
                system("cls");
                printf("Chcesz wyswietlic rosnaco czy malejaco: \n");
                printf("1- Rosnaco           2- Malejaco\n");
                scanf_s("%d", &wybor);
                if (wybor == 1)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY telefon ASC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else if (wybor == 2)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY telefon DESC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else
                {
                    printf("blad, prosze wybrac 1 lub 2.\n");
                    Sleep(5000);
                    goto menu;
                }
                result = mysql_store_result(*con);
                if (result == NULL) {
                    fprintf(stderr, "Blad pobierania wynikow: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                } 
                printf("Wyniki zapytania:\n");
                while ((row = mysql_fetch_row(result)) != NULL) {
                    printf("ID: %s, Imie: %s, Nazwisko: %s, Ulica: %s, Nr domu: %s, Kod pocztowy: %s, Miasto: %s, Telefon: %s\n",
                        row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
                }
                mysql_free_result(result);
                break;
            case 8:
                system("cls");
                printf("Chcesz wyswietlic rosnaco czy malejaco: \n");
                printf("1- Rosnaco           2- Malejaco\n");
                scanf_s("%d", &wybor);
                if (wybor == 1)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY id ASC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else if (wybor == 2)
                {
                    if (mysql_query(*con, "SELECT * FROM Dane ORDER BY id DESC")) 
                    {
                        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                        Sleep(5000);
                        exit(1);
                    }
                }
                else
                {
                    printf("blad, prosze wybrac 1 lub 2.\n");
                    Sleep(5000);
                    goto menu;
                }
                result = mysql_store_result(*con);
                if (result == NULL) {
                    fprintf(stderr, "Blad pobierania wynikow: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                printf("Wyniki zapytania:\n");
                while ((row = mysql_fetch_row(result)) != NULL) {
                    printf("ID: %s, Imie: %s, Nazwisko: %s, Ulica: %s, Nr domu: %s, Kod pocztowy: %s, Miasto: %s, Telefon: %s\n",
                        row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
                }
                mysql_free_result(result);
                break;
            default:
                printf("Prosze wybrac odpowiedni numer\n");
                goto menu;
        }
    }
    else
    {
        system("cls");
        printf("Nieprawidlowy wybor. Uzyj cyfr. Wybierz ponownie.\n");
    }
// Zapytanie do uzytkownika czy chce jeszcze raz wyswietlic dane
    printf("Chcesz jeszcze wyswietlic dane: \n");
    printf("1 - Tak       2 - Nie\n");
    scanf_s("%2s", opcja, (unsigned int)sizeof(opcja));
    system("cls");
    if (isdigit(opcja[0]))
    {
        wybor = atoi(opcja);
        if (wybor == 1)
        {
            goto menu;
        }
        system("cls");
    }
    else
    {
        system("cls");
        printf("Nieprawidlowy wybor. Uzyj cyfr. Wybierz ponownie.\n");
    }
}
// Funkcja do usuwanie danych 
void usunDane(MYSQL** con) 
{
    char query[200], warunek[500];
    printf("Podaj warunek: (przyklad: imie='Adam')");
    scanf_s("%s", warunek, (unsigned int)sizeof(warunek));
    snprintf(query, sizeof(query), "DELETE FROM Dane WHERE %s", warunek);

    if (mysql_query(*con, query))
    {
        fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
        Sleep(5000);
        exit(1);
    }

    printf("Dane zostaly usuniete z tabeli Dane.\n");
}
// Funkcja do edycji danych w bazie danych
void edycjabazy(MYSQL** con)
{
    int wybor, id, zamiana;
    char zamianac[100], query[255];
start:
    printf("Prosze podaj id, ktore chcesz edytowac\n");
    scanf_s("%d", &id);
menu:
    printf("Witaj w edycji bazy danych, prosze wybierz, ktore dane chcesz edytowac:\n");
    printf("1. Imie\n");
    printf("2. Nazwisko\n");
    printf("3. Ulica\n");
    printf("4. Nr domu/bloku\n");
    printf("5. Kod pocztowy\n");
    printf("6. Miasto\n");
    printf("7. Telefon\n");
    printf("Prosze wpisz numer przy opcji, ktora chcesz edytowac: ");
    char opcja[2];
    scanf_s("%2s", opcja, (unsigned int)sizeof(opcja));

    if (isdigit(opcja[0]))
    {
         wybor = atoi(opcja);
        switch (wybor)
        {
            case 1:
                printf("Wpisz nowa dane, ktora ma zastapic stara: ");
                scanf_s("%s", zamianac, (unsigned int)sizeof(zamianac));
                snprintf(query, sizeof(query), "UPDATE dane SET imie='%s' WHERE id=%d", zamianac, id);
                if (mysql_query(*con, query) != 0)
                {
                    fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                system("cls");
                printf("Udalo sie edytowac dane\n");
                break;
            case 2:
                printf("Wpisz nowa dane, ktora ma zastapic stara: ");
                scanf_s("%s", zamianac, (unsigned int)sizeof(zamianac));
                snprintf(query, sizeof(query), "UPDATE dane SET nazwisko='%s' WHERE id=%d", zamianac, id);
                if (mysql_query(*con, query) != 0)
                {
                    fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                system("cls");
                printf("Udalo sie edytowac dane\n");
                break;
            case 3:
                printf("Wpisz nowa dane, ktora ma zastapic stara: ");
                scanf_s("%s", zamianac, (unsigned int)sizeof(zamianac));
                snprintf(query, sizeof(query), "UPDATE dane SET ulica='%s' WHERE id=%d", zamianac, id);
                if (mysql_query(*con, query) != 0)
                {
                    fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                system("cls");
                printf("Udalo sie edytowac dane\n");
                break;
            case 4:
                printf("Wpisz nowa dane, ktora ma zastapic stara: ");
                scanf_s("%d", &zamiana);
                snprintf(query, sizeof(query), "UPDATE dane SET nr_dom='%d' WHERE id=%d", zamiana, id);
                if (mysql_query(*con, query) != 0)
                {
                    fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                    Sleep(5000);
 
                    exit(1);
                }
                system("cls");
                printf("Udalo sie edytowac dane\n");
                break;
            case 5:
                printf("Wpisz nowa dane, ktora ma zastapic stara: ");
                scanf_s("%s", zamianac, (unsigned int)sizeof(zamianac));
                snprintf(query, sizeof(query), "UPDATE dane SET kod_pocztowy='%s' WHERE id=%d", zamianac, id);
                if (mysql_query(*con, query) != 0)
                {
                    fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                system("cls");
                printf("Udalo sie edytowac dane\n");
                break;
            case 6:
                printf("Wpisz nowa dane, ktora ma zastapic stara: ");
                scanf_s("%s", zamianac, (unsigned int)sizeof(zamianac));
                snprintf(query, sizeof(query), "UPDATE dane SET miasto='%s' WHERE id=%d", zamianac, id);
                if (mysql_query(*con, query) != 0)
                {
                    fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                system("cls");
                printf("Udalo sie edytowac dane\n");
                break;
            case 7:
                printf("Wpisz nowa dane, ktora ma zastapic stara: ");
                scanf_s("%d", &zamiana);
                snprintf(query, sizeof(query), "UPDATE dane SET telefon='%d' WHERE id=%d", zamiana, id);
                if (mysql_query(*con, query) != 0)
                {
                    fprintf(stderr, "Blad zapytania: %s\n", mysql_error(*con));
                    Sleep(5000);
                    exit(1);
                }
                system("cls");
                printf("Udalo sie edytowac dane\n");
                break;
            default:
                printf("Prosze wybrac odpowiedni numer\n");
                goto menu;
        }
    }
    else
    {
        system("cls");
        printf("Nieprawidlowy wybor. Uzyj cyfr. Wybierz ponownie.\n");
    }
// Pyta uzytkownika czy chce jeszcze zedytowac dane
    printf("Chcesz jeszcze edytowac dane: \n");
    printf("1 - Tak       2 - Nie\n");
    scanf_s("%d", &wybor);
    if (wybor == 1)
    {
        goto start;
    }
    system("cls");
}
// funkcja sluzaca do wylogowania i wylaczenia programu
void wyloguj(MYSQL** con)
{
    printf("Wylogowanie i wylaczanie bazy danych");
    fflush(stdout);
    mysql_close(*con);
    Sleep(5000);
    exit(0);
}

int main(int argc, char** argv)
{
    int wybor=0;
    MYSQL* con;
    logowanie(&con);
// Menu umozliwianie wybranie dzialania w bazie danych
    do 
    {
        printf("============= MENU ==============\n");
        printf("|| 1. Sprawdzenie bazy         ||\n");
        printf("|| 2. Dodanie osoby do bazy    ||\n");
        printf("|| 3. Wyswietlenie bazy danych ||\n");
        printf("|| 4. Edycja bazy danych       ||\n");
        printf("|| 5. Usuniecie bazy danych    ||\n");
        printf("|| 6. Wyjscie                  ||\n");
        printf("=================================\n");
        printf("Wybierz opcje: ");
        char opcja[2];
        scanf_s("%2s", opcja, (unsigned int)sizeof(opcja));

        if (isdigit(opcja[0])) 
        {
                wybor = atoi(opcja);
            switch (wybor) 
            {
                case 1:
                    system("cls");
                    printf("Wybrano Sprawdzenie bazy. \n");
                    sprawdzaniebazydanych(&con);
                    break;
                case 2:
                    system("cls");
                    printf("Wybrano Dodanie osoby do bazy.\n");
                    dodajdane(&con);
                    break;
                case 3:
                    system("cls");
                    printf("Wybrano Wyswietlenie bazy danych.\n");
                    wyswietlBazeDanych(&con);
                    break;
                case 4:
                    system("cls");
                    printf("Wybrano Edycja bazy danych.\n");
                    edycjabazy(&con);
                    break;
                case 5:
                    system("cls");
                    printf("Wybrano Usuniecie bazy danych.\n");
                    usunDane(&con);
                    break;
                case 6:
                    system("cls");
                    printf("Wyjscie.\n");
                    wyloguj(&con);
                    break;
                default:
                    system("cls");
                    printf("Nieprawidlowy wybor. Wybierz ponownie.\n");
                    break;
            }
        }
        else 
        {
            system("cls");
            printf("Nieprawidlowy wybor. Uzyj cyfr. Wybierz ponownie.\n");
        }
    } while (wybor != 6);
    return 0;
}