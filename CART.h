#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>      //���
#include <math.h>       //pow
#include <float.h>      //���������
#include "VspdCTOMySQL.h"

using namespace std;

//enum Data_Info_Fields {iHour,iMinute,iWeek,iHoliday,iPre_N_Volume,iPre_N_Capacity,iPre_N_InVolume,iUpstream_Volume,iWeather,iLong_Ratio};
                    //Pre_Volume,ǰһʱ�̳�������
                   //Pre_Capacity,ǰһʱ�̳���ͨ������
                    //Pre_InVolume,ǰһʱ�̽��ڵ�������
                  //Upstream_Volume,���ε�ƽ����������
struct Tree_Node {                                                                                        //˫�׺��ӱ�ʾ��
    Tree_Node(int mnode_Index, string mnode_Value) :node_Index(mnode_Index), node_Value(mnode_Value) {
        left_Tree = nullptr; right_Tree = nullptr; parent_Node = nullptr;
        node_Level = 0;
    }
    Tree_Node() {
        left_Tree = nullptr; right_Tree = nullptr; parent_Node = nullptr;
        node_Level = 0;
    }
    int node_Index;
    string node_Value;
    Tree_Node* left_Tree;
    Tree_Node* right_Tree;
    Tree_Node* parent_Node;
    double min_Square;                                                  //�ڵ�ľ������
    int node_Level;                                                    //�ڵ����Ȼ�߶�

};

template <typename T>
class CART_MODEL {
public:
    CART_MODEL() {
        head = new Tree_Node();
        load_Data_From_SQL();
        choose_Feature(Data_Info, head);
    }
    void load_Data_From_SQL();
    void load_Data_From_Tree(unordered_map<string, vector<string>> mData_Info);             //����ǰһ�������Ľ����������
    void choose_Feature(unordered_map<string, vector<string>> mData_Info, unordered_map<string, vector<string>>& data_Info_Left, unordered_map<string, vector<string>>& data_Info_Right, int& min_Feature_Index, string& min_Feature_Split);
    void data_Split(unordered_map<string, vector<string>> mData_Info, int field_Index, T field_Value, unordered_map<string, vector<string>>& data_Temp_Left, unordered_map<string, vector<string>>& data_Temp_Right);
    double caculate_Square(unordered_map<string, vector<string>> data_Temp);
    Tree_Node* build_Tree(Tree_Node* head, unordered_map<string, vector<string>> mData_Info);
    Tree_Node* build_Leaf_Node(Tree_Node* head, unordered_map<string, vector<string>> mData_Info);
    void sub_Tree_Square(Tree_Node* head, double& square_Sum, int& leaf_Num);
    void post_Pruning();
    void predict(unordered_map<string, vector<string>> mData_Info, Tree_Node* optimal);

private:
    unordered_map<string, vector<string>> Data_Info;
    vector<string> data_Info_Fields{ "Hour","Minute","Week","Holiday","Pre_Volume","Pre_Week_Volume","Pre_Capacity","Pre_InVolume","Upstream_Volume","Weather","Long_Ratio","Volume" };
    vector<string> data_Info_Types{ "Linear","Linear","Category","Category","Linear","Linear", "Linear", "Linear", "Linear", "Linear", "Category","Linear","Linear" };
    unordered_map<string, vector<string>> data_Info;
    unordered_map<string, vector<string>> category_Fields{ {"Week", {"1","2","3","4","5","6","7"}},
                                                      {"Holiday",{"0","1"}} };
    double volume_Window = 5.0;                //����ֵ�ķ�����ֵ�������зֵ��ѡȡ
    double leaf_Node_Interval = 5.0;          //Ҷ�ӽڵ�������С��ֵ����ֵ
    int leaf_Node_Min_Size = 5;               //Ҷ�ӽڵ����С���ݸ���
    double min_Error = 1e+6;
    int max_Depth_Limit;

public:
    Tree_Node* head;
    unordered_map<int, vector<Tree_Node*>> tree_Level;  //���Ĳ㼶
    unordered_map<double, Tree_Node*> optimal_Sub_Trees_Alpha;
};

template <typename T>
bool desc_sort(T first, T second) {
    return (first > second);
}