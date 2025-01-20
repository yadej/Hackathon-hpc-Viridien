echo "Computing with openmp on the black_sholes function with precompute constant"
g++ -O -fopenmp BSM.cxx -o BSM
for i in {1..10};
do
	echo "Run $i"
	./BSM 1000 1000;
done
