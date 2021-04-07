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
// todo:: add coefficient of determination

// R^2 = 1 - (SS_res)/(SS_tot)

float getCoefficientOfDetermination(vector<float> &measure, vector<float> &ground_truth) {
    if (measure.size() != ground_truth.size()) {
        cerr << "Error:: measured result size is not equal to ground truth result size." << endl;
        return -1.0f;
    }
    float SS_res = 0.0f;
    float SS_tot = 0.0f;
    float average_y = 0.0f;
    for (int i = 0; i < measure.size(); i++) {
        SS_res += pow(measure[i] - ground_truth[i], 2);
        average_y += measure[i];
    }
    average_y /= measure.size();
    for (int i = 0; i < measure.size(); i++) {
        SS_tot += pow(measure[i] - average_y, 2);
    }

    return 1.0f - SS_res / SS_tot;
}

int main() {

    // only need change "output_file_path" and "parent_directory"
//
//        string output_file_path =
//                "/home/sth/CLionProjects/CompareData_SE_Sth/output/compare_result_after_smooth_1024_" + it + "_200.csv";

    string parent_directory = "/home/sth/CLionProjects/SolarEnergy_Chier/OutputFiles/RayDensityExperiment/p_[100_2.57_0]_g_64_r_320_d_[1_1024]/";
    string pre = "before_smooth_d_";
    string suf = "_r_320.csv";

    string output_file_path =
            "/home/sth/CLionProjects/CompareData_SE_Sth/output/compare_result_before_smooth_experiment_d_[1_1024]_t_320_with_r2.csv";
    fstream out_file(output_file_path, fstream::out);
    out_file << "ray_density,gt_ray_density,max,sum,rms,ave,pixel,r2" << endl;

    // construct ground truth result
    vector<float> ground_truth;
    fstream ground_truth_file(
            parent_directory +
            "before_smooth_d_2048_r_320.csv");
    assert(ground_truth_file.good());
    stringstream ss;
    ss << ground_truth_file.rdbuf();
    ground_truth_file.close();
    string str;
    getline(ss, str);
    while (getline(ss, str)) {
        int index = str.find_last_of(',');
        float val = atof(str.substr(index + 1).c_str());
        ground_truth.push_back(val);
    }
    int cnt = 1024;
    float percent = 0.0f;
    process(percent);
    for (int i = 1; i <= cnt; i++) {
        vector<float> result;

        string file_path =
                parent_directory + pre + to_string(i) + suf;
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
        float R2 = 0.0f;
        while (getline(ss, str)) {
            int index = str.find_last_of(',');
            float val = atof(str.substr(index + 1).c_str());
            result.push_back(val);
            float dif = abs(val - ground_truth[r++]);
            pixel++;
            rms_dif += (dif * dif);
            max_dif = max(max_dif, dif);
            sum_dif += dif;
        }

        rms_dif /= pixel;
        rms_dif = sqrt(rms_dif);
        ave_dif = sum_dif / pixel;
        R2 = getCoefficientOfDetermination(result, ground_truth);
        out_file << i << ",2048," << max_dif << "," << sum_dif << "," << rms_dif << "," << ave_dif << "," << pixel
                 << "," << R2 << endl;

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
        cout << "=";
    }
    cout << ">";
    for (int i = num; i < 10; i++) {
        cout << " ";
    }

    cout << static_cast<int>(percent * 100) << "%" << endl;

}