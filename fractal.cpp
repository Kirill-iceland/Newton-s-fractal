#include <cstdint>
#include <complex>
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
    size_t n, m;
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
