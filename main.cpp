/* Program ma na celu otwarcie plikow i zrobienia analizy statystycznej
   Zapisuje gotowy plik bazadanych.cvs ktory zawiera:
   - sume wszystkich liter alfabetu i ich procentowy udzial  w  tekscie
   - sume wszystkich slow
   - sume wszystkich zdan
*/
#include <iostream>
#include <fstream>
#include <cstring>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct oblsuga_plikow
{
    std::fstream plik;
    std::fstream zapis;
};

struct znaki_spec_type
{
    char znak;
};

struct znaki_spec_type tablica[10] =
{
    {'!'},{'?'},{'.'},{';'},{':'},{'('},{')'},{'"'},{'-'},{','}
};

struct names
{
    char file[255] = "";
    char folder_in[255] = "";
    char folder_out[255] = "";
};

const char FILE_NAME[15] = "bazadanych.csv";
const char FOLDER_IN[7] = "pliki/";
const char FOLDER_OUT[] = "";
const int ALPHABET_LENGHT = 26;
bool menu_choose=true;

using std::string;
using std::endl;

void default_settings(names & settings);
char * new_options(void);
int fileExist(const char* fileName);
int zdania(string uppload_sentence, int tag_char, char symbol);
int slowa(string uppload_sentence, int tag_char);
int litery(string uppload_sentence, int tag_char, char symbol);
void title_menu(void);
void menu(void);

