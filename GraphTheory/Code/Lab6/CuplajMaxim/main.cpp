#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <cmath>

typedef std::pair<int, int> muchie;

using namespace std;

ostream&operator<<(ostream& out, vector<int> v) {
    for(auto a : v) {
        cout<<a<<" ";
    }
//    cout<<endl;
    return out;
}


struct pair_hash {
    template <class T1, class T2>
    size_t operator () (const pair<T1,T2> &p) const {
        auto h1 = hash<T1>{}(p.first);
        auto h2 = hash<T2>{}(p.second);

        return h1 ^ h2;
    }
};

class Graph {
    friend void edmonds_karp(Graph &g);
    friend bool bf(Graph g, vector<int> &path);
    friend ostream&operator<<(ostream& out, Graph g);
    friend void amelioreaza(Graph &g, const vector<int> parents);
    friend bool bipartit(Graph& g, vector<int> &bipartitie);
    friend vector<muchie> detCuplajMaxim(Graph &g, vector<int> &bip);
    vector<std::list<int>> adjList;
    vector<std::list<int>> incList;
    unordered_map<std::pair<int, int>, int, pair_hash> capacity;
    unordered_map<std::pair<int, int>, int, pair_hash> flux;
    int n;
    int s, t;
public:
    Graph(int n): n(n), adjList((unsigned long long int) n), incList((unsigned long long int) n) {}

    void addEdge(int u, int v) {
        adjList[u].push_back(v);
        incList[v].push_back(u);
        capacity[muchie(u, v)] = 1;
        flux[muchie(u, v)] = 0;
    }

    void setStart(int s) { this->s = s; }
    void setSink(int t) { this->t = t; }
};

Graph citire() {
    ifstream fin("graf.in");
    int n, m;
    fin>>n>>m;
    n++;
    Graph g(n);
    for(int i = 0; i < m; i++) {
        int u, v;
        fin>>u>>v;
        g.addEdge(u, v);
    }
    fin.close();
    return g;
}

ostream&operator<<(ostream& out, Graph g) {
    for(int i = 1; i < g.n; i++) {
        for(int v : g.adjList[i]) {
            cout<<i<<" "<<v<<" "<<g.flux[muchie(i, v)]<<"/"<<g.capacity[muchie(i, v)]<<endl;
        }
    }
    return out;
}

bool bf(Graph g, vector<int> &path) {
    queue<int> myQ;
    vector<int> roots((unsigned long long int) g.n, 0);
    myQ.push(g.s);
    roots[g.s] = g.s;
    while(myQ.size()) {
        int crt = myQ.front();
        myQ.pop();

        if(crt == g.t) {
            int p = roots[abs(crt)];

            do {
                path.push_back(crt);
                crt = p;
                p = roots[abs(crt)];
            } while(abs(p) != abs(crt));

            path.push_back(crt);
            return true;
        }

        for(int a : g.adjList[crt]) {
          if(roots[a] == 0 && g.flux[muchie(crt, a)] < g.capacity[muchie(crt, a)]) {    // muchie directa: trebuie sa mai am loc
              myQ.push(a);
              roots[a] = crt;
            }
        }
        for(int i : g.incList[crt]) {  // muchie de intoarcere: trebuie ca fluxul sa nu fie 0
            if(roots[i] == 0 && g.flux[muchie(i, crt)] > 0) {
                myQ.push(i);
                roots[i] = -crt;
            }
        }
    }

    return false;
}

void amelioreaza(Graph &g, const vector<int> parents) {
    int minDirect = 0x7fffffff;
    int minBack = 0x7fffffff;
    int bottleneck = 0;


    for(int i = (int) (parents.size() - 1); i >= 1; i--) {
        int vs = parents[i - 1];
        int us = parents[i];
        int v = abs(parents[i - 1]);
        int u = abs(parents[i]);
        if(us < 0) {   // muchie de intoarcere
            minBack = g.flux[muchie(v, u)] < minBack ? g.flux[muchie(v, u)] : minBack;
        } else {   // muchie directe
            minDirect = g.capacity[muchie(u, v)] - g.flux[muchie(u, v)] < minDirect ? g.capacity[muchie(u, v)] - g.flux[muchie(u, v)] : minDirect;
        }

        bottleneck = minDirect < minBack ? minDirect : minBack;
    }

    for(int i = (int) (parents.size() - 1); i >= 1; i--) {
        int vs = parents[i - 1];
        int us = parents[i];
        int v = abs(parents[i - 1]);
        int u = abs(parents[i]);
        if(us < 0) {  // muchie de intoarcere
            g.flux[muchie(v, u)] -=  bottleneck;
        } else { // muchie directa
            g.flux[muchie(u, v)] += bottleneck;
        }
    }
}

void edmonds_karp(Graph &g) {
    vector<int> parents;

    while(bf(g, parents)) {
        amelioreaza(g, parents);
        parents.clear();
    }
}

bool bipartit(Graph& g, vector<int>& bipartitie) {
    queue <int> q;
    bipartitie.resize((unsigned long long int) g.n, 0);

    for(int i = 1; i < g.n; i++) {
        if(bipartitie[i] == 0) {
            q.push(i);
            bipartitie[i] = 1;
            while(q.size()) {
                int crt = q.front();
                q.pop();

                for(int a : g.adjList[crt]) {
                    if(bipartitie[a] == 0) {
                        bipartitie[a] = bipartitie[i] == 1 ? 2 : 1;
                    } else if (bipartitie[a] == bipartitie[i]) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

vector<muchie> detCuplajMaxim(Graph &g, vector<int> &bip) {
    vector<muchie> cuplaj;
    g.setStart(g.n++);
    g.setSink(g.n++);
    g.adjList.resize((unsigned long long int) g.n);
    g.incList.resize((unsigned long long int) g.n);
    for(int i = 1; i < bip.size(); i++) {
        if(bip[i] == 1) {
            g.addEdge(g.s, i);
        } else {
            g.addEdge(i, g.t);
        }
    }

    edmonds_karp(g);

    for(auto m : g.flux) {
        if(m.second == 1 && (m.first.first != g.s && m.first.second != g.t)) {
//            cout<<"("<<m.first.first<<", "<<m.first.second<<")"<<endl;
            cuplaj.push_back(m.first);
        }
    }
    return cuplaj;
}



int main() {
    Graph g = citire();

    vector<int> bip;

    bipartit(g, bip);

    vector<muchie> cuplaj = detCuplajMaxim(g, bip);

    for(auto m : cuplaj) {
        cout<<"("<<m.first<<", "<<m.second<<")"<<endl;
    }

    return 0;
}