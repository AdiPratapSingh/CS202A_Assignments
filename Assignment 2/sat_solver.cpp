#include <bits/stdc++.h>
using namespace std;

int n;
vector<vector<int>> clauses;

bool solve(vector<vector<int>> &formula, vector<int> &interpretation) {
    // if ( I ⇒ F ) return true
    
    // if ( I ⇒ ¬F ) return false
    // choose xi that I does not assign
    // if sat(F, I ∪ { xi=true }) return true
    // if sat(F, I ∪ { xi=false }) return true
    // return false
}


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
            string tmp;
            ss >> tmp >> tmp >> num_vars >> num_clauses;   // p cnf 20 91
            break;
        }
        getline(file, line);
    }
    cout << "Number of variables: " << num_vars << endl;
    cout << "Number of clauses: " << num_clauses << endl;

    getline(file, line);
    while(!line.empty()) {
        if (line[0] == 'c') {
            getline(file, line);
            continue;
        }
        stringstream ss(line);
        int tmp;
        vector<int> clause;
        while (ss >> tmp) {
            clause.push_back(tmp);
        }
        clause.pop_back();
        clauses.push_back(clause);
        getline(file, line);
    }
    file.close();

    n = num_clauses;
    vector<int> cur_node;
    if (solve(cur_node)) {
        cout << "SAT!" << endl;
        vector<int>::iterator ip;
        ip = unique(cur_node.begin(), cur_node.end());
        cur_node.resize(distance(cur_node.begin(), ip));
        for (int i = 0; i < cur_node.size(); i++) {
            cout << cur_node[i] << " ";
        }
        cout << endl;
    } else {
        cout << "UNSAT!" << endl;
    }


    return 0;
}