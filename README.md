# tiny_renderer
Tiny rendered tutorial from https://github.com/ssloy/tinyrenderer/


## Profiling

g++ -ggdb -g -pg -O0  *.cpp -o app && ./app &&  gprof app gmon.out