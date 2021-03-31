#include "stdio.h"
#include <iostream>
#include <tuple>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <vector>
#include <assert.h>

#define EPSILON 1e-6
using namespace std;

void process(float percent);

int main() {
//    /CLionProjects/SolarEnergy_Chier/OutputFiles/RayDensityExperiment/-150_0_0

    string output_file_path = "/home/sth/CLionProjects/CompareData_SE_Sth/output/compare_result_after_smooth_1024_918.csv";
    string parent_directory = "918_2.57_-877";
    fstream out_file(output_file_path, fstream::out);
    out_file << "ray_density,gt_ray_density,max,sum,rms,ave,pixel" << endl;
    vector<float> result;

    fstream input_file(
            "/home/sth/CLionProjects/SolarEnergy_Chier/OutputFiles/RayDensityExperiment/" + parent_directory +
            "/2048_0_receiver_after_smooth.csv");

    assert(input_file.good());

    stringstream ss;
    ss << input_file.rdbuf();
    input_file.close();
    string str;
    getline(ss, str);
    while (getline(ss, str)) {
        int index = str.find_last_of(',');
        float val = atof(str.substr(index + 1).c_str());
        result.push_back(val);
    }

    float percent = 0.0f;
    int cnt = 1024;
    process(percent);

    for (int i = 1; i <= cnt; i++) {
        string file_path =
                "/home/sth/CLionProjects/SolarEnergy_Chier/OutputFiles/RayDensityExperiment/" + parent_directory + "/" +
                to_string(i) + "_0_receiver_after_smooth.csv";
        fstream file(file_path);

        if (!file.good()) {
            cerr << "File " << file_path << " opened fail." << endl;
            continue;
        }

        stringstream ss;
        ss << file.rdbuf();
        file.close();

        int r = 0;
        string str;
        getline(ss, str);
        float max_dif = -FLT_MAX;
        float sum_dif = 0.0f;
        float rms_dif = 0.0f;
        float ave_dif = 0.0f;
        int pixel = 0;

        while (getline(ss, str)) {
            int index = str.find_last_of(',');
            float val = atof(str.substr(index + 1).c_str());
            float dif = abs(val - result[r++]);

            pixel++;

            rms_dif += (dif * dif);
            max_dif = max(max_dif, dif);
            sum_dif += dif;
        }
        rms_dif /= pixel;
        rms_dif = sqrt(rms_dif);
        ave_dif = sum_dif / pixel;

        out_file << i << ",2048," << max_dif << "," << sum_dif << "," << rms_dif << "," << ave_dif << "," << pixel
                 << endl;

        if (static_cast<float>(i) / cnt >= percent + 0.1f) {
            percent = static_cast<float>(i) / cnt;
            process(percent);
        }
    }
    out_file.close();
    process(1.0f);
    return 0;
}

void process(float percent) {
    percent = min(1.0f, percent);
    percent = max(0.0f, percent);

    int num = static_cast<int>(percent / 0.1f);
    cout << "[";
    for (int i = 0; i < num; i++) {
        cout << "-";
    }
    cout << ">";
    for (int i = num; i < 10; i++) {
        cout << " ";
    }

    cout << static_cast<int>(percent * 100) << "%" << endl;

}