/*
程序功能：唱片销售管理系统
作者：郑恒卓
日期：2026-07-15
功能：实现唱片基本信息录入、进货、销售、查询、统计、汇总等功能
 */

#include<bits/stdc++.h>
using namespace std;

//结构体定义

// 唱片基本信息
typedef struct record
{
	int id;    // 唱片编号
	char name[50];    // 唱片名
	char singer[50];    // 歌手名
	char style[30];    // 风格
	double price;    // 价格
	char release_date[20];    // 发行日期
	char note[100];    // 备注
	struct record *next;    // 链表指针
} Record;

// 进货信息
typedef struct input_record
{
	int id;    // 唱片编号
	char name[50];    // 唱片名称
	int quantity;    // 进货数量
	char date[20];    // 进货日期
	char operator_name[30];    // 进货人
	struct input_record *next;
} InputRecord;

// 库存信息
typedef struct stock_record
{
	int id;     // 唱片编号
	char name[50];    // 唱片名称
	int stock_qty;    // 库存数量
	int input_qty;    // 进货累计数量
	int sell_qty;    // 销售累计数量
	struct stock_record *next;
} StockRecord;

// 销售信息
typedef struct sell_record
{
	int sell_id;    // 销售编号
	int record_id;    // 唱片编号
	char record_name[50];    // 唱片名称
	double sell_price;    // 销售价格
	int sell_qty;    // 销售数量
	double sell_amount;    // 销售金额
	char customer_name[30];    // 客户名称
	char sell_date[20];    // 销售日期
	struct sell_record *next;
} SellRecord;

//  全局变量 

// 链表头指针
Record *record_head = 0;
InputRecord *input_head = 0;
StockRecord *stock_head = 0;
SellRecord *sell_head = 0;

// 文件名常量
#define PWD_FILE "pwd.dat"
#define BASE_FILE "record_base.dat"
#define INPUT_FILE "record_input.dat"
#define STOCK_FILE "record_stock.dat"
#define SELL_FILE "record_sell.dat"
#define LOG_FILE "log.txt"

//  辅助函数 

/*
函数：writeLog()
功能：记录操作日志到 log.txt
参数：操作类型、操作内容
 */
void writeLog(const char *type, const char *content)
{
	FILE *fp = fopen(LOG_FILE, "a");
	if(fp == 0)
	{
		return;
	}
	time_t now = time(0);	//获取时间存储在专门存储时间的time_t中
	char *time_str = ctime(&now);	//把专门存储时间的time_t转换成字符串

	for(int i = 0; time_str[i] != '\0'; i++)	//最后默认有/n要去掉
	{
		if(time_str[i] == '\n')
		{
			time_str[i] = '\0';
			break;
		}
	}
	fprintf(fp, "[%s] %s: %s\n", time_str, type, content);
	fclose(fp);
}

//  文件加载函数 

/*
函数：loadPassword()
功能：检查密码文件，首次运行则设置密码，否则校验密码
 */
void loadPassword()
{
	FILE *fp = fopen(PWD_FILE, "rb");
	if(fp == 0)
	{
		// 首次运行，设置密码
		char pwd[20];
		cout << "首次使用系统，请设置密码: ";
		cin >> pwd;
		fp = fopen(PWD_FILE, "wb");
		fwrite(pwd, sizeof(char), 20, fp);
		fclose(fp);
		cout << "密码设置成功！" << endl << endl;
		writeLog("系统", "首次设置密码");
		return;
	}
	// 读取已有密码
	char saved_pwd[20];
	fread(saved_pwd, sizeof(char), 20, fp);
	fclose(fp);
	// 校验密码
	while(1)
	{
		char input_pwd[20];
		cout << "请输入系统密码: ";
		cin >> input_pwd;
		if(strcmp(saved_pwd, input_pwd) == 0)
		{
			cout << "密码正确" << endl << endl;
			break;
		}
		else
		{
			cout << "密码错误" << endl;
		}
	}
}

/*
函数：loadAllData()
功能：从文件加载全部数据到链表
 */
void loadAllData()
{
	FILE *fp;
	Record r_temp;
	InputRecord i_temp;
	StockRecord st_temp;
	SellRecord se_temp;

	// 加载唱片基本信息
	fp = fopen(BASE_FILE, "rb");
	if(fp != 0)
	{
		while(fread(&r_temp, sizeof(Record), 1, fp) == 1)
		{
			Record *node = (Record *)malloc(sizeof(Record));
			*node = r_temp;
			node->next = 0;
			// 追加到链表尾
			if(record_head == 0)
			{
				record_head = node;
			}
			else
			{
				Record *p = record_head;
				while(p->next != 0)
				{
					p = p->next;
				}
				p->next = node;
			}
		}
		fclose(fp);
	}

	// 加载进货信息
	fp = fopen(INPUT_FILE, "rb");
	if(fp != 0)
	{
		while(fread(&i_temp, sizeof(InputRecord), 1, fp) == 1)
		{
			InputRecord *node = (InputRecord *)malloc(sizeof(InputRecord));
			*node = i_temp;
			node->next = 0;
			if(input_head == 0)
			{
				input_head = node;
			}
			else
			{
				InputRecord *p = input_head;
				while(p->next != 0)
				{
					p = p->next;
				}
				p->next = node;
			}
		}
		fclose(fp);
	}

	// 加载库存信息
	fp = fopen(STOCK_FILE, "rb");
	if(fp != 0)
	{
		while(fread(&st_temp, sizeof(StockRecord), 1, fp) == 1)
		{
			StockRecord *node = (StockRecord *)malloc(sizeof(StockRecord));
			*node = st_temp;
			node->next = 0;
			if(stock_head == 0)
			{
				stock_head = node;
			}
			else
			{
				StockRecord *p = stock_head;
				while(p->next != 0)
				{
					p = p->next;
				}
				p->next = node;
			}
		}
		fclose(fp);
	}

	// 加载销售信息
	fp = fopen(SELL_FILE, "rb");
	if(fp != 0)
	{
		while(fread(&se_temp, sizeof(SellRecord), 1, fp) == 1)
		{
			SellRecord *node = (SellRecord *)malloc(sizeof(SellRecord));
			*node = se_temp;
			node->next = 0;
			if(sell_head == 0)
			{
				sell_head = node;
			}
			else
			{
				SellRecord *p = sell_head;
				while(p->next != 0)
				{
					p = p->next;
				}
				p->next = node;
			}
		}
		fclose(fp);
	}
}

