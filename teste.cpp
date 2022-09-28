#include <windows.h>
#include <iostream>

int main()
{

    int count = 100;

    while (true)
    {
        count -= 1;
        std::cout << count << " \n";
        Sleep(2000);
    }
}