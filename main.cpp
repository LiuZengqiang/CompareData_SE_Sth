#include <iostream>
#include <tuple>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <vector>

using namespace std;

// max different
// sum different
// average different
// file name
// bias?
void process(float percent);

auto different(string filepath_1, string filepath_2, string output_file_path, int den) {
    float max_dif = -FLT_MAX;
    float sum_dif = 0.0f;
    int pixel_cnt = 0;
    // ave_dif = sum_dif / pixel_cnt
    float ave_dif = 0.0f;

    fstream out_file(output_file_path, fstream::app);

    if (!out_file.good()) {
        std::cerr << "Open file:" << output_file_path << " failed." << endl;
        exit(1);
    }
    ifstream file_1(filepath_1);
    ifstream file_2(filepath_2);
    stringstream str_stream_file_1;
    stringstream str_stream_file_2;
    str_stream_file_1 << file_1.rdbuf();
    str_stream_file_2 << file_2.rdbuf();
    string str_1;
    string str_2;
//    str_stream_file_1.getline()

    while (getline(str_stream_file_1, str_1) && getline(str_stream_file_2, str_2)) {
        float val_1 = 0.0f;
        float val_2 = 0.0f;
        stringstream str_stream_1(str_1);
        stringstream str_stream_2(str_2);
        while (str_stream_1 >> val_1 && str_stream_2 >> val_2) {
            max_dif = max(max_dif, fabs(val_1 - val_2));
            sum_dif += fabs(val_1 - val_2);
            pixel_cnt++;
            char ch;
            str_stream_1 >> ch;
            str_stream_2 >> ch;
        }
    }
    ave_dif = sum_dif / pixel_cnt;
    out_file << den << "," << 2048 << "," << max_dif << "," << sum_dif << "," << ave_dif << "," << pixel_cnt << endl;
    return tie(max_dif, sum_dif, ave_dif, pixel_cnt);
}

int main() {
    vector<int> data_list = {1, 64, 128, 256, 384, 512, 576, 640, 704, 768, 832, 896, 960, 992, 1008, 1016, 1024, 1280,
                             1408, 1536, 1664, 2048};
    string output_file_path = "/home/sth/CLionProjects/CompareData_SE_Sth/file_out.csv";
    fstream out_file(output_file_path, fstream::out);
    out_file << "den,t_den,max,sum,ave,pixel" << endl;
    out_file.close();

    for (int i = 0; i < data_list.size(); ++i) {
        different("/home/sth/CLionProjects/CompareData_SE_Sth/input/" + to_string(data_list[i]) + "_average.csv",
                  "/home/sth/CLionProjects/CompareData_SE_Sth/input/2048_average.csv",
                  output_file_path, data_list[i]);

        if (100 * i / data_list.size() % 10 == 0) {
            process(static_cast<float>(i) / data_list.size());
        }
    }
    return 0;
}

void process(float percent) {
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