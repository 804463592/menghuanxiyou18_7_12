
//注释5

//注释6

//menghuanxiyou18_7_12.cpp: 定义应用程序的入口点。
//
#include <stdio.h>
#include <Windows.h>
#include "stdafx.h"
#include "menghuanxiyou18_7_12.h"
#include <tchar.h>
#include <time.h>
#include <string.h>
//多媒体设备的头文件和库文件
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Msimg32.lib")    


///
//#include <gdiplus.h>
//using namespace Gdiplus

#define MAX_LOADSTRING 100
#define WND_WIDTH  1400
#define WND_HEIGHT 900                    //后面不能加逗号，否则会引起不可预知的错误

//角色结构体
 struct 
{
	int   NowMp;       //当前的法力值
	int   NowHp;       //当前血量
	int   MaxMp;       //最大的法力值
	int   MaxHp;       //最大生命值
	int   level;       //等级
	int   strength;    //力量
	int   Intell;      //智力
	int   Agility;     //敏捷

}Hero,Boss;               //定义英雄和boss

 //枚举技能
 enum ActionType { //结尾处不能加分号，加的是逗号
	 NORMAL = 0,      //普通攻击
     CRITICAL = 1,    //暴击
	 MAGIC = 2,       //魔法攻击
	 MISS = 3,        //隐匿
	 RECOVER = 4,      //恢复 
	 UTIMATE=5          //大招
 }; 

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING]=L"这里定义一个名字";                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HDC    g_hdc, g_memdc,g_bufdc;                   //窗口的设备句柄，内存设备的句柄 
int    boss_num = 0;                             //计数Boss图片数量
int    hero_num = 0;                             // 计数hero图片数量
DWORD  g_tPre, g_tNow;                           //用作计时器
BOOL   g_bCanAttack,g_bGameOver;                 //表示人是否可以攻击,判断游戏是否结束
BOOL   g_bIsAngry=false;                         //表示boss进入狂暴
BOOL   g_bAboutHero=true;                             //表示输出文字信息关于hero

int    g_frame=0;                                //帧数
ActionType HeroAction, BossAction;               //枚举类型的Hero和Boss的技能

wchar_t str[100];                                 //复用存储原始的文字
wchar_t  text[8][100];                           //存储着输出文字的字符串数组
int      g_iTextNum=0;                           //文字的个数
RECT     g_rect;                                 //做一个矩形区域显示文字

HBITMAP  g_bmp;                                 //空位图
HBITMAP  g_backbmp,g_skillbutton1, g_skillbutton2, g_skillbutton3, g_skillbutton4;         //背景图,以及其他
HBITMAP  g_hero,g_boss;                        //英雄和怪物
HBITMAP  g_heroskill1, g_heroskill2, g_heroskill3, g_heroskill4;
HBITMAP  g_bossskill1, g_bossskill2, g_bossskill3, g_bossskill4;
HBITMAP  g_recover, g_snow;                            //恢复效果
HBITMAP  g_gameover, g_gamevictory;                 //游戏失败和游戏胜利的图像


BOOL  GameInit(HWND hwnd);                      //函数声明声明
VOID  GamePaint(HWND hwnd);
VOID  HeroAction_Logic();                     //玩家的逻辑代码
VOID  HeroActionPaint();
VOID  BossAction_Logic();
VOID  BossActionPaint();
VOID DieCheck(int nowhp, bool ishero);
VOID Message_Insert(wchar_t* str);


//ULONG_PTR m_gdiplusToken;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,         //当前实例句柄
                     _In_opt_ HINSTANCE hPrevInstance,     //先前实例2句柄
                     _In_ LPWSTR    lpCmdLine,               //命令行指针
                     _In_ int       nCmdShow)                 //显示的格式
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MENGHUANXIYOU18712, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

   /* // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }*/
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hwnd = CreateWindowW(szWindowClass, L"梦幻西游demo（单回合练习）", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT, nullptr, nullptr, hInstance, nullptr);

	if (!hwnd)
	{
		return FALSE;
	}
	
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);



   //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MENGHUANXIYOU18712));

	
   if (!GameInit(hwnd))
	   return -1;
	
	
  // mciSendString(L"open ./res/fade.mp3 alias music", 0, 0, 0); //向驱动层发送字符串消息，open后面要空一格啊，不然没声音，alias为别名，化名的意思
  // mciSendString(L"play music ", 0, 0, 0);                  //播放音乐
   mciSendString(L"open ./res/bgmusic.mp3 alias music2", 0, 0, 0); //向驱动层发送字符串消息，open后面要空一格啊，不然没声音，alias为别名，化名的意思
   mciSendString(L"play music2 ", 0, 0, 0);                  //播放音乐
