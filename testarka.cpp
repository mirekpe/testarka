#include <bits/stdc++.h>
using namespace std;
 
 
bool check7(int i) {
    for (int c = i; c > 0; c = c / 10) if ((i % 7 == 0) || (c % 10 == 7)) return true;
    return false;
}
 
 
int main() { //Zadanie plum
    int A, B;
    cin >> A >> B;
    for (int i = A; i <= B; i++) if(check7(i)) cout << "plum" << "\n"; else cout << i << "\n";
    return 0;
}
 
 
//-------------------------------testarka---------------------------------------
 
// Paste your code above, the main function will be called for all *.in files in the program foder
 
//  Prepare *.in (input) and *.out (for comparison) files yourself, 
//  or download them from the Olympics website and throw it into the folder with the code.
//  They should be in pairs of the same name before the dot . 
//  Compile and run the program (on your computer! - Ideone will not work)
 
bool iflike(const string chktxt, const string mask) {
// The function checks if the given text matches the pattern, e.g., "to_our_olimpics" matches "to*olimp?*"
    // note: corrected idea from: http://www.geeksforgeeks.org/wildcard-character-matching/
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
 
#include <dirent.h>
vector<string> list_dir(const string& folder, const string& filemask = "*") {
// The function returns a file list from a given folder and pattern, e.g., list_dir("c:\test", "*.in")
//  input: folder to be searched
//     and optionally a pattern/filter for file names 
//     with "*" characters (whatever) and/or "?" (any one character)
//  result: a list of files found in the folder (according to the pattern, if provided) - as vector
    vector<string> fn_list;
    DIR* dir = opendir(folder.c_str());
    if (dir != NULL) {
        struct dirent * de;
        while((de = readdir(dir)) != NULL)
            if(iflike(de->d_name, filemask)) fn_list.push_back(de->d_name);
        closedir(dir);
    } else perror ("");
    return fn_list;
}
 
string compare_files(string testName, string outName) {
// The function compares two files line by line, ignores blank lines at the end
//   input: file name for comparison
//   output: result, last fragment, number of rows; first two characters- :) when OK, :( when wrong
    ifstream outFile, testFile;
    testFile.open(testName.c_str()); // Open an output file
    outFile.open(outName.c_str()); // Open a file with the data to compare
 
    // Compare all the lines in the test and out, ignore the blanks in the end
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
 
    // Prepare/shorten and specify the result
    int checkTextLen = checkText.length();
    if(checkTextLen>63) trailers = "..."; else trailers = "";
    checkTextLen = min((63 - int(trailers.length())), checkTextLen);
    char lineNoChr[100];
    if((errLineNo)==0) {
        itoa(chkLinesCnt,lineNoChr,10);
        return ":) good " + trailers + checkText.substr(checkText.length() - checkTextLen)
            + lineNoChr + "r ";
    } else {
        itoa(errLineNo,lineNoChr,10);
        return ":( BAD! " + trailers + checkText.substr(checkText.length() - checkTextLen)
            + " line " + lineNoChr + " ";
    }
}
 
 
//-------------------------test-main-------------------------------
 
#include <bits/stdc++.h>
#include <windows.h>
 
int test_main_on_testpack() {
  // The program allows you to test the main() code on all test data at once.
  //   input: pairs of *.in and *.out files in the current folder
  //   output: result of comparing *.out with those created by the tested code *_t.out
  //   (including: comparison result, last data, number of checked lines, execution time)
  // Functions can be called instead of main() because of static declaration - see at the end
    int lastReturnCode = 0, testOkCount = 0;
    string in_file, test_file, out_file, result;
 
    string app_comm_line = GetCommandLine(); // The path and the name and arguments of the program
    if(iflike(app_comm_line,"*nowRunMain*")) // Check if the program has been called up for testing
 
       lastReturnCode = main();  // If yes (with the nowRunMain argument), then run the main()
 
    else {
        vector<string> files; // Vector on the file list
 
        // Delete old test files *_t.out
        files = list_dir(".", "*_t.out");
        for(int f = 0; f < files.size(); ++f)
            remove(files[f].c_str());
 
        // Run tests with redirected input from successive *.in and output to *_t.out files
        files = list_dir(".", "*.in");
        for(int f = 0; f < files.size(); ++f) {
            in_file = files[f];
            test_file = in_file.substr(0,in_file.find_last_of(".")) + "_t.out";
 
            cout << in_file << " ";
            clock_t time_start = clock();
 
 
          // Now, invoke yourself, but with the command to run main() - i.e. for test
          // with redirected input from all of *.in and output to *_t.*out
            app_comm_line = app_comm_line + " nowRunMain < " + in_file + " > " + test_file;
            system(app_comm_line.c_str());
 
          // Instead of call himself, this code can call exe programs from the current folder
          //  (others than the tester, these should be the ones compiled from your code)
          // with redirected input from all of *.in and an output to *_t.*out
          // (    so a universal tester can be compiled, to do it:
          //      comment on the line above with: system(app_comm_line.c_str());
          //      and uncomment these six lines below - compile and create an exe file 
          //      paste it into a test folder and run it "manually". )
            /*
            vector<string> filesExe = list_dir(".", "*.exe");
            for(int f = 0; f < filesExe.size(); ++f)
                if( !iflike(app_comm_line, "*" + filesExe[f] + "*") ) {
                    string exe_comm_line = filesExe[f] + " nowRunMain < " + in_file + " > " + test_file;
                    system(exe_comm_line.c_str()); // nowRunMain, if your code had already had a tester :)
                }
            */
 
 
            clock_t time_stop = clock();
 
            // Now, validate the result by comparing the file pairs *.out and *_t.out
            out_file = in_file.substr(0,in_file.find_last_of(".")) + ".out";
            result = compare_files(test_file, out_file);
            if(result.substr(0,2) == ":)") testOkCount++;
 
            // Print the test result for the pair
            cout << result << ((double)(time_stop - time_start) / CLOCKS_PER_SEC) << "s\n";
 
        }
        // Write a summary and wait for Enter
        cout << "\n" << (testOkCount * 100 / files.size()) << "% tests OK,";
        cout << " press Enter, to finish.\n";
        cin.get();
    }
 
    // Return what main() returned, but with emergency exit() so that you don't jump into main()
    exit(lastReturnCode);
    //return 0;
}
     
// Testarka (tester) is my first adventure with C/C++ and I would like to ask for your comments. 
// This code helps my son with his programming tasks.
// The idea and the code are mine, you can copy it (but see iflike).
// C++ compiler can be almost any version
    
    
     
// To do something instead of main, we use static declaration, which is executed before main()
//  and our function will be performed because its result is to be set to TEST at the time of declaration
static int TEST = test_main_on_testpack();
     

