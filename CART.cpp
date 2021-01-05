#include "CART.h"

template <typename T>
void CART_MODEL<T>::load_Data_From_SQL() {
	VspdCToMySQL* vspdctomysql = new VspdCToMySQL;
	MYSQL mysql;
	mysql_init(&mysql);
	const char* host = "localhost";
	const char* user = "root";
	const char* pwd = "123456";
	const char* db = "School";
	const char* charset = "utf-8";
	char* Msg = "";


	if (vspdctomysql->ConnMySQL(host, 3306, db, user,pwd, charset, Msg)){
		printf("mysql connected failed, ");
	}else{
		printf("mysql connected success");
	}

	mysql_close(&mysql);
	return;
}

template <typename T>
void CART_MODEL<T>::load_Data_From_Tree(unordered_map<string, vector<string>> mData_Info) {

	return;
}

template <typename T>
void CART_MODEL<T>::choose_Feature(unordered_map<string, vector<string>> mData_Info, unordered_map<string, vector<string>>& data_Info_Left, unordered_map<string, vector<string>>& data_Info_Right,int& min_Feature_Index, string& min_Feature_Split) {
	double left_Error = 0.0, right_Error = 0.0;
	unordered_map<string, vector<string>> data_Temp_Left;
	unordered_map<string, vector<string>> data_Temp_Right;
	//选择最优的切分变量i和切分点s
	int n = data_Info_Fields.size() - 1;
	for (int i = 2; i < data_Info_Fields.size(); i++) {                             //遍历切分变量
		if (data_Info_Types[i] == "Linear") {
			vector<double> field_Float_Value;
			for (int j = 0; j < mData_Info[data_Info_Fields[i]].size(); j++) {
				field_Float_Value.emplace_back(stof(mData_Info[data_Info_Fields[i]][j]));
			}
			double max_Value = *max_element(field_Float_Value.begin(), field_Float_Value.end());
			double min_Value = *min_element(field_Float_Value.begin(), field_Float_Value.end());
			double s_begin = volume_Window * ceil(min_Value / volume_Window), s_end = volume_Window * floor(max_Value / volume_Window);
			for (double s = s_begin; s <= s_end; s = s + volume_Window) {         //流量的切分点间隔为volume_Window
				category_Fields[data_Info_Fields[i]].emplace(to_string(s));
			}
		}

		for (int s = 0; s < category_Fields[data_Info_Fields[i]].size(); s++) {       //遍历切分点 
			if (data_Info_Types[i] == "Linear") {
				data_Split(mData_Info, i, stof(category_Fields[data_Info_Fields[i]][s]), data_Temp_Left, data_Temp_Right);
				category_Fields.erase([data_Info_Fields[i]]);
			}
			else
				data_Split(mData_Info, i, category_Fields[data_Info_Fields[i]][s], data_Temp_Left, data_Temp_Right);
			left_Error = (data_Temp_Left.size()) ? caculate_Square(data_Temp_Left) : 0.0;
			right_Error = (data_Temp_Left.size()) ? caculate_Square(data_Temp_Right) : 0.0;
			if ((left_Error + right_Error) < min_Error) {
				min_Error = left_Error + right_Error;
				data_Info_Left.swap(data_Temp_Left);
				data_Info_Right.swap(data_Temp_Right);
				min_Feature_Index = i;
				min_Feature_Split = category_Fields[data_Info_Fields[i]][s];
			}
		}
	}

	return;
}

template <typename T>
void CART_MODEL<T>::data_Split(unordered_map<string, vector<string>> mData_Info, int field_Index, T field_Value, unordered_map<string, vector<string>>& data_Temp_Left, unordered_map<string, vector<string>>& data_Temp_Right) {

	for (int i = 0; i < mData_Info[data_Info_Fields[field_Index]].size(); i++) {
		if (mData_Info[data_Info_Fields[field_Index]][i] < field_Value) {
			for (int j = 0; j < data_Info_Fields.size(); j++) {
				data_Temp_Left[data_Info_Fields[j]].emplace_back(mData_Info[data_Info_Fields[j]][i]);       //将同行的数据存入左边集合
			}
		}
		else {
			for (int j = 0; j < data_Info_Fields.size(); j++) {
				data_Temp_Right[data_Info_Fields[j]].emplace_back(mData_Info[data_Info_Fields[j]][i]);       //将同行的数据存入左边集合
			}
		}
	}
	return;
}

template <typename T>
double CART_MODEL<T>::caculate_Square(unordered_map<string, vector<string>> data_Temp) {      	               //计算方差
double sum = 0.0;
int n = data_Info_Fields.size() - 1;
vector<string> temp = data_Temp[data_Info_Fields[n]];
vector<double> temp_Float;
for (int i = 0; i < temp.size(); i++) {
	temp_Float.emplace_back(stof(temp[i]));
	sum += pow(temp_Float[i], 2.0);
}
double ave = accumulate(temp_Float.begin(), temp_Float.end(), 0.0) / temp_Float.size();
return sum - temp_Float.size() * pow(ave, 2.0);
}

