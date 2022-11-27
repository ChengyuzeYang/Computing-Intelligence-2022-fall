#include<vector>
#include<iostream>
#include<ctime>
#include<cstdlib>
#include<algorithm>
#include<cmath>

using namespace std;

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

// Benchmark
double Tablet(vector<double> x){
    double y = pow(10,6) * x[0] * x[0];
    for(int i = 1; i < x.size(); i++){
        y += x[i] * x[i];
    }
    return y;
}

double Quadric(vector<double> x){
    double y = 0;
    for(int i = 0; i < x.size(); i++){
        double part_sum = 0;
        for(int j = 0; j <= i; j++){
            part_sum += x[j];
        }
        y += part_sum * part_sum;
    }
    return y;
}

double Rosenbrock(vector<double> x){
    double y = 0;
    for(int i = 0; i < x.size()-1; i++){
        y += 100 * pow((x[i+1] - pow(x[i],2)),2) + pow(x[i]-1,2);
    }
    return y;
}

double Griewank(vector<double> x){
    double y = 1;
    double tmp1 = 0;
    double tmp2 = 1;
    for(int i = 0; i < x.size(); i++){
        tmp1 += pow(x[i],2);
        tmp2 *= cos(x[i]/sqrt(i+1));
    }
    y += tmp1 / 4000 - tmp2;
    return y;
}

double Rastrigrin(vector<double> x){
    double y = 0;
    double A = 10;
    for(int i = 0; i < x.size(); i++){
        y += pow(x[i],2) - A * cos(2 * M_PI * x[i]) + A;
    }
    return y;
}

double Schaffers_f7(vector<double> x){
    double y = 0;
    for(int i = 0; i < x.size()-1; i++){
        double tmp = pow(x[i],2) + pow(x[i+1],2);
        y += pow(tmp,0.25) * (sin(50 * pow(tmp,0.1)) + 1);
    }
    return y;
}

bool cmp(const vector<double> &a, const vector<double> &b){
    return Schaffers_f7(a) < Schaffers_f7(b);
}


class PSO
{
public:
    PSO(int d, int p, int times, int gen, double f_b, double (*f)(vector<double>)):dim(d),num_p(p),times(times),gene(gen),func_bound(f_b),func(f){};
    ~PSO(){};

    double U(double a, double b){
        double ret = (double)rand() / double(RAND_MAX);
        ret = ret * (b - a) + a;
        return ret;
    }

    void Init(){
        // clear and reset
        vector<vector<double>> ().swap(x);
        vector<vector<double>> ().swap(v);
        vector<vector<double>> ().swap(pbest);
        vector<double> ().swap(pfitness);
        vector<double> ().swap(gbest);
        gfitness = (double)INFINITY;
        w = 0.95;
        Vmax = U(0.1,1) * func_bound;

        for(int i = 0; i < num_p; i++){
            vector<double> xi;
            vector<double> vi;
            for(int j = 0; j < dim; j++){
                double xij = U(-1 * func_bound, func_bound);
                xi.push_back(xij);
                vi.push_back(0);
            }
            x.push_back(xi);
            v.push_back(vi);
            pbest.push_back(xi);
            double tmp = func(xi);
            pfitness.push_back(tmp);
            if(tmp < gfitness){
                gfitness = tmp;
                gbest.swap(xi);
            }
        }
        return;
    }

