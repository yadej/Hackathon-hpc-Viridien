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
#include <omp.h>

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
float gaussian_box_muller() {
    static std::mt19937 generator(std::random_device{}());
    static std::normal_distribution<float> distribution(0.0, 1.0);
    return distribution(generator);
}

float black_scholes_monte_carlo(ui64 S0, ui64 K, float T, float r, float sigma, float q, ui64 num_simulations) {
    float sum_payoffs = 0.0f;
    // Constants
    float32x4_t S0_vec = vdupq_n_f32(static_cast<float>(S0));
    float32x4_t K_vec = vdupq_n_f32(static_cast<float>(K));
    float32x4_t r_vec = vdupq_n_f32(r);
    float32x4_t q_vec = vdupq_n_f32(q);
    float32x4_t sigma_vec = vdupq_n_f32(sigma);
    float32x4_t T_vec = vdupq_n_f32(T);
    float32x4_t half = vdupq_n_f32(0.5f);

    // Not affected by the random number
    float32x4_t drift = vmulq_f32(
        vsubq_f32(
            vsubq_f32(r_vec, q_vec),
            vmulq_f32(half, vmulq_f32(sigma_vec, sigma_vec))
        ),
        T_vec
    );
    float32x4_t sub_diffusion = vmulq_f32(sigma_vec, vdupq_n_f32(sqrtf(T)));
    float32x4_t zero = vdupq_n_f32(0.0f);

    for (ui64 i = 0; i < num_simulations; i += 4) {
        // Generate random numbers (4 per iteration)
        float32x4_t Z = {gaussian_box_muller(), gaussian_box_muller(), gaussian_box_muller(), gaussian_box_muller()};
        // Stock price at maturity
        float32x4_t diffusion = vmulq_f32(sub_diffusion, Z);
        float32x4_t exponent = vaddq_f32(drift, diffusion);
        float32x4_t ST = vmulq_f32(S0_vec,
            float32x4_t{expf(vgetq_lane_f32(exponent, 0)), expf(vgetq_lane_f32(exponent, 1)),
                        expf(vgetq_lane_f32(exponent, 2)), expf(vgetq_lane_f32(exponent, 3))});
        // Payoff calculation
        float32x4_t payoff = vmaxq_f32(vsubq_f32(ST, K_vec), zero);
        // Sum up payoffs
        sum_payoffs += vgetq_lane_f32(payoff, 0) + vgetq_lane_f32(payoff, 1)
                     + vgetq_lane_f32(payoff, 2) + vgetq_lane_f32(payoff, 3);
    }
    return expf(-r * T) * (sum_payoffs / num_simulations);
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
    if(rank==0)
	    std::cout << "Number of process MPI: " << size << "\n";
    ui64 num_sims = num_simulations/size;
    ui64 simulations_per_process = ( rank == size - 1 ) ? num_simulations - num_sims * rank :
	    num_sims;
    // To ensure at least num_simulations in total
    // But since we do more

    // Input parameters
    ui64 S0      = 100;                   // Initial stock price
    ui64 K       = 110;                   // Strike price
    float T     = 1.0;                   // Time to maturity (1 year)
    float r     = 0.06;                  // Risk-free interest rate
    float sigma = 0.2;                   // Volatility
    float q     = 0.03;                  // Dividend yield

    // Generate a random seed at the start of the program using random_device
    std::random_device rd;
    unsigned long long global_seed = rd();  // This will be the global seed
    if(rank == 0){
        std::cout << "Global initial seed: " << global_seed << "      argv[1]= " << argv[1] << "     argv[2]= " << argv[2] <<  std::endl;
    }
    double local_sum=0.0;
    double global_sum=0.0;
    double t1=dml_micros();
    #pragma omp parallel for reduction(+:local_sum)
    for (ui64 run = 0; run < num_runs; ++run) {
        local_sum+= black_scholes_monte_carlo(S0, K, T, r, sigma, q, simulations_per_process);
    }
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    double t2=dml_micros();
    if( rank == 0)
    	std::cout << std::fixed << std::setprecision(6) << " value= " << global_sum/(num_runs * size) << " in " << (t2-t1)/1000000.0 << " seconds" << std::endl;
    MPI_Finalize(); 
    return 0;
}
