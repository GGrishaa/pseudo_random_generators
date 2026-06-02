GXX=g++


all: gen check_chi2 make_bin nist fix_time plot

gen: generation
	mkdir -p generated_data/1 generated_data/2 generated_data/3
	touch generated_data/1/data.txt generated_data/1/stats.txt
	touch generated_data/2/data.txt generated_data/2/stats.txt
	touch generated_data/3/data.txt generated_data/3/stats.txt
	./generation

check_chi2: chi2
	touch generated_data/1/chi2.txt generated_data/2/chi2.txt generated_data/3/chi2.txt
	./chi2
	echo "$$(grep -e "1$$" generated_data/1/chi2.txt | wc -l | awk '{print $$1}')/20" >> generated_data/1/chi2.txt
	echo "$$(grep -e "1$$" generated_data/2/chi2.txt | wc -l | awk '{print $$1}')/20" >> generated_data/2/chi2.txt
	echo "$$(grep -e "1$$" generated_data/3/chi2.txt | wc -l | awk '{print $$1}')/20" >> generated_data/3/chi2.txt

make_bin: to_bin
	touch generated_data/1/bin_data.txt generated_data/2/bin_data.txt generated_data/3/bin_data.txt
	./to_bin

nist: nist_test
	touch generated_data/1/nist.txt generated_data/2/nist.txt generated_data/3/nist.txt
	./nist_test

fix_time: timer
	mkdir -p time/1 time/2 time/3 time/4
	touch time/1/time.txt time/2/time.txt time/3/time.txt time/4/time.txt
	./timer

generation: generation.o generators.o
	$(GXX) $^ -o $@

generation.o: generation.cpp generators.hpp
	$(GXX) -c $< -o $@

generators.o: generators.cpp generators.hpp
	$(GXX) -c $< -o $@

chi2: chi2.cpp
	$(GXX) $< -o $@

to_bin: to_bin.cpp
	$(GXX) $< -o $@

nist_test: nist_test.cpp
	$(GXX) $< -o $@

timer: timer.o generators.o
	$(GXX) $^ -o $@

timer.o: timer.cpp generators.hpp
	$(GXX) -c $< -o $@

plot: plot.py
	python3 plot.py

clean:
	rm -f generation chi2 to_bin nist_test timer *.o
	rm -f generated_data/1/*.txt generated_data/2/*.txt generated_data/3/*.txt
	rm -f time/1/*.txt time/2/*.txt time/3/*.txt time/4/*.txt time/*.png
	rm -rf docs/*
	clear

rebuild: clean all


clang:
	touch .clang-format
	echo "---" > .clang-format
	echo "BasedOnStyle: Google" >> .clang-format
	clang-format -i *.cpp *.hpp
	rm -f .clang-format
	clear

dox:
	doxygen ./doxygen_config
	open docs/html/index.html

.PHONY = all clean clang dox rebuild gen check_chi2 make_bin nist fix_time plot
