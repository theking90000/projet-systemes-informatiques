# Le script est exécuté directement après le redémarrage.

function tests() {
    echo "Test $1 input.txt"
    perf stat -r 10 -d ./bin/math_suite --input input.txt > /dev/null
    sleep 10m

    echo "Test $1 input.txt only_longest"
    perf stat -r 10 -d ./bin/math_suite --input input.txt --only-longest > /dev/null
    sleep 10m

    echo "Test $1 input_slow.txt"
    perf stat -r 10 -d ./bin/math_suite --input input_slow.txt > /dev/null
    sleep 10m

    echo "Test $1 input_slow.txt only_longest"
    perf stat -r 10 -d ./bin/math_suite --input input_slow.txt --only-longest > /dev/null
    sleep 10m
}

function test_python() {
    echo "Test PYTHON input.txt"
    perf stat -r 10 -d python math_suite.py --input input.txt > /dev/null
    sleep 10m

    echo "Test PYTHON input.txt only_longest"
    perf stat -r 10 -d python math_suite.py --input input.txt --only-longest > /dev/null
    sleep 10m

    echo "Test PYTHON input_slow.txt"
    perf stat -r 10 -d python math_suite.py --input input_slow.txt > /dev/null
    sleep 10m

    echo "Test PYTHON input_slow.txt only_longest"
    perf stat -r 10 -d python math_suite.py --input input_slow.txt --only-longest > /dev/null
    sleep 10m
}

sleep 10m

make clean all
tests "SOLVE" > result_solve.txt 2>&1

make clean all FORK=1
tests "FORK" > result_fork.txt 2>&1

make clean all THREAD=1
tests "THREAD" > result_thread.txt 2>&1

test_python > result_python.txt 2>&1