/*    // 主消息循环: 
      MSG msg = { };
   // while (GetMessage(&msg, nullptr, 0, 0))
		while (msg.message!=WM_QUIT)
    {
        // if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			if(PeekMessage(&msg,0,0,0,PM_NOREMOVE))
        {
			// 接受到消息不为WM_QUIT消息的情况
			// 将虚拟键消息转化为字符消息，字符消息被传递到调用线程的消息队列中，当下一次调用GetMessage函数被取出
            TranslateMessage(&msg);
			// 分发一个消息到窗口过程，由窗口过程函数对消息进行处理
            DispatchMessage(&msg);

			g_tNow = GetTickCount();//2018 1 9 10 26 20 10 
			if (g_tNow - g_tPre>60)
			{
				GamePaint(hwnd);
			}
			//Rectangle(g_hdc, 200, 200, 500, 500);
        }
		else {

			g_tNow = GetTickCount();//2018 1 9 10 26 20 10 
			if (g_tNow - g_tPre>60)
			{
				GamePaint(hwnd);
			}
			
			//Rectangle(g_hdc, 200, 200, 500, 500);
		}
    }*/

   //下面这种主消息循环不会卡死，效果比上面的好
   MSG msg = { 0 };
   while (TRUE)
   {
	   if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	   {
		   if (msg.message == WM_QUIT)
			   break;
		   TranslateMessage(&msg);
		   DispatchMessage(&msg);

		   g_tNow = GetTickCount();//2018 1 9 10 26 20 10 
		   if (g_tNow - g_tPre>80)
		   {
			   GamePaint(hwnd);
		   }
	
	   }
	   else
	   {
		   
		   g_tNow = GetTickCount();//2018 1 9 10 26 20 10 
		   if (g_tNow - g_tPre>80)
		   {
			   GamePaint(hwnd);
		   }
		   // 完成某些工作的其它行程序        
	   }
   }

		//return 0;
		return (int) msg.wParam;

}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM    MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);                                               //窗口在内存中需要的内存大小
    wcex.style          = CS_HREDRAW | CS_VREDRAW;                                  //窗口样式，通过水平和垂直的方向来改变窗口的大小
    wcex.lpfnWndProc    = WndProc;                                                  //窗口的过程函数
    wcex.cbClsExtra     = 0;                                                        //程序的附加内存
    wcex.cbWndExtra     = 0;                                                        //窗口的附加内存
    wcex.hInstance      = hInstance;                                                //当前实例句柄
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MENGHUANXIYOU18712)); //像标，图标
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);                           //鼠标的样式，可以自己定义
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);                                 //背景的画刷，背景的颜色
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MENGHUANXIYOU18712);                 //菜单
    wcex.lpszClassName  = szWindowClass;                                            //窗口的类名称
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	
    return RegisterClassExW(&wcex);//注册窗口
}


