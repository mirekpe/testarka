//--------------------------testarka-----ver2.2--(pozwala-debugowac-i-dziala-tez-na-linuksie-i-macu)------

// Skopiuj testarka.cpp do folderu swojego programu i podlacz przez dodanie w programie w ostatniej linii:
//   #include "testarka.cpp"
// Uwaga !: zmienne deklarowane przed main() nalezy ew. wyzerowac w main(), np., fill(tabl,tabl+rozmiar);
//  poniewaz testarka wywoluje main() wielokrotnie, te zmienne sa wyzerowane tylko za pierwszym razem
// Uwaga 2: testarka wywoluje:  ios_base::sync_with_stdio(false);  - aby przyspieszyc dzialanie,
//  jezeli sam nie umiescisz tej funkcji w main(), po wylaczeniu testarki program moze znacznie spowolnic
// Uwaga 3: nie zapomnij wylaczyc testarki w gotowym programie (skasuj: #include "testarka.cpp") 
//  i sprawdz, czy program kompiluje sie bez niej

// Testarka wywola funkcje main dla wszystkich plikow *.in w foderze programu (ew. w podfolderach in out)
//  Pliki *.in (wejsciowe) i *.out (do porownania) przygotuj sam, albo pobierz ze strony olimpiady
//  i wrzuc do folderu z progamem. Powinny byc w parach o takiej samej nazwie przed kropka.
//  (moga byc tez w podloderze ./in lub ./inout , albo osobno w podfolderach ./in ./out)
//  Skompiluj i uruchom program (na swoim komputerze! - na Ideone nie bedzie dzialac)

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <vector>
using namespace std;

bool iflike(const string chktxt, const string mask) {
// Funkcja sprawdza, czy podany tekst pasuje do wzorca, np., "nasza_olimpiada" pasuje do "na*olimp?*"
   // corrected idea from: http: // www.geeksforgeeks.org/wildcard-character-matching/
    if (mask == "" && chktxt == "") return true;
    if (mask.substr(0,2) == "**") return iflike(chktxt, mask.substr(1)); //added by me
    //    the source working on the character tables can also be improved by adding:
    //    if (*first  == '*' && *(first +1) != '\0' && *(first +1) == '*') return match(first+1, second);
    if (mask.substr(0,1) == "*" && mask.length() > 1 && chktxt == "") return false;
    if (mask.substr(0,1) == "?" || mask.substr(0,1) == chktxt.substr(0,1))
        return iflike(chktxt.substr(1), mask.substr(1));
    if (mask.substr(0,1) == "*") return iflike(chktxt, mask.substr(1)) || iflike(chktxt.substr(1), mask);
    return false;
}

vector<string> list_dir(const string& folder, const string& filemask = "*") {
// Funkcja zwraca liste plikow z podanego folderu i wzorca, np., list_dir("c:\test", "*.in")
//  wejscie: folder do przeszukania
//     i opcjonalnie wzorzec/filtr nazw plikow ze znakami "*" (cokolwiek) i/lub "?" (dowolny jeden znak)
//  wynik: lista znalezionych plikow w folderze (zgodnych ze wzorcem, jezeli go podano) w postaci wektora
    vector<string> fn_list;
    DIR* dir = opendir(folder.c_str());
    if (dir != NULL) {
        struct dirent * de;
        while((de = readdir(dir)) != NULL)
            if(iflike(de->d_name, filemask)) fn_list.push_back(de->d_name);
        closedir(dir);
    } // else perror (""); // Zm001. zakomentowane - nie wypisywac bledow, gdy nie ma podfolderow in out
    return fn_list;
}