    void Particle_Swarm_Optimization(){
        for(int t = 0; t < times; t++){
            Init();
            for(int round = 0; round < gene; round++){
                for(int i = 0; i < num_p; i++){
                    double tmp = func(x[i]);
                    if(tmp < pfitness[i]){
                        pbest[i].assign(x[i].begin(),x[i].end());
                        pfitness[i] = tmp;
                    }
                    if(tmp < gfitness){
                        gbest.assign(x[i].begin(),x[i].end());
                        gfitness = tmp;
                    }
                }
                for(int i = 0; i < num_p; i++){
                    for(int j = 0; j < dim; j++){
                        v[i][j] = w * v[i][j] + c1 * U(0,1) * (pbest[i][j] - x[i][j]) + c2 * U(0,1) * (gbest[j] - x[i][j]);
                        if(abs(v[i][j]) > Vmax){
                            v[i][j] = v[i][j]/abs(v[i][j]) * Vmax;
                        }
                        x[i][j] += v[i][j];
                    }
                }
                w -= (0.95-0.4)/(double)gene;
            }
            print_double(gbest);
            cout << gfitness << endl;
        }
        return;
    }

private:
    int dim;        //dimension of a particle
    int num_p;      //number of particles
    int gene;       //number of generation
    int times = 1;  // times of algorithm
    double func_bound;
    double (*func)(vector<double>);
    
    double w = 0.95;
    double c1 = 1.4;
    double c2 = 1.4;
    
    vector<vector<double>> x;
    vector<vector<double>> v;
    vector<vector<double>> pbest;
    vector<double> pfitness;
    vector<double> gbest;
    double gfitness;
    double Vmax;
};

class MAEPSO
{
public:
    MAEPSO(int d, int p, int t, int iter, double f_b, double (*f)(vector<double>)):dim(d),num_p(p),times(t),iter(iter),func_bound(f_b),func(f){};
    ~MAEPSO(){};

    double U(double a, double b){
        double ret = (double)rand() / double(RAND_MAX);
        ret = ret * (b - a) + a;
        return ret;
    }

    double random_normal_distribution(){    // Box-Muller
        double u1 = U(0,1);
        double u2 = U(0,1);
        int randi = rand() % 2;
        if(randi){
            return cos(2 * M_PI * u1) * sqrt(-2 * log(u2));
        }else{
            return sin(2 * M_PI * u1) * sqrt(-2 * log(u2));
        }
    }

    void Init(){
        // clear and reset
        vector<vector<double>> ().swap(x);
        vector<vector<double>> ().swap(v);
        vector<vector<double>> ().swap(pbest);
        vector<double> ().swap(pfitness);
        vector<double> ().swap(gbest);
        gfitness = (double)INFINITY;
        w = 0.95;
        sigma_0 = func_bound;
        Vmax = U(0.1,1) * func_bound;
        vector<double> ().swap(T);
        vector<int> ().swap(G);
        vector<double> ().swap(sigma);

        for(int i = 0; i < num_p; i++){
            vector<double> xi;
            vector<double> vi;
            for(int j = 0; j < dim; j++){
                double xij = U(-1 * func_bound, func_bound);
                xi.push_back(xij);
                vi.push_back(0);
            }
            x.push_back(xi);
            v.push_back(vi);
            pbest.push_back(xi);
            double tmp = func(xi);
            pfitness.push_back(tmp);
            if(tmp < gfitness){
                gfitness = tmp;
                gbest.swap(xi);
            }
        }

        for(int i = 0; i < dim; i++){
            T.push_back(T_init);
            G.push_back(0);
        }
        for(int i = 0; i < M; i++){
            sigma.push_back(sigma_0);
        }
        return;
    }

    void Update_fitness(){
        for(int i = 0; i < num_p; i++){
            double tmp = func(x[i]);
            if(tmp < pfitness[i]){
                pbest[i].assign(x[i].begin(),x[i].end());
                pfitness[i] = tmp;
            }
            if(tmp < gfitness){
                gbest.assign(x[i].begin(),x[i].end());
                gfitness = tmp;
            }
        }
        return;
    }