/*
函数：saveAllData()
功能：将链表数据保存到文件
 */
void saveAllData()
{
	FILE *fp;

	// 保存唱片基本信息
	fp = fopen(BASE_FILE, "wb");
	if(fp != 0)
	{
		Record *p = record_head;
		while(p != 0)
		{
			fwrite(p, sizeof(Record), 1, fp);
			p = p->next;
		}
		fclose(fp);
	}

	// 保存进货信息
	fp = fopen(INPUT_FILE, "wb");
	if(fp != 0)
	{
		InputRecord *p = input_head;
		while(p != 0)
		{
			fwrite(p, sizeof(InputRecord), 1, fp);
			p = p->next;
		}
		fclose(fp);
	}

	// 保存库存信息
	fp = fopen(STOCK_FILE, "wb");
	if(fp != 0)
	{
		StockRecord *p = stock_head;
		while(p != 0)
		{
			fwrite(p, sizeof(StockRecord), 1, fp);
			p = p->next;
		}
		fclose(fp);
	}

	// 保存销售信息
	fp = fopen(SELL_FILE, "wb");
	if(fp != 0)
	{
		SellRecord *p = sell_head;
		while(p != 0)
		{
			fwrite(p, sizeof(SellRecord), 1, fp);
			p = p->next;
		}
		fclose(fp);
	}
}

//  功能函数（空壳占位） 

/*
函数：modifyPassword()
功能：修改系统密码
菜单：1
 */
void modifyPassword()
{
	char old_pwd[20], new_pwd[20];

	cout << "修改密码" << endl;
	cout << "请输入旧密码: ";
	cin >> old_pwd;

	// 读取保存的密码
	FILE *fp = fopen(PWD_FILE, "rb");
	char saved_pwd[20];
	fread(saved_pwd, sizeof(char), 20, fp);
	fclose(fp);

	if(strcmp(old_pwd, saved_pwd) != 0)
	{
		cout << "旧密码错误" << endl;
		return;
	}

	cout << "请输入新密码: ";
	cin >> new_pwd;

	fp = fopen(PWD_FILE, "wb");
	fwrite(new_pwd, sizeof(char), 20, fp);
	fclose(fp);

	cout << "密码修改成功" << endl;
	writeLog("密码", "修改密码");
}

/*
函数：addRecord()
功能：录入唱片基本信息
菜单：2
 */
void addRecord()
{
	Record *r = (Record *)malloc(sizeof(Record));

	cout << "录入唱片基本信息" << endl;
	cout << "唱片编号: ";
	cin >> r->id;

	// 检查编号是否重复
	Record *p = record_head;
	while(p != 0)
	{
		if(p->id == r->id)
		{
			cout << "该编号已存在！" << endl;
			free(r);
			return;
		}
		p = p->next;
	}

	cout << "唱片名: ";
	cin >> r->name;
	cout << "歌手名: ";
	cin >> r->singer;
	cout << "风格: ";
	cin >> r->style;
	cout << "价格: ";
	cin >> r->price;
	cout << "发行日期: ";
	cin >> r->release_date;
	cout << "备注: ";
	cin >> r->note;

	r->next = 0;

	// 追加到链表尾
	if(record_head == 0)
	{
		record_head = r;
	}
	else
	{
		p = record_head;
		while(p->next != 0)
		{
			p = p->next;
		}
		p->next = r;
	}

	// 写入文件
	FILE *fp = fopen(BASE_FILE, "ab");
	fwrite(r, sizeof(Record), 1, fp);
	fclose(fp);

	cout << "唱片基本信息录入成功" << endl;
	writeLog("录入", r->name);
}

/*
函数：inputRecord()
功能：唱片进货管理
菜单：3
 */
void inputRecord()
{
	InputRecord *ir = (InputRecord *)malloc(sizeof(InputRecord));

	cout << "唱片进货管理" << endl;
	cout << "唱片编号: ";
	cin >> ir->id;

	// 从唱片基本信息中查找唱片名
	Record *r = record_head;
	while(r != 0)
	{
		if(r->id == ir->id)
		{
			strcpy(ir->name, r->name);
			break;
		}
		r = r->next;
	}
	if(r == 0)
	{
		cout << "该唱片编号不存在，请先录入唱片基本信息" << endl;
		free(ir);
		return;
	}

	cout << "唱片名称: " << ir->name << endl;
	cout << "进货数量: ";
	cin >> ir->quantity;
	cout << "进货日期: ";
	cin >> ir->date;
	cout << "进货人: ";
	cin >> ir->operator_name;

	ir->next = 0;

	// 追加到进货链表尾
	if(input_head == 0)
	{
		input_head = ir;
	}
	else
	{
		InputRecord *p = input_head;
		while(p->next != 0)
		{
			p = p->next;
		}
		p->next = ir;
	}

	// 更新库存：查找库存中是否有该唱片
	StockRecord *st = stock_head;
	while(st != 0)
	{
		if(st->id == ir->id)
		{
			break;
		}
		st = st->next;
	}
	if(st == 0)
	{
		// 库存中没有，新增一条
		st = (StockRecord *)malloc(sizeof(StockRecord));
		st->id = ir->id;
		strcpy(st->name, ir->name);
		st->stock_qty = ir->quantity;
		st->input_qty = ir->quantity;
		st->sell_qty = 0;
		st->next = 0;
		// 追加到库存链表尾
		if(stock_head == 0)
		{
			stock_head = st;
		}
		else
		{
			StockRecord *p = stock_head;
			while(p->next != 0)
			{
				p = p->next;
			}
			p->next = st;
		}
	}
	else
	{
		// 库存中已有，累加
		st->stock_qty += ir->quantity;
		st->input_qty += ir->quantity;
	}

	// 保存进货文件和库存文件
	FILE *fp = fopen(INPUT_FILE, "ab");
	fwrite(ir, sizeof(InputRecord), 1, fp);
	fclose(fp);

	cout << "进货成功" << endl;
	writeLog("进货", ir->name);
}

/*
函数：sellRecord()
功能：唱片销售管理
菜单：4
 */
