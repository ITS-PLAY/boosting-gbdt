#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <algorithm>     //sort
#include <numeric>      //求和
#include <math.h>       //pow
#include <float.h>      //定义无穷大
#include "VspdCTOMySQL.h"

using namespace std;

struct Tree_Node {                                                                                        //双亲孩子表示法
    Tree_Node(int mnode_Index, string mnode_Value) :node_Index(mnode_Index), node_Value(mnode_Value) {
        left_Tree = nullptr; right_Tree = nullptr; parent_Node = nullptr;
        node_Level = 0;
    }
    Tree_Node() {
        left_Tree = nullptr; right_Tree = nullptr; parent_Node = nullptr;
        node_Level = 0;
    }
    int node_Index;                                                     //特征变量在数组data_Info_Fields中的下标
    string node_Value;                                                  //特征变量的切分点
    Tree_Node* left_Tree;
    Tree_Node* right_Tree;
    Tree_Node* parent_Node;
    double min_Square;                                                  //节点的均方误差
    int node_Level;                                                    //节点的深度或高度
};


class GBDT_MODEL {
public:
    GBDT_MODEL() { 
        load_Data_From_SQL();
    }
public:
    void gdbt_Model();                                                                      //提升树模型的循环体
    void predict(unordered_map<string, vector<string>> mData_Info, Tree_Node* optimal);

protected:
    void load_Data_From_SQL();
    void load_Data_From_Tree(unordered_map<string, vector<string>> mData_Info);             //根据前一决策树的结果加载数据
    void choose_Feature(unordered_map<string, vector<string>> mData_Info, unordered_map<string, vector<string>>& data_Info_Left, unordered_map<string, vector<string>>& data_Info_Right, int& min_Feature_Index, string& min_Feature_Split);
    void data_Split(unordered_map<string, vector<string>> mData_Info, int field_Index, string field_Value, unordered_map<string, vector<string>>& data_Temp_Left, unordered_map<string, vector<string>>& data_Temp_Right);
    double caculate_Square(unordered_map<string, vector<string>> data_Temp);
    Tree_Node* build_Tree(Tree_Node* parent, Tree_Node* head, unordered_map<string, vector<string>> mData_Info);
    Tree_Node* build_Leaf_Node(Tree_Node* head, unordered_map<string, vector<string>> mData_Info);
    void sub_Tree_Square(Tree_Node* head, double& square_Sum, int& leaf_Num);
    void post_Pruning();
    void copy_Node(Tree_Node* sub_Tree_Node,Tree_Node* head_Node);
    Tree_Node* copy_Tree(Tree_Node* head, Tree_Node* sub_Tree_Parent, unordered_map<int, vector<Tree_Node*>>& tree_Level_Temp);
    void residual_Caculation();

private:
    unordered_map<string, vector<string>> Data_Info;     
    vector<string> data_Info_Fields{ "Hour","Minute","Week","Holiday","Pre_Volume","Pre_Week_Volume","Pre_Capacity","Pre_InVolume","Upstream_Volume","Weather","Long_Ratio","Volume" };
    vector<string> data_Info_Types{ "Linear","Linear","Category","Category","Linear","Linear", "Linear", "Linear", "Linear", "Linear", "Category","Linear","Linear" };
    unordered_map<string, vector<string>> category_Fields{ {"Week", {"1","2","3","4","5","6","7"}},
                                                      {"Holiday",{"0","1"}} };
    double volume_Window = 5.0;                //流量值的分箱阈值，用于切分点的选取
    double leaf_Node_Interval = 5.0;          //叶子节点的最大最小差值的阈值
    int leaf_Node_Min_Size = 2;               //叶子节点的最小数据个数
    double min_Error = FLT_MAX;
    int max_Depth_Limit;

public:
    Tree_Node* head;
    int tree_Max_Nums = 5;
    unordered_map<int, vector<Tree_Node*>> tree_Level;  //树的层级
    unordered_map<double, Tree_Node*> optimal_Sub_Trees_Alpha;    //每个提升树的剪枝子树
    vector<Tree_Node*> gradient_Boost_Trees;                      //梯度提升树的集合
};

template <typename T>
bool desc_sort(T first, T second) {
    return (first > second);
}