    void Update_v_x(){
        for(int i = 0; i < num_p; i++){
            for(int j = 0; j < dim; j++){
                v[i][j] = w * v[i][j] + c1 * U(0,1) * (pbest[i][j] - x[i][j]) + c2 * U(0,1) * (gbest[j] - x[i][j]);

                if(v[i][j] < T[j]){
                    G[j] += 1;
                    vector<double> randn;
                    for(int m = 0; m < M; m++){
                        randn.push_back(random_normal_distribution());
                    }
                    double f_min = (double)INFINITY;
                    int idx_min = -1;
                    double xij_cache = x[i][j];
                    for(int m = 0; m < M; m++){
                        x[i][j] += randn[m] * sigma[m];
                        double f = func(x[i]);
                        if(f < f_min){
                            f_min = f;
                            idx_min = m;
                        }
                        x[i][j] = xij_cache;
                    }
                    double v_threshold = U(-1,1) * Vmax;
                    x[i][j] += v_threshold;
                    if(f_min < func(x[i])){
                        v[i][j] = randn[idx_min] * sigma[idx_min];
                    }else{
                        v[i][j] = v_threshold;
                    }
                    x[i][j] = xij_cache;
                }
                x[i][j] += v[i][j];
            }
        }
        return;
    }

    double FitX_idx(int M_idx){
        double sum = 0;
        for(int i = M_idx*P; i < (M_idx+1) * P; i++){
            sum += func(x[i]);
        }
        return sum / P;
    }

    void Update_sigma(){
        vector<double> FitX;

        double FitX_max = -1 * (double)INFINITY;
        double FitX_min = (double)INFINITY;
        double FitX_sum = 0;
        for(int i = 0; i < M; i++){
            double Fitxi = FitX_idx(i);
            FitX.push_back(Fitxi);
            FitX_max = (Fitxi > FitX_max) ? Fitxi : FitX_max;
            FitX_min = (Fitxi < FitX_min) ? Fitxi : FitX_min;
            FitX_sum += Fitxi;
        }
        
        for(int i = 0; i < M; i++){
            sigma[i] *= exp((M * FitX[i] - FitX_sum) / (FitX_max - FitX_min));
            while(sigma[i] > (func_bound / 2.0)){
                sigma[i] = abs(func_bound / 2.0 - sigma[i]);
            }
        }
        vector<double> ().swap(FitX);
        vector<vector<double>> ().swap(sorted_x);
        return;
    }

    void Update_T(){
        for(int i = 0; i < dim; i++){
            if(G[i] > K1){
                G[i] = 0;
                T[i] /= (double)K2;
            }
        }
        return;
    }

    void Multiscale_Cooperative_Mutatingly_AEPSO(){
        for(int t = 0; t < times; t++){
            Init();
            for(int round = 0; round < iter; round++){
                Update_fitness();
                Update_v_x();
                Update_sigma();
                Update_T();
            }
            print_double(gbest);
            cout << gfitness << endl;
        }
        return;
    }

private:
    int dim;
    int num_p;
    int iter;
    int times = 1;

    double func_bound;
    double (*func)(vector<double>);

    vector<vector<double>> x;
    vector<vector<double>> v;
    vector<vector<double>> pbest;
    vector<double> pfitness;
    vector<double> gbest;
    double gfitness;

    double w = 0.95;
    double c1 = 1.4;
    double c2 = 1.4;
    double Vmax;

    vector<vector<double>> sorted_x;

    int M = 5;
    int P = num_p / M;
    vector<double> T;   // threshold
    vector<int> G;
    vector<double> sigma;
    double sigma_0;
    double T_init = 0.5;
    double K1 = 5.0;
    double K2 = 10.0;
};

int main(){
    unsigned int seed = time(0);
    srand(seed);

    int dimension = 30;
    int num_particles = 20;
    int generation = 6000;
    double bound = 100;
    int times = 1;
    PSO pso(dimension,num_particles,times,generation,bound,Schaffers_f7);
    pso.Particle_Swarm_Optimization();
    MAEPSO maepso(dimension,num_particles,times,generation,bound,Schaffers_f7);
    maepso.Multiscale_Cooperative_Mutatingly_AEPSO();
    return 0;
}