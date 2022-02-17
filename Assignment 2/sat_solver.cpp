#include <bits/stdc++.h>
using namespace std;

int main() {
    cout << "Welcome to the SAT Solver!" << endl;
    cout << "Please provide the path to the cnf file: ";
    string path;
    getline(cin, path);
    ifstream file(path);
    if (!file.is_open()) {
        cout << "File not found!" << endl;
        return 0;
    }
    int num_vars, num_clauses;
    string line;
    getline(file, line);
    while(!line.empty()) {
        if (line[0] == 'p') {
            stringstream ss(line);
            string s;
            getline(ss, s, ' ');
            getline(ss, s, ' ');
            num_vars = stoi(s);
            getline(ss, s, ' ');
            num_clauses = stoi(s);
            break;
        }
        getline(file, line);
    }
    cout << "Number of variables: " << num_vars << endl;
    cout << "Number of clauses: " << num_clauses << endl;
    file.close();
    cout << "The cnf file has been read successfully!" << endl;
    return 0;
}