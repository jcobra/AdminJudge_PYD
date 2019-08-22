all: bin/AdminJudge.so clean install
.PHONY: all

#AdminJudge
bin/AdminJudge.so: ./main.o ./AdminRegionMgr.o
	g++ -shared -o bin/AdminJudge.so ./main.o ./AdminRegionMgr.o \
	-I/usr/local/python2.7/include/python2.7 -I./utility/jsoncpp/include \
	./utility/jsoncpp/lib/libjson_linux-gcc-4.4.5_libmt.a \
	-lpython2.7 -lrt -pthread -ldl -lutil

#main
main.o: ./main.cpp 
	g++ -I/usr/local/python2.7/include/python2.7 -I./utility/jsoncpp/include -c ./main.cpp -o ./main.o

#AdminRegionMgr
AdminRegionMgr.o: ./AdminRegionMgr.cpp
	g++ -c ./AdminRegionMgr.cpp -o ./AdminRegionMgr.o	

#clean
clean:
    #@echo "clean getmap project"
	rm -rf ./*.o  
	#@echo "clean completed"
.PHONY: clean

#install
#install:
#	cp bin/*.so /usr/local/apache2/fcgi-bin
