#include "CART.h"

void CART_MODEL::load_Data_From_SQL() {
	unique_ptr<VspdCToMySQL> vspdctomysql = make_unique<VspdCToMySQL>();

	/*
	char* host = (char*)"localhost";
	char* port = (char*)"3306";
	char* db = (char*)"shanghai_dw";
	char* user = (char*)"root";
	char* passwd = (char*)"123456";
	char* charset = (char*)"utf-8";
	char* Msg = (char*)"";

	if (vspdctomysql->ConnMySQL(host, port, db, user, passwd, charset, Msg) == 1){
		printf("mysql connected failed");
	}else{
		printf("mysql connected success");
	}
	*/

	Data_Info.emplace("Hour", vector<string>{"9","9","9","9","9"});     //2020年9月1号，9点0分的数据示例
	Data_Info.emplace("Minute", vector<string>{ "0","0","0","0","0" });
	Data_Info.emplace("Week", vector<string>{ "2","2","2","2","2" });
	Data_Info.emplace("Holiday", vector<string>{"0","0", "0", "0", "0" });
	Data_Info.emplace("Pre_Volume", vector<string>{ "15","15", "15", "30", "30" });           //前一时刻的流量数据
	Data_Info.emplace("Pre_Week_Volume", vector<string>{ "20","20", "20", "20", "20" });
	Data_Info.emplace("Pre_Capacity", vector<string>{ "1000","1000", "1000", "1000", "1000" });
	Data_Info.emplace("Pre_InVolume", vector<string>{ "100","100", "100", "100", "100" });
	Data_Info.emplace("Upstream_Volume", vector<string>{ "100","100", "100", "100", "100" });
	Data_Info.emplace("Weather", vector<string>{ "1","1", "1", "1", "1" });              //1表示天气晴朗
	Data_Info.emplace("Long_Ratio", vector<string>{ "1.0","1.0", "1.0", "1.0", "1.0" });
	Data_Info.emplace("Volume", vector<string>{ "18","18", "19", "29", "29" });                 //当前流量的输出值

	return;
}

void CART_MODEL::load_Data_From_Tree(unordered_map<string, vector<string>> mData_Info) {

	return;
}