//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//        该函数暂时未使用
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, L"超级无敌顶顶顶顶顶", WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。针对用户的不同指令做出不同的反应，实现窗口过程的函数
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_LBUTTONDOWN://鼠标左键按下
		if (!g_bCanAttack)
		{
			int x = LOWORD(lParam);     //获取鼠标坐标x，y
			int y = HIWORD(lParam);

			if (x >= 1200 && x <= 1314 && y >= 100 && y <= 214)
			{
				g_bCanAttack = true;
				HeroAction = NORMAL;
			}
			else if (x >= 1200 && x <= 1314 && y >= 250 && y <= 364) 
			{
				g_bCanAttack = true;
				HeroAction = MAGIC;
			}
			else if (x >= 1200 && x <= 1314 && y >= 400 && y <= 514) 
			{
				g_bCanAttack = true;
				HeroAction = RECOVER;
			}
			else if (x >= 1200 && x <= 1314 && y >= 550 && y <= 664) 
			{
			
				g_bCanAttack = true;				
				HeroAction = UTIMATE;

			}
				break;
		}


		/*//贴第一个技能图标
		SelectObject(g_bufdc, g_skillbutton1);
		BitBlt(g_memdc, 1200, 100, 114, 114, g_bufdc, 0, 0, SRCCOPY);

		//贴第二个技能图标
		SelectObject(g_bufdc, g_skillbutton2);
		BitBlt(g_memdc, 1200, 250, 114, 114, g_bufdc, 0, 0, SRCCOPY);

		//贴第三个技能图标
		SelectObject(g_bufdc, g_skillbutton3);
		BitBlt(g_memdc, 1200, 400, 114, 114, g_bufdc, 0, 0, SRCCOPY);

		//贴第四个技能图标
		SelectObject(g_bufdc, g_skillbutton4);
		BitBlt(g_memdc, 1200, 550, 114, 114, g_bufdc, 0, 0, SRCCOPY);*/

		break;
    case WM_COMMAND:
         {   int wmId = LOWORD(wParam);
            // 分析菜单选择: 
               switch (wmId)
               {
                case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
				mciSendString(L"open ./res/fade.mp3 alias music", 0, 0, 0); //向驱动层发送字符串消息，open后面要空一格啊，不然没声音，alias为别名，化名的意思
				mciSendString(L"play music ", 0, 0, 0);                     //播放音乐
                break;
                case IDM_EXIT:
                DestroyWindow(hwnd);
                break;
                default:
                return DefWindowProc(hwnd, message, wParam, lParam);
                }
         }
                break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);	
			//为指定句柄hwnd窗口进行绘画准备工作
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
		    //	Rectangle(hdc, 10, 30, 400, 400);

            EndPaint(hwnd, &ps);
        }
        break;
    case WM_DESTROY:

        PostQuitMessage(0);
		//mciSendString(L"open ./res/fade.mp3 alias music", 0, 0, 0); //向驱动层发送字符串消息，open后面要空一格啊，不然没声音，alias为别名，化名的意思
		//mciSendString(L"play music ", 0, 0, 0);                   //播放音乐

        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:

		//打开子窗口时即播放
	//	mciSendString(L"open ./res/fade.mp3 alias music", 0, 0, 0); //向驱动层发送字符串消息，open后面要空一格啊，不然没声音，alias为别名，化名的意思
	//	mciSendString(L"play music ", 0, 0, 0);                   //播放音乐

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {

            EndDialog(hDlg, LOWORD(wParam));

			//关闭子窗口播放
			//mciSendString(L"open ./res/fade.mp3 alias music", 0, 0, 0); //向驱动层发送字符串消息，open后面要空一格啊，不然没声音，alias为别名，化名的意思
			//mciSendString(L"play music ", 0, 0, 0);                   //播放音乐
            return (INT_PTR)TRUE;

        }
        break;
    }
    return (INT_PTR)FALSE;
}


