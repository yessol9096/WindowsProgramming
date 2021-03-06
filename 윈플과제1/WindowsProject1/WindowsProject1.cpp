// WindowsProject1.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "WindowsProject1.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define BSIZE 20
double LengthPts(int x1, int y1, int x2, int y2)
{
	return(sqrt((float)((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1))));
}
BOOL InCircle(int x, int y, int mx, int my)
{
	if (LengthPts(x, y, mx, my) < BSIZE)
		return TRUE;
	else
		return FALSE;
}
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, 574, 750, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	HPEN hPen, oldPen;
	static float mx, my;
	static float endx, endy;
	static float x1, y1;
	HBRUSH hBrush, oldBrush;
	static BOOL Selection;
	static int score;
	static struct Ball
	{
		int Cx, Cy;//좌표
		int speedx, speedy; //속도
		double ball_x, ball_y; // 볼의 방향값
	};
	static bool ballmove; // 마우스 누르면 스타트
	static struct obstacle
	{
		int x1, x2, y1, y2;
		int crush;
	};
	static struct obstacle brick[12][7];
	static struct Ball circle[5];
	static int count;
	static float Degree;
	static float targetX, targety; //공이 올라가는 높이
	srand((unsigned)time(NULL));
	switch (iMsg)
	{
	case WM_CREATE:
	{
		ballmove = FALSE;
		Selection = FALSE;
		mx = my = score = 0;
		x1 = circle[0].Cx = 300, y1 = circle[0].Cy = 600;					//초기공 x,y
		circle[0].ball_x = -1, circle[0].ball_y = -1;			//속도
		circle[0].speedx = 3, circle[0].speedy = 5;     // 볼의 방향값
		count = 1;
		for (int j = 0; j < 12; ++j) // 벽돌초기화
		{
			for (int i = 0; i < (rand() % 5) + 3; i++)
			{
				brick[j][i].y1 = 20;
				brick[j][i].y2 = 70;
				brick[j][i].x1 = (80 * (rand() % 7));
				brick[j][i].x2 = brick[j][i].x1 + 77;
				brick[j][i].crush = j + 1;
			}
		}
		score = 0;
		for (int j = 0; j < count; ++j)
		{
			for (int i = 0; i < 7; i++)
			{
				if (brick[j][i].crush == 0)
					score++;
			}
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		TCHAR input[5];
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
		hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
		oldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, 0, 640, NULL);
		LineTo(hdc, 700, 640);
		SelectObject(hdc, oldPen);
		DeleteObject(hPen);
		Ellipse(hdc, circle[0].Cx, circle[0].Cy, circle[0].Cx + 40, circle[0].Cy + 40); //공
		for (int j = 0; j < count; ++j) // 벽돌그리기
		{
			for (int i = 0; i < 7; i++)
			{
				rect.left = brick[j][i].x1, rect.top = brick[j][i].y1 + 53 * (count - j), rect.right = brick[j][i].x2, rect.bottom = brick[j][i].y2 + 53 * (count - j);
				if (brick[j][i].crush > 0)
				{
					hBrush = CreateSolidBrush(RGB(255, 230, 0));
					oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
					Rectangle(hdc, brick[j][i].x1, brick[j][i].y1 + 53 * (count - j), brick[j][i].x2, brick[j][i].y2 + 53 * (count - j));
					SelectObject(hdc, oldBrush);
					DeleteObject(hBrush);
					wsprintf(input, TEXT("%d"), brick[j][i].crush);
					DrawText(hdc, input, lstrlen(input), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
		}
		for (int j = 0; j < count; ++j)
		{
			for (int i = 0; i < 7; i++)
			{
				if (brick[j][i].crush == 0)
				{
					brick[j][i] = {};
				}
			}
		}
		for (int j = 0; j < count; ++j)
		{
			for (int i = 0; i < 7; i++)
			{
				if (brick[j][i].y2 + 53 * (count - j) > 640)
				{
					Rectangle(hdc, 0, 0, 574, 700);
					rect.left = 100, rect.top = 200, rect.right = 300, rect.bottom = 600;
					TextOut(hdc, 150, 370, "lose 게임이 종료되었습니다.", strlen("게임이 종료되었습니다."));
					TextOut(hdc, 150, 390, "점수", strlen("점수"));
					wsprintf(input, TEXT("%d"), score);
					DrawText(hdc, input, lstrlen(input), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					TextOut(hdc, 150, 450, "다시시작:1 끝:2.", strlen("다시시작:1 끝:2."));
					break;
				}
			}
			EndPaint(hWnd, &ps);
		}
	}
	break;
	case WM_CHAR:
		if (wParam == '1')
		{
			ballmove = FALSE;
			Selection = FALSE;
			mx = my = score = 0;
			x1 = circle[0].Cx = 300, y1 = circle[0].Cy = 600;					//초기공 x,y
			circle[0].ball_x = -1, circle[0].ball_y = -1;			//속도
			circle[0].speedx = 3, circle[0].speedy = 5;
			count = 1;
			for (int j = 0; j < 12; ++j) // 벽돌초기화
			{
				for (int i = 0; i < (rand() % 5) + 3; i++)
				{
					brick[j][i].y1 = 20;
					brick[j][i].y2 = 70;
					brick[j][i].x1 = (80 * (rand() % 7));
					brick[j][i].x2 = brick[j][i].x1 + 77;
					brick[j][i].crush = j + 1;
				}
			}
		}
		else if (wParam == '2')
			PostQuitMessage(0);
		InvalidateRgn(hWnd, NULL, TRUE);
		break;
	case WM_TIMER:
	{
		if (ballmove == TRUE)
		{
			switch (wParam)
			{
			case 1:
				KillTimer(hWnd, 2);
				KillTimer(hWnd, 3);
				KillTimer(hWnd, 4);
				if (circle[0].Cx < targetX + 10)
					circle[0].Cx += 3;
				else
					circle[0].Cx -= 3;
				if (circle[0].Cy < targety + 10)
					circle[0].Cy += 3;
				else
					circle[0].Cy -= 3;
				if (circle[0].Cx - 20 < 0)
					SetTimer(hWnd, 2, 30, NULL);
				if (circle[0].Cx + 20 > 540)
					SetTimer(hWnd, 3, 30, NULL);
				if (circle[0].Cy - 20 < 0)
					SetTimer(hWnd, 4, 30, NULL);

				for (int j = 0; j < count + 1; ++j)
				{
					for (int i = 0; i < 7; i++)
					{
						if (circle[0].Cy - (brick[j][i].y2 + 53 * (count - j)) < 20 && brick[j][i].x1 < circle[0].Cx &&circle[0].Cx < brick[j][i].x2)
						{
							--brick[j][i].crush;
							SetTimer(hWnd, 4, 30, NULL);
						}

					}
				}
				if (circle[0].Cy >600)
				{
					ballmove = FALSE;
					circle[0].ball_x = -1;
					circle[0].ball_y = -1;
					circle[0].Cy = 600;
					++count; KillTimer(hWnd, 1);

				}
				break;
			case 2:
				KillTimer(hWnd, 1);
				KillTimer(hWnd, 3);
				KillTimer(hWnd, 4);
				//왼쪽 벽
				{
					if (circle[0].ball_x == 2)
					{
						circle[0].Cx -= circle[0].speedx * circle[0].ball_x;
						circle[0].Cy += circle[0].speedy * circle[0].ball_y;
						circle[0].ball_y = -0.7;
					}
					{
						circle[0].Cx -= circle[0].speedx * circle[0].ball_x;
						circle[0].Cy += circle[0].speedy * circle[0].ball_y;
						circle[0].ball_x = -1;
					}
					if (circle[0].Cx + 20 > 540)
						SetTimer(hWnd, 3, 30, NULL);
					if (circle[0].Cy - 20 < 0)
						SetTimer(hWnd, 4, 30, NULL);
					for (int j = 0; j < count + 1; ++j)
					{
						for (int i = 0; i < 7; i++)
						{
							if (circle[0].Cy - (brick[j][i].y2 + 53 * (count - j)) < 20 && brick[j][i].x1 < circle[0].Cx &&circle[0].Cx < brick[j][i].x2)
							{
								--brick[j][i].crush;
								SetTimer(hWnd, 4, 30, NULL);
							}
						}
					}
					if (circle[0].Cy > 600)
					{
						ballmove = FALSE;
						circle[0].Cy = 600;
						circle[0].ball_x = -1;
						circle[0].ball_y = -1;
						++count; KillTimer(hWnd, 2);
					}
				}
				break;
			case 3:
				KillTimer(hWnd, 1);
				KillTimer(hWnd, 2);
				KillTimer(hWnd, 4);
				//오른쪽 벽
				{
					if (circle[0].ball_x == 2)
					{
						circle[0].Cx -= circle[0].speedx * circle[0].ball_x;
						circle[0].Cy += circle[0].speedy * circle[0].ball_y;
						circle[0].ball_x = +0.7;
					}
					{
						circle[0].Cx -= circle[0].speedx * circle[0].ball_x;
						circle[0].Cy += circle[0].speedy * circle[0].ball_y;
						circle[0].ball_x = +1;
					}
					for (int j = 0; j < count + 1; ++j)
					{
						for (int i = 0; i < 7; i++)
						{
							if (circle[0].Cy - (brick[j][i].y2 + 53 * (count - j)) < 20 && brick[j][i].x1 < circle[0].Cx &&circle[0].Cx < brick[j][i].x2)
							{
								--brick[j][i].crush;
								SetTimer(hWnd, 4, 30, NULL);
							}

						}
					}
					if (circle[0].Cx - 20 < 0)
						SetTimer(hWnd, 2, 30, NULL);
					if (circle[0].Cy - 20 < 0)
						SetTimer(hWnd, 4, 30, NULL);
					if (circle[0].Cy > 600)
					{
						ballmove = FALSE;
						circle[0].Cy = 600;
						circle[0].ball_x = -1;
						circle[0].ball_y = -1;
						++count; KillTimer(hWnd, 3);

					}
				}
				break;
			case 4:
				KillTimer(hWnd, 1);
				KillTimer(hWnd, 2);
				KillTimer(hWnd, 3);
				//윗면
				{
					if (circle[0].ball_y == 2)
					{
						circle[0].Cx -= circle[0].speedx * circle[0].ball_x;
						circle[0].Cy += circle[0].speedy * circle[0].ball_y;
						circle[0].ball_x = -0.7;
					}
					{
						circle[0].Cx -= circle[0].speedx * circle[0].ball_x;
						circle[0].Cy += circle[0].speedy * circle[0].ball_y;
						circle[0].ball_y = +1;
					}
					if (circle[0].Cx - 20 < 0)
						SetTimer(hWnd, 2, 30, NULL);
					if (circle[0].Cx + 20 > 540)
						SetTimer(hWnd, 3, 30, NULL);
					if (circle[0].Cy > 600)
					{
						ballmove = FALSE;
						circle[0].Cy = 600;
						circle[0].ball_x = -1;
						circle[0].ball_y = -1;
						++count; KillTimer(hWnd, 4);

					}

				}

				break;
			}
			InvalidateRgn(hWnd, NULL, TRUE);
		}
	}
	break;
	case WM_LBUTTONDOWN:
		/*mx = LOWORD(lParam);
		my = HIWORD(lParam);*/
		Selection = TRUE;
		InvalidateRgn(hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONUP:
		ballmove = TRUE;
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (mx > circle[0].Cx)
		{
			Degree = atan2f((float)my - circle[0].Cy, (float)mx - circle[0].Cx) * 180 / 3.1415f;
			targety = (574 - circle[0].Cx) / cos(Degree) * sin(Degree);
			targetX = 574;
		}
		else
		{
			Degree = atan2f((float)my - circle[0].Cy, circle[0].Cx - (float)mx) * 180 / 3.1415f;
			targety = circle[0].Cx / cos(Degree) * sin(Degree);
			targetX = 0;
		}
		SetTimer(hWnd, 1, 30, NULL);
		Selection = FALSE;
		InvalidateRgn(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return 0;
}