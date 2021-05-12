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
int experiment_times = 1;

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


void getSum(string input_file_path = "") {
    fstream input_file(input_file_path, fstream::in);
    if (!input_file.good()) {
        cerr << "file " << input_file_path << " open failed." << endl;
        return;
    }
    stringstream ss;
    ss << input_file.rdbuf();
    input_file.close();

    string str;
    getline(ss, str);
    float sum = 0.0f;
    while (getline(ss, str)) {
        int index = str.find_last_of(',');
        float val = atof(str.substr(index + 1).c_str());
        sum += val;
    }
    sum *= (0.05 * 0.05);
    sum /= 320;
    cout << "sum:" << sum << endl;
}

void done(string x) {
    // change the parent_directory,pre,suf,output_file_path and ground_truth_file
    string parent_directory =
            "/home/sth/CLionProjects/SolarEnergy_Chier/OutputFiles/RayDensityExperiment/different_azimuth/azimuth_" +
            x + "/";

//    /CLionProjects/SolarEnergy_Chier/OutputFiles/RayDensityExperiment/sun_dir_[90_0]_p_[50_0_0]
//    /CLionProjects/DifferentAtitude/atitude_40/OutputFiles/sun_dir_[90_40]_p_[50_0_0]
    string pre = "before_smooth_d_";
    string suf = "_r_128.csv";
    float coefficience = 1.0f;
//    [100,200,500,800,1000]
//    if (x == "100" || x == "200" || x == "500" || x == "800" || x == "1000") {
//        coefficience /= 320.0f;
//    }
//    string output_file_path =
//            "/home/sth/CLionProjects/CompareData_SE_Sth/output/before_smooth_p_[50_2.57_0]_dir_[90_" + x +
//            "]_d_[1_512]_r_" + to_string(experiment_times) + "_old.csv";
    string output_file_path =
            "/home/sth/CLionProjects/CompareData_SE_Sth/output/RayDensity/diff_azimuth/before_smooth_p[200_2.57_0]_azi_" +
            x + "_r_128.csv";

    cout << "output file:" << output_file_path << endl;

    fstream out_file(output_file_path, fstream::out);

    out_file << "ray_density,gt_ray_density,max,sum,rms,ave,pixel,r2" << endl;

    // construct ground truth result
    vector<float> ground_truth;
    fstream ground_truth_file(
            parent_directory +
            "before_smooth_d_2048_r_128.csv");
//    cout << parent_directory + "before_smooth_d_2048_r_320.csv" << endl;
    assert(ground_truth_file.good());
    stringstream ss;
    ss << ground_truth_file.rdbuf();
    ground_truth_file.close();
    string str;
    getline(ss, str);
    while (getline(ss, str)) {
        int index = str.find_last_of(',');
        float val = atof(str.substr(index + 1).c_str()) * coefficience;
        ground_truth.push_back(val);
    }
    int cnt = 512;
    float percent = 0.0f;
    process(percent);

    float max_dif, sum_dif, rms_dif, ave_dif, pixel, R2;

    for (int i = 1; i <= cnt; i++) {
        vector<float> result;

        string file_path =
                parent_directory + pre + to_string(i) + suf;
        fstream file(file_path);
//        cout << "file:" << file_path << endl;
        if (!file.good()) {
            cerr << "File " << file_path << " opened fail." << endl;
            out_file << i << ",2048," << max_dif << "," << sum_dif << "," << rms_dif << "," << ave_dif << "," << pixel
                     << "," << R2 << endl;
            continue;
        }
        stringstream ss;
        ss << file.rdbuf();
        file.close();

        int r = 0;
        string str;
        getline(ss, str);
        max_dif = -FLT_MAX;
        sum_dif = 0.0f;
        rms_dif = 0.0f;
        ave_dif = 0.0f;
        pixel = 0;
        R2 = 0.0f;
        while (getline(ss, str)) {
            int index = str.find_last_of(',');
            float val = atof(str.substr(index + 1).c_str()) * coefficience;
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

}

//
void combineResult(vector<string> &input_file_lists, vector<string> &title_lists, string output_file_path) {
    if (input_file_lists.size() != title_lists.size()) {
        cerr << "file list size is different with title lists." << endl;
        return;
    }
    fstream output_file(output_file_path, fstream::out);
    if (output_file.bad()) {
        cerr << "Create output file " << output_file_path << " failed." << endl;
        return;
    }
    // add title
    for (auto &title:title_lists) {
        if (&title == &title_lists.back()) {
            output_file << title << endl;
        } else {
            output_file << title << ",";
        }
    }
    // add value
    vector<stringstream> sss;
    // init sss
    for (auto &input_file_path:input_file_lists) {
        fstream input_file(input_file_path, fstream::in);
        if (input_file.bad()) {
            cerr << "Read input file " << input_file_path << " failed." << endl;
            return;
        }
        stringstream ss;
        ss << input_file.rdbuf();
        sss.push_back(move(ss));
        input_file.close();
    }
    vector<int> row_num;
    for (vector<stringstream>::size_type index = 0; index < sss.size(); index++) {
        string str = move(sss[index].str());
        row_num.push_back(count(str.begin(), str.end(), '\n'));
    }
    for (vector<stringstream>::size_type index = 0; index < row_num.size(); index++) {
        cout << row_num[index] << " ";
    }
    cout << endl;
}

void split(string str, vector<string> &v, string spacer) {
    v.clear();
    int pos1, pos2;
    int len = spacer.length();     //记录分隔符的长度
    pos1 = 0;
    pos2 = str.find(spacer);
    while (pos2 != string::npos) {
        v.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + len;
        pos2 = str.find(spacer, pos1);    // 从str的pos1位置开始搜寻spacer
    }
    if (pos1 != str.length()) //分割最后一个部分
        v.push_back(str.substr(pos1));
}

void
combineResults(vector<string> &input_file_lists, vector<string> titles, string target_title, string output_file_path) {
    int n = input_file_lists.size();
    vector<stringstream> input_ss(n);
    int index = -1;

    for (int i = 0; i < input_file_lists.size(); i++) {
        fstream file(input_file_lists[i], fstream::in);
        input_ss[i] << file.rdbuf();
        file.close();
    }
    fstream output_file(output_file_path, fstream::out);

    string temp_str = input_ss[0].str();
    int row = count(temp_str.begin(), temp_str.end(), '\n');
    vector<string> strs(input_ss.size());

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < input_ss.size(); j++) {
            getline(input_ss[j], strs[j]);
        }
        if (i == 0) {
            vector<string> title_split;
            split(strs[0], title_split, ",");
            for (int j = 0; j < title_split.size(); j++) {
                if (title_split[j] == target_title) {
                    index = j;
                    break;
                }
            }
            if (index == -1) {
                return;
            } else {
                cout << "index:" << index << " title:" << title_split[index];
            }
            for (int j = 0; j < titles.size() - 1; j++) {
                output_file << titles[j] << ",";
            }
            output_file << titles.back() << endl;
        } else {
            vector<float> values(input_ss.size(), 0.0f);
            for (int j = 0; j < input_ss.size(); j++) {
                vector<string> v;
                split(strs[j], v, ",");
                values[j] = atof(v[index].c_str());
            }
            for (int j = 0; j < values.size() - 1; j++) {
                output_file << values[j] << ",";
            }
            output_file << values.back() << endl;
        }
    }
    output_file.close();
}

