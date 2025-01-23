echo "Computing with openmp on the black_sholes function"
echo "gcc compiler"
g++ -O -fopenmp BSM.cxx -o BSM
for i in {1..10};
do
	echo "Run $i"
	./BSM 100000 100;
done
echo "Arm compiler"
armclang++ -O -fopenmp BSM.cxx -o BSMarm
for i in {1..10};
do
	echo "Run $i"
	./BSMarm 100000 100;
done
