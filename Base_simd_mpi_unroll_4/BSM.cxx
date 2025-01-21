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
#include <mpi.h>

#include <arm_acle.h>
#include <cblas.h>
#include <arm_neon.h>
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
    float64x2_t half= vdupq_n_f64(0.5);
    // Not affected by the random number
    float64x2_t drift = vmulq_f64(
	    vsubq_f64(
		    vsubq_f64(r_vec, q_vec),
		    vmulq_f64( half, vmulq_f64(sigma_vec, sigma_vec))
		    ),
	    T_vec
    );
    float64x2_t sub_diffusion = vmulq_f64(sigma_vec, vdupq_n_f64(sqrt(T)));
    float64x2_t zero =  vdupq_n_f64(0.0);
    for (ui64 i = 0; i < num_simulations; i += 8) {
        // Generate random numbers (4 per iteration)
        float64x2_t Z1 = {gaussian_box_muller(), gaussian_box_muller()};
	float64x2_t Z2 = {gaussian_box_muller(), gaussian_box_muller()};
	float64x2_t Z3 = {gaussian_box_muller(), gaussian_box_muller()};
	float64x2_t Z4 = {gaussian_box_muller(), gaussian_box_muller()};

        float64x2_t diffusion1 = vmulq_f64(sub_diffusion, Z1);
        float64x2_t diffusion2 = vmulq_f64(sub_diffusion, Z2);
	float64x2_t diffusion3 = vmulq_f64(sub_diffusion, Z3);
        float64x2_t diffusion4 = vmulq_f64(sub_diffusion, Z4);

        float64x2_t exponent1 = vaddq_f64(drift, diffusion1);
        float64x2_t exponent2 = vaddq_f64(drift, diffusion2);
	float64x2_t exponent3 = vaddq_f64(drift, diffusion3);
        float64x2_t exponent4 = vaddq_f64(drift, diffusion4);

        float64x2_t ST1 = vmulq_f64(S0_vec,
		       	float64x2_t{exp(vgetq_lane_f64(exponent1, 0)), exp(vgetq_lane_f64(exponent1, 1))});
        float64x2_t ST2 = vmulq_f64(S0_vec,
		       	float64x2_t{exp(vgetq_lane_f64(exponent2, 0)), exp(vgetq_lane_f64(exponent2, 1))});
	float64x2_t ST3 = vmulq_f64(S0_vec,
		       	float64x2_t{exp(vgetq_lane_f64(exponent3, 0)), exp(vgetq_lane_f64(exponent3, 1))});
        float64x2_t ST4 = vmulq_f64(S0_vec,
		       	float64x2_t{exp(vgetq_lane_f64(exponent4, 0)), exp(vgetq_lane_f64(exponent4, 1))});
 
	float64x2_t payoff = vaddq_f64(vaddq_f64(vmaxq_f64(vsubq_f64(ST1, K_vec), zero),
			vmaxq_f64(vsubq_f64(ST2, K_vec), zero)),
		    vaddq_f64(vmaxq_f64(vsubq_f64(ST3, K_vec), zero),
			vmaxq_f64(vsubq_f64(ST4, K_vec), zero)));
        sum_payoffs += vgetq_lane_f64(payoff, 0) + vgetq_lane_f64(payoff, 1);
    }
    return exp(-r * T) * (sum_payoffs / num_simulations);
}

#include <cmath> // Pour std::erf et std::sqrt
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc != 3) {
	if(rank == 0)std::cerr << "Usage: " << argv[0] << " <num_simulations> <num_runs>" << std::endl;
	MPI_Finalize();
        return 1;
    }

    ui64 num_simulations = std::stoull(argv[1]);
    ui64 num_runs        = std::stoull(argv[2]);
    if (size == 0) {
        std::cerr << "Error: MPI size is zero." << std::endl;
        MPI_Finalize();
        return 1;
    }
    ui64 runs_per_process = num_runs/size;

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
    if(rank == 0){
        std::cout << "Global initial seed: " << global_seed << "      argv[1]= " << argv[1] << "     argv[2]= " << argv[2] <<  std::endl;
    }
    double local_sum=0.0;
    double global_sum=0.0;
    double t1=dml_micros();
    for (ui64 run = 0; run < runs_per_process; ++run) {
        local_sum+= black_scholes_monte_carlo(S0, K, T, r, sigma, q, num_simulations);
    }
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    double t2=dml_micros();
    if( rank == 0)
    	std::cout << std::fixed << std::setprecision(6) << " value= " << global_sum/num_runs << " in " << (t2-t1)/1000000.0 << " seconds" << std::endl;
    MPI_Finalize(); 
    return 0;
}
