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

#if (defined(__clang__) || defined(__GCC__)) && (defined(__ARM_NEON) || defined(__aarch64__))
/* compatible compiler, targeting arm neon */
#include <arm_neon.h>
#include <arm_acle.h>
#endif

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
double gaussian_box_muller() {
    static std::mt19937 generator(std::random_device{}());
    static std::normal_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

#include <cmath> // Pour std::erf et std::sqrt
// Function to calculate the Black-Scholes call option price using Monte Carlo method
double black_scholes_monte_carlo(ui64 S0, ui64 K, double T, double r, double sigma, double q, ui64 num_simulations) {
    double sum_payoffs = 0.0;
    // Constants
    float64x2_t S0_vec = vdupq_n_f64(S0);
    float64x2_t K_vec = vdupq_n_f64(K);
    float64x2_t r_vec = vdupq_n_f64(r);
    float64x2_t q_vec = vdupq_n_f64(q);
    float64x2_t sigma_vec = vdupq_n_f64(sigma);
    float64x2_t T_vec = vdupq_n_f64(T);
    for (ui64 i = 0; i < num_simulations; i += 2) {
        // Generate random numbers (2 per iteration)
        float64x2_t Z = {gaussian_box_muller(), gaussian_box_muller()};
        // Stock price at maturity
        float64x2_t drift = vmulq_f64(vsubq_f64(r_vec, q_vec), T_vec);
        float64x2_t diffusion = vmulq_f64(vmulq_f64(sigma_vec, vdupq_n_f64(std::sqrt(T))), Z);
        float64x2_t exponent = vaddq_f64(drift, diffusion);
        float64x2_t ST = vmulq_f64(S0_vec, float64x2_t{std::exp(vgetq_lane_f64(exponent, 0)), std::exp(vgetq_lane_f64(exponent, 1))});
        // Payoff calculation
        float64x2_t payoff = vmaxq_f64(vsubq_f64(ST, K_vec), vdupq_n_f64(0.0));
        // Sum up payoffs
        sum_payoffs += vgetq_lane_f64(payoff, 0) + vgetq_lane_f64(payoff, 1);
    }
    return std::exp(-r * T) * (sum_payoffs / num_simulations);
}

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
