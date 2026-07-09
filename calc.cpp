// calc++.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "calc.h"
#include "math/NMath.h"

#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <random>

#include <commdlg.h>
#include <fstream>
#include <string>
#include <cwctype>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <thread>
#include <atomic>

#pragma comment(lib, "Comdlg32.lib") // need this

#define MAX_LOADSTRING 100

/* Calculator Button IDs. */
#define IDC_DISPLAY 1000 
#define IDC_BTN_0 2000
#define IDC_BTN_1 2001
#define IDC_BTN_2 2002
#define IDC_BTN_3 2003
#define IDC_BTN_4 2004
#define IDC_BTN_5 2005
#define IDC_BTN_6 2006
#define IDC_BTN_7 2007
#define IDC_BTN_8 2008
#define IDC_BTN_9 2009
#define IDC_BTN_ADD 2010
#define IDC_BTN_SUB 2011
#define IDC_BTN_MUL 2012
#define IDC_BTN_DIV 2013
#define IDC_BTN_EQ  2014
#define IDC_BTN_CLR 2015
#define IDC_BTN_DOT 2016
#define IDC_BTN_SQUARE   2100
#define IDC_BTN_CUBE     2101
#define IDC_BTN_SQRT     2102
#define IDC_BTN_POWER    2103
#define IDC_BTN_ABS      2104
#define IDC_BTN_FLOOR    2105
#define IDC_BTN_CEIL     2106
#define IDC_BTN_ROUND    2107
#define IDC_BTN_SIN      2108
#define IDC_BTN_COS      2109
#define IDC_BTN_TAN      2110
#define IDC_BTN_LOG      2111
#define IDC_BTN_LOG10    2112
#define IDC_BTN_EXP      2113
#define IDC_BTN_PI       2114
#define IDC_BTN_E        2115
#define IDC_BTN_RAND     2116
#define IDC_BTN_RANDINT  2117
#define IDC_BTN_RANDBOOL 2118
#define IDC_BTN_LPAREN 2120
#define IDC_BTN_RPAREN 2121

HWND g_hDisplay = nullptr; // Display handle
HWND g_hHistory = nullptr; // History handle

double g_LeftValue = 0.0;
wchar_t g_Operator = 0;
bool g_NewInput = true;
int g_PrecisionMode = 5; // 1=2dp, 2=3dp, 3=4dp, 4=5dp, 5=auto
std::atomic_bool g_Running = true;

