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

void ReadFile_coordinate(vector<pair<double,double>> &cities){    //初始化cities坐标
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
    for(int i = 0; i < 5 + dim + 1; i++){
        getline(ifs,s);
    }
    for(int i = 0; i < dim; i++){
        int num;
        double x,y;
        ifs >> num;
        ifs >> x;
        ifs >> y;
        pair<double,double> pos = {x,y};
        cities.push_back(pos);
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
    for(int i = 0; i < 5; i++){
        getline(ifs,s);
    }
    for(int i = 0; i < route.size(); i++){
        int tmp;
        ifs >> tmp;
        opt_route.push_back(tmp-1);
    }
    return;
}

class SOM_TSP           //自组织映射类
{
public:
    SOM_TSP(vector<pair<double,double>> cities, int k = 8):cities(cities){  //初始化，k：神经元个数是城市个数的k倍
        num_cities = cities.size();
        num_neurons = k * num_cities;
    };
    ~SOM_TSP(){};

    void Initialize_neurons(){                     //随机初始化神经元
        double max_x = cities[0].first;
        double max_y = cities[0].first;
        double min_x = cities[0].second;
        double min_y = cities[0].second;
        for(int i = 1; i < num_cities; i++){
            max_x = (max_x < cities[i].first) ? cities[i].first : max_x;
            min_x = (min_x > cities[i].first) ? cities[i].first : min_x;
            max_y = (max_y < cities[i].second) ? cities[i].second : max_y;
            min_y = (min_y > cities[i].second) ? cities[i].second : min_y;
        }
        for(int j = 0; j < num_neurons; j++){
            double rand_x = (double)rand() / (double)RAND_MAX * (max_x - min_x) + min_x;
            double rand_y = (double)rand() / (double)RAND_MAX * (max_y - min_y) + min_y;
            pair<double,double> rand_neuron = {rand_x, rand_y};
            neurons.push_back(rand_neuron);
        }
        return;
    }

    double euclidean_distance(pair<double,double> pos1,pair<double,double> pos2){   //计算pos1和pos2之间的欧氏距离
        double dx = pos1.first - pos2.first;
        double dy = pos1.second - pos2.second;
        return sqrt(dx * dx + dy * dy);
    }

    int winning_unit(pair<double,double> city){                             //返回对于city的获胜神经元（距离city最近的神经元）在neurons中的下标
        int min_idx = 0;
        double min_dis = euclidean_distance(city, neurons[0]);
        for(int i = 1; i < num_neurons; i++){
            double temp_dis = euclidean_distance(city, neurons[i]);
            if(temp_dis < min_dis){
                min_idx = i;
                min_dis = temp_dis;
            }
        }
        return min_idx;
    }

    double G_proximity(int cur_idx, int winner_idx, double K){              //测度一个神经元与获胜神经元的“相似性”，详见论文41、42页
        double idx_dis = (double)min(abs(cur_idx - winner_idx), num_neurons - abs(cur_idx - winner_idx));
        return 1 / sqrt(2.0) * exp(-(idx_dis * idx_dis)/(K * K));
    }

    void Compute_Route(vector<int> &route, vector<int> &nearest_neurons_idx){   //生成路径，“只需要将一个城市和它的获胜神经元相连，从任意一点开始，按获胜神经元的顺序对城市排序”
        for(int i = 0; i < num_cities; i++){
            nearest_neurons_idx.push_back(winning_unit(cities[i]));
            route.push_back(i);
        }
        for(int i = 0; i < num_cities; i++){
            for(int j = i + 1; j < num_cities; j++){
                if(nearest_neurons_idx[i] > nearest_neurons_idx[j]){
                    int temp_n_idx = nearest_neurons_idx[i];
                    nearest_neurons_idx[i] = nearest_neurons_idx[j];
                    nearest_neurons_idx[j] = temp_n_idx;
                    int temp_r_idx = route[i];
                    route[i] = route[j];
                    route[j] = temp_r_idx;
                }
            }
        }
        return;
    }

    double Route_Distance(vector<int> route){                               //计算路径长度
        double dis = euclidean_distance(cities[route[0]],cities[route[route.size()-1]]);
        for(int i = 0; i < num_cities-1; i++){
            dis += euclidean_distance(cities[route[i]], cities[route[i+1]]);
        }
        return dis;
    }

    void Neurons2File(vector<pair<double,double>> neurons, int iter){
        ofstream ofs;
        string name = "output\\" + to_string(iter);
        //cout << name << endl;
        ofs.open(name,ios::out);
        for(int i = 0; i < neurons.size(); i++){
            ofs << neurons[i].first << "\t" << neurons[i].second << endl;
        }
        return;
    }

    void Winners2File(vector<int> winners_idx, int iter){
        ofstream ofs;
        string name = "winner\\" + to_string(iter);
        ofs.open(name,ios::out);

        for(int i = 0; i < winners_idx.size(); i++){
            ofs << neurons[winners_idx[i]].first << "\t" << neurons[winners_idx[i]].second << endl;
        }
        return;
    }

    void Self_Organizing_Map_TSP(int iter = 10000){                         //SOM，迭代次数iter
        Initialize_neurons();
        int cur_iter = 0;
        vector<int> cur_route;
        vector<int> nearest_neurons_idx;
        while(cur_iter < iter){
            int rand_city_idx = rand() % num_cities;                        //随机选一个城市
            int winning_unit_idx = winning_unit(cities[rand_city_idx]);     //找到获胜神经元
            for(int i = 0; i < num_neurons; i++){                           //对所有神经元Tj进行更新
                neurons[i].first += lr * G_proximity(i, winning_unit_idx, K) * (cities[rand_city_idx].first - neurons[i].first);
                neurons[i].second += lr * G_proximity(i, winning_unit_idx, K) * (cities[rand_city_idx].second - neurons[i].second);
            }
            lr *= 0.99997;                                                  //学习率更新
            K *= 0.9997;                                                    //K更新
            Compute_Route(cur_route,nearest_neurons_idx);                   //计算当前路径
            double cur_distance = Route_Distance(cur_route);                //当前路径长度
            if(cur_iter == 0 || cur_distance < best_distance){              //更新最优路径
                best_route.swap(cur_route);
                best_distance = cur_distance;
            }

            if(cur_iter == 0 || (cur_iter + 1) % 100 == 0){
                Neurons2File(neurons, cur_iter + 1);
                Winners2File(nearest_neurons_idx, cur_iter + 1);
            }
            vector<int>().swap(cur_route);
            vector<int>().swap(nearest_neurons_idx);
            cur_iter++;
        }
        return;
    }

public:
    vector<pair<double,double>> cities;     //X,城市坐标
    int num_cities;
    vector<pair<double,double>> neurons;    //Tj,神经元坐标
    int num_neurons;

    vector<int> best_route;                 //最佳路径
    double best_distance;                   //最佳路径距离

private:
    double lr = 0.8;                        //学习率
    double K = 30;                          //G函数中的K值
};

int main(){
    vector<pair<double,double>> cities;
    ReadFile_coordinate(cities);

    unsigned seed = time(0);
    srand(seed);

    SOM_TSP som(cities);

    som.Self_Organizing_Map_TSP();

    vector<int> opt_route;
    CompareWithOptimum(som.best_route,opt_route);

    cout << "The SOM-optimum route is:" << endl;
    for(int i = 0; i < som.best_route.size(); i++){
        cout << som.best_route[i] << " ";
    }
    cout << endl;
    cout << "The som-optimum route's distance is:" << endl << som.best_distance << endl;

    cout << "The optimum route is:" << endl;
    for(int i = 0; i < opt_route.size(); i++){
        cout << opt_route[i] << " ";
    }
    cout << endl;
    cout << "The optimum route's distance is:" << endl << som.Route_Distance(opt_route) << endl;

    return 0;
}