void sellRecord()
{
	SellRecord *sr = (SellRecord *)malloc(sizeof(SellRecord));

	cout << "唱片销售管理" << endl;
	cout << "唱片编号: ";
	cin >> sr->record_id;

	// 从唱片基本信息中查找唱片名
	Record *r = record_head;
	while(r != 0)
	{
		if(r->id == sr->record_id)
		{
			strcpy(sr->record_name, r->name);
			break;
		}
		r = r->next;
	}
	if(r == 0)
	{
		cout << "该唱片编号不存在！" << endl;
		free(sr);
		return;
	}

	cout << "唱片名称: " << sr->record_name << endl;

	// 检查库存
	StockRecord *st = stock_head;
	while(st != 0)
	{
		if(st->id == sr->record_id)
		{
			break;
		}
		st = st->next;
	}
	if(st == 0)
	{
		cout << "该唱片没有库存记录" << endl;
		free(sr);
		return;
	}

	cout << "当前库存: " << st->stock_qty << endl;
	cout << "销售价格: ";
	cin >> sr->sell_price;
	cout << "销售数量: ";
	cin >> sr->sell_qty;

	if(sr->sell_qty > st->stock_qty)
	{
		cout << "库存不足！" << endl;
		free(sr);
		return;
	}

	cout << "客户名称: ";
	cin >> sr->customer_name;
	cout << "销售日期: ";
	cin >> sr->sell_date;

	// 计算销售金额
	sr->sell_amount = sr->sell_price * sr->sell_qty;

	// 自动生成销售编号：取当前最大编号+1
	int max_id = 0;
	SellRecord *p = sell_head;
	while(p != 0)
	{
		if(p->sell_id > max_id)
		{
			max_id = p->sell_id;
		}
		p = p->next;
	}
	sr->sell_id = max_id + 1;
	sr->next = 0;

	// 追加到销售链表尾
	if(sell_head == 0)
	{
		sell_head = sr;
	}
	else
	{
		p = sell_head;
		while(p->next != 0)
		{
			p = p->next;
		}
		p->next = sr;
	}

	// 更新库存
	st->stock_qty -= sr->sell_qty;
	st->sell_qty += sr->sell_qty;

	// 追加销售记录到文件
	FILE *fp = fopen(SELL_FILE, "ab");
	fwrite(sr, sizeof(SellRecord), 1, fp);
	fclose(fp);

	cout << "销售成功！销售编号: " << sr->sell_id << ", 销售金额: " << fixed << setprecision(2) << sr->sell_amount << endl;
	writeLog("销售", sr->record_name);
}

/*
函数：modifyRecord()
功能：修改唱片基本信息（含修改+删除子菜单）
菜单：5
 */
void modifyRecord()
{
	// 先验证密码
	char input_pwd[20];
	cout << "请输入密码以验证身份: ";
	cin >> input_pwd;

	FILE *fp = fopen(PWD_FILE, "rb");
	char saved_pwd[20];
	fread(saved_pwd, sizeof(char), 20, fp);
	fclose(fp);

	if(strcmp(input_pwd, saved_pwd) != 0)
	{
		cout << "密码错误" << endl;
		return;
	}

	int choice;
	cout << "修改唱片基本信息" << endl;
	cout << "  1. 修改唱片信息" << endl;
	cout << "  2. 删除唱片信息" << endl;
	cout << "  0. 返回上级" << endl;
	cout << "请选择: ";
	cin >> choice;

	if(choice == 0)
	{
		return;
	}

	int id;
	cout << "请输入唱片编号: ";
	cin >> id;

	// 在链表中查找唱片
	Record *p = record_head;
	Record *prev = 0;  // 记录前面的节点
	while(p != 0)
	{
		if(p->id == id)
		{
			break;
		}
		prev = p;
		p = p->next;
	}

	if(p == 0)
	{
		cout << "未找到该唱片！" << endl;
		return;
	}

	if(choice == 1)
	{
		// 修改唱片信息
		cout << "当前信息:" << endl;
		cout << "  唱片名: " << p->name << endl;
		cout << "  歌手名: " << p->singer << endl;
		cout << "  风格: " << p->style << endl;
		cout << "  价格: " << p->price << endl;
		cout << "  发行日期: " << p->release_date << endl;
		cout << "  备注: " << p->note << endl;
		cout << "请输入新信息:" << endl;

		cout << "唱片名: ";
		cin >> p->name;
		cout << "歌手名: ";
		cin >> p->singer;
		cout << "风格: ";
		cin >> p->style;
		cout << "价格: ";
		cin >> p->price;
		cout << "发行日期: ";
		cin >> p->release_date;
		cout << "备注: ";
		cin >> p->note;

		cout << "修改成功！" << endl;
		writeLog("修改", p->name);
	}
	else if(choice == 2)
	{
		// 删除唱片信息
		if(prev == 0)
		{
			record_head = p->next;
		}
		else
		{
			prev->next = p->next;
		}
		cout << "唱片 \"" << p->name << "\" 已删除！" << endl;
		writeLog("删除", p->name);
		free(p);
	}
	else
	{
		cout << "输入有误！" << endl;
	}
}

/*
函数：showAll()
功能：输出全部信息
菜单：6
 */
