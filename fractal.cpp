#include <cstdint>
#include <complex>
#include <numbers>
#include <iostream>
#include <fstream>
#include <vector>

using cx = std::complex<long double>;


struct PPM {
    std::vector<std::vector<uint32_t>> map;
    size_t n, m;

    PPM(size_t n, size_t m) : n(n), m(m), map(n, std::vector<uint32_t>(m)) {}

};

std::ostream& operator<<(std::ostream& out, const PPM& ppm)  {
    out << "P6\n" << ppm.m << ' ' << ppm.n << "\n255\n";
    for (const auto& line : ppm.map) {
        for (auto color : line) {
            char blue = color % 256;
            color >>= 8;
            char green = color % 256;
            color >>= 8;
            char red = color % 256;
            out << red << green << blue;
        }
    }
    return out;
}

struct Point {
    cx x, y;
};


int main(int argc, char** argv) {
    using namespace std::complex_literals;
    using namespace std::numbers;
    size_t n, m;
    std::vector<cx> poly = {1, 0, 0, -1};
    std::vector<cx> roots = {1, pow(e_v<cx>, 1il * pi_v<cx> * 2.0l / 3.0l), pow(e_v<cx>, 1il * pi_v<cx> * 4.0l / 3.0l)};
    std::vector<uint32_t> r_colors = {0xFF0000, 0x00FF00, 0x0000FF};
    
    if (argc >= 2) {
        n = atoi(argv[1]);
    } else {
        n = 100;
    }
    if (argc >= 2) {
        m = atoi(argv[2]);
    } else {
        m = 100;
    }
    PPM ppm(n, m);
    uint32_t c = 0;
    for (auto& line :ppm.map) {
        for (auto& color : line) {
            color = c * 0x010101;
        }
        c += 255 / n;
    }
    std::fstream file("test.ppm", std::fstream::out);
    file << ppm;
}
