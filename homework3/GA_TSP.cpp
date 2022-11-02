#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdlib>
#include<ctime>
#include<algorithm>
#include<cmath>
#include<queue>

using namespace std;

void ReadFile(vector<vector<int>> &map){    //初始化map
    ifstream ifs;
    ifs.open("bays29.tsp",ios::in);
    if(!ifs.is_open()){
        cout << "Fail to open file.";
        return;
    }
    string s;
    for(int i = 0; i < 3; i++){
        getline(ifs,s);
    }
    ifs >> s;
    int dim;
    ifs >> dim;
    for(int i = 0; i < 5; i++){
        getline(ifs,s);
    }
    for(int i = 0; i < dim; i++){
        vector<int> row;
        for(int j = 0; j < dim; j++){
            int tmp;
            ifs >> tmp;
            row.push_back(tmp);
        }
        map.push_back(row);
    }
    ifs.close();
    return;
}

void CompareWithOptimum(vector<int> route,vector<int> &opt_route){  //从文件读取最优解
    ifstream ifs;
    ifs.open("bays29.opt.tour",ios::in);
    if(!ifs.is_open()){
        cout << "Fail to open file.";
        return;
    }
    string s;
    for(int i = 0; i < 6; i++){
        getline(ifs,s);
    }
    for(int i = 0; i < route.size(); i++){
        int tmp;
        ifs >> tmp;
        opt_route.push_back(tmp-1);
    }
    return;
}
/*
// debug unit
void print_int(vector<int> r){
    for(int i = 0; i < r.size(); i++){
        cout << r[i] << " ";
    }
    cout << endl;
    return;
}

void print_double(vector<double> r){
    for(int i = 0; i < r.size(); i++){
        cout << r[i] << " ";
    }
    cout << endl;
    return;
}
*/
class GA_TSP
{
public:
    GA_TSP(vector<vector<int>> map, int N, double P_c, double P_m):map(map),N(N),P_crossover(P_c),P_mutation(P_m){
        cities = map.size();
        M = 0.75 * N;
    };
    ~GA_TSP(){};

    void shuffle(vector<int> &r){           //shuffle
        for(int i = r.size()-1; i > 0; i--){
            swap(r[i],r[rand()%i]);
        }
        return;
    }

    void Encode(vector<int> route, vector<int> &gene){
        vector<int> W(cities-1);
        for(int i = 0; i < cities-1; i++){
            W[i] = i+1;
        }
        for(int j = 0; j < cities-1; j++){
            vector<int>::iterator it = std::find(W.begin(),W.end(),route[j]);
            int idx = distance(W.begin(),it);
            gene.push_back(idx);
            W.erase(it);
        }
        return;
    }

    void Decode(vector<int> gene, vector<int> &route){
        vector<int> W(cities-1);
        for(int i = 0; i < cities-1; i++){
            W[i] = i+1;
        }
        for(int j = 0; j < cities-1; j++){ 
            route.push_back(W[gene[j]]);
            W.erase(W.begin()+gene[j]);
        }
        return;
    }

    int Distance(vector<int> r){
        int dis = map[0][r[0]] + map[r[r.size()-1]][0];
        for(int i = 0; i < r.size()-1; i++){
            dis += map[r[i]][r[i+1]];
        }
        return dis;
    }

    int Distance_from_gene(vector<int> gene){
        vector<int> r;
        Decode(gene,r);
        return Distance(r);
    }
    
    void Initial_population(){              //初始化种群
        vector<int> route_ini(cities-1);
        for(int i = 0; i < cities-1; i++){
            route_ini[i] = i+1;
        }
        for(int j = 0; j < N; j++){
            shuffle(route_ini);
            vector<int> gene_ini;
            Encode(route_ini,gene_ini);
            population.push_back(route_ini);
            chromosome.push_back(gene_ini);
            fitness.push_back(Distance(route_ini));
            vector<int>().swap(gene_ini);
        }
        vector<int>().swap(route_ini);
        return;
    }

    void Calculate_reproduction_probability(vector<int> fitness, vector<double> &p_r){
        long sum = 0;
        int max = 0;
        for(int i = 0; i < fitness.size(); i++){
            sum -= fitness[i];
            max = (max < fitness[i]) ? fitness[i] : max;
        }
        sum += fitness.size()*(max + 1);
        for(int i = 0; i < fitness.size(); i++){
            p_r.push_back((double)(max + 1 - fitness[i]) / sum);
        }
        return;
    }

