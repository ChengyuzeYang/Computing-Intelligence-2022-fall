#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdlib>
#include<ctime>
#include<algorithm>
#include<cmath>

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

class SA_TSP
{
public:
    SA_TSP(vector<vector<int>> map,vector<int> r_ini,double t_ini,int L_ini):map(map),route(r_ini),temperature(t_ini),route_distance(Distance(r_ini)),Length(L_ini){}
    ~SA_TSP(){}

    void Route_Generate(vector<int> &r){    //采用2变换法产生新解
        int u = rand()%r.size();
        int v = rand()%r.size();
        if((u == 0 && v == r.size()-1)
            || (v == 0 && u == r.size()-1)
            || (u == v)){
                Route_Generate(r);
                return;
            }
        else{
            reverse(r.begin()+min(u,v),r.begin()+max(u,v)+1);
            return;
        }
    }

    int Distance(vector<int> r){            //计算路径距离
        int dis = map[0][r[0]] + map[r[r.size()-1]][0];
        for(int i = 0; i < r.size()-1; i++){
            dis += map[r[i]][r[i+1]];
        }
        return dis;
    }

    bool Accept_Metropolis(double t,int d_old, int d_new){  //Metropolis算法
        if(d_new <= d_old){
            return true;
        }else{
            double roulette = (double)rand() / (double)RAND_MAX;
            double p = exp((d_old - d_new)/t);
            return (roulette <= p);
        }
    }

    double Cooling(double t){               //冷却函数
        return 0.9 * t;
    }

    void Simulated_Annealing_TSP(){         //模拟退火算法
        int times = 0;
        int same = 0;
        while(temperature > 0.00001 || same < 2){                    //停止准则：same < 2 or temperature > 0.00001
            bool changeFlag = false;
            for(int i = 0; i < Length; i++){
                vector<int> route_new(route);
                Route_Generate(route_new);
                int new_distance = Distance(route_new);
                if(Accept_Metropolis(temperature,route_distance,new_distance)){
                    route_distance = new_distance;
                    route.swap(route_new);
                    changeFlag = true;
                }
                vector<int>().swap(route_new);
            }
            temperature = Cooling(temperature);
            if(!changeFlag){
                same++;
            }else{
                same = 0;
            }
            times++;
            cout << times << " " << temperature << " " << same << endl;
        }
        return;
    }

public:
    vector<vector<int>> map;
    vector<int> route;
    int route_distance;
    double temperature;
    int Length;
};


int main(){
    vector<vector<int>> map;
    ReadFile(map);

    unsigned seed = time(0);
    srand(seed);

    int cities = map.size();    //初始路径

    vector<int> route_ini(cities-1);
    for(int i = 0; i < cities-1; i++){
        route_ini[i] = i+1;
    }

    double temp_ini = 100;      //初始温度
    int len_ini = 20000;        //Mapkob链长

    SA_TSP sa(map,route_ini,temp_ini,len_ini);

    sa.Simulated_Annealing_TSP();

    vector<int> opt_route;
    CompareWithOptimum(sa.route,opt_route);

    cout << "The SA-optimum route is:" << endl;
    for(int i = 0; i < sa.route.size(); i++){
        cout << sa.route[i] << " ";
    }
    cout << endl;
    cout << "The SA-optimum route's distance is:" << endl << sa.route_distance << endl;

    cout << "The optimum route is:" << endl;
    for(int i = 0; i < opt_route.size(); i++){
        cout << opt_route[i] << " ";
    }
    cout << endl;
    cout << "The optimum route's distance is:" << endl << sa.Distance(opt_route) << endl;

    return 0;
}