void CART_MODEL::choose_Feature(unordered_map<string, vector<string>> mData_Info, unordered_map<string, vector<string>>& data_Info_Left, unordered_map<string, vector<string>>& data_Info_Right,int& min_Feature_Index, string& min_Feature_Split) {
	double left_Error = 0.0, right_Error = 0.0;
	unordered_map<string, vector<string>> data_Temp_Left;
	unordered_map<string, vector<string>> data_Temp_Right;
	min_Error = FLT_MAX;
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
				category_Fields[data_Info_Fields[i]].emplace_back(to_string(s));
			}
		}
		//TODO: 当变量的数据特征只有一个时，为无效的特征，则跳过该变量
		for (int s = 0; s < category_Fields[data_Info_Fields[i]].size(); s++) {       //遍历切分点 
			data_Split(mData_Info, i, category_Fields[data_Info_Fields[i]][s], data_Temp_Left, data_Temp_Right);

			left_Error = (data_Temp_Left.size()) ? caculate_Square(data_Temp_Left) : 0.0;
			right_Error = (data_Temp_Right.size()) ? caculate_Square(data_Temp_Right) : 0.0;
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

void CART_MODEL::data_Split(unordered_map<string, vector<string>> mData_Info, int field_Index, string field_Value, unordered_map<string, vector<string>>& data_Temp_Left, unordered_map<string, vector<string>>& data_Temp_Right) {
	bool temp = true;
	data_Temp_Left.clear();
	data_Temp_Right.clear();
	for (int i = 0; i < mData_Info[data_Info_Fields[field_Index]].size(); i++) {
		if (data_Info_Types[field_Index] == "Linear") {
			temp = stof(mData_Info[data_Info_Fields[field_Index]][i]) < stof(field_Value);
		}else
			temp = mData_Info[data_Info_Fields[field_Index]][i] < field_Value;

		for (int j = 0; j < data_Info_Fields.size(); j++) {
			if (temp) {
				data_Temp_Left[data_Info_Fields[j]].emplace_back(mData_Info[data_Info_Fields[j]][i]);       //将同行的数据存入左边集合
			}else {
				data_Temp_Right[data_Info_Fields[j]].emplace_back(mData_Info[data_Info_Fields[j]][i]);       //将同行的数据存入右边集合
			}
		}		
	}
	return;
}

double CART_MODEL::caculate_Square(unordered_map<string, vector<string>> data_Temp) {      	               //计算方差
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

Tree_Node* CART_MODEL::build_Tree(Tree_Node* parent, Tree_Node* head, unordered_map<string, vector<string>> mData_Info) {         	//创建树
	if (mData_Info.size() == 0)
		return nullptr;
	int index = 0;
	string value = "";
	unordered_map<string, vector<string>> data_Info_Left, data_Info_Right;
	choose_Feature(mData_Info, data_Info_Left, data_Info_Right, index, value);
                                                  
	if (head == nullptr)
	    head = new Tree_Node(index, value);
	head->min_Square = caculate_Square(mData_Info);
	head->parent_Node = parent;                                                    //保留双亲节点
	head->node_Level = parent ? (parent->node_Level + 1) : 1;                      //计算节点的深度
	tree_Level[head->node_Level].emplace_back(head);                             //同一层的节点放在vector中

	if (data_Info_Left["Volume"].size() < leaf_Node_Min_Size || data_Info_Right["Volume"].size() < leaf_Node_Min_Size) {
		head = build_Leaf_Node(head, mData_Info);
	}
	if (data_Info_Left["Volume"].size() >= leaf_Node_Min_Size && data_Info_Right["Volume"].size() >= leaf_Node_Min_Size) {
		head->left_Tree = build_Tree(head,head->left_Tree, data_Info_Left);
		head->right_Tree = build_Tree(head,head->right_Tree, data_Info_Right);
	}
	return head;
}

Tree_Node* CART_MODEL::build_Leaf_Node(Tree_Node* head, unordered_map<string, vector<string>> mData_Info) {
	//创建叶节点
	int n = data_Info_Fields.size() - 1;
	vector<string> temp = mData_Info[data_Info_Fields[n]];
	vector<double> temp_Float;
	for (int i = 0; i < temp.size(); i++) {
		temp_Float.emplace_back(stof(temp[i]));
	}
	head->node_Value = to_string(accumulate(temp_Float.begin(), temp_Float.end(), 0.0) / temp_Float.size());     //计算叶节点集合的平均值
	return head;
}

void CART_MODEL::post_Pruning() {
	vector<int> tree_Level_Sort;
	for (auto it = tree_Level.begin(); it != tree_Level.end(); it++) {
		tree_Level_Sort.emplace_back(it->first);
	}
	sort(tree_Level_Sort.begin(), tree_Level_Sort.end(), desc_sort<int>);

	double alpha_Temp = 0.0;
	for (int i = 1; i < tree_Level_Sort.size(); i++) {       //从底向上，对决策树进行剪枝
		for (int j = 0; j < tree_Level[tree_Level_Sort[i]].size(); j++) {
			if (!tree_Level[tree_Level_Sort[i]][j]->parent_Node)
				break;
			double sub_Tree_Square_Sum = 0.0;
			int leaf_Num = 0;
			sub_Tree_Square(tree_Level[tree_Level_Sort[i]][j], sub_Tree_Square_Sum, leaf_Num);                    //计算以中间节点为跟节点的子树均方误差和叶子节点数
			alpha_Temp = (tree_Level[tree_Level_Sort[i]][j]->min_Square - sub_Tree_Square_Sum) / (leaf_Num - 1);

			unordered_map<int, vector<Tree_Node*>> tree_Level_Temp;              //建立子树，把整个树复制给子树
			Tree_Node* sub_Tree = nullptr;                               
			sub_Tree = copy_Tree(this->head, nullptr, tree_Level_Temp);
			
			tree_Level_Temp[tree_Level_Sort[i]][j]->left_Tree = nullptr;
			tree_Level_Temp[tree_Level_Sort[i]][j]->right_Tree = nullptr;
			optimal_Sub_Trees_Alpha[alpha_Temp] = sub_Tree;                                                             //剪枝完成的子树

		}
	}
	return;
}

void CART_MODEL::sub_Tree_Square(Tree_Node* head, double& square_Sum, int& leaf_Num) {
	if (head->left_Tree == nullptr && head->right_Tree == nullptr) {
		square_Sum += head->min_Square;
		leaf_Num++;
		return;
	}
	sub_Tree_Square(head->left_Tree, square_Sum, leaf_Num);
	sub_Tree_Square(head->right_Tree, square_Sum, leaf_Num);
	return;
}

void CART_MODEL::copy_Node(Tree_Node* sub_Tree_Node, Tree_Node* head_Node) {
	sub_Tree_Node->min_Square = head_Node->min_Square;
	sub_Tree_Node->node_Index = head_Node->node_Index;
	sub_Tree_Node->node_Level = head_Node->node_Level;
	sub_Tree_Node->node_Value = head_Node->node_Value;
	return;
}

Tree_Node* CART_MODEL::copy_Tree(Tree_Node* head, Tree_Node* sub_Tree_Parent, unordered_map<int, vector<Tree_Node*>>& tree_Level_Temp) {
	if (!head)
		return nullptr;
	Tree_Node* sub_Tree = new Tree_Node();
	copy_Node(sub_Tree,head);
	sub_Tree->parent_Node = sub_Tree_Parent;
	tree_Level_Temp[sub_Tree->node_Level].emplace_back(sub_Tree);

	sub_Tree->left_Tree = copy_Tree(head->left_Tree, sub_Tree, tree_Level_Temp);
	sub_Tree->right_Tree = copy_Tree(head->right_Tree, sub_Tree, tree_Level_Temp);
	return sub_Tree;
}

void CART_MODEL::predict(unordered_map<string, vector<string>> mData_Info, Tree_Node* optimal) {  //只有一行数

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

int main(){

	unordered_map<string, vector<string>> Data_Info;
	Data_Info.emplace("Hour", vector<string>{"9"});     //2020年9月1号，9点0分的数据示例
	Data_Info.emplace("Minute", vector<string>{ "0"});
	Data_Info.emplace("Week", vector<string>{ "2"});
	Data_Info.emplace("Holiday", vector<string>{"0"});
	Data_Info.emplace("Pre_Volume", vector<string>{ "15"});           //前一时刻的流量数据
	Data_Info.emplace("Pre_Week_Volume", vector<string>{ "20"});
	Data_Info.emplace("Pre_Capacity", vector<string>{ "1000"});
	Data_Info.emplace("Pre_InVolume", vector<string>{ "100"});
	Data_Info.emplace("Upstream_Volume", vector<string>{ "100"});
	Data_Info.emplace("Weather", vector<string>{ "1"});              //1表示天气晴朗
	Data_Info.emplace("Long_Ratio", vector<string>{ "1.0"});

	CART_MODEL cart_test;
	cart_test.post_Pruning();
	cart_test.predict(Data_Info, cart_test.head);

	return 0;
}