    pair<int,int> Select_pair(vector<double> p_r){
        double roulette1 = (double)rand() / (double)RAND_MAX;
        double roulette2 = (double)rand() / (double)RAND_MAX;
        pair<int,int> parents;
        pair<bool,bool> flags(true,true);
        double sum = 0;
        for(int i = 0; i < p_r.size(); i++){
            sum += p_r[i];
            if(roulette1 < sum && flags.first){
                parents.first = i;
                flags.first = false;
            }
            if(roulette2 < sum && flags.second){
                parents.second = i;
                flags.second = false;
            }
            if(!(flags.first | flags.second))break;
        }
        if(parents.first == parents.second){
            parents = Select_pair(p_r);
        }
        return parents;
    }

    void one_point_crossover(pair<int,int> parents, vector<int> &mid1, vector<int> &mid2){
        int idx = rand()%(cities-2);
        for(int i = 0; i < idx+1; i++){
            mid1.push_back(chromosome[parents.first][i]);
            mid2.push_back(chromosome[parents.second][i]);
        }
        for(int i = idx+1; i < cities-1; i++){
            mid1.push_back(chromosome[parents.second][i]);
            mid2.push_back(chromosome[parents.first][i]);
        }
        return;
    }

    void two_point_crossover(pair<int,int> parents, vector<int> &mid1, vector<int> &mid2){
        int idx1 = rand()%(cities-2);
        int idx2 = rand()%(cities-2);
        if(idx1 == idx2){
            two_point_crossover(parents,mid1,mid2);
        }else{
            int left = min(idx1,idx2);
            int right = max(idx1,idx2);
            for(int i = 0; i < left+1; i++){
                mid1.push_back(chromosome[parents.first][i]);
                mid2.push_back(chromosome[parents.second][i]);
            }
            for(int i = left+1; i < right+1; i++){
                mid1.push_back(chromosome[parents.second][i]);
                mid2.push_back(chromosome[parents.first][i]);
            }
            for(int i = right+1; i < cities-1; i++){
                mid1.push_back(chromosome[parents.first][i]);
                mid2.push_back(chromosome[parents.second][i]);
            }
        }        
        return;
    }

    void Crossover(pair<int,int> parents, vector<int> &mid1, vector<int> &mid2){
        int rand_cross = rand()%2;
        switch (rand_cross)
        {
        case 0:
            one_point_crossover(parents,mid1,mid2);
            break;
        case 1:
            two_point_crossover(parents,mid1,mid2);
            break;
        default:
            break;
        }
        return;
    }

    void reverse_mutation(vector<int> &mid){
        vector<int> route;
        Decode(mid,route);
        int u = rand()%route.size();
        int v = rand()%route.size();
        if((u == 0 && v == route.size()-1)
            || (v == 0 && u == route.size()-1)
            || (u == v)){
                reverse_mutation(mid);
                return;
        }
        else{
            reverse(route.begin()+min(u,v),route.begin()+max(u,v)+1);
            vector<int> gene_new;
            Encode(route,gene_new);
            mid.swap(gene_new);
            return;
        }
    }

    void exchange_mutation(vector<int> &mid){
        vector<int> route;
        Decode(mid,route);
        int u = rand()%route.size();
        int v = rand()%route.size();
        if(u == v){
            exchange_mutation(mid);
            return;
        }
        else{
            swap(route[u],route[v]);
            vector<int> gene_new;
            Encode(route,gene_new);
            mid.swap(gene_new);
            return;
        }
    }

    void insert_mutation(vector<int> &mid){
        vector<int> route;
        Decode(mid,route);
        int u = rand()%route.size();
        int v = rand()%route.size();
        if(u == v){
            insert_mutation(mid);
            return;
        }
        else{
            route.insert(route.begin()+min(u,v)+1,route[max(u,v)]);
            route.erase(route.begin()+max(u,v)+1);
            vector<int> gene_new;
            Encode(route,gene_new);
            mid.swap(gene_new);
            return;
        }
    }
    