void SecretBeep()
{
    DWORD frequency = g_pRandom->RandomULong(200, 2000);
    DWORD duration = static_cast<DWORD>(g_pRandom->RandomULong(50, 1000) * g_pMath->GetPi());

    DWORD randomvalue = static_cast<DWORD>(g_pRandom->RandomDouble(0.1, 0.5) * g_pMath->Add(2.405, g_pMath->GetPi());

    Beep(frequency, duration);
}

void AddHistoryLine(const wchar_t* line)
{
    int len = GetWindowTextLengthW(g_hHistory);

    SendMessageW(g_hHistory, EM_SETSEL, len, len);
    SendMessageW(g_hHistory, EM_REPLACESEL, FALSE, (LPARAM)line);
    SendMessageW(g_hHistory, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

    OutputDebugStringW(line);
}

void SaveHistoryToFile(HWND hWnd)
{
    wchar_t fileName[MAX_PATH] = L"history.txt";

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (!GetSaveFileNameW(&ofn))
        return;

    int len = GetWindowTextLengthW(g_hHistory);

    std::wstring history(len, L'\0');
    GetWindowTextW(g_hHistory, history.data(), len + 1);

    std::wofstream file(fileName);

    if (!file)
    {
        MessageBoxW(hWnd, L"Failed to save history.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    file << history;

    MessageBoxW(hWnd, L"History saved successfully.", L"Saved", MB_OK | MB_ICONINFORMATION);
}

void ConsoleCommandLoop(HWND hWnd)
{
    std::string command;

    while (g_Running && std::getline(std::cin, command))
    {
        if (command == "clear")
        {
            system("cls");
        }
        else if (command == "close")
        {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        else
        {
            std::cerr << "Unknown command: " << command << std::endl;
            std::cerr << "Commands: clear, close" << std::endl;
        }
    }
}

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Options(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    AllocConsole();

    SetConsoleTitleW(L"Calc++ Issue Log");

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CALC, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CALC));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CALC);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(
       szWindowClass,
       szTitle,
       WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
       CW_USEDEFAULT,
       CW_USEDEFAULT,
       620,
       540,
       nullptr,
       nullptr,
       hInstance,
       nullptr
   );

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   std::thread(ConsoleCommandLoop, hWnd).detach();

   return TRUE;
}

double GetDisplayValue()
{
    wchar_t text[256];
    GetWindowTextW(g_hDisplay, text, 256);
    return _wtof(text);
}

void SetDisplayValue(double value)
{
    std::wostringstream ss;

    if (g_PrecisionMode == 5)
    {
        ss << value; // automatic
    }
    else
    {
        ss << std::fixed
            << std::setprecision(g_PrecisionMode + 1)
            << value;
    }

    SetWindowTextW(g_hDisplay, ss.str().c_str());
}

void AppendDisplay(wchar_t ch)
{
    wchar_t text[256];
    GetWindowTextW(g_hDisplay, text, 256);

    if (g_NewInput || wcscmp(text, L"0") == 0)
    {
        text[0] = ch;
        text[1] = L'\0';
        g_NewInput = false;
    }
    else
    {
        size_t len = wcslen(text);
        if (len < 255)
        {
            text[len] = ch;
            text[len + 1] = L'\0';
        }
    }

    SetWindowTextW(g_hDisplay, text);
}

void AppendDot()
{
    wchar_t text[256];
    GetWindowTextW(g_hDisplay, text, 256);

    if (g_NewInput)
    {
        SetWindowTextW(g_hDisplay, L"0.");
        g_NewInput = false;
        return;
    }

    if (!wcschr(text, L'.'))
    {
        wcscat_s(text, L".");
        SetWindowTextW(g_hDisplay, text);
    }
}

void ClearCalculator()
{
    g_LeftValue = 0.0;
    g_Operator = 0;
    g_NewInput = true;
    SetWindowTextW(g_hDisplay, L"0");
}

void ChooseOperator(wchar_t op)
{
    g_LeftValue = GetDisplayValue();
    g_Operator = op;
    g_NewInput = true;
}

void CalculateResult()
{
    double rightValue = GetDisplayValue();
    double result = rightValue;

    switch (g_Operator)
    {
    case L'+':
        result = NMath::g_pMath->Add(g_LeftValue, rightValue);
        break;
    case L'-':
        result = NMath::g_pMath->Subtract(g_LeftValue, rightValue);
        break;
    case L'*':
        result = NMath::g_pMath->Multiply(g_LeftValue, rightValue);
        break;
    case L'/':
        if (rightValue == 0.0)
        {
            SetWindowTextW(g_hDisplay, L"Cannot Divide By Zero.");
            g_NewInput = true;
            g_Operator = 0;
            return;
        }
        result = NMath::g_pMath->Divide(g_LeftValue, rightValue);
        break;
    case L'^':
        result = NMath::g_pMath->Power(g_LeftValue, rightValue);
        break;
    default:
        return;
    }

    wchar_t history[256];
    swprintf_s(history, L"%g %c %g = %g", g_LeftValue, g_Operator, rightValue, result);
    AddHistoryLine(history);

    SetDisplayValue(result);
    g_LeftValue = result;
    g_Operator = 0;
    g_NewInput = true;
}

void SetPowerOperator()
{
    g_LeftValue = GetDisplayValue();
    g_Operator = L'^';
    g_NewInput = true;
}

std::wstring g_Expression;

class CExpressionParser
{
public:
    CExpressionParser(const std::wstring& text)
        : m_Text(text), m_Pos(0)
    {
    }

    double Parse()
    {
        double value = ParseExpression();
        SkipSpaces();

        if (m_Pos != m_Text.length())
            throw std::runtime_error("Unexpected text");

        return value;
    }

private:
    std::wstring m_Text;
    size_t m_Pos;

    void SkipSpaces()
    {
        while (m_Pos < m_Text.length() && iswspace(m_Text[m_Pos]))
            m_Pos++;
    }

    bool Match(wchar_t ch)
    {
        SkipSpaces();

        if (m_Pos < m_Text.length() && m_Text[m_Pos] == ch)
        {
            m_Pos++;
            return true;
        }

        return false;
    }

    double ParseExpression()
    {
        double value = ParseTerm();

        while (true)
        {
            if (Match(L'+'))
                value = NMath::g_pMath->Add(value, ParseTerm());
            else if (Match(L'-'))
                value = NMath::g_pMath->Subtract(value, ParseTerm());
            else
                return value;
        }
    }

    double ParseTerm()
    {
        double value = ParsePower();

        while (true)
        {
            if (Match(L'*'))
                value = NMath::g_pMath->Multiply(value, ParsePower());
            else if (Match(L'/'))
            {
                double rhs = ParsePower();

                if (rhs == 0.0)
                    throw std::runtime_error("Divide by zero");

                value = NMath::g_pMath->Divide(value, rhs);
            }
            else
                return value;
        }
    }

    double ParsePower()
    {
        double value = ParseFactor();

        if (Match(L'^'))
            value = NMath::g_pMath->Power(value, ParsePower());

        return value;
    }

    double ParseFactor()
    {
        SkipSpaces();

        if (Match(L'-'))
            return -ParseFactor();

        if (Match(L'+'))
            return ParseFactor();

        if (Match(L'('))
        {
            double value = ParseExpression();

            if (!Match(L')'))
                throw std::runtime_error("Missing )");

            return value;
        }

        if (m_Text.compare(m_Pos, 4, L"abs(") == 0)
        {
            m_Pos += 4;

            double value = ParseExpression();

            if (!Match(L')'))
                throw std::runtime_error("Missing )");

            return NMath::g_pMath->Absolute(value);
        }

        if (m_Text.compare(m_Pos, 5, L"sqrt(") == 0)
        {
            m_Pos += 5;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->SquareRoot(value);
        }

        if (m_Text.compare(m_Pos, 7, L"square(") == 0)
        {
            m_Pos += 7;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Square(value);
        }

        if (m_Text.compare(m_Pos, 5, L"cube(") == 0)
        {
            m_Pos += 5;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Cube(value);
        }

        if (m_Text.compare(m_Pos, 6, L"floor(") == 0)
        {
            m_Pos += 6;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Floor(value);
        }

        if (m_Text.compare(m_Pos, 5, L"ceil(") == 0)
        {
            m_Pos += 5;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Ceiling(value);
        }

        if (m_Text.compare(m_Pos, 6, L"round(") == 0)
        {
            m_Pos += 6;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Round(value);
        }

        if (m_Text.compare(m_Pos, 4, L"sin(") == 0)
        {
            m_Pos += 4;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Sin(value);
        }

        if (m_Text.compare(m_Pos, 4, L"cos(") == 0)
        {
            m_Pos += 4;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Cos(value);
        }

        if (m_Text.compare(m_Pos, 4, L"tan(") == 0)
        {
            m_Pos += 4;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Tan(value);
        }

        if (m_Text.compare(m_Pos, 4, L"log(") == 0)
        {
            m_Pos += 4;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Log(value);
        }

        if (m_Text.compare(m_Pos, 6, L"log10(") == 0)
        {
            m_Pos += 6;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Log10(value);
        }

        if (m_Text.compare(m_Pos, 4, L"exp(") == 0)
        {
            m_Pos += 4;
            double value = ParseExpression();
            if (!Match(L')')) throw std::runtime_error("Missing )");
            return NMath::g_pMath->Exp(value);
        }

        return ParseNumber();
    }

    double ParseNumber()
    {
        SkipSpaces();

        size_t start = m_Pos;

        while (m_Pos < m_Text.length() &&
            (iswdigit(m_Text[m_Pos]) || m_Text[m_Pos] == L'.'))
        {
            m_Pos++;
        }

        if (start == m_Pos)
            throw std::runtime_error("Expected number");

        return _wtof(m_Text.substr(start, m_Pos - start).c_str());
    }
};

void AppendExpression(const wchar_t* text)
{
    g_Expression += text;
    SetWindowTextW(g_hDisplay, g_Expression.c_str());
    g_NewInput = false;
}

void ClearExpression()
{
    g_Expression.clear();
    SetWindowTextW(g_hDisplay, L"0");
    g_NewInput = true;
}

void EvaluateExpression(HWND hWnd)
{
    try
    {
        CExpressionParser parser(g_Expression);
        double result = parser.Parse();

        wchar_t history[512];
        swprintf_s(history, L"%s = %g", g_Expression.c_str(), result);
        AddHistoryLine(history);

        SetDisplayValue(result);

        wchar_t resultText[128];
        swprintf_s(resultText, L"%g", result);
        g_Expression = resultText;

        g_NewInput = true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Parser error: " << e.what() << std::endl;

        SetWindowTextW(g_hDisplay, L"Error");
        g_Expression.clear();
        g_NewInput = true;
    }
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {

        // Summon the boxes

        g_hDisplay = CreateWindowW(
            L"EDIT",
            L"0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_READONLY,
            10, 10, 300, 35,
            hWnd,
            (HMENU)IDC_DISPLAY,
            hInst,
            nullptr
        );

        g_hHistory = CreateWindowW(
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
            10, 330, 285, 90,
            hWnd,
            nullptr,
            hInst,
            nullptr
        );

        // Summon the buttons

        CreateWindowW(L"BUTTON", L"7", WS_CHILD | WS_VISIBLE, 10, 60, 60, 45, hWnd, (HMENU)IDC_BTN_7, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"8", WS_CHILD | WS_VISIBLE, 85, 60, 60, 45, hWnd, (HMENU)IDC_BTN_8, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"9", WS_CHILD | WS_VISIBLE, 160, 60, 60, 45, hWnd, (HMENU)IDC_BTN_9, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"/", WS_CHILD | WS_VISIBLE, 235, 60, 60, 45, hWnd, (HMENU)IDC_BTN_DIV, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"4", WS_CHILD | WS_VISIBLE, 10, 115, 60, 45, hWnd, (HMENU)IDC_BTN_4, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"5", WS_CHILD | WS_VISIBLE, 85, 115, 60, 45, hWnd, (HMENU)IDC_BTN_5, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"6", WS_CHILD | WS_VISIBLE, 160, 115, 60, 45, hWnd, (HMENU)IDC_BTN_6, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"*", WS_CHILD | WS_VISIBLE, 235, 115, 60, 45, hWnd, (HMENU)IDC_BTN_MUL, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"1", WS_CHILD | WS_VISIBLE, 10, 170, 60, 45, hWnd, (HMENU)IDC_BTN_1, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"2", WS_CHILD | WS_VISIBLE, 85, 170, 60, 45, hWnd, (HMENU)IDC_BTN_2, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"3", WS_CHILD | WS_VISIBLE, 160, 170, 60, 45, hWnd, (HMENU)IDC_BTN_3, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"-", WS_CHILD | WS_VISIBLE, 235, 170, 60, 45, hWnd, (HMENU)IDC_BTN_SUB, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"0", WS_CHILD | WS_VISIBLE, 10, 225, 60, 45, hWnd, (HMENU)IDC_BTN_0, hInst, nullptr);
        CreateWindowW(L"BUTTON", L".", WS_CHILD | WS_VISIBLE, 85, 225, 60, 45, hWnd, (HMENU)IDC_BTN_DOT, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"=", WS_CHILD | WS_VISIBLE, 160, 225, 60, 45, hWnd, (HMENU)IDC_BTN_EQ, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE, 235, 225, 60, 45, hWnd, (HMENU)IDC_BTN_ADD, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"C", WS_CHILD | WS_VISIBLE, 10, 280, 285, 25, hWnd, (HMENU)IDC_BTN_CLR, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"x²", WS_CHILD | WS_VISIBLE, 320, 60, 70, 35, hWnd, (HMENU)IDC_BTN_SQUARE, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"x³", WS_CHILD | WS_VISIBLE, 400, 60, 70, 35, hWnd, (HMENU)IDC_BTN_CUBE, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"√x", WS_CHILD | WS_VISIBLE, 480, 60, 70, 35, hWnd, (HMENU)IDC_BTN_SQRT, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"xʸ", WS_CHILD | WS_VISIBLE, 320, 105, 70, 35, hWnd, (HMENU)IDC_BTN_POWER, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"abs", WS_CHILD | WS_VISIBLE, 400, 105, 70, 35, hWnd, (HMENU)IDC_BTN_ABS, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"floor", WS_CHILD | WS_VISIBLE, 480, 105, 70, 35, hWnd, (HMENU)IDC_BTN_FLOOR, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"ceil", WS_CHILD | WS_VISIBLE, 320, 150, 70, 35, hWnd, (HMENU)IDC_BTN_CEIL, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"round", WS_CHILD | WS_VISIBLE, 400, 150, 70, 35, hWnd, (HMENU)IDC_BTN_ROUND, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"sin", WS_CHILD | WS_VISIBLE, 480, 150, 70, 35, hWnd, (HMENU)IDC_BTN_SIN, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"cos", WS_CHILD | WS_VISIBLE, 320, 195, 70, 35, hWnd, (HMENU)IDC_BTN_COS, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"tan", WS_CHILD | WS_VISIBLE, 400, 195, 70, 35, hWnd, (HMENU)IDC_BTN_TAN, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"log", WS_CHILD | WS_VISIBLE, 480, 195, 70, 35, hWnd, (HMENU)IDC_BTN_LOG, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"log10", WS_CHILD | WS_VISIBLE, 320, 240, 70, 35, hWnd, (HMENU)IDC_BTN_LOG10, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"exp", WS_CHILD | WS_VISIBLE, 400, 240, 70, 35, hWnd, (HMENU)IDC_BTN_EXP, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"π", WS_CHILD | WS_VISIBLE, 480, 240, 70, 35, hWnd, (HMENU)IDC_BTN_PI, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"e", WS_CHILD | WS_VISIBLE, 320, 285, 70, 35, hWnd, (HMENU)IDC_BTN_E, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"rand", WS_CHILD | WS_VISIBLE, 400, 285, 70, 35, hWnd, (HMENU)IDC_BTN_RAND, hInst, nullptr);
        CreateWindowW(L"BUTTON", L"randint", WS_CHILD | WS_VISIBLE, 480, 285, 70, 35, hWnd, (HMENU)IDC_BTN_RANDINT, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"bool", WS_CHILD | WS_VISIBLE, 320, 330, 230, 35, hWnd, (HMENU)IDC_BTN_RANDBOOL, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"(", WS_CHILD | WS_VISIBLE,
            320, 375, 50, 35, hWnd, (HMENU)IDC_BTN_LPAREN, hInst, nullptr);

        CreateWindowW(L"BUTTON", L")", WS_CHILD | WS_VISIBLE,
            380, 375, 50, 35, hWnd, (HMENU)IDC_BTN_RPAREN, hInst, nullptr);

        return 0;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDC_BTN_0: AppendExpression(L"0"); break;
            case IDC_BTN_1: AppendExpression(L"1"); break;
            case IDC_BTN_2: AppendExpression(L"2"); break;
            case IDC_BTN_3: AppendExpression(L"3"); break;
            case IDC_BTN_4: AppendExpression(L"4"); break;
            case IDC_BTN_5: AppendExpression(L"5"); break;
            case IDC_BTN_6: AppendExpression(L"6"); break;
            case IDC_BTN_7: AppendExpression(L"7"); break;
            case IDC_BTN_8: AppendExpression(L"8"); break;
            case IDC_BTN_9: AppendExpression(L"9"); break;

            case IDC_BTN_DOT: AppendExpression(L"."); break;

            case IDC_BTN_ADD: AppendExpression(L"+"); break;
            case IDC_BTN_SUB: AppendExpression(L"-"); break;
            case IDC_BTN_MUL: AppendExpression(L"*"); break;
            case IDC_BTN_DIV: AppendExpression(L"/"); break;
            case IDC_BTN_POWER: AppendExpression(L"^"); break;

            case IDC_BTN_LPAREN: AppendExpression(L"("); break;
            case IDC_BTN_RPAREN: AppendExpression(L")"); break;

            case IDC_BTN_ABS:   AppendExpression(L"abs("); break;
            case IDC_BTN_SQRT:  AppendExpression(L"sqrt("); break;
            case IDC_BTN_SQUARE: AppendExpression(L"square("); break;
            case IDC_BTN_CUBE:  AppendExpression(L"cube("); break;

            case IDC_BTN_FLOOR: AppendExpression(L"floor("); break;
            case IDC_BTN_CEIL:  AppendExpression(L"ceil("); break;
            case IDC_BTN_ROUND: AppendExpression(L"round("); break;

            case IDC_BTN_SIN:   AppendExpression(L"sin("); break;
            case IDC_BTN_COS:   AppendExpression(L"cos("); break;
            case IDC_BTN_TAN:   AppendExpression(L"tan("); break;

            case IDC_BTN_LOG:   AppendExpression(L"log("); break;
            case IDC_BTN_LOG10: AppendExpression(L"log10("); break;
            case IDC_BTN_EXP:   AppendExpression(L"exp("); break;

            case IDC_BTN_PI:
                AppendExpression(L"3.1415926535897932384");
                break;

            case IDC_BTN_E:
                AppendExpression(L"2.7182818284590452353");
                break;

            case IDC_BTN_RAND:
            {
                wchar_t buffer[64];
                swprintf_s(buffer, L"%g", NMath::g_pRandom->RandomDouble(0.0, 1.0));
                AppendExpression(buffer);
                break;
            }

            case IDC_BTN_RANDINT:
            {
                wchar_t buffer[64];
                swprintf_s(buffer, L"%d", NMath::g_pRandom->RandomInt(0, 100));
                AppendExpression(buffer);
                break;
            }

            case IDC_BTN_RANDBOOL:
                AppendExpression(NMath::g_pRandom->RandomBool() ? L"1" : L"0");
                break;

            case IDC_BTN_EQ:
                EvaluateExpression(hWnd);
                break;

            case IDC_BTN_CLR:
                ClearExpression();
                break;

            case ID_FILE_SAVEHISTORY:
                SaveHistoryToFile(hWnd);
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case ID_FILE_OPTIONS:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_OPTIONS), hWnd, Options);
                break;

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        g_Running = false;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_SECRETTEXT:
            SecretBeep();
            return (INT_PTR)TRUE;

        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        CheckRadioButton(
            hDlg,
            IDC_RADIO1,
            IDC_RADIO5,
            IDC_RADIO1 + g_PrecisionMode - 1
        );
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_RADIO1:
        case IDC_RADIO2:
        case IDC_RADIO3:
        case IDC_RADIO4:
        case IDC_RADIO5:
            g_PrecisionMode = LOWORD(wParam) - IDC_RADIO1 + 1;

            CheckRadioButton(
                hDlg,
                IDC_RADIO1,
                IDC_RADIO5,
                LOWORD(wParam)
            );
            return (INT_PTR)TRUE;

        case IDOK:
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}