void showAll()
{
	Record *r;
	InputRecord *ir;
	StockRecord *st;
	SellRecord *sr;

	// 输出唱片基本信息
	cout << " 唱片基本信息 " << endl;
	if(record_head == 0)
	{
		cout << "暂无数据" << endl;
	}
	else
	{
		r = record_head;
		while(r != 0)
		{
			cout << "编号: " << r->id << endl;
			cout << "  唱片名: " << r->name << endl;
			cout << "  歌手名: " << r->singer << endl;
			cout << "  风格: " << r->style << endl;
			cout << "  价格: " << fixed << setprecision(2) << r->price << endl;
			cout << "  发行日期: " << r->release_date << endl;
			cout << "  备注: " << r->note << endl;
			cout << endl;
			r = r->next;
		}
	}

	// 输出进货信息
	cout << "唱片进货信息" << endl;
	if(input_head == 0)
	{
		cout << "暂无数据" << endl;
	}
	else
	{
		ir = input_head;
		while(ir != 0)
		{
			cout << "唱片编号: " << ir->id << endl;
			cout << "  唱片名称: " << ir->name << endl;
			cout << "  进货数量: " << ir->quantity << endl;
			cout << "  进货日期: " << ir->date << endl;
			cout << "  进货人: " << ir->operator_name << endl;
			cout << endl;
			ir = ir->next;
		}
	}

	// 输出库存信息
	cout << " 唱片库存信息 " << endl;
	if(stock_head == 0)
	{
		cout << "暂无数据" << endl;
	}
	else
	{
		st = stock_head;
		while(st != 0)
		{
			cout << "唱片编号: " << st->id << endl;
			cout << "  唱片名称: " << st->name << endl;
			cout << "  库存数量: " << st->stock_qty << endl;
			cout << "  累计进货: " << st->input_qty << endl;
			cout << "  累计销售: " << st->sell_qty << endl;
			cout << endl;
			st = st->next;
		}
	}

	// 输出销售信息
	cout << " 唱片销售信息 " << endl;
	if(sell_head == 0)
	{
		cout << "暂无数据" << endl;
	}
	else
	{
		sr = sell_head;
		while(sr != 0)
		{
			cout << "销售编号: " << sr->sell_id << endl;
			cout << "  唱片编号: " << sr->record_id << endl;
			cout << "  唱片名称: " << sr->record_name << endl;
			cout << "  销售价格: " << fixed << setprecision(2) << sr->sell_price << endl;
			cout << "  销售数量: " << sr->sell_qty << endl;
			cout << "  销售金额: " << fixed << setprecision(2) << sr->sell_amount << endl;
			cout << "  客户名称: " << sr->customer_name << endl;
			cout << "  销售日期: " << sr->sell_date << endl;
			cout << endl;
			sr = sr->next;
		}
	}
}

/*
函数：searchRecord()
功能：查询管理（二级菜单：选信息类型 → 选查询条件 → 输出全部匹配结果）
菜单：7
 */