//游戏的初始化函数
BOOL GameInit(HWND hwnd) {

	srand((unsigned int)time(NULL));

	//绘图工具
	g_hdc = GetDC(hwnd);
	g_memdc = CreateCompatibleDC(g_hdc);
	g_bmp = CreateCompatibleBitmap(g_hdc, WND_WIDTH, WND_HEIGHT);
	g_bufdc= CreateCompatibleDC(g_hdc);
	SelectObject(g_memdc, g_bmp);     //第一步 g_bmp->g_memdc，这是空位图到设备

	//游戏图片素材,这个地方res前的那个.的前面不能有空格，即L" ./res/101.bmp"会导致不能加载项目的错误
	g_backbmp = (HBITMAP)LoadImage(NULL, L"./res/background2.bmp", IMAGE_BITMAP, 1600,900, LR_LOADFROMFILE);
	g_skillbutton1= (HBITMAP)LoadImage(NULL, L"./res/skillbutton1.bmp", IMAGE_BITMAP, 114, 114, LR_LOADFROMFILE);
	g_skillbutton2 = (HBITMAP)LoadImage(NULL, L"./res/skillbutton2.bmp", IMAGE_BITMAP, 114, 114, LR_LOADFROMFILE);
	g_skillbutton3 = (HBITMAP)LoadImage(NULL, L"./res/skillbutton3.bmp", IMAGE_BITMAP, 114, 114, LR_LOADFROMFILE);
	g_skillbutton4 = (HBITMAP)LoadImage(NULL, L"./res/skillbutton4.bmp", IMAGE_BITMAP, 114, 114, LR_LOADFROMFILE);
	g_hero= (HBITMAP)LoadImage(NULL, L"./res/hero2.bmp", IMAGE_BITMAP, 544, 108, LR_LOADFROMFILE);
	g_boss = (HBITMAP)LoadImage(NULL, L"./res/boss1.bmp", IMAGE_BITMAP, 1240, 120, LR_LOADFROMFILE);


	g_heroskill1= (HBITMAP)LoadImage(NULL, L"./res/heroslash.bmp", IMAGE_BITMAP, 160, 120, LR_LOADFROMFILE);     //普攻
	g_heroskill2 = (HBITMAP)LoadImage(NULL, L"./res/herocritical.bmp", IMAGE_BITMAP, 233, 120, LR_LOADFROMFILE); //暴击
	g_heroskill3 = (HBITMAP)LoadImage(NULL, L"./res/heromagic.bmp", IMAGE_BITMAP, 160, 120, LR_LOADFROMFILE);     //魔法攻击
	g_heroskill4= (HBITMAP)LoadImage(NULL, L"./res/101.bmp", IMAGE_BITMAP, 1024, 768, LR_LOADFROMFILE);

	g_bossskill1 = (HBITMAP)LoadImage(NULL, L"./res/monsterslash.bmp", IMAGE_BITMAP, 125, 143, LR_LOADFROMFILE); 
	g_bossskill2 = (HBITMAP)LoadImage(NULL, L"./res/monstercritical.bmp", IMAGE_BITMAP, 217, 120, LR_LOADFROMFILE);
	g_bossskill3 = (HBITMAP)LoadImage(NULL, L"./res/monstermagic.bmp", IMAGE_BITMAP, 144, 132, LR_LOADFROMFILE);

	g_recover = (HBITMAP)LoadImage(NULL, L"./res/recover.bmp", IMAGE_BITMAP, 182, 128, LR_LOADFROMFILE);
    g_snow= (HBITMAP)LoadImage(NULL, L"./res/snow.bmp", IMAGE_BITMAP, 182, 128, LR_LOADFROMFILE);
	g_gameover= (HBITMAP)LoadImage(NULL, L"./res/gameover2.bmp", IMAGE_BITMAP, 398, 180, LR_LOADFROMFILE);
	g_gamevictory= (HBITMAP)LoadImage(NULL, L"./res/victory1.bmp", IMAGE_BITMAP, 632, 338, LR_LOADFROMFILE);

		

	//初始化一些数据
	//hero
	Hero.Agility = 20;
	Hero.Intell = 10;
	Hero.strength = 10;
	Hero.level = 6;
	Hero.MaxHp = 1200;
	Hero.NowHp = 1200;
	Hero.MaxMp = 60;
	Hero.NowMp = 60;
	//Boss
	Boss.Agility = 10;
	Boss.Intell = 10;
	Boss.strength = 10;
	Boss.level = 10;
	Boss.MaxHp = 1500;
	Boss.NowHp = 1500;
	//Boss.MaxMp = 60;
	//Boss.NowMp = 60;

	GetClientRect(hwnd, &g_rect);


	HFONT hfont;
	hfont = CreateFont(20,0,0,0,900,0,0,0,0,0,0,0,0,L"楷体");
	SelectObject(g_memdc, hfont);
	SetBkMode(g_memdc,0);

	/*
       GDI绘图步骤：
           g_bmp->g_memdc
           g_backbmp->g_bufdc
		   g_bufdc->g_memdc
           g_memdc->g_hdc
*/
	

	return TRUE;

}
//绘制函数，控制整个绘制过程
VOID GamePaint(HWND hwnd) {

	wchar_t str[100];//字符串用来保存血量

	//贴背景图片
	// 第二步 g_backbmp->g_bufdc,
	SelectObject(g_bufdc, g_backbmp);   
	//第三步，g_bufdc->g_memdc
	BitBlt(g_memdc,0,0,WND_WIDTH,WND_HEIGHT,g_bufdc,0,0,SRCCOPY);        

	

	//显示战斗信息//g_iTextNum
	

	for (int i = 0; i <6; i++) 
	{
		if (g_bAboutHero) {
			SetTextColor(g_memdc, RGB(255, 255, 0));
		}
		else
		{
			SetTextColor(g_memdc, RGB(180, 0, 120));
		}
		TextOut(g_memdc, 250, 500 + i * 20, text[i], wcslen(text[i]));
	}


	//贴boss相关信息
	if (Boss.NowHp > 0) {

		SelectObject(g_bufdc, g_boss);
		//BitBlt(g_memdc, 400, 550, 60, 140, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 180, 280, 200, 160, g_bufdc, boss_num * 154, 0, 154, 120, RGB(255, 255, 255));//透明贴图boss
		boss_num++;
		if (boss_num>8)
			boss_num = 0;

		//显示血量
		swprintf_s(str,L"%d/%d",Boss.NowHp,Boss.MaxHp);    //1000/1000
		SetTextColor(g_memdc, RGB(255, 0, 0));
		TextOut(g_memdc, 255, 440, str, wcslen(str));

		TCHAR g_bossname4[] = _T("土豺狼");
		SetTextColor(g_memdc, RGB(255, 0, 0));
		TextOut(g_memdc, 185, 440, g_bossname4, _tcslen(g_bossname4));
	}
	
	 //绘制英雄相关信息

	if (Hero.NowHp > 0) {

		SelectObject(g_bufdc, g_hero);
		//BitBlt(g_memdc, 400, 550, 60, 140, g_bufdc, 0, 0, SRCCOPY);
		TransparentBlt(g_memdc, 800, 500, 68, 108, g_bufdc, hero_num * 68, 0, 68, 108, RGB(255, 255, 255));//透明贴图hero2
		hero_num++;
		if (hero_num>8)
			hero_num = 0;

		//绘制英雄的血量和蓝量
		swprintf_s(str, L"%d/%d", Hero.NowHp, Hero.MaxHp);    //1000/1000
		SetTextColor(g_memdc, RGB(255, 0, 0));
		TextOut(g_memdc, 800, 610, str, wcslen(str));

		swprintf_s(str, L"%d/%d", Hero.NowMp, Hero.MaxMp);    //1000/1000
		SetTextColor(g_memdc, RGB(0, 0, 255));
		TextOut(g_memdc, 820, 630, str, wcslen(str));

	}
	if (g_bGameOver)
	{
		if (Hero.NowHp <= 0) {
			//游戏结束
			SelectObject(g_bufdc, g_gameover);
			//BitBlt(g_memdc, 420,250, 398, 180, g_bufdc, 0, 0, SRCCOPY);
			TransparentBlt(g_memdc, 420, 250, 398, 180, g_bufdc, 0, 0, 398, 180, RGB(255, 255, 255));
		}
		else {
			//游戏胜利

			SelectObject(g_bufdc, g_gamevictory);
			//BitBlt(g_memdc, 420, 250, 632, 338, g_bufdc, 0, 0, SRCCOPY);
			TransparentBlt(g_memdc, 420, 250, 632, 338, g_bufdc, 0, 0, 632, 338, RGB(255, 255, 255));
		}


	}
	else   if (!g_bCanAttack) {

		//贴第一个技能图标

		SelectObject(g_bufdc, g_skillbutton1);
		BitBlt(g_memdc, 1200, 100, 114, 114, g_bufdc, 0, 0, SRCCOPY);

		TCHAR g_skillname1[] = _T("电磁打击（普攻，消耗0）");
		SetTextColor(g_memdc, RGB(255, 255, 255));
		TextOut(g_memdc, 1100, 220, g_skillname1, _tcslen(g_skillname1));

		//贴第二个技能图标

		SelectObject(g_bufdc, g_skillbutton2);
		BitBlt(g_memdc, 1200, 250, 114, 114, g_bufdc, 0, 0, SRCCOPY);

		TCHAR g_skillname2[] = _T("志远之力（魔法，消耗26）");
		SetTextColor(g_memdc, RGB(255, 255, 255));

		TextOut(g_memdc, 1100, 370, g_skillname2, _tcslen(g_skillname2));

		//贴第三个技能图标

		SelectObject(g_bufdc, g_skillbutton3);
		BitBlt(g_memdc, 1200, 400, 114, 114, g_bufdc, 0, 0, SRCCOPY);

		TCHAR g_skillname3[] = _T("微波加热（恢复，消耗36）");
		SetTextColor(g_memdc, RGB(255, 255, 255));
		TextOut(g_memdc, 1100, 520, g_skillname3, _tcslen(g_skillname3));

		//贴第四个技能图标

		SelectObject(g_bufdc, g_skillbutton4);
		BitBlt(g_memdc, 1200, 550, 114, 114, g_bufdc, 0, 0, SRCCOPY);

		TCHAR g_skillname4[] = _T("致命辐射（大招，消耗32）");
		SetTextColor(g_memdc, RGB(255, 255, 255));
		TextOut(g_memdc, 1100, 670, g_skillname4, _tcslen(g_skillname4));
	}
	else {
		 g_frame++;         //消息循环      ，每一次循环就是一帧
		if (g_frame>=5&&g_frame<=10)          //在第五帧到第十帧显示玩家技能效果
		{

			if (g_frame == 5) {
				//贴技能图，不知道，点击的技能
				//hero的逻辑
				HeroAction_Logic();
				DieCheck(Boss.NowHp, false);
			}
			
			//根据不同的动作贴不同的技能图片
			HeroActionPaint();
		}
		if (g_frame == 15) 
		{
			//怪物的逻辑
			BossAction_Logic();

		}		
		//下面显示怪物的攻击图
		  
		if (g_frame >= 16 && g_frame <= 30) 
		{
			//显示怪物攻击效果，并计算伤害
			BossActionPaint();
			DieCheck(Hero.NowHp, true);
		}
		//第三十帧回合结束
		if (g_frame == 30) 
		{
			g_bCanAttack = false;
			g_frame = 0;               //30*80=2400ms,即为2.4秒，一个回合的时间

			int MpRecover;              //定义每个回合需要恢复的蓝量
			MpRecover = 2*rand() % Hero.Intell+6;       //0-9    最低恢复6点，最高恢复24点
			Hero.NowMp += MpRecover;
			if (Hero.NowMp >= Hero.MaxMp)
				Hero.NowMp = Hero.MaxMp;

		}

	}

	//第四步，g_memdc->g_hdc
	BitBlt(g_hdc,              //g_hdc的窗口句柄,目标设备
		0, 0, WND_WIDTH, WND_HEIGHT,    //目标设备范围:起始点x，y，以及长和宽 
		g_memdc,          //源设备
		0, 0,             //源设备中的起始值
		SRCCOPY);          //拷贝

	g_tPre = GetTickCount();        //2018 1 9 22 25 35 45


}


