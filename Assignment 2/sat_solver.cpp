#include <bits/stdc++.h>
using namespace std;

// Global Variables
int num_vars, num_clauses;
long long total = 0;
vector<vector<int>> F;         // Formula
vector<int> tv_clause;         // Truth Value of each clause
vector<int> I;                 // Interpretation
vector<pair<int,int>> cnt;     // Count of positive and negative literals
vector<int> tcnt;              // Total Count of literals
vector<vector<int>> adj;       // Adjacency list - clauses each literal is part of


void initialize_variables() {
    F.push_back({});
    for (int i = 0; i < num_clauses+1; i++) {
        tv_clause.push_back(0);
    }
    for (int i = 0; i <= num_vars; i++) {
        I.push_back(0);
        cnt.push_back({0,0});
        tcnt.push_back(0);
        adj.push_back({});
    }
}


int simplify_and_check() {
    // simplify and check satisfiability of the formula
    for (auto c : adj[I[0]]) {             // I[0] is the last literal specified in I
        int ac = abs(c);
        if (tv_clause[ac] == 0) {      // Check if truth value of clause is not found yet
            if (c*I[I[0]] > 0) {           // Check if clause gets satisfied
                tv_clause[ac] = 1;
                for (auto l : F[ac]) {
                    (l > 0)? cnt[l].first-- : cnt[-l].second--;
                    tcnt[abs(l)]--;
                }
                F[ac].clear();
            }
            else {
                if (F[ac].size() >= 2) {
                    // remove literal from the clause
                    F[ac].erase(remove(F[ac].begin(), F[ac].end(), I[0]*(ac/c)), F[ac].end());
                    (c > 0)? cnt[I[0]].first-- : cnt[I[0]].second--;
                    tcnt[I[0]]--;
                }
                else {
                    return -1;
                }
            }
        }
    }
    for (int i = 1; i <= num_clauses; i++) {
        // if any clause is not satisfied, return -1
        // if any clause is not determined yet, return 0
        if (tv_clause[i] != 1) {
            return tv_clause[i];
        }
    }
    return 1;  // if all clauses are satisfied, return 1
}


int unit_propagation() {
    for (int i = 1; i <= num_clauses; i++) {
        if (tv_clause[i] == 1)  continue;
        if (F[i].size() == 1) {
            I[abs(F[i][0])] = F[i][0];
            I[0] = abs(F[i][0]);
            tv_clause[i] = 1;
            (F[i][0] > 0)? cnt[F[i][0]].first-- : cnt[-F[i][0]].second--;
            tcnt[abs(F[i][0])]--;
            int res = simplify_and_check();
            if (res != 0) return res;
            i = 0;
        }
    }
    return 0;
}


int pure_literal() {
    for (int i = 1; i <= num_vars; i++) {
        if (I[i] == 0 && (cnt[i].first == 0 || cnt[i].second == 0) && tcnt[i] > 0) {
            int sign = cnt[i].first > 0 ? 1 : -1;
            I[i] = sign;
            I[0] = i;
            int res = simplify_and_check();
            if (res != 0) return res;
            i = 0;
        }
    }
    return 0;
}


int find_xi() {
    int xi = 0;
    long long maxm = 0;
    for (int i = 1; i <= num_vars; i++) {
        long long cur = 1024*cnt[i].first*cnt[i].second;
        cur += cnt[i].first + cnt[i].second;
        if (cur > maxm) {
            maxm = cur;
            xi = i;
        }
    }
    return xi;
}


bool sat() {
    // if ( I ⇒ F ) return true
    // if ( I ⇒ ¬F ) return false
    int res = simplify_and_check();
    if (res != 0) return (res == 1);
    
    
    // F,I = pure(F,I)
    // if F = ∅ return true
    res = pure_literal();
    if (res != 0) return (res == 1);

    // F,I = up(F,I)
    // if I is inconsistent return false
    res = unit_propagation();
    if (res != 0) return (res == 1);
    
    
    // choose xi that I does not assign
    // if sat(F, I ∪ { xi=true }) return true
    // if sat(F, I ∪ { xi=false }) return true
    // return false.
    vector<vector<int>> F_bckup(F);
    vector<int> I_bckup(I), tcnt_backup(tcnt), tv_clause_backup(tv_clause);
    vector<pair<int,int>> cnt_bckup(cnt);

    total++;
    int xi = find_xi();
    I[xi] = 1;      I[0] = xi;
    if (sat()) {total--; return true;}
    
    F = F_bckup;    I = I_bckup;
    tv_clause = tv_clause_backup;
    tcnt = tcnt_backup;
    cnt = cnt_bckup;

    I[xi] = -1;     I[0] = xi;
    if (sat()) {total--; return true;}
    total--;
    return false;
}


void print_model() {
    for (int i = 1; i <= num_vars; i++) {
        if (I[i] == 1) {
            cout << i << " ";
        }
        else {
            cout << -i << " ";
        }
    }
    cout << endl;
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
    
    initialize_variables();

    getline(file, line);
    while(!line.empty()) {
        if (line[0] == 'c') {
            getline(file, line);
            continue;
        }
        stringstream ss(line);
        int tmp;
        vector<int> clause;
        bool valid = false;
        while (!valid && ss >> tmp) {
            if (tmp == 0) break;
            // if both xi and ~xi are present in the clause, mark the clause as true
            // additionally, add xi only one time in a clause
            if (adj[abs(tmp)].size() != 0 && abs(adj[abs(tmp)].back()) == F.size()) {
                if (tmp*adj[abs(tmp)].back() < 0) {
                    tv_clause[F.size()] = 1;
                    valid = true;
                }
                continue;
            }
            clause.push_back(tmp);
            (tmp > 0)? cnt[tmp].first++ : cnt[-tmp].second++;
            tcnt[abs(tmp)]++;
            adj[abs(tmp)].push_back(F.size()*(tmp/abs(tmp)));    // Add clause to adjacency list
        }
        if (valid) {
            for (auto l : clause) {
                adj[abs(l)].pop_back();
                (l > 0)? cnt[l].first-- : cnt[-l].second--;
                tcnt[abs(l)]--;
            }
            clause.clear();
        }
        F.push_back(clause);
        getline(file, line);
    }
    file.close();

    vector<vector<int>> F_bckup(F);
    vector<int> I_bckup(I), tcnt_backup(tcnt), tv_clause_backup(tv_clause);
    vector<pair<int,int>> cnt_bckup(cnt);

    int xi = find_xi();
    I[xi] = 1;
    I[0] = xi;
    
    if (sat()) {
        cout << "SAT" << endl;
        print_model();
    }
    else {
        F = F_bckup;    I = I_bckup;
        tv_clause = tv_clause_backup;
        tcnt = tcnt_backup;
        cnt = cnt_bckup;
        I[xi] = -1;
        I[0] = xi;
        if (sat()) {
            cout << "SAT" << endl;
            print_model();
        }
        else {
            cout << "UNSAT" << endl;
        }
    }

    return 0;
}