void searchRecord()
{
	int type;
	cout << " 查询管理 " << endl;
	cout << "  1. 查询唱片基本信息" << endl;
	cout << "  2. 查询唱片进货信息" << endl;
	cout << "  3. 查询唱片库存信息" << endl;
	cout << "  4. 查询唱片销售信息" << endl;
	cout << "  0. 返回上级" << endl;
	cout << "请选择查询类型: ";
	cin >> type;

	if(type == 0)
	{
		return;
	}

	if(type == 1)
	{
		// 查询唱片基本信息
		int cond;
		cout << "  1. 按唱片编号  2. 按唱片名  3. 按歌手名" << endl;
		cout << "  4. 按风格      5. 按价格    6. 按发行日期" << endl;
		cout << "请选择查询条件: ";
		cin >> cond;

		int id;
		double price;
		char keyword[50];
		Record *p;
		int found = 0;

		if(cond == 1)
		{
			cout << "请输入唱片编号: ";
			cin >> id;
			p = record_head;
			while(p != 0)
			{
				if(p->id == id)
				{
					cout << p->id << "\t" << p->name << "\t" << p->singer << "\t" << p->style << "\t" << fixed << setprecision(2) << p->price << "\t" << p->release_date << "\t" << p->note << endl;
					found = 1;
				}
				p = p->next;
			}
		}
		else if(cond == 2)
		{
			cout << "请输入唱片名: ";
			cin >> keyword;
			p = record_head;
			while(p != 0)
			{
				if(strcmp(p->name, keyword) == 0)
				{
					cout << p->id << "\t" << p->name << "\t" << p->singer << "\t" << p->style << "\t" << fixed << setprecision(2) << p->price << "\t" << p->release_date << "\t" << p->note << endl;
					found = 1;
				}
				p = p->next;
			}
		}
		else if(cond == 3)
		{
			cout << "请输入歌手名: ";
			cin >> keyword;
			p = record_head;
			while(p != 0)
			{
				if(strcmp(p->singer, keyword) == 0)
				{
					cout << p->id << "\t" << p->name << "\t" << p->singer << "\t" << p->style << "\t" << fixed << setprecision(2) << p->price << "\t" << p->release_date << "\t" << p->note << endl;
					found = 1;
				}
				p = p->next;
			}
		}
		else if(cond == 4)
		{
			cout << "请输入风格: ";
			cin >> keyword;
			p = record_head;
			while(p != 0)
			{
				if(strcmp(p->style, keyword) == 0)
				{
					cout << p->id << "\t" << p->name << "\t" << p->singer << "\t" << p->style << "\t" << fixed << setprecision(2) << p->price << "\t" << p->release_date << "\t" << p->note << endl;
					found = 1;
				}
				p = p->next;
			}
		}
		else if(cond == 5)
		{
			cout << "请输入价格: ";
			cin >> price;
			p = record_head;
			while(p != 0)
			{
				if(p->price == price)
				{
					cout << p->id << "\t" << p->name << "\t" << p->singer << "\t" << p->style << "\t" << fixed << setprecision(2) << p->price << "\t" << p->release_date << "\t" << p->note << endl;
					found = 1;
				}
				p = p->next;
			}
		}
		else if(cond == 6)
		{
			cout << "请输入发行日期(如2026-07-15): ";
			cin >> keyword;
			p = record_head;
			while(p != 0)
			{
				if(strcmp(p->release_date, keyword) == 0)
				{
					cout << p->id << "\t" << p->name << "\t" << p->singer << "\t" << p->style << "\t" << fixed << setprecision(2) << p->price << "\t" << p->release_date << "\t" << p->note << endl;
					found = 1;
				}
				p = p->next;
			}
		}
		else
		{
			cout << "输入有误" << endl;
			return;
		}

		if(found == 0)
		{
			cout << "未找到匹配的唱片基本信息" << endl;
		}
	}
	else if(type == 2)
	{
		// 查询唱片进货信息
		int cond;
		cout << "  1. 按唱片编号  2. 按唱片名称  3. 按进货数量" << endl;
		cout << "  4. 按进货日期  5. 按进货人" << endl;
		cout << "请选择查询条件: ";
		cin >> cond;

		int id, qty;
		char keyword[50];
		InputRecord *ir;
		int found = 0;

		if(cond == 1)
		{
			cout << "请输入唱片编号: ";
			cin >> id;
			ir = input_head;
			while(ir != 0)
			{
				if(ir->id == id)
				{
					cout << ir->id << "\t" << ir->name << "\t" << ir->quantity << "\t" << ir->date << "\t" << ir->operator_name << endl;
					found = 1;
				}
				ir = ir->next;
			}
		}
		else if(cond == 2)
		{
			cout << "请输入唱片名称: ";
			cin >> keyword;
			ir = input_head;
			while(ir != 0)
			{
				if(strcmp(ir->name, keyword) == 0)
				{
					cout << ir->id << "\t" << ir->name << "\t" << ir->quantity << "\t" << ir->date << "\t" << ir->operator_name << endl;
					found = 1;
				}
				ir = ir->next;
			}
		}
		else if(cond == 3)
		{
			cout << "请输入进货数量: ";
			cin >> qty;
			ir = input_head;
			while(ir != 0)
			{
				if(ir->quantity == qty)
				{
					cout << ir->id << "\t" << ir->name << "\t" << ir->quantity << "\t" << ir->date << "\t" << ir->operator_name << endl;
					found = 1;
				}
				ir = ir->next;
			}
		}
		else if(cond == 4)
		{
			cout << "请输入进货日期: ";
			cin >> keyword;
			ir = input_head;
			while(ir != 0)
			{
				if(strcmp(ir->date, keyword) == 0)
				{
					cout << ir->id << "\t" << ir->name << "\t" << ir->quantity << "\t" << ir->date << "\t" << ir->operator_name << endl;
					found = 1;
				}
				ir = ir->next;
			}
		}
		else if(cond == 5)
		{
			cout << "请输入进货人: ";
			cin >> keyword;
			ir = input_head;
			while(ir != 0)
			{
				if(strcmp(ir->operator_name, keyword) == 0)
				{
					cout << ir->id << "\t" << ir->name << "\t" << ir->quantity << "\t" << ir->date << "\t" << ir->operator_name << endl;
					found = 1;
				}
				ir = ir->next;
			}
		}
		else
		{
			cout << "输入有误！" << endl;
			return;
		}

		if(found == 0)
		{
			cout << "未找到匹配的进货信息！" << endl;
		}
	}
	else if(type == 3)
	{
		// 查询唱片库存信息
		int cond;
		cout << "  1. 按唱片编号  2. 按唱片名称  3. 按库存数量" << endl;
		cout << "  4. 按进货数量  5. 按销售数量" << endl;
		cout << "请选择查询条件: ";
		cin >> cond;

		int id, qty;
		char keyword[50];
		StockRecord *st;
		int found = 0;

		if(cond == 1)
		{
			cout << "请输入唱片编号: ";
			cin >> id;
			st = stock_head;
			while(st != 0)
			{
				if(st->id == id)
				{
					cout << st->id << "\t" << st->name << "\t" << st->stock_qty << "\t" << st->input_qty << "\t" << st->sell_qty << endl;
					found = 1;
				}
				st = st->next;
			}
		}
		else if(cond == 2)
		{
			cout << "请输入唱片名称: ";
			cin >> keyword;
			st = stock_head;
			while(st != 0)
			{
				if(strcmp(st->name, keyword) == 0)
				{
					cout << st->id << "\t" << st->name << "\t" << st->stock_qty << "\t" << st->input_qty << "\t" << st->sell_qty << endl;
					found = 1;
				}
				st = st->next;
			}
		}
		else if(cond == 3)
		{
			cout << "请输入库存数量: ";
			cin >> qty;
			st = stock_head;
			while(st != 0)
			{
				if(st->stock_qty == qty)
				{
					cout << st->id << "\t" << st->name << "\t" << st->stock_qty << "\t" << st->input_qty << "\t" << st->sell_qty << endl;
					found = 1;
				}
				st = st->next;
			}
		}
		else if(cond == 4)
		{
			cout << "请输入进货数量: ";
			cin >> qty;
			st = stock_head;
			while(st != 0)
			{
				if(st->input_qty == qty)
				{
					cout << st->id << "\t" << st->name << "\t" << st->stock_qty << "\t" << st->input_qty << "\t" << st->sell_qty << endl;
					found = 1;
				}
				st = st->next;
			}
		}
		else if(cond == 5)
		{
			cout << "请输入销售数量: ";
			cin >> qty;
			st = stock_head;
			while(st != 0)
			{
				if(st->sell_qty == qty)
				{
					cout << st->id << "\t" << st->name << "\t" << st->stock_qty << "\t" << st->input_qty << "\t" << st->sell_qty << endl;
					found = 1;
				}
				st = st->next;
			}
		}
		else
		{
			cout << "输入有误！" << endl;
			return;
		}

		if(found == 0)
		{
			cout << "未找到匹配的库存信息！" << endl;
		}
	}
	else if(type == 4)
	{
		// 查询唱片销售信息
		int cond;
		cout << "  1. 按销售编号  2. 按唱片编号  3. 按唱片名称" << endl;
		cout << "  4. 按销售价格  5. 按销售数量  6. 按客户名称" << endl;
		cout << "  7. 按销售日期  8. 按销售金额" << endl;
		cout << "请选择查询条件: ";
		cin >> cond;

		int id, qty;
		double val;
		char keyword[50];
		SellRecord *sr;
		int found = 0;

		if(cond == 1)
		{
			cout << "请输入销售编号: ";
			cin >> id;
			sr = sell_head;
			while(sr != 0)
			{
				if(sr->sell_id == id)
				{
					cout << sr->sell_id << "\t" << sr->record_id << "\t" << sr->record_name << "\t" << fixed << setprecision(2) << sr->sell_price << "\t" << sr->sell_qty << "\t" << sr->customer_name << "\t" << sr->sell_date << "\t" << fixed << setprecision(2) << sr->sell_amount << endl;
					found = 1;
				}
				sr = sr->next;
			}
		}
		else if(cond == 2)
		{
			cout << "请输入唱片编号: ";
			cin >> id;
			sr = sell_head;
			while(sr != 0)
			{
				if(sr->record_id == id)
				{
					cout << sr->sell_id << "\t" << sr->record_id << "\t" << sr->record_name << "\t" << fixed << setprecision(2) << sr->sell_price << "\t" << sr->sell_qty << "\t" << sr->customer_name << "\t" << sr->sell_date << "\t" << fixed << setprecision(2) << sr->sell_amount << endl;
					found = 1;
				}
				sr = sr->next;
			}
		}
		else if(cond == 3)
		{
			cout << "请输入唱片名称: ";
			cin >> keyword;
			sr = sell_head;
			while(sr != 0)
			{
				if(strcmp(sr->record_name, keyword) == 0)
				{
					cout << sr->sell_id << "\t" << sr->record_id << "\t" << sr->record_name << "\t" << fixed << setprecision(2) << sr->sell_price << "\t" << sr->sell_qty << "\t" << sr->customer_name << "\t" << sr->sell_date << "\t" << fixed << setprecision(2) << sr->sell_amount << endl;
					found = 1;
				}
				sr = sr->next;
			}
		}
		else if(cond == 4)
		{
			cout << "请输入销售价格: ";
			cin >> val;
			sr = sell_head;
			while(sr != 0)
			{
				if(sr->sell_price == val)
				{
					cout << sr->sell_id << "\t" << sr->record_id << "\t" << sr->record_name << "\t" << fixed << setprecision(2) << sr->sell_price << "\t" << sr->sell_qty << "\t" << sr->customer_name << "\t" << sr->sell_date << "\t" << fixed << setprecision(2) << sr->sell_amount << endl;
					found = 1;
				}
				sr = sr->next;
			}
		}
		else if(cond == 5)
		{
			cout << "请输入销售数量: ";
			cin >> qty;
			sr = sell_head;
			while(sr != 0)
			{
				if(sr->sell_qty == qty)
				{
					cout << sr->sell_id << "\t" << sr->record_id << "\t" << sr->record_name << "\t" << fixed << setprecision(2) << sr->sell_price << "\t" << sr->sell_qty << "\t" << sr->customer_name << "\t" << sr->sell_date << "\t" << fixed << setprecision(2) << sr->sell_amount << endl;
					found = 1;
				}
				sr = sr->next;
			}
		}
		else if(cond == 6)
		{
			cout << "请输入客户名称: ";
			cin >> keyword;
			sr = sell_head;
			while(sr != 0)
			{
				if(strcmp(sr->customer_name, keyword) == 0)
				{
					cout << sr->sell_id << "\t" << sr->record_id << "\t" << sr->record_name << "\t" << fixed << setprecision(2) << sr->sell_price << "\t" << sr->sell_qty << "\t" << sr->customer_name << "\t" << sr->sell_date << "\t" << fixed << setprecision(2) << sr->sell_amount << endl;
					found = 1;
				}
				sr = sr->next;
			}
		}
		else if(cond == 7)
		{
			cout << "请输入销售日期: ";
			cin >> keyword;
			sr = sell_head;
			while(sr != 0)
			{
				if(strcmp(sr->sell_date, keyword) == 0)
				{
					cout << sr->sell_id << "\t" << sr->record_id << "\t" << sr->record_name << "\t" << fixed << setprecision(2) << sr->sell_price << "\t" << sr->sell_qty << "\t" << sr->customer_name << "\t" << sr->sell_date << "\t" << fixed << setprecision(2) << sr->sell_amount << endl;
					found = 1;
				}
				sr = sr->next;
			}
		}
		else if(cond == 8)
		{
			cout << "请输入销售金额: ";
			cin >> val;
			sr = sell_head;
			while(sr != 0)
			{
				if(sr->sell_amount == val)
				{
					cout << sr->sell_id << "\t" << sr->record_id << "\t" << sr->record_name << "\t" << fixed << setprecision(2) << sr->sell_price << "\t" << sr->sell_qty << "\t" << sr->customer_name << "\t" << sr->sell_date << "\t" << fixed << setprecision(2) << sr->sell_amount << endl;
					found = 1;
				}
				sr = sr->next;
			}
		}
		else
		{
			cout << "输入有误！" << endl;
			return;
		}

		if(found == 0)
		{
			cout << "未找到匹配的销售信息！" << endl;
		}
	}
	else
	{
		cout << "输入有误！" << endl;
	}
}

