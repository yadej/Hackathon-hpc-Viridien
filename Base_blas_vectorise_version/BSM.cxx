/* 

    Monte Carlo Hackathon created by Hafsa Demnati and Patrick Demichel @ Viridien 2024
    The code compute a Call Option with a Monte Carlo method and compare the result with the analytical equation of Black-Scholes Merton : more details in the documentation
    
    Compilation : g++ -O BSM.cxx -o BSM

    Exemple of run: ./BSM #simulations #runs

    ./BSM 100 1000000
    Global initial seed: 21852687      argv[1]= 100     argv[2]= 1000000
    value= 5.136359 in 10.191287 seconds

    ./BSM 100 1000000
Global initial seed: 4208275479      argv[1]= 100     argv[2]= 1000000
 value= 5.138515 in 10.223189 seconds

   We want the performance and value for largest # of simulations as it will define a more precise pricing
   If you run multiple runs you will see that the value fluctuate as expected
   The large number of runs will generate a more precise value then you will converge but it require a large computation

   give values for ./BSM 100000 1000000        
               for ./BSM 1000000 1000000
               for ./BSM 10000000 1000000
               for ./BSM 100000000 1000000

   We give points for best performance for each group of runs 
   You need to tune and parallelize the code to run for large # of simulations

*/

#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include <limits>
#include <algorithm>
#include <iomanip>   // For setting precision
#include <cblas.h>

#define ui64 u_int64_t

#include <sys/time.h>
double
dml_micros()
{
        static struct timezone tz;
        static struct timeval  tv;
        gettimeofday(&tv,&tz);
        return((tv.tv_sec*1000000.0)+tv.tv_usec);
}

// Function to generate Gaussian noise using Box-Muller transform
void gaussian_box_muller(std::vector<double> &Z) {
    static std::mt19937 generator(std::random_device{}());
    static std::normal_distribution<double> distribution(0.0, 1.0);
    for( size_t i=0; i < Z.size(); i+=4){
	Z[i] = distribution(generator);
        Z[i+1] = distribution(generator);
	Z[i+2] = distribution(generator);
	Z[i+3] = distribution(generator);
    }
}

// Function to calculate the Black-Scholes call option price using Monte Carlo method
double black_scholes_monte_carlo(ui64 S0, ui64 K, double T, double r, double sigma, double q, ui64 num_simulations) {
    std::vector<double> Z(num_simulations);
    std::vector<double> payoffs(num_simulations);

    gaussian_box_muller( Z );
    double drift = (r - q - 0.5 * sigma * sigma) * T;
    double diffusion = sigma * sqrt(T);

    double sum_payoffs = 0.0;
    for (ui64 i = 0; i < num_simulations; i+=4) {
	double ST1 = S0 * exp( drift + diffusion * Z[i]);
	double ST2 = S0 * exp( drift + diffusion * Z[i+1]);
	double ST3 = S0 * exp( drift + diffusion * Z[i+2]);
	double ST4 = S0 * exp( drift + diffusion * Z[i+3]);

	payoffs[i] = std::max( ST1 - K, 0.0);
       	payoffs[i+1] = std::max( ST2 - K, 0.0); 
	payoffs[i+2] = std::max( ST3 - K, 0.0); 
	payoffs[i+3] = std::max( ST4 - K, 0.0); 

    }

    sum_payoffs = cblas_dasum(num_simulations, payoffs.data(), 1);
    return exp(-r * T) * (sum_payoffs / num_simulations);
}

#include <cmath> // Pour std::erf et std::sqrt
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <num_simulations> <num_runs>" << std::endl;
        return 1;
    }

    ui64 num_simulations = std::stoull(argv[1]);
    ui64 num_runs        = std::stoull(argv[2]);

    // Input parameters
    ui64 S0      = 100;                   // Initial stock price
    ui64 K       = 110;                   // Strike price
    double T     = 1.0;                   // Time to maturity (1 year)
    double r     = 0.06;                  // Risk-free interest rate
    double sigma = 0.2;                   // Volatility
    double q     = 0.03;                  // Dividend yield

    // Generate a random seed at the start of the program using random_device
    std::random_device rd;
    unsigned long long global_seed = rd();  // This will be the global seed

    std::cout << "Global initial seed: " << global_seed << "      argv[1]= " << argv[1] << "     argv[2]= " << argv[2] <<  std::endl;

    double sum=0.0;
    double t1=dml_micros();
    for (ui64 run = 0; run < num_runs; ++run) {
        sum+= black_scholes_monte_carlo(S0, K, T, r, sigma, q, num_simulations);
    }
    double t2=dml_micros();
    std::cout << std::fixed << std::setprecision(6) << " value= " << sum/num_runs << " in " << (t2-t1)/1000000.0 << " seconds" << std::endl;
    return 0;
}
