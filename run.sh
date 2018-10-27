rm -fr /tmp/test_engine/*
rm -fr lib
rm -fr test/test

make clean all
make
cd test
g++ -std=c++11 -o test -g -I.. test.cc -L../lib -lengine -lpthread -lz
./test