/*
函数：statistics()
功能：统计管理（库存统计/销售金额统计/进货总量统计）
菜单：8
 */
void statistics()
{
	int type;
	cout << " 统计管理 " << endl;
	cout << "  1. 唱片进货信息统计" << endl;
	cout << "  2. 唱片库存信息统计" << endl;
	cout << "  3. 唱片销售信息统计" << endl;
	cout << "  0. 返回上级" << endl;
	cout << "请选择: ";
	cin >> type;

	if(type == 0)
	{
		return;
	}

	if(type == 1)
	{
		// 唱片进货信息统计
		int cond;
		cout << "  1. 按唱片编号  2. 按唱片名称" << endl;
		cout << "  3. 按进货日期  4. 按进货人" << endl;
		cout << "请选择统计条件: ";
		cin >> cond;

		// 用一个临时结构体数组存放汇总结果
		struct	//用来记录结果，日期、名称、进货人都记录在qty，id记录在id。这样在后面查找的时候会很方便。qty作为数量信息是这里的必要信息，其他两个则按需记录
		{
			int id;
			char name[50];
			int total_qty;
		} result[100];
		int result_count = 0;

		InputRecord *ir = input_head;
		while(ir != 0)
		{
			// 确定当前记录的分组 key
			int key_id = 0;
			char key_str[50] = "";

			if(cond == 1)
			{
				key_id = ir->id;
			}
			else if(cond == 2)
			{
				strcpy(key_str, ir->name);
			}
			else if(cond == 3)
			{
				strcpy(key_str, ir->date);
			}
			else if(cond == 4)
			{
				strcpy(key_str, ir->operator_name);
			}
			else
			{
				cout << "输入有误！" << endl;
				return;
			}

			// 在 result 中查找是否已有该组
			int found = 0;
			for(int i = 0; i < result_count; i++)
			{
				if((cond == 1 && result[i].id == key_id) ||
				(cond != 1 && strcmp(result[i].name, key_str) == 0))
				{
					result[i].total_qty += ir->quantity;
					found = 1;
					break;
				}
			}

			// 没有则新增一组
			if(found == 0)
			{
				if(cond == 1)
				{
					result[result_count].id = key_id;
				}
				strcpy(result[result_count].name, key_str);
				result[result_count].total_qty = ir->quantity;
				result_count++;
			}

			ir = ir->next;
		}

		// 输出统计结果
		cout << "进货统计结果" << endl;
		for(int i = 0; i < result_count; i++)
		{
			if(cond == 1)
			{
				cout << "编号: " << result[i].id;
			}
			else
			{
				cout << result[i].name;
			}
			cout << "进货总量: " << result[i].total_qty << endl;
		}
		if(result_count == 0)
		{
			cout << "暂无进货数据" << endl;
		}
	}
	else if(type == 2)
	{
		// 唱片库存信息统计：按唱片名称
		char keyword[50];
		cout << "请输入唱片名称: ";
		cin >> keyword;

		StockRecord *st = stock_head;
		int found = 0;
		while(st != 0)
		{
			if(strcmp(st->name, keyword) == 0)
			{
				cout << "库存统计结果" << endl;
				cout << "唱片名称: " << st->name << endl;
				cout << "库存数量: " << st->stock_qty << endl;
				cout << "进货数量: " << st->input_qty << endl;
				cout << "销售数量: " << st->sell_qty << endl;
				found = 1;
				break;
			}
			st = st->next;
		}
		if(found == 0)
		{
			cout << "未找到该唱片的库存信息！" << endl;
		}
	}
	else if(type == 3)
	{
		// 唱片销售信息统计
		int cond;
		cout << "  1. 按唱片编号  2. 按唱片名称" << endl;
		cout << "  3. 按客户名称  4. 按销售日期" << endl;
		cout << "请选择统计条件: ";
		cin >> cond;

		struct
		{
			int id;
			char name[50];
			int total_qty;
			double total_amount;
		} result[100];
		int result_count = 0;

		SellRecord *sr = sell_head;
		while(sr != 0)
		{
			int key_id = 0;
			char key_str[50] = "";

			if(cond == 1)
			{
				key_id = sr->record_id;
			}
			else if(cond == 2)
			{
				strcpy(key_str, sr->record_name);
			}
			else if(cond == 3)
			{
				strcpy(key_str, sr->customer_name);
			}
			else if(cond == 4)
			{
				strcpy(key_str, sr->sell_date);
			}
			else
			{
				cout << "输入有误" << endl;
				return;
			}

			// 在 result 中查找
			int found = 0;
			for(int i = 0; i < result_count; i++)
			{
				if((cond == 1 && result[i].id == key_id) ||
				(cond != 1 && strcmp(result[i].name, key_str) == 0))
				{
					result[i].total_qty += sr->sell_qty;
					result[i].total_amount += sr->sell_amount;
					found = 1;
					break;
				}
			}

			if(found == 0)
			{
				if(cond == 1)
				{
					result[result_count].id = key_id;
				}
				strcpy(result[result_count].name, key_str);
				result[result_count].total_qty = sr->sell_qty;
				result[result_count].total_amount = sr->sell_amount;
				result_count++;
			}

			sr = sr->next;
		}

		// 输出统计结果
		cout << "销售统计结果" << endl;
		for(int i = 0; i < result_count; i++)
		{
			if(cond == 1)
			{
				cout << "编号: " << result[i].id;
			}
			else
			{
				cout << result[i].name;
			}
			cout << "  销售总量: " << result[i].total_qty;
			cout << "  销售总额: " << fixed << setprecision(2) << result[i].total_amount << endl;
		}
		if(result_count == 0)
		{
			cout << "暂无销售数据" << endl;
		}
	}
	else
	{
		cout << "输入有误！" << endl;
	}
}


