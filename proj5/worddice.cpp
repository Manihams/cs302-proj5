//Lab Name: Project 5 Edmonds-Karp Algorithm
//Names: Pravin Adithya Srinivasan and Manihams Suraparaju
//Program Description: This program reads a list of dice with letters and a list of words.
//It then constructs a flow network that models the assignment of dice to letters in each
//word using the Edmonds–Karp (maximum flow) algorithm. It determines for each word
//whether every letter can be matched to a unique dice that contains the corresponding
//letter. If a valid assignment exists, the program outputs which die is used for
//each letter otherwise it reports that the word cannot be spelled.
//Help: Class Notes, GeeksForGeeks, Youtube, Bing, StackOverflow

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>

using namespace std;

//helper function for adding edges
void addEdge(int u, int v, int cap, vector<vector<int>>& capacity, vector<vector<int>>& adj){
    capacity[u][v] = cap;
    //adding both forward and reverse edges for residual network
    adj[u].push_back(v);
    adj[v].push_back(u);
}

//The network is represented by a matrix and an adjacency list
//algo in the Edmonds-Karp algorithm function
int algo(int source, int sink, vector<vector<int>> &capacity, const vector<vector<int>> &adj){

    int flow = 0;
    int n = capacity.size();

    //storing its parent node in the BFS search for each node
    while(true){
        vector<int> parent(n, -1);
        parent[source] = source;
        queue<int> q;
        q.push(source);

        //BFS to find an augmenting path
        while(!q.empty() && parent[sink] == -1){
            int u = q.front();
            q.pop();
            //for loop to iterate through u's adjacent nodes
            for(int v : adj[u]){
                //if edge from u to v has remaining capacity and v has not been visited
                if(capacity[u][v] > 0 && parent[v] == -1){
                    //marking u as parent of v and enqueue v to continue BFS
                    parent[v] = u;
                    q.push(v);
                    //if v is sink node, we exit early
                    if(v == sink){
                        break;
                    }
                }
            }
        }


        //exiting if no augmenting path is found
        if(parent[sink] == -1) break;
        //flow increment of 1
        int augment = 1;
        //starting from the sink
        int v = sink;
        while(v != source){
            //getting parent of v
            int u = parent[v];
            //decreasing/increasing forward edge capacity respsectively
            capacity[u][v] -= augment;
            capacity[v][u] += augment;
            //setting v to its parent
            v = u;
        }
        //adding augment to total flow
        flow += augment;
    }

    return flow;
}

int main(int argc, char* argv[]){

    //getting info from dice file
    string diceFile = argv[1];
    ifstream finDice(diceFile);
    if(!finDice){
        cerr << "Unable to open dice file: " << diceFile << endl;
        return 1;
    }

    vector<string> dice;
    string line;
    while(getline(finDice, line)){
        if(!line.empty())
            dice.push_back(line);
    }

    finDice.close();

    //getting info from words file
    string wordsFile = argv[2];
    ifstream finWords(wordsFile);
    if(!finWords){
        cerr << "Unable to open words file: " << wordsFile << endl;
        return 1;
    }
    vector<string> words;
    while(getline(finWords, line)){
        if(!line.empty())
            words.push_back(line);
    }
    finWords.close();

    //processing each word
    for(const auto &word : words){
        //getting total dice count
        int D = dice.size();
        //getting total letter count
        int L = word.size();
        //getting the total # of nodes
        int n = 2 + D + L;
        //setting source node to index 0
        int source = 0;
        //setting the sink node index
        int sink = n - 1;

        //initializing matrix and adjacency list
        vector<vector<int>> capacity(n, vector<int>(n, 0));
        vector<vector<int>> adj(n);

        //creating edge from source to every dice node
        for(int i = 0; i < D; i++){
            addEdge(source, 1 + i, 1, capacity, adj);
        }

        //nested for loop for adding an edge if the dice contains the letter for each dice and letter of word
        for(int i = 0; i < D; i++){
            for(int j = 0; j < L; j++){
                //getting current letter
                char letter = word[j];
                //finding letter in dice
                size_t pos = dice[i].find(letter);
                //if letter is in dice, we map the dice to the respectice letter node
                if(pos < dice[i].size()){
                    addEdge(1 + i, 1 + D + j, 1, capacity, adj);
                }
            }
        }

        //creating an edge from respective letter node to sink node with capacity of 1
        for(int j = 0; j < L; j++){
            addEdge(1 + D + j, sink, 1, capacity, adj);
        }

        //keeping the original capacities for later
        vector<vector<int>> original_capacity = capacity;

        //calling the Edmonds-Karp function
        int maxFlow = algo(source, sink, capacity, adj);

        //if max flow does not equal length of words, letter cannot be matched
        if(maxFlow != L){
            cout << "Cannot spell " << word << endl;
        } else {

            //initializing each letter's assignment as unmatched
            vector<int> assignment(L, -1);
            //nested loop for every dice and letter node
            for(int i = 0; i < D; i++){
                int diceNode = 1 + i;
                for(int j = 0; j < L; j++){
                    int letterNode = 1 + D + j;
                    //if original edge existed and is now fully saturated then, assign dice 'i' to letter at 'j'
                    if(original_capacity[diceNode][letterNode] == 1 && capacity[diceNode][letterNode] == 0){
                        assignment[j] = i;
                    }
                }
            }

            //checking that every letter position is assigned to dice
            bool valid = true;
            for(int j = 0; j < L; j++){
                if(assignment[j] == -1){
                    valid = false;
                    break;
                }
            }

            //seeing what to print
            if(!valid){
                cout << "Cannot spell " << word << endl;
            }
            else {
                for(int j = 0; j < L; j++){
                    cout << assignment[j];
                    if(j < L - 1)
                        cout << ",";
                }
                cout << ": " << word << endl;
            }
        }
    }
    return 0;
}
