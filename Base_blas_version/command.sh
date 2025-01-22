echo "some vector optimisation"
echo "gcc compiler"
g++ -O -larmpl BSM.cxx -o BSM
for i in {1..10};
do
	echo "Run $i"
	./BSM 100 100000;
done
echo "Arm compiler"
armclang++ -O -larmpl BSM.cxx -o BSMarm
for i in {1..10};
do
	echo "Run $i"
	./BSMarm 100 100000;
done
echo "O3 opt"
echo "gcc compiler"
g++ -O3 -larmpl BSM.cxx -o BSM
for i in {1..10};
do
	echo "Run $i"
	./BSM 100 100000;
done
echo "Arm compiler"
armclang++ -O3 -larmpl BSM.cxx -o BSMarm
for i in {1..10};
do
	echo "Run $i"
	./BSMarm 100 100000;
done