template<typename T>
Tree_Node* CART_MODEL<T>::build_Tree(Tree_Node* head, unordered_map<string, vector<string>> mData_Info) {
	//创建树
	if (mData_Info.size() == 0)
		return nullptr;
	int index = 0;
	string value = "";
	unordered_map<string, vector<string>> data_Info_Left, data_Info_Right;
	choose_Feature(mData_Info, data_Info_Left, data_Info_Right, index, value);
	Tree_Node* root = head;                                                      //保留双亲节点
	head = new Tree_Node(index, value);
	head->min_Square = caculate_Square(mData_Info);
	head->parent_Node = root;
	head->node_Level = root->node_Level + 1;                                     //计算节点的深度

	tree_Level[head->node_Level].emplace_back(head);                             //同一层的节点放在vector中
	if (data_Info_Left.size() < leaf_Node_Min_Size || data_Info_Right.size() < leaf_Node_Min_Size) {
		head = build_Leaf_Node(head, mData_Info);
	}

	if (data_Info_Left.size() >= leaf_Node_Min_Size && data_Info_Right.size() >= leaf_Node_Min_Size) {
		head->left_Tree = build_Tree(head->left_Tree, data_Info_Left);
		head->right_Tree = build_Tree(head->right_Tree, data_Info_Right);
	}

	return head;
}

template<typename T>
Tree_Node* CART_MODEL<T>::build_Leaf_Node(Tree_Node* head, unordered_map<string, vector<string>> mData_Info) {
	//创建叶节点
	int n = data_Info_Fields.size() - 1;
	vector<string> temp = mData_Info[data_Info_Fields[n]];
	vector<double> temp_Float;
	for (int i = 0; i < temp.size(); i++) {
		temp_Float.emplace_back(stof(temp[i]));
	}
	head->node_Value = to_string(accumulate(temp_Float.begin(), temp_Float.end(), 0.0) / temp_Float.size());
	return head;
}

template<typename T>
void CART_MODEL<T>::post_Pruning() {
	vector<int> tree_Level_Sort(1);
	for (auto it = tree_Level.begin(); it != tree_Level.end(); it++) {
		tree_Level_Sort.emplace_back(it->first);
	}
	sort(tree_Level_Sort.begin(), tree_Level_Sort.end(), desc_sort<int>);

	double alpha_Temp = 0.0;
	for (int i = 1; i < tree_Level_Sort.size(); i++) {       //从底向上，对决策树进行剪枝
		for (int j = 0; j < tree_Level[tree_Level_Sort[i]].size(); j++) {

			double sub_Tree_Square_Sum = 0.0;
			int leaf_Num = 0;
			sub_Tree_Square(tree_Level[tree_Level_Sort[i]][j], sub_Tree_Square_Sum, leaf_Num);                    //计算以中间节点为跟节点的子树均方误差和叶子节点数
			alpha_Temp = (tree_Level[tree_Level_Sort[i]][j]->min_Square - sub_Tree_Square_Sum) / (leaf_Num - 1);

			Tree_Node* temp = tree_Level[tree_Level_Sort[i]][j];
			tree_Level[tree_Level_Sort[i]][j]->left_Tree = nullptr;
			tree_Level[tree_Level_Sort[i]][j]->right_Tree = nullptr;
			optimal_Sub_Trees_Alpha[alpha_Temp] = head;                                                             //剪枝完成的子树
			if (tree_Level[tree_Level_Sort[i]][j] == tree_Level[tree_Level_Sort[i]][j]->parent_Node->left_Tree) {   //回溯到原来的状态
				tree_Level[tree_Level_Sort[i]][j]->parent_Node->left_Tree = temp;
			}
			else {
				tree_Level[tree_Level_Sort[i]][j]->parent_Node->right_Tree = temp;
			}
		}
	}
	return;
}

template <typename T>
void CART_MODEL<T>::sub_Tree_Square(Tree_Node* head, double& square_Sum, int& leaf_Num) {
	if (head->left_Tree == nullptr && head->right_Tree == nullptr) {
		square_Sum += head->min_Square;
		leaf_Num++;
		return;
	}
	sub_Tree_Square(head->left_Tree, square_Sum, leaf_Num);
	sub_Tree_Square(head->right_Tree, square_Sum, leaf_Num);
	return;
}

template<typename T>
void CART_MODEL<T>::predict(unordered_map<string, vector<string>> mData_Info, Tree_Node* optimal) {  //只有一行数

	if (data_Info_Types[optimal->node_Index] == "Category") {
		if (mData_Info[data_Info_Fields[optimal->node_Index]][0] < optimal->node_Value) {           //数值小于节点值，从左子树遍历
			if (optimal->left_Tree)
				predict(mData_Info, optimal->left_Tree);
			else
				printf("predict_result: %s", optimal->node_Value.c_str()); return;
		}else {
			if (optimal->right_Tree)                                                                //数值大于节点值，从右子树遍历
				predict(mData_Info, optimal->right_Tree);
			else
				printf("predict_result: %s", optimal->node_Value.c_str()); return;
		}
	}
	if (data_Info_Types[optimal->node_Index] == "Linear") {
		if (stof(mData_Info[data_Info_Fields[optimal->node_Index]][0]) < stof(optimal->node_Value)) {
			if (optimal->left_Tree)
				predict(mData_Info, optimal->left_Tree);
			else
				printf("predict_result: %s", optimal->node_Value.c_str()); return;
		}else {
			if (optimal->right_Tree)
				predict(mData_Info, optimal->right_Tree);
			else
				printf("predict_result: %s", optimal->node_Value.c_str()); return;
		}
	}
	return;
}

int main() {

	return 0;
}