//玩家的逻辑代码
VOID  HeroAction_Logic() 
{
	//暴击伤害

	int damage = 0;
	switch (HeroAction)
	{
	    case NORMAL:
		//包括暴击和普攻
		//暴击
		if (rand()%4==1)    //暴击率25%
		{
			HeroAction = CRITICAL;
			damage =(int)3.5* (4 * (rand() % Hero.Agility) + Hero.level*Hero.strength + 40);
			Boss.NowHp -= (int)damage;

			swprintf_s(str, L"神天兵触发暴击，造成了%d点伤害,伤害爆炸", damage);
			Message_Insert(str);
			g_bAboutHero = true;
			
		}
		else {
		
			damage =2*(rand() % Hero.Agility) + Hero.level*Hero.strength+20;//最低伤害80点，最高伤害100+
			Boss.NowHp -= (int)damage;
			
			swprintf_s(str,L"神天兵使用了普通攻击，造成了%d点伤害",damage);
			Message_Insert(str);
			g_bAboutHero = true;

		}
		break;
	  case MAGIC:
		  //消耗蓝量26
		  if (Hero.NowMp >= 26)
		  {
			  damage =4 * (2 * (rand() % Hero.Agility)+Hero.level*Hero.Intell);
			  Boss.NowHp -= (int)damage;
			  Hero.NowMp -= 26;
			 
			  swprintf_s(str, L"神天兵使用魔法攻击，造成了%d点伤害,伤害较高", damage);
			  Message_Insert(str);
			  g_bAboutHero = true;
		  }
		  else
		  {
			  HeroAction = MISS;
		 
			  swprintf_s(str, L"英雄法力值不足，攻击机会浪费");
			  Message_Insert(str);
			  g_bAboutHero = true;
		  }

			break;
	  case RECOVER://消耗36点蓝

		  if (Hero.NowMp >= 36)
		  {
			  Hero.NowMp -= 36;
			  int HpRecover = 2*(4 * (rand() % Hero.Intell + 30))+ (int)(Boss.MaxHp - Boss.NowHp)*(0.05);   //范围200~540+
			  Hero.NowHp += HpRecover;
			  swprintf_s(str, L"神天兵使用微波加热，恢复%d点生命值", HpRecover);
			  Message_Insert(str);
			  g_bAboutHero = true;
			  if (Hero.NowHp > Hero.MaxHp)
			  {
				  Hero.NowHp = Hero.MaxHp;
			  }
		  }
		  else
		  {
			  HeroAction = MISS;

			  swprintf_s(str, L"神天兵法力值不足36，此回合攻击机会浪费");
			  Message_Insert(str);
			  g_bAboutHero = true;
		  }
		  break;       //之前忘了这个break，导致bug，气人啊

	  case UTIMATE:
		  if (Hero.NowMp >= 32)
		  {

			  Hero.NowMp -= 32;
			  damage =(int)(Boss.MaxHp-Boss.NowHp)*0.3+  (5 * (rand() % Hero.Intell + 30));
			  Boss.NowHp -= (int)damage;
			  swprintf_s(str, L"神天兵使用斩杀终极技能，造成%d点伤害",damage);
			  Message_Insert(str);
			  g_bAboutHero = true;
			 
		  }
		  else
		  {
			  HeroAction = MISS;
			  swprintf_s(str, L"神天兵法力值不足35，本回合攻击机会浪费");
			  Message_Insert(str);
			  g_bAboutHero = true;
		  }
		  break;
	}

}
//绘制玩家的技能效果
VOID HeroActionPaint() 
{  
	switch (HeroAction)
	{
	case NORMAL:
		SelectObject(g_bufdc, g_heroskill1);
		TransparentBlt(g_memdc, 250, 280, 110, 120, g_bufdc, 0, 0, 160, 120, RGB(0,0,0));

		break;
	case CRITICAL:

		SelectObject(g_bufdc, g_heroskill2);
		TransparentBlt(g_memdc, 250, 280, 233, 120, g_bufdc, 0, 0, 233, 120, RGB(0, 0, 0));

		break;
	case MAGIC:

		SelectObject(g_bufdc, g_heroskill3);
		TransparentBlt(g_memdc, 250, 280, 160, 120, g_bufdc, 0, 0,160, 120, RGB(255, 255,255));

		break;
	case RECOVER:

		SelectObject(g_bufdc, g_recover);
		TransparentBlt(g_memdc, 750,480, 182, 128, g_bufdc, 0, 0, 182, 128, RGB(255, 255, 255));
		break;
	case UTIMATE:

		SelectObject(g_bufdc, g_heroskill4);
		TransparentBlt(g_memdc, 200, 300, 624, 368, g_bufdc, 0, 0, 1024, 768, RGB(255, 255, 255));
		break;
    case  MISS:

		SelectObject(g_bufdc, g_snow);
		TransparentBlt(g_memdc, 950, 500, 182, 128, g_bufdc, 0, 0, 182, 128, RGB(255, 255, 255));
		break;
	//default:
	//	break;
	}

}