names settings;
int main(int argc, char ** argv)
{
    default_settings(settings);
    while(menu_choose)
    {
        char menu_options;
        title_menu();
        menu();
        std::cin >> menu_options;
        switch(menu_options)
        {
            case '1':
            {
                char buff_rec[10];          //zmienna sluzy do stworzenia jednej tablicy znakow gvotowej do zapisu
                oblsuga_plikow handle;
                dirent *entry;
                DIR *dir;
                string str_name_file;
                char location_file[255] = "";
                if(settings.folder_out[0] == '\0')
                {
                    for (int i = 0; i <sizeof(settings.file); i++)
                    {
                        location_file[i] = settings.file[i];
                    }
                }
                else
                {
                    for (int i = 0; i <sizeof(settings.folder_out); i++)
                    {
                        location_file[i] = settings.folder_out[i];
                    }
                    int j=0;
                    for(int i = 0; i < sizeof(location_file); i++)
                    {
                        if(location_file[i] == '\0')
                        {
                            location_file[i] = settings.file[j];
                            j++;
                        }
                    }
                }
                if(fileExist(location_file))
                {
                    remove(location_file);
                }
                char folder_location[255] = "./";
                int j=0;
                for(int i=0; i<=sizeof(folder_location); i++)
                {
                    if(folder_location[i] == '\0')
                    {
                        folder_location[i] = settings.folder_in[j];
                        j++;
                    }
                }
                dir = opendir(folder_location);              // Przeszukiwanie folderu ./pliki w poszukiwaniu plikow
                while(entry = readdir(dir))                  // zapetlanie az do ostatniegu pliku
                {
                    if(entry->d_name[0] != '.')              //Sprawdza czy znajduja sie ukryte foldery jesli tak to je omija
                    {
                        puts(entry->d_name);
                        str_name_file.assign(entry->d_name); //Zapis nazwy pliku do zmiennej str_name_file

                        handle.plik.open(folder_location + str_name_file);      //Otwieranie pliku //"./pliki/" + str_name_file
                        if (handle.plik.good())                                 // Kontrola wejscia czy powiodlo sie otwarcie pliku
                        {
                            float suma_liter_m[ALPHABET_LENGHT] {0}, suma_liter_d[ALPHABET_LENGHT] {0};
                            unsigned short int suma_zdan {0}, suma_lin {0}, suma_slow {0};
                            unsigned short int suma_wszystkich_liter {0};
                            char litery_m = 'a',litery_d = 'A';
                            string zdanie;
                            while(!handle.plik.eof())                     // Wyrzucanie kazdej lini z pliku az skoncza sie dane
                            {
                                getline(handle.plik, zdanie);            //Przypisywanie kazdej lini z pliku do zmiennej zdanie

                                unsigned int symbol_location;
                                unsigned int result_fun;
                                for (int i=0; i<3; i++)
                                {
                                    symbol_location = zdanie.find(tablica[i].znak);                         //Szukanie w zmiennej zdanie wystapienia znaku "!"
                                    if(symbol_location != string::npos)                                     //Jesli wynikiem wyszukania bedzie string::npos (pusty wynik) program pojdzie dalej
                                        suma_zdan += zdania(zdanie, symbol_location, tablica[i].znak);      //oraz lokalizacje znaku w zmiennej symbol_location i zwraca liczbe wystapien znaku do zmiennej
                                }
                                symbol_location = zdanie.find_last_of(" ");
                                if(symbol_location != string::npos)
                                    suma_slow += slowa(zdanie, symbol_location);

                                litery_m = 'a';
                                for(int i=0; i<ALPHABET_LENGHT; ++i)
                                {
                                    symbol_location = zdanie.find(litery_m);
                                    if(symbol_location != string::npos)
                                    {
                                        result_fun = litery(zdanie, symbol_location, litery_m);
                                        suma_liter_m[i] += result_fun;
                                        suma_wszystkich_liter += result_fun;                                //podliczenie wszystkich liter w pliku do obliczenia procentowego udzialu liter
                                    }
                                        litery_m++;
                                }
                                litery_d = 'A';
                                for(int i=0; i<ALPHABET_LENGHT; ++i)
                                {
                                    symbol_location = zdanie.find(litery_d);
                                    if(symbol_location != string::npos)
                                    {
                                        result_fun = litery(zdanie, symbol_location, litery_d);
                                        suma_liter_d[i] += result_fun;
                                        suma_wszystkich_liter += result_fun;
                                    }
                                    litery_d++;
                                }
                                suma_lin++;
                            }
                            handle.plik.close();                                                    //zamkniecie zmiennej plik
                            handle.zapis.open(location_file, std::ios::app | std::ios::out);        //otwarcie i utworzenie pliku bazadanych.csv z mozliwoscia nadpisywania pliku
                            handle.zapis << str_name_file << endl;                                  //zapisanie do bazadanych.csv nazwy ostatnio oblsugiwanego piku

                            sprintf(buff_rec, "zdania\t%d", suma_zdan);                             //sprintf nie wyswietla zmiennychm jak printf w konsoli tylko zapisuje wynik do zmiennej buff_rec
                            handle.zapis << buff_rec << endl;                                       //zapis zmiennej buff_rec do pliku
                            sprintf(buff_rec, "slowa\t%d", suma_slow);
                            handle.zapis << buff_rec << endl;
                            sprintf(buff_rec, "linie\t%d", suma_lin);
                            handle.zapis << buff_rec << endl;

                            float procent[ALPHABET_LENGHT];
                            litery_m = 'a';
                            for(int i=0; i<ALPHABET_LENGHT; i++)
                            {
                                procent[i] = (suma_liter_m[i] / suma_wszystkich_liter)*100;                            //Obliczanie jaki procent z wszystkich liter stanowi jedna z liter
                                sprintf(buff_rec, "%c\t%.*f\t  %.*f%%",litery_m, 0, suma_liter_m[i], 1, procent[i]);   //Zapis do zmiennej buuf_rec
                                handle.zapis << buff_rec << endl;                                                      //Zapis do pliku
                                litery_m++;
                            }
                            litery_d = 'A';
                            for(int i=0; i<ALPHABET_LENGHT; i++)
                            {
                                procent[i] = (suma_liter_d[i] / suma_wszystkich_liter)*100;
                                sprintf(buff_rec, "%c\t%.*f\t  %.*f%%",litery_d, 0, suma_liter_d[i], 1, procent[i]);
                                handle.zapis << buff_rec << endl;
                                litery_d++;
                            }
                            handle.zapis.close();
                            std::cout << "Plik jest gotowy!" << endl;
                        }else
                        {
                            std::cout << "Nie udalo sie otworzyc pliku!" << endl;
                        }
                    }
                } 
                closedir(dir);
                getchar();
                break;
            }

            case '2':
            {
                std::cout << "Podaj nazwe pliku: " << endl;
                for(int i=0; i<sizeof(settings.file); i++)
                {
                    settings.file[i] = {0};
                }
                std::cin >> settings.file;
                for(int i=0; i<sizeof(settings.file); i++)
                {
                    if(settings.file[i] == '\0')
                    {
                        settings.file[i] = '.';
                        settings.file[i+1] = 'c';
                        settings.file[i+2] = 's';
                        settings.file[i+3] = 'v';
                        break;
                    }
                }
                break;
            }

            case '3':
            {
                char * name_folder;
                std::cout << "Podaj nazwe folderu: ";
                name_folder = new_options();
                if(fileExist(name_folder))
                {
                    std::cout << "Wykonano Pomyslnie!\n";
                    for(int i=0; i<sizeof(settings.folder_in); i++)
                    {
                        settings.folder_in[i] = {0};
                    }
                    for(int i=0; i<=sizeof(name_folder); i++)
                    {
                        settings.folder_in[i] = name_folder[i];
                    }
                    for(int i=0; i<sizeof(settings.folder_in); i++)
                    {
                        if(settings.folder_in[i] == '\0')
                        {
                            settings.folder_in[i] = '/';
                            break;
                        }
                    }
                }
                else
                {
                    int break_while=1;
                    std::cout << "Folder nie istnieje!\n";
                    while(break_while)
                    {
                        std::cout << "Czy chcesz go utworzyć? (Y/N):";
                        char decision;
                        std::cin >> decision;
                        switch(decision)
                        {
                            case 'Y':
                            case 'y':
                            {
                                std::cout << "Utworzono nowy folder!\n";
                                char create_folder[255] = "mkdir ";
                                int j=0;
                                for(int i=0; i<=sizeof(create_folder); i++)
                                {
                                    if(create_folder[i] == '\0')
                                    {
                                        create_folder[i] = name_folder[j];
                                        j++;
                                    }
                                }
                                system(create_folder);
                                for(int i=0; i<sizeof(settings.folder_in); i++)
                                {
                                    settings.folder_in[i] = {0};
                                }
                                for(int i=0; i<=sizeof(name_folder); i++)
                                {
                                    settings.folder_in[i] = name_folder[i];
                                }
                                for(int i=0; i<sizeof(settings.folder_in); i++)
                                {
                                    if(settings.folder_in[i] == '\0')
                                    {
                                        settings.folder_in[i] = '/';
                                        break;
                                    }
                                }
                                break_while=0;
                                break;
                            }
                            case 'N':
                            case 'n':
                            {
                                std::cout << "Nie utowrzono folderu!\n";
                                break_while=0;
                                break;
                            }
                            default:
                            {
                                std::cin.clear();
                                std::cin.ignore();
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case '4':
            {
                char * name_folder;
                std::cout << "Podaj nazwe folderu: ";
                name_folder = new_options();
                if(fileExist(name_folder))
                {
                    std::cout << "Wykonano Pomyslnie!\n";
                    for(int i=0; i<sizeof(settings.folder_out); i++)
                    {
                        settings.folder_out[i] = {0};
                    }
                    for(int i=0; i<=sizeof(name_folder); i++)
                    {
                        settings.folder_out[i] = name_folder[i];
                    }
                    for(int i=0; i<sizeof(settings.folder_out); i++)
                    {
                        if(settings.folder_out[i] == '\0')
                        {
                            settings.folder_out[i] = '/';
                            break;
                        }
                    }
                }
                else
                {
                    int break_while=1;
                    std::cout << "Folder nie istnieje!\n";
                    while(break_while)
                    {
                        std::cout << "Czy chcesz go utworzyć? (Y/N):";
                        char decision;
                        std::cin >> decision;
                        switch(decision)
                        {
                            case 'Y':
                            case 'y':
                            {
                                std::cout << "Utworzono nowy folder!\n";
                                char create_folder[255] = "mkdir ";
                                int j=0;
                                for(int i=0; i<=sizeof(create_folder); i++)
                                {
                                    if(create_folder[i] == '\0')
                                    {
                                        create_folder[i] = name_folder[j];
                                        j++;
                                    }
                                }
                                system(create_folder);
                                for(int i=0; i<sizeof(settings.folder_out); i++)
                                {
                                    settings.folder_out[i] = {0};
                                }
                                for(int i=0; i<=sizeof(name_folder); i++)
                                {
                                    settings.folder_out[i] = name_folder[i];
                                }
                                for(int i=0; i<sizeof(settings.folder_out); i++)
                                {
                                    if(settings.folder_out[i] == '\0')
                                    {
                                        settings.folder_out[i] = '/';
                                        break;
                                    }
                                }
                                break_while=0;
                                break;
                            }
                            case 'N':
                            case 'n':
                            {
                                std::cout << "Nie utowrzono folderu!\n";
                                break_while=0;
                                break;
                            }
                            default:
                            {
                                std::cin.clear();
                                std::cin.ignore();
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case '5':
            {
                default_settings(settings);
                break;
            }

            case '6':
            {
                menu_choose = false;
                break;
            }
        }
    }
        return 0;
}

int zdania(string uppload_sentence, int tag_char, char symbol)
{
    int w=0;
    while(tag_char != string::npos)
    {
        uppload_sentence.erase(tag_char,1);         //funkcja przyjmuje zdanie i lokalizacje znaku
        w++;                                        //po kazdym znalezieniu znaku funkcja usuwa znak z tego miejsca i inkrementuje zmienna w
        tag_char = uppload_sentence.find(symbol);   //funkcja zakonczy prace kiedy nie znajdzie juz szukanego znaku
    }
    return w;
}
int slowa(string uppload_sentence, int tag_char)
{
    int s=0, p=0;
    while(tag_char != string::npos)                                                           //funkcja przyjmuje zdanie i lokalizacje znaku
    {                                                                                         //po kazdym znalezieniu znaku funkcja usuwa wszystko co wystepuje od lokalizacji wystapienia symbolu do konca lancucha
        uppload_sentence.erase(uppload_sentence.begin()+tag_char, uppload_sentence.end());    //funkcja zakonczy prace kiedy nie znajdzie juz szukanego znaku
        s++;                                                                                  //Na koncu zawsze zostaje jedno slowo dla tego inkrementuje zmienna s++
        tag_char = uppload_sentence.find_last_of(" ");                                        //Jest rowniez kontrola czy aby na pewno string nie jest pusty
    }
    p = sizeof(uppload_sentence);
    if(p > 0)
    {
        s++;
    }
    return s;
}
int litery(string uppload_sentence, int tag_char, char symbol)
{
    int l = 0;
    while(tag_char != string::npos)          //funkcja przyjmuje zdanie i lokalizacje znaku
    {                                        //po kazdym znalezieniu znaku funkcja usuwa znak z tego miejsca i inkrementuje zmienna p
        uppload_sentence.erase(tag_char,1);  //funkcja zakonczy prace kiedy nie znajdzie juz szukanego znaku
        l++;
        tag_char = uppload_sentence.find(symbol);
    }
    return l;
}
int fileExist (const char* fileName)
{
    FILE* plik;
    plik = fopen(fileName, "r");
    if (plik)
    {
        fclose(plik);
        return 1;
    }
    return 0;
}
void default_settings(names & settings)
{
    for(int i=0; i<sizeof(settings.file); i++)
    {
        settings.file[i] = {0};
    }
    for(int i=0; i<=sizeof(settings.file); i++)
    {
        settings.file[i] = FILE_NAME[i];
    }
    for(int i=0; i<sizeof(settings.folder_in); i++)
    {
        settings.folder_in[i] = {0};
    }
    for(int i=0; i<=sizeof(settings.file); i++)
    {
        settings.folder_in[i] = FOLDER_IN[i];
    }
    for(int i=0; i<sizeof(settings.folder_out); i++)
    {
        settings.folder_out[i] = {0};
    }
    for(int i=0; i<=sizeof(settings.folder_out); i++)
    {
        settings.folder_out[i] = FOLDER_OUT[i];
    }
}
char * new_options(void)
{
    char temp[255];
    std::cin >> temp;
    char * ni = new char[sizeof(temp) +1];
    strcpy(ni, temp);

    return ni;
}
void title_menu(void)
{
    std::cout <<"***************************************************\n"
              <<"***************************************************\n"
              <<"**-----------------------------------------------**\n"
              <<"**--------######-------######----##--------##----**\n"
              <<"**------##-----------##-----------##------##-----**\n"
              <<"**------##-------------##----------##----##------**\n"
              <<"**------##----------------##--------##--##-------**\n"
              <<"**------##---------------##----------####--------**\n"
              <<"**--------#####-----#####-------------##---------**\n"
              <<"**-----------------------------------------------**\n"
              <<"***************************************************\n"
              <<"***************************************************\n";
}
void menu(void)
{
    std::cout << "1. Start Programu.\n" 
              << "2. Ustawianie nazwy pliku wykonanego.\n"
              << "3. Wybor folderu do wczytania pliku/plikow.\n" 
              << "4. Wybor folderu do zapisu pliku.\n"
              << "5. Ustawienia domyslne\n" 
              << "6. Koniec programu.\n";
}