echo "NEON Simd in place of normal loop"
echo "gcc compiler"
# J'arrive pas à faire marcher openblas acfl sur g++
g++ -O -larmpl BSM.cxx -o BSM -lboost_random
for i in {1..10};
do
	echo "Run $i"
	./BSM 1000 100000;
done
echo "Arm compiler"
armclang++ -O -larmpl BSM.cxx -o BSMarm -lboost_random
for i in {1..10};
do
	echo "Run $i"
	./BSMarm 1000 100000;
done