//Boss的逻辑代码
VOID  BossAction_Logic()
{
	srand((unsigned int)time(NULL));
	//依据boss的血量多少来决定放什么技能
	if (Boss.NowHp > (Boss.MaxHp/2))//血量还比较充足，至少不会放恢复技能
	{
		switch (rand()%4)
		{
		case 0:
			BossAction = NORMAL;
			
			break;
		case 1:
			BossAction = CRITICAL;

			break;
		case 2:
			BossAction = MAGIC;

			break;
		case 3:
			BossAction = NORMAL;

			break;
		}
	
	}
	else {        //气血比较低，进入狂暴状态,爆发
		
		if (!g_bIsAngry) {//让程序只输出一次进入狂暴模式
			swprintf_s(str, L"土豺狼进入狂暴状态,以暴击，魔法，恢复技能反击");
			Message_Insert(str);
			g_bAboutHero = false;
			g_bIsAngry=true;
		}
		
		switch (rand() % 3)
		{
		case 0:
			BossAction = CRITICAL;
			

			break;
		case 1:
			BossAction = MAGIC;

			break;
		case 2:
			BossAction = RECOVER;

			break;
		}

	}
}

//绘制Boss的技能效果
VOID BossActionPaint()
{     
	int damage = 0;
	int recover = 0;

	switch (BossAction)
	{
	case NORMAL:
		SelectObject(g_bufdc, g_bossskill1);
		TransparentBlt(g_memdc, 800, 540, 125, 143, g_bufdc, 0, 0, 125, 143, RGB(255, 255, 255));
		if (g_frame == 30) {
			damage = rand() % Boss.Agility + Boss.level*Boss.strength;
			Hero.NowHp -= damage;

			swprintf_s(str, L"土豺狼使用普通攻击，造成%d点伤害", damage);
			Message_Insert(str);
			g_bAboutHero = false;
		}
		break;
	case CRITICAL:

		SelectObject(g_bufdc, g_bossskill2);
		TransparentBlt(g_memdc, 800, 540, 217, 120, g_bufdc, 0, 0, 217, 120, RGB(0, 0, 0));
		if (g_frame == 30) {
			damage = 3 * (rand() % Boss.Agility) + 2*Boss.level*Boss.strength;
			Hero.NowHp -= damage;
			
			swprintf_s(str, L"土豺狼触发暴击，造成%d点伤害", damage);
			Message_Insert(str);
			
			g_bAboutHero = false;
		}
		break;
	case MAGIC:

		SelectObject(g_bufdc, g_bossskill3);
		TransparentBlt(g_memdc, 800, 540, 144, 132, g_bufdc, 0, 0, 144, 132, RGB(255,255, 255));

		if (g_frame == 30) {
			damage = 2 * (2 * (rand() % Boss.Agility) + Boss.Intell*Boss.strength);
			Hero.NowHp -= damage;
			
			//这个boss自带法术吸血效果
			recover = (int)(damage * 0.2);
		    Boss.NowHp += recover;
			swprintf_s(str, L"土豺狼魔法攻击，造成%d点伤害,并恢复%d点生命值", damage,recover);
			Message_Insert(str);
			g_bAboutHero = false;
			if (Boss.NowHp >= Boss.MaxHp)
				Boss.NowHp = Boss.MaxHp;
		}
		break;
	case RECOVER:

		SelectObject(g_bufdc, g_recover);
		TransparentBlt(g_memdc, 250, 300, 182, 128, g_bufdc, 0, 0, 182, 128, RGB(255, 255, 255));

		if (g_frame == 30) {
			recover =(int) Boss.Intell*Boss.Intell*0.8+(int)(Boss.MaxHp-Boss.NowHp)*(0.10)+10;
			Boss.NowHp += recover;

			swprintf_s(str, L"土豺狼使用恢复技能，恢复%d点生命值", recover);
			Message_Insert(str);
			g_bAboutHero = false;
			if (Boss.NowHp >= Boss.MaxHp)				
			{
				Boss.NowHp = Boss.MaxHp;
				swprintf_s(str, L"土豺狼生命值已恢复至最大值");
			    Message_Insert(str);
			    g_bAboutHero = false;
			
			}
			
		}

		break;
	default:
		break;
	}
}

//判断死亡
VOID DieCheck(int nowhp,bool ishero) {

	

	if (nowhp < 0)
	{    
		g_bGameOver = true;
		if(ishero)
		{ 
			//PlaySound(NULL, NULL, SND_FILENAME);好像停止不了啊

			PlaySound(L"./res/failure.wav", NULL, SND_FILENAME | SND_ASYNC);

			wchar_t str[100];
			
			swprintf_s(str, L"胜败乃兵家常事，少侠请不要气馁... ");
			Message_Insert(str);
			
		}
		else {

			PlaySound(L"./res/victory.wav", NULL, SND_FILENAME | SND_ASYNC);
			wchar_t str[100];
			swprintf_s(str, L"成功击败土豺狼，少侠果真武功盖世... ");
			Message_Insert(str);
		   
		}

	}
	

}


VOID Message_Insert(wchar_t* str) 
{    

	if (g_iTextNum <6) 
	{
		swprintf_s(text[g_iTextNum],str);

		g_iTextNum++;
	}
	else {
		for (int i = 0; i < g_iTextNum; i++)
		{
			swprintf_s(text[i], text[i + 1]);   //输出消息进行移位
		}
		swprintf_s(text[5], str);
	}
	 
}