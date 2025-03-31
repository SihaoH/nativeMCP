#include "computer_use.h"
#include <QVariant>
#include <QThread>
#include <Windows.h>

ComputerUse::ComputerUse()
    : MCPServer()
{
}

QString ComputerUse::waitTime(const QVariant& sec)
{
    QThread::sleep(sec.toInt());
    return "OK";
}

MCPServer::ToolInfo ComputerUse::waitTime$info()
{
    return {
        "waitTime",
        "等待指定的时间，每次操作电脑完后都应该等待适当的时间，等电脑做出反应",
        {
            {"sec", "int", "等待的时间，单位为秒数"}
        }
    };
}

QString ComputerUse::moveCursor(const QVariant& delta_x, const QVariant& delta_y)
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = delta_x.toInt();
    input.mi.dy = delta_y.toInt();
    return SendInput(1, &input, sizeof(INPUT)) != 0 ? "OK" : "fail";
}

MCPServer::ToolInfo ComputerUse::moveCursor$info()
{
    return {
        "moveCursor",
        "移动光标，需要操作游戏时很有用",
        {
            {"delta_x", "int", "水平移动的像素距离"},
            {"delta_y", "int", "垂直移动的像素距离"}
        }
    };
}

QString ComputerUse::setCursor(const QVariant& x, const QVariant& y)
{
    // 将坐标转换为屏幕坐标
    double fScreenWidth = GetSystemMetrics(SM_CXSCREEN) - 1;
    double fScreenHeight = GetSystemMetrics(SM_CYSCREEN) - 1;
    double fx = x.toInt() * (65535.0f / fScreenWidth);
    double fy = y.toInt() * (65535.0f / fScreenHeight);

    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    input.mi.dx = static_cast<LONG>(fx);
    input.mi.dy = static_cast<LONG>(fy);
    return SendInput(1, &input, sizeof(INPUT)) != 0 ? "OK" : "fail";
}

MCPServer::ToolInfo ComputerUse::setCursor$info()
{
    return {
        "setCursor",
        "移动光标到指定的位置，针对需要点击的元素",
        {
            {"x", "int", "光标的绝对位置x"},
            {"y", "int", "光标的绝对位置y"}
        }
    };
}

QString ComputerUse::click()
{
    INPUT inputs[2] = {0};
    
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    return SendInput(2, inputs, sizeof(INPUT)) != 0 ? "OK" : "fail";
}

MCPServer::ToolInfo ComputerUse::click$info()
{
    return {
        "click",
        "单击光标当前所在的位置，用于操作某些按钮；操作前请先将光标移动到正确的位置"
    };
}

QString ComputerUse::doubleClick()
{
    INPUT inputs[4] = {0};

    // 第一次点击
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // 第二次点击
    inputs[2].type = INPUT_MOUSE;
    inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    inputs[3].type = INPUT_MOUSE;
    inputs[3].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    return SendInput(4, inputs, sizeof(INPUT)) != 0 ? "OK" : "fail";
}

MCPServer::ToolInfo ComputerUse::doubleClick$info()
{
    return {
        "doubleClick",
        "双击光标当前所在的位置，用于打开某些应用程序；操作前请先将光标移动到正确的位置"
    };
}

// 利用剪切板对任意编辑框输入文本
QString ComputerUse::type(const QVariant& text)
{
    // 保存原来的剪贴板内容
    HANDLE hOldData = NULL;
    if (OpenClipboard(NULL)) {
        hOldData = GetClipboardData(CF_UNICODETEXT);
        if (hOldData) {
            HANDLE hOldCopy = GlobalAlloc(GMEM_MOVEABLE, GlobalSize(hOldData));
            if (hOldCopy) {
                void* pOldData = GlobalLock(hOldData);
                void* pOldCopy = GlobalLock(hOldCopy);
                if (pOldData && pOldCopy) {
                    memcpy(pOldCopy, pOldData, GlobalSize(hOldData));
                    GlobalUnlock(hOldData);
                    GlobalUnlock(hOldCopy);
                    hOldData = hOldCopy;
                } else {
                    setLastCallError(true);
                    return "保存剪贴板原先的内容失败了";
                }
            }
        }
        CloseClipboard();
    }

    // 将要输入的文本复制到剪贴板
    QString str = text.toString();
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, (str.length() + 1) * sizeof(wchar_t));
        if (hData) {
            wchar_t* pData = (wchar_t*)GlobalLock(hData);
            if (pData) {
                str.toWCharArray(pData);
                pData[str.length()] = '\0';
                GlobalUnlock(hData);
                SetClipboardData(CF_UNICODETEXT, hData);
            } else {
                setLastCallError(true);
                return "利用剪切板输入内容失败";
            }
        }
        CloseClipboard();
    }

    // 模拟Ctrl+V
    INPUT inputs[4] = {0};
    
    // 按下Ctrl
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    
    // 按下V
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'V';
    
    // 释放V
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    
    // 释放Ctrl
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    
    SendInput(4, inputs, sizeof(INPUT));
    
    // 等待粘贴完成
    QThread::msleep(200);

    // 恢复原来的剪贴板内容
    if (hOldData && OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, hOldData);
        CloseClipboard();
    }
    return "OK";
}

MCPServer::ToolInfo ComputerUse::type$info()
{
    return {
        "type",
        "对当前获得键盘焦点的任意编辑框键入文本内容",
        {
            {"text", "string", "需要输入的文本内容"}
        }
    };
}

QString ComputerUse::pressKey(const QVariant& key)
{
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key.toUInt();
    return SendInput(1, &input, sizeof(INPUT)) != 0 ? "OK" : "fail";
}

MCPServer::ToolInfo ComputerUse::pressKey$info()
{
    return {
        "pressKey",
        "按下键盘的按键，多次调用可以达到组合按键的功能",
        {
            {"key", "int", "Windows系统定义的按键VK码，如DELETE键码为0x2E，如果是字母数字则是其ascii码"}
        }
    };
}

QString ComputerUse::releaseKey(const QVariant& key)
{
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key.toUInt();
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    return SendInput(1, &input, sizeof(INPUT)) != 0 ? "OK" : "fail";
}

MCPServer::ToolInfo ComputerUse::releaseKey$info()
{
    return {
        "releaseKey",
        "松开键盘的按键",
        {
            {"key", "int", "Windows系统定义的按键VK码，如DELETE键码为0x2E，如果是字母数字则是其ascii码"}
        }
    };
}

QString ComputerUse::knockKey(const QVariant& key)
{
    INPUT inputs[2] = {0};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key.toInt();

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key.toInt();
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    return SendInput(2, inputs, sizeof(INPUT)) != 0 ? "OK" : "fail";
}

MCPServer::ToolInfo ComputerUse::knockKey$info()
{
    return {
        "knockKey",
        "敲击键盘的按键，即按下按键后马上松开，是操作键盘的快捷方法",
        {
            {"key", "int", "Windows系统定义的按键VK码，如DELETE键码为0x2E，如果是字母数字则是其ascii码"}
        }
    };
}

MCP_SERVER_MAIN(ComputerUse)
