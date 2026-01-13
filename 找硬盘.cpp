//
// Created by AndyCui on 2023/10/17.
//

#include <windows.h>
#include <iostream>

int main() {
    // 获取所有逻辑驱动器字符串
    char drives[256][256];
    DWORD drivesSize = sizeof(drives);
    if (!GetLogicalDriveStrings(drivesSize, drives)) {
        std::cout << "无法获取逻辑驱动器字符串" << std::endl;
        return 1;
    }

    // 遍历每个驱动器并获取其卷信息
    for (char* drive = drives[0]; drive; drive = strchr(drive, '\0') + 1) {
        std::wstring driveW(drive);
        wchar_t driveWStr[256];
        size_t converted = 0;
        wcscpy_s(driveWStr, driveW.c_str());

        // 转换为大写
        for (wchar_t* p = driveWStr; *p; ++p) {
            *p = towupper(*p);
        }

        // 获取卷信息
        DWORD serialNumber = 0, maxComponentLength = 0, numberOfRootDirectories = 0;
        FILETIME volumeCreationTime, volumeLastAccessTime, volumeLastWriteTime;
        if (GetVolumeInformationW(driveWStr, NULL, 0, &serialNumber, &maxComponentLength, &numberOfRootDirectories, &volumeCreationTime, &volumeLastAccessTime, &volumeLastWriteTime)) {
            // 如果驱动器有卷信息，则输出其信息
            SYSTEMTIME st;
            FileTimeToSystemTime(&volumeCreationTime, &st);
            std::wcout << L"驱动器: " << driveWStr << std::endl;
            std::wcout << L"卷序列号: " << serialNumber << std::endl;
            std::wcout << L"最大组件长度: " << maxComponentLength << std::endl;
            std::wcout << L"根目录条目数: " << numberOfRootDirectories << std::endl;
            std::wcout << L"创建时间: " << st.wYear << '-' << st.wMonth << '-' << st.wDay << ' ' << st.wHour << ':' << st.wMinute << ':' << st.wSecond << std::endl;
        } else {
            // 如果无法获取驱动器的卷信息，则输出错误代码
            std::cout << "无法获取驱动器 " << driveWStr << " 的卷信息" << std::endl;
        }
    }

    return 0;
}