string compare_files(string testName, string outName) {
// Funkcja porownuje dwa pliki linia po linii, ignoruje puste linie na koncu
//   wejscie: nazwy plikow do porownania
//   wyjscie: rezultat, ostatni fragment, liczba wierszy; dwa pierwsze znaki - :) kiedy OK, :O kiedy zle
    ifstream outFile, testFile;
    testFile.open(testName.c_str()); // Otworz plik z danymi wyjsciowymi
    outFile.open(outName.c_str()); // Otworz plik z danymi do porownania

    // Porownuj wszystkie linie w test i out, ignoruj puste na koncu
    string testLine, outLine, checkText = "", trailers;
    int chkLinesCnt = 0, errLineNo = 0;
    bool testEmpty, outEmpty;
    for(int i=1; true; i++){
        testLine = ""; outLine = "";
        outEmpty = getline(outFile, outLine).eof();
        testEmpty = getline(testFile ,testLine).eof();
        if(outLine != "" && outLine.substr(outLine.size() - 1) == "\r")  // ostatni znak-powrot karetki
            outLine = outLine.substr(0, outLine.size() - 1); // obetnij go
        if(testLine != "" && testLine.substr(testLine.size() - 1) == "\r") // ostatni znak-powrot karetki
            testLine = testLine.substr(0, testLine.size() - 1); // obetnij go
        if(testLine != "" || outLine != "") {
            chkLinesCnt++;
            if(testLine != outLine) {
                errLineNo = i;
                checkText.append( testLine + " != " + outLine + "  ");
                break;
            } else checkText.append( testLine + "  ");
        }
        if(testEmpty && outEmpty) break;
    }

    outFile.close();
    testFile.close();

    // Przygotuj/skroc i podaj rezultat
    int checkTextLen = checkText.length();
    if(checkTextLen>42) trailers = "..."; else trailers = "";
    checkTextLen = min((42 - int(trailers.length())), checkTextLen);
    stringstream respStr;
    if((errLineNo)==0) {
        respStr << ":) +   " << trailers 
            << checkText.substr(checkText.length() - checkTextLen) ;
        if(chkLinesCnt != 1) respStr << "  /" << chkLinesCnt <<"";
    } else {
        respStr << ":O -   " << trailers 
            << checkText.substr(checkText.length() - checkTextLen)
            << " /line: " << errLineNo;
    }
    return respStr.str();
}

//-------------------------test-main-------------------------------

int test_main_on_testpack() {
  // Program pozwala testowac kod main() na wszystkich danych testowych na raz
  //   wejscie: pary plikow *.in i *.out w biezacym folderze (lub podfolderach in,out lub w in lub inout)
  //   wyjscie: wynik porownania *.out z utworzonymi przez testowy kod *_t.out
  //   (w tym, wynnik porownania, ostatnie dane, ilosc sprawdzonych wierszy, czas wykonania)
  // Funkcje mozna wywolac zamiast main() dzieki deklaracji static - patrz na koncu

    int lastReturnCode = 0, testOkCount = 0; // zmienne na kod wyjscia z main i ranking
    string in_file, test_file, out_file, result; // zmienne na nazwy plikow i rezultaty
    vector<string> files; // wektor na listy plikow
    double total_time = 0, max_time = 0, tst_count = 0; // calk. i max czas wykonania, il. testów

    string in_folder = ".", out_folder = ".", t_out_folder = "."; // foldery z testami
    files = list_dir("./in", "*.in"); // sprawdz, czy istnieja pliki w folderze in
    if(files.size()>0) {
        files = list_dir("./out", "*.out"); // czy istnieja pliki *.in w folderze out
        if(files.size()>0) {
            in_folder = "./in";
            out_folder = "./out";
            t_out_folder = "./out";
        } else {
            files = list_dir("./in", "*.out"); // czy istnieja pliki *.out w folderze in
            if(files.size()>0) {
                in_folder = "./in";
                out_folder = "./in";
                t_out_folder = "./in";
            }
        }
    } else {
        files = list_dir("./inout", "*.in"); // czy istnieja pliki *.in w folderze inout
        if(files.size()>0) {
            in_folder = "./inout";
            out_folder = "./inout";
            t_out_folder = "./inout";
        }
    }

    files = list_dir(t_out_folder, "*_t.out"); // znajdz i usun stare pliki testowe *_t.out
    for(int f = 0; f < files.size(); ++f) remove(string(t_out_folder + "/" + files[f]).c_str());

    // Uruchamiaj testy z przekierowanym wejsciem z kolejnych *.in z wyjsciem do plikow *_t.out
    files = list_dir(in_folder, "*.in"); // znajdz pliki *.in w folderze programu i zapisz w wektorze
    sort(files.begin(), files.end()); // posortuj nazwy plikow
    for(int f = 0; f < files.size(); ++f) { // powtarzaj dla dla znalezionych plikow
        in_file = files[f]; // pobierz kolejna nazwe wejsciowego pliku testowego
        test_file = in_file.substr(0,in_file.find_last_of(".")) + "_t.out"; // nazwij wyjsciowy

        ifstream inFile; ofstream testFile;
        inFile.open(string(in_folder + "/" + in_file).c_str()); //otworz plik in do danych wejsciowych
        testFile.open(string(t_out_folder + "/" + test_file).c_str()); //i _t.out do danych wyjsciowych

        ios_base::sync_with_stdio(false);cout.tie(NULL);cin.tie(NULL); // wylacz synchro. we/wy
        // jest szybciej, ale moze tez powodowac problemy z pomieszanym wyjscie - wtedy zakomentuj 

        std::streambuf * cincon = cin.rdbuf(inFile.rdbuf()); // przekieruj wejscie na pliku in
        std::streambuf * coutcon = cout.rdbuf(testFile.rdbuf()); // przekieruj wyjscie na plik out


        // A teraz, wywolaj main() - czyli test i zmierz czas
        clock_t time_start = clock(); //zlap czas startu
        
        lastReturnCode = main();  //  uruchom kod main()
  
        clock_t time_stop = clock(); // zlap czas zakonczenia

        // Wyrzuc dane z pamieci na dysk - zapisze sie plik wyjsciowy
        cout.flush();

        // Teraz, sprawdz poprawnosc wyniku przez porownanie pary plikow *.out i *_t.out
        out_file = in_file.substr(0,in_file.find_last_of(".")) + ".out"; // nazwa pliku do porownania
        result = compare_files(t_out_folder + "/" + test_file, out_folder + "/" + out_file); // porownaj 
        // Wypisz rezultat testu dla danej pary
        cerr << left << setw(12) << in_file.substr(0,in_file.find_last_of(".")); // nazwa pl.testowego
        cerr << right << setw(7) << setprecision(2) << ((double)(time_stop - time_start) / CLOCKS_PER_SEC);
        cerr << "s " << result << "\n";
        if(result.substr(0,2) == ":)") testOkCount++; // jezeli ok, to dodaj do pozytywnych
        total_time += (double)(time_stop - time_start);
        max_time = max(max_time, (double)(time_stop - time_start));
        tst_count ++;
    }
    if(files.size() == 0)
        cerr << "No files: " << getcwd(NULL, 0) << "/*.in"; 
    else { // wypisz podsumowanie
        cerr << "\n" << (testOkCount * 100 / files.size()) << "% OK / " << tst_count; //% ok z ilosci 
        cerr << ".  Total: " << (total_time / CLOCKS_PER_SEC) << " s.";  // i czas calkowity
        cerr << "  Max.: " << (max_time / CLOCKS_PER_SEC) << " s.";  // i czas maksymalny
    }
    cerr << "\n\n"; 
    exit(lastReturnCode); // Zwroc to, co ostatnio main(), ale z wyjsciem exit(), przerywajac program
}
// Testarka to moja pierwsza przygoda z C/C++ i bardzo prosze o uwagi. Pomaga mojemu synowi w zadaniach.
// Pomysly i kod moje (ale patrz iflike), kopiowac mozna. Wersja C++ dowolna. Testowane na Win10 i Ubuntu
//                                                              p.s. ani ja ani syn nie mamy na imie Arek

// Aby zrobic cos zamiast main wykorzystamy to, ze deklaracje zmiennych static wykonywane sa przed main,
//  a przy tym wykona sie nasza funkcja, poniewaz jej wartosc ma byc podstawiona przy deklaracji TEST
static int TEST = test_main_on_testpack();
