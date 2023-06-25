#include <iostream>
#include <fstream>
#include <omp.h>

using namespace std;

struct Color {
    int red;
    int green;
    int blue;
};

int main() {
    int size = 2000; // wielkość obrazu
    int max_iter = 5000; // maksymalna liczba iteracji
    double x_min = -1.5; // minimalna wartość x
    double x_max = 1.5; // maksymalna wartość x
    double y_min = -1.5; // minimalna wartość y
    double y_max = 1.5; // maksymalna wartość y
    double dx = (x_max - x_min) / size; // krok x
    double dy = (y_max - y_min) / size; // krok y
    Color** image = new Color * [size];
    for (int i = 0; i < size; i++) {
        image[i] = new Color[size];
    }

    // inicjalizacja obrazu
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            image[i][j].red = 0;
            image[i][j].green = 0;
            image[i][j].blue = 0;
        }
    }

    double t_start = omp_get_wtime();

    // Parametry do zmiany
    int num_threads = 16; // Liczba wątków

    // generowanie grafiki zbioru Julii
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < size; i++) {
        double y = y_max - i * dy;
        for (int j = 0; j < size; j++) {
            double x = x_min + j * dx;
            double zx = x; // Wartość zx to współrzędna x piksela
            double zy = y; // Wartość zy to współrzędna y piksela
            int iter = 0;
            while (zx * zx + zy * zy < 4.0 && iter < max_iter) {
                double zx_new = zx * zx - zy * zy + 0.285; // Stała wartość dla zbioru Julii (-0.8, 0)
                double zy_new = 2.0 * zx * zy + 0.01; // Stała wartość dla zbioru Julii (0, 0.156)
                zx = zx_new;
                zy = zy_new;
                iter++;
            }
            if (iter == max_iter) {
                image[i][j].red = 255;
                image[i][j].green = 255;
                image[i][j].blue = 255;
            }
            else {
                // Obliczenie wartości koloru w zależności od liczby iteracji
                double blue_value = iter / static_cast<double>(max_iter) * 15000;
                double red_value = iter / static_cast<double>(max_iter) * 6000;
                double green_value = iter / static_cast<double>(max_iter) * 8000;
                image[i][j].red = static_cast<int>(red_value);
                image[i][j].green = static_cast<int>(green_value);
                image[i][j].blue = static_cast<int>(blue_value);
            }
        }
    }

    double t_end = omp_get_wtime();
    cout << "Czas wykonania " << t_end - t_start << "s" << endl;

    // zapis obrazu do pliku .ppm
    ofstream outfile("julia_set.ppm");
    outfile << "P3" << endl;
    outfile << size << " " << size << endl;
    outfile << "255" << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            outfile << image[i][j].red << " " << image[i][j].green << " " << image[i][j].blue << " ";
        }
        outfile << endl;
    }
    outfile.close();

    // zwolnienie pamięci
    for (int i = 0; i < size; i++) {
        delete[] image[i];
    }
    delete[] image;

    return 0;
}