/*
函数：report()
功能：汇总报表（唱片信息统计 / 进货日期统计 / 销售日期统计）
菜单：9
 */
void report()
{
	int type;
	cout << "汇总报表" << endl;
	cout << "1. 唱片信息统计" << endl;
	cout << "2. 进货日期信息统计" << endl;
	cout << "3. 销售日期信息统计" << endl;
	cout << "0. 返回上级" << endl;
	cout << "请选择: ";
	cin >> type;

	if(type == 0)
	{
		return;
	}

	if(type == 1)
	{
		// 唱片信息统计
		int cond;
		cout << "  1. 按唱片名统计  2. 按歌手名统计  3. 按风格统计" << endl;
		cout << "请选择: ";
		cin >> cond;

		// 汇总结构体
		struct
		{
			char name[50];
			int cd_count;    // 唱片数
			int total_stock;    // 总库存
			int total_input;    // 总进货
			int total_sell;    // 总销售
		} result[100];
		int result_count = 0;

		// 遍历唱片基本信息链表，按条件分组
		Record *r = record_head;
		while(r != 0)
		{
			char key[50];
			if(cond == 1)
			{
				strcpy(key, r->name);
			}
			else if(cond == 2)
			{
				strcpy(key, r->singer);
			}
			else if(cond == 3)
			{
				strcpy(key, r->style);
			}
			else
			{
				cout << "输入有误！" << endl;
				return;
			}

			// 在 result 中查找该 key
			int found = 0;
			for(int i = 0; i < result_count; i++)
			{
				if(strcmp(result[i].name, key) == 0)
				{
					result[i].cd_count++;
					found = 1;
					break;
				}
			}

			if(found == 0)
			{
				strcpy(result[result_count].name, key);
				result[result_count].cd_count = 1;
				result[result_count].total_stock = 0;
				result[result_count].total_input = 0;
				result[result_count].total_sell = 0;
				result_count++;
			}

			r = r->next;
		}

		// 再从库存链表补充每个 key 的库存/进货/销售数据
		for(int i = 0; i < result_count; i++)
		{
			// 找到该 key 对应的所有唱片编号，从库存汇总
			r = record_head;
			while(r != 0)
			{
				//因为库存信息里没有歌手和风格信息，基础信息里没有库存数量，所以要现在基础信息里配对，然后再对应库存信息
				int match = 0;
				if(cond == 1)
				{
					match = (strcmp(r->name, result[i].name) == 0);
				}
				else if(cond == 2)
				{
					match = (strcmp(r->singer, result[i].name) == 0);
				}
				else
				{
					match = (strcmp(r->style, result[i].name) == 0);
				}

				if(match)
				{
					StockRecord *st = stock_head;
					while(st != 0)
					{
						if(st->id == r->id)
						{
							result[i].total_stock += st->stock_qty;
							result[i].total_input += st->input_qty;
							result[i].total_sell += st->sell_qty;
							break;
						}
						st = st->next;
					}
				}
				r = r->next;
			}
		}

		// 输出
		cout << "唱片信息统计报表" << endl;
		for(int i = 0; i < result_count; i++)
		{
			cout << result[i].name << ": ";
			cout << "唱片数" << result[i].cd_count;
			cout << "  库存总量" << result[i].total_stock;
			cout << "  进货总量" << result[i].total_input;
			cout << "  销售总量" << result[i].total_sell << endl;
		}
		if(result_count == 0)
		{
			cout << "暂无数据" << endl;
		}
	}
	else if(type == 2)
	{
		// 进货日期信息统计：按进货日期汇总
		struct
		{
			char date[20];
			int total_qty;    // 进货总量
			int batch_count;    // 进货批数
		} result[100];
		int result_count = 0;

		InputRecord *ir = input_head;
		while(ir != 0)
		{
			// 在 result 中查找该日期
			int found = 0;
			for(int i = 0; i < result_count; i++)
			{
				if(strcmp(result[i].date, ir->date) == 0)
				{
					result[i].total_qty += ir->quantity;
					result[i].batch_count++;
					found = 1;
					break;
				}
			}

			if(found == 0)
			{
				strcpy(result[result_count].date, ir->date);
				result[result_count].total_qty = ir->quantity;
				result[result_count].batch_count = 1;
				result_count++;
			}

			ir = ir->next;
		}

		cout << "进货日期统计报表" << endl;
		for(int i = 0; i < result_count; i++)
		{
			cout << result[i].date << ": ";
			cout << "进货总量" << result[i].total_qty;
			cout << "  进货批次" << result[i].batch_count << endl;
		}
		if(result_count == 0)
		{
			cout << "暂无进货数据" << endl;
		}
	}
	else if(type == 3)
	{
		// 销售日期信息统计：按销售日期汇总
		struct
		{
			char date[20];
			int total_qty;    // 销售总量
			double total_amount;    // 销售总额
			int order_count;    // 订单数
		} result[100];
		int result_count = 0;

		SellRecord *sr = sell_head;
		while(sr != 0)
		{
			// 在 result 中查找该日期
			int found = 0;
			for(int i = 0; i < result_count; i++)
			{
				if(strcmp(result[i].date, sr->sell_date) == 0)
				{
					result[i].total_qty += sr->sell_qty;
					result[i].total_amount += sr->sell_amount;
					result[i].order_count++;
					found = 1;
					break;
				}
			}

			if(found == 0)
			{
				strcpy(result[result_count].date, sr->sell_date);
				result[result_count].total_qty = sr->sell_qty;
				result[result_count].total_amount = sr->sell_amount;
				result[result_count].order_count = 1;
				result_count++;
			}

			sr = sr->next;
		}

		cout << "销售日期统计报表" << endl;
		for(int i = 0; i < result_count; i++)
		{
			cout << result[i].date << ": ";
			cout << "销售总量" << result[i].total_qty;
			cout << "  销售总额" << fixed << setprecision(2) << result[i].total_amount;
			cout << "  订单数" << result[i].order_count << endl;
		}
		if(result_count == 0)
		{
			cout << "暂无销售数据" << endl;
		}
	}
	else
	{
		cout << "输入有误" << endl;
	}
}

