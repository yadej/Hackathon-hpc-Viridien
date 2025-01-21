echo "using precompute + std thread to share load"
echo "gcc compiler"
g++ -O -pthread BSM.cxx -o BSM
for i in {1..10};
do
	echo "Run $i"
	./BSM 100 1000;
done
echo "Arm compiler"
armclang++ -O -pthread BSM.cxx -o BSMarm
for i in {1..10};
do
	echo "Run $i"
	./BSMarm 100 1000;
done