int main() {
    // note: need divide 320 [100,200,500,800,1000]

//    getSum("/home/sth/CLionProjects/SolarEnergy_Chier/OutputFiles/RayDensityExperiment/p_[50_2.57_0]_g_64_t_320_d_[1_1024]/before_smooth_d_1_r_320.csv");

//    return 0;
//    vector<int> var;
//    for (int i = -90; i <= 90; i += 10) {
//        var.push_back(i);
//    }
////    var.push_back(256);
////    var.push_back(512);
////    var.push_back(1024);
//    for (auto &i:var) {
//        done(to_string(i));
//    }

    // only need change target_titles, output_path, input_file_lists and titles
    vector<string> input_file_lists;
    vector<string> titles;
    // target title = "rms", "max" or "r2"
    string target_tile = "r2";
    string output_path = "../output/RayDensity/diff_azimuth/all_" + target_tile + "_azimuth.csv";

    for (int i = -90; i <= 90; i += 10) {
//        RayDensity/diff_azimuth/before_smooth_p[200_2.57_0]_azi_" +
//        x + "_r_128.csv";
        input_file_lists.push_back(
                "../output/RayDensity/diff_azimuth/before_smooth_p[200_2.57_0]_azi_" + to_string(i) + "_r_128.csv");
        titles.push_back("azimuth" + to_string(i));
    }

    combineResults(input_file_lists, titles, target_tile, output_path);
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