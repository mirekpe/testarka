#include <bits/stdc++.h>
using namespace std;


bool check7(int i) {
    for (int c = i; c > 0; c = c / 10) if ((i % 7 == 0) || (c % 10 == 7)) return true;
    return false;
}


int main() { //plum
    int A, B;
    cin >> A >> B;
    for (int i = A; i <= B; i++) if(check7(i)) cout << "plum" << "\n"; else cout << i << "\n";
    return 0;
}


//-------------------------------testarka---------------------------------------

// Powyzej wklej swoj program, funkcja main bedzie wywolana dla wszystkich plikow *.in w foderze programu

bool iflike(const string chktxt, const string mask) {
// Funkcja sprawdza, czy podany tekst pasuje do wzorca, np., "nasza_olimpiada" pasuje do "na*olimp?*"
    if (mask == "" && chktxt == "") return true;
    if (mask.substr(0,2) == "**") return iflike(chktxt, mask.substr(1));
    if (mask.substr(0,1) == "*" && mask.length() > 1 && chktxt == "") return false;
    if (mask.substr(0,1) == "?" || mask.substr(0,1) == chktxt.substr(0,1))
        return iflike(chktxt.substr(1), mask.substr(1));
    if (mask.substr(0,1) == "*") return iflike(chktxt, mask.substr(1)) || iflike(chktxt.substr(1), mask);
    return false;
}

#include <dirent.h>
vector<string> list_dir(const string& folder, const string& filemask = "*") {
// Funkcja zwraca liste plikow z podanego folderu i wzorca, np., list_dir("c:\test", "*.in")
//  wejscie: folder do przeszukania
//     i opcjonalnie wzorzec/filtr nazw plikow ze znakami "*" (cokolwiek) i/lub "?" (dowolny jeden znak)
//  wynik: lista znalezionych plikow w folderze (zgodnych ze wzorcem, jezeli go podano) w postaci wektora
    vector<string> fn_list;
    DIR* dir = opendir(folder.c_str());
    if (dir != NULL) {
        struct dirent * de;
        while((de = readdir(dir))!=NULL)
            if(iflike(de->d_name, filemask)) fn_list.push_back(de->d_name);
        closedir(dir);
    } else perror ("");
    return fn_list;
}

string compare_files(string testName, string outName) {
// Funkcja porownoje dwa pliki linia po linii, ignoruje puste linie na koncu
//   wejscie: nazwy plikow do porownania
//   wyjscie: rezultat, ostatni fragmente, liczba wierszy; dwa pierwsze znaki - :) kiedy OK, :( kiedy zle
    ifstream outFile, testFile;
    testFile.open(testName.c_str()); // Otworz plik z danymi wyjsciowymi
    outFile.open(outName.c_str()); // Otworz plik z danymi do porownania

    //porownuj wszystkie linie w test i out, ignoruj puste na koncu
    string testLine, outLine, checkText = "", trailers;
    int chkLinesCnt = 0, errLineNo = 0;
    bool testEmpty, outEmpty;
    for(int i=1; true; i++){
        testLine = ""; outLine = "";
        outEmpty = getline(outFile, outLine).eof();
        testEmpty = getline(testFile ,testLine).eof();
        if(testLine != "" || outLine != "") {
            chkLinesCnt++;
            if(testLine != outLine) {
                errLineNo = i;
                checkText.append( testLine + " != " + outLine + "  ");
                break;
            } else checkText.append( testLine + "=" + outLine + "  ");
        }
        if(testEmpty && outEmpty) break;
    }

    outFile.close();
    testFile.close();

    // przygotuj/skroc i podaj rezultat
    int checkTextLen = checkText.length();
    if(checkTextLen>63) trailers = "..."; else trailers = "";
    checkTextLen = min((63 - int(trailers.length())), checkTextLen);
    char lineNoChr[100];
    if((errLineNo)==0) {
        itoa(chkLinesCnt,lineNoChr,10);
        return ":) dobrze " + trailers + checkText.substr(checkText.length() - checkTextLen)
            + lineNoChr + "w ";
    } else {
        itoa(errLineNo,lineNoChr,10);
        return ":( ZLE! " + trailers + checkText.substr(checkText.length() - checkTextLen)
            + " linia " + lineNoChr + " ";
    }
}


//-------------------------test-main-------------------------------

#include <bits/stdc++.h>
#include <windows.h>

int test_main_on_testpack() {
  // Program pozwala testowac kod main() na wszystkich danych testowych na raz
  //   wejscie: pary plikow *.in i *.out w biezacym folderze
  //   wyjscie: wynik porownania *.out z utworzynumi przez testowy kod *_t.out
  //   (w tym, wynnik porownania, ostatnie dane, ilosc sprawdzonych wierszy, czas wykonania)
  //Funkcje mozna wywolac zamiast main() dzieki deklaracji static - patrz na koncu
    int lastReturnCode = 0, testOkCount = 0;
    string in_file, test_file, out_file, result;

    string app_comm_line = GetCommandLine();
    if(iflike(app_comm_line,"*nowRunMain*")) // Sprawdz, czy program wywolano do testowania

       lastReturnCode = main();  // Jezeli tak (z argumentem nowRunMain), to utuchom kod main()

    else {
        vector<string> files; // Wektor na liste plikow

        // Usun stare pliki testowe *_t.out
        files = list_dir(".", "*_t.out");
        for(int f = 0; f < files.size(); ++f)
            remove(files[f].c_str());

        // Uruchamiaj testy z przekierowanym wejsciem z kolejnych *.in z wyjsciem do plikow *_t.out
        files = list_dir(".", "*.in");
        for(int f = 0; f < files.size(); ++f) {
            in_file = files[f];
            test_file = in_file.substr(0,in_file.find_last_of(".")) + "_t.out";

            cout << in_file << " ";
            clock_t time_start = clock();


          // A teraz, wywolaj sam siebie, ale z poleceniem wykanania main() - czyli testu
          // z przekierowanym wejsciem ze wszystkich *.in i wyjsciem do *_t.*out
            app_comm_line = app_comm_line + " nowRunMain < " + in_file + " > " + test_file;
            system(app_comm_line.c_str());

          // Zamiast samego siebie, mozna wywolac programy exe z biezacego folderu
          //  (inne niz testarka, czyli powinny byc te skompilowane z twojego kodu)
          // z przekierowanym wejsciem ze wszystkich *.in i wyjsciem do *_t.*out
          // (    tak moze byc skompilowana testarka uniwersalna - zeby to zrobic,
          //      zakomentuj linie powyzej z: system(app_comm_line.c_str());
          //      i odkomentuj te 6 linii ponizej - skomiluj i powstaly plik exe
          //      wklej do testowego folderu u uruchamiaj recznie )
            /*
            vector<string> filesExe = list_dir(".", "*.exe");
            for(int f = 0; f < filesExe.size(); ++f)
                if( !iflike(app_comm_line, "*" + filesExe[f] + "*") ) {
                    string exe_comm_line = filesExe[f] + " < " + in_file + " > " + test_file;
                    system(exe_comm_line.c_str());
                }
            */


            clock_t time_stop = clock();
            out_file = in_file.substr(0,in_file.find_last_of(".")) + ".out";
            result = compare_files(test_file, out_file);
            if(result.substr(0,2) == ":)") testOkCount++;

            cout << result << ((double)(time_stop - time_start) / CLOCKS_PER_SEC) << "s\n";

        }
        cout << "\n" << (testOkCount * 100 / files.size()) << "% testow OK,";
        cout << " wcisnij Enter, aby zakonczyc.\n";
        cin.get();
    }

    // Zwroc to, co zwrocila main(), ale z wyjsciem awaryjnym exit(), aby nie wskoczyc do main()
    exit(lastReturnCode);
    //return 0;
}

// Wykorzystujujemy to, ze deklaracje zmiennych static wykonywane sa przed main(),
//  a przy tym wykona sie nasza funkcja, poniewaz jej wartosc ma byc podstawiona przy deklaracji
static int test = test_main_on_testpack();
