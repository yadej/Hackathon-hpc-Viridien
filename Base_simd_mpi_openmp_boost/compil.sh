mpic++ -O -march=native -larmpl_mp -fopenmp BSM.cxx -o BSM -lboost_random
mpic++ -O3 -larmpl_mp -march=native -fopenmp BSM.cxx -o BSMwithopt -lboost_random
