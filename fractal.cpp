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

struct Fractal {
    PPM ppm;
    std::vector<cx> poly;
    std::vector<cx> poly_diff;
    std::vector<cx> roots;
    std::vector<uint32_t> r_colors;
    size_t n, m;
    long double max_x, min_x, max_y, min_y;
    long double rad;
    const size_t max_it = 200;

    size_t get_x(cx x) {
        return n - 1 - round((x.imag() - min_y) / (max_y - min_y) * n);
    }

    size_t get_y(cx y) {
        return round((y.real() - min_x) / (max_x - min_x) * m);
    }

    cx get_val(cx x, std::vector<cx>& poly) {
        cx val = 0;
        cx mul = 1;
        for (size_t i = 0; i < poly.size(); i++) {
            val += poly[i] * mul;
            mul *= x;
        }
        return val;
    }

    uint32_t get_color(cx num, size_t it = 0) {
        ++it;
        size_t x = get_x(num);
        size_t y = get_y(num);
        // std::cout << num << ' ' << x << ' ' << y << std::endl;
        if (it > max_it) {
            return 0;
        }
        if (x < n && y < m && x >= 0 && y >= 0 && ppm.map[x][y] != UINT32_MAX) {
            return ppm.map[x][y];
        }
        for (size_t i = 0; i < roots.size(); i++) {
            if (std::abs(num - roots[i]) < rad) {
                if (x < n && y < m && x >= 0 && y >= 0) {
                    ppm.map[x][y] = r_colors[i];
                }
                return r_colors[i];
            }
        }
        // std::cout << get_val(num, poly) << ' ' << get_val(num, poly_diff) << std::endl;
        cx new_num = num - get_val(num, poly) / get_val(num, poly_diff);
        uint32_t color = get_color(new_num, it);
        if (x < n && y < m && x >= 0 && y >= 0) {
            ppm.map[x][y] = color;
        }
        return color;
    }
};


int main(int argc, char** argv) {
    using namespace std::complex_literals;
    using namespace std::numbers;
    size_t n, m;
    std::vector<cx> poly = {-1, 0, 0, 1};
    std::vector<cx> roots = {1, pow(e_v<long double>, 1il * pi_v<long double> * 2.0l / 3.0l), pow(e_v<long double>, 1il * pi_v<long double> * 4.0l / 3.0l)};
    std::vector<uint32_t> r_colors = {0xFF0000, 0x00FF00, 0x0000FF};
    long double rad = 0.1;

    std::vector<cx> poly_diff(poly.size() - 1);
    for (size_t i = 1; i < poly.size(); i++) {
        poly_diff[i - 1] = poly[i] * static_cast<cx>(i);
    }
    
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
    long double max_x = 2.0 * m / n, min_x = -2.0 * m / n, max_y = 2, min_y = -2;
    PPM ppm(n, m);
    for (auto& line :ppm.map) {
        for (auto& color : line) {
            color = UINT32_MAX;
        }
    }

    Fractal fractal(ppm, poly, poly_diff, roots, r_colors, n, m, max_x, min_x, max_y, min_y, rad);
    // uint32_t c = 0;
    // for (auto& line :ppm.map) {
    //     for (auto& color : line) {
    //         color = c * 0x010101;
    //     }
    //     c += 255 / n;
    // }

    size_t next_per = 0;
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            long double x = min_x + (max_x - min_x) * j / m;
            long double y = min_y + (max_y - min_y) * i / n;
            cx num = x + y * 1il;
            fractal.get_color(num);
        }
        if (i == next_per) {
            std::cout << static_cast<double>(i) / n * 100 << '%' << std::endl;
            next_per += n / 10;
        }
    }
    // fractal.get_color(2);

    
    std::fstream file("test.ppm", std::fstream::out);
    file << fractal.ppm;
}