/*
函数：saveAndExit()
功能：保存数据并退出
菜单：10
 */
/*
函数：freeAllData()
功能：释放所有链表节点内存
 */
void freeAllData()
{
	Record *r = record_head;
	while(r != 0)
	{
		Record *tmp = r;
		r = r->next;
		free(tmp);
	}
	record_head = 0;

	InputRecord *ir = input_head;
	while(ir != 0)
	{
		InputRecord *tmp = ir;
		ir = ir->next;
		free(tmp);
	}
	input_head = 0;

	StockRecord *st = stock_head;
	while(st != 0)
	{
		StockRecord *tmp = st;
		st = st->next;
		free(tmp);
	}
	stock_head = 0;

	SellRecord *sr = sell_head;
	while(sr != 0)
	{
		SellRecord *tmp = sr;
		sr = sr->next;
		free(tmp);
	}
	sell_head = 0;
}

void saveAndExit()
{
	cout << "保存数据" << endl;
	saveAllData();
	freeAllData();
	cout << "感谢使用唱片销售管理系统，再见" << endl;
	writeLog("系统", "退出系统");
	exit(0);
}

//主函数

int main()
{
	system("chcp 65001 > nul");  // 设置控制台编码为UTF-8，防止中文乱码

	cout << "=========================" << endl;
	cout << "     唱片销售管理系统     " << endl;
	cout << "=========================" << endl;

	// 密码校验
	loadPassword();

	// 从文件加载已有数据
	loadAllData();

	// 主菜单循环
	int choice;
	while(1)
	{
		cout << "=========================" << endl;
		cout << "     唱片销售管理系统     " << endl;
		cout << "=========================" << endl;
		cout << "  1. 设置及修改密码" << endl;
		cout << "  2. 录入唱片基本信息" << endl;
		cout << "  3. 唱片进货管理" << endl;
		cout << "  4. 唱片销售管理" << endl;
		cout << "  5. 修改唱片基本信息" << endl;
		cout << "  6. 输出全部信息" << endl;
		cout << "  7. 查询管理" << endl;
		cout << "  8. 统计管理" << endl;
		cout << "  9. 汇总报表" << endl;
		cout << "  10. 退出系统" << endl;
		cout << "" << endl;
		cout << "请选择(1-10): ";
		cin >> choice;
		// 处理错误的输入
		if(cin.fail())
		{
			cin.clear();                  // 清除错误状态
			cin.ignore(10000, '\n');      // 丢弃缓冲区中的错误内容
			cout << "输入有误，请重新选择！" << endl;
			continue;
		}

		switch(choice)
		{
		case 1:
			modifyPassword();
			break;
		case 2:
			addRecord();
			break;
		case 3:
			inputRecord();
			break;
		case 4:
			sellRecord();
			break;
		case 5:
			modifyRecord();
			break;
		case 6:
			showAll();
			break;
		case 7:
			searchRecord();
			break;
		case 8:
			statistics();
			break;
		case 9:
			report();
			break;
		case 10:
			saveAndExit();
			break;
		default:
			cout << "输入有误，请重新选择！" << endl;
		}
		cout << endl;
	}

	return 0;
}