    void general_mutation(vector<int> &mid){
        int idx = rand()%(cities-1);
        while(idx == cities-2){
            idx = rand()%(cities-1);
        }
        int mut = rand()%(cities-1-idx);
        while(mut == mid[idx]){
            mut = rand()%(cities-1-idx);
        }
        mid[idx] = mut;
        return;
    }

    void Mutation(vector<int> &mid){
        int rand_mutation = rand()%4;
        switch (rand_mutation)
        {
        case 0:
            reverse_mutation(mid);
            break;
        case 1:
            exchange_mutation(mid);
            break;
        case 2:
            insert_mutation(mid);
            break;
        case 3:
            general_mutation(mid);
            break;
        default:
            break;
        }
        return;
    }

    int Elitist(){
        int idx = 0;
        for(int i = 1; i < N; i++){
            if(fitness[i] < fitness[idx]){
                idx = i;
            }
        }
        return idx;
    }

    void Genetic_Algorithm_TSP(){
        Initial_population();
        generation = 0;
        while(generation < 1000){

            vector<vector<int>> children;
            priority_queue<pair<int,vector<int>>, vector<pair<int,vector<int>>>, cmp> pq;

            int m_count = 0;

            //1
            vector<double> p_r;
            Calculate_reproduction_probability(fitness, p_r);

            while(m_count < M){
                //2
                pair<int,int> parents = Select_pair(p_r);
            
                //3
                double roulette_crossover = (double)rand() / (double)RAND_MAX;
                if(P_crossover < roulette_crossover)continue;

                vector<int> mid_g1,mid_g2;
                Crossover(parents, mid_g1, mid_g2);
            
                //4
                double roulette_mutation1 = (double)rand() / (double)RAND_MAX;
                if(P_mutation > roulette_mutation1){
                    Mutation(mid_g1);
                }
                double roulette_mutation2 = (double)rand() / (double)RAND_MAX;
                if(P_mutation > roulette_mutation2){
                    Mutation(mid_g2);
                }

                pair<int,vector<int> > p_mid1 = {Distance_from_gene(mid_g1),mid_g1};
                pair<int,vector<int> > p_mid2 = {Distance_from_gene(mid_g2),mid_g2};
                pq.push(p_mid1);
                pq.push(p_mid2);

                m_count++;
            }

            vector<int> children_fitness;
            children_fitness.push_back(fitness[Elitist()]);
            children.push_back(chromosome[Elitist()]);
            for(int i = 1; i < N; i++){
                children_fitness.push_back(pq.top().first);
                children.push_back(pq.top().second);
                pq.pop();
            }
            priority_queue<pair<int,vector<int>>, vector<pair<int,vector<int>>>, cmp> empty;
            swap(empty,pq);

            chromosome.swap(children);
            fitness.swap(children_fitness);

            vector<int> r;
            Decode(chromosome[0],r);
            elitist_route.swap(r);
            elitist_fitness = fitness[0];

            generation++;
        }
        return;
    }

public:
    vector<vector<int>> map;
    int N;
    double P_crossover;
    double P_mutation;

    vector<int> elitist_route;
    int elitist_fitness;

private:
    int M;
    int cities;
    int generation = 0;
    vector<vector<int>> population;
    vector<vector<int>> chromosome;
    vector<int> fitness;

    struct cmp{
        template<typename T, typename U>
        bool operator()(T const& left, U const& right){
            if(left.first > right.first)
                return true;
            return false;
        }
    };
};



int main(){
    vector<vector<int>> map;
    ReadFile(map);

    unsigned seed = time(0);
    srand(seed);

    int N = 50;
    double P_crossover = 0.6;
    double P_mutation = 0.01;

    GA_TSP ga(map, N, P_crossover, P_mutation);

    ga.Genetic_Algorithm_TSP();

    vector<int> opt_route;
    CompareWithOptimum(ga.elitist_route,opt_route);

    cout << "The GA-optimum route is:" << endl;
    for(int i = 0; i < ga.elitist_route.size(); i++){
        cout << ga.elitist_route[i] << " ";
    }
    cout << endl;
    cout << "The GA-optimum route's distance is:" << endl << ga.elitist_fitness << endl;

    cout << "The optimum route is:" << endl;
    for(int i = 0; i < opt_route.size(); i++){
        cout << opt_route[i] << " ";
    }
    cout << endl;
    cout << "The optimum route's distance is:" << endl << ga.Distance(opt_route) << endl;

    return 0;
}