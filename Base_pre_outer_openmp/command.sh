echo "Omp for loop in the outer layer for the simulation"
echo "gcc compiler"
g++ -O BSM.cxx -o BSM
for i in {1..10};
do
	echo "Run $i"
	./BSM 100 100000;
done
echo "Arm compiler"
armclang++ -O BSM.cxx -o BSMarm
for i in {1..10};
do
	echo "Run $i"
	./BSMarm 100 100000;
done
