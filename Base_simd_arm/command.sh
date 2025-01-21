echo "NEON Simd in place of normal loop"
# echo "gcc compiler"
# J'arrive pas à faire marcher openblas acfl sur g++
# g++ -O -I /tools/openblas/acfl/24.04/include -L /tools/openblas/acfl/24.04/lib/libopenblas.a BSM.cxx -o BSM
#for i in {1..10};
#do
#	echo "Run $i"
#	./BSM 100 100000;
#done
echo "Arm compiler"
armclang++ -O -I /tools/openblas/acfl/24.04/include -L /tools/openblas/acfl/24.04/lib/libopenblas.a BSM.cxx -o BSMarm
for i in {1..10};
do
	echo "Run $i"
	./BSMarm 100 100000;
done
