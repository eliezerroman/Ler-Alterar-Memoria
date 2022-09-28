#include <iostream>
#include <windows.h>
#include <TlHelp32.h>

/*(Handle é um "ponteiro abstrato". O ponteiro geralmente é definido como uma variável que aponta para um recurso do endereço de memória.
Enquanto o Handle referencia um recurso, mas não precisa ser necessáriamente um endereço de memória, pode ser um index de um vetor ou lista por exemplo)*/
HANDLE handleProcess = NULL;

// Guarda o Id do processo que será executado. (DWORD é uma variável similar a um unsigned int)
DWORD processID;

bool attachProcess(char *processName)
{
    PROCESSENTRY32 processEntry32;

    // Setar o tamanho da variável de acordo com o tamanho do processo
    processEntry32.dwSize = sizeof(PROCESSENTRY32);

    // tirando um snapshot de todos os processos que estão sendo executados
    auto handleProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // validando o snapshot
    if (handleProcessSnapshot == INVALID_HANDLE_VALUE)
    {
        std::cout << "Não foi possível encontrar os processos" << std::endl;
        return false;
    }

    // Passa por todos os processos da lista
    while (Process32Next(handleProcessSnapshot, &processEntry32))
    {
        // Mostra o nome do processo enquanto passa pela lista
        std::cout << processEntry32.szExeFile << std::endl;

        if (!strcmp(processName, processEntry32.szExeFile))
        {
            std::cout << "Processo: " << processEntry32.szExeFile << " encontrado!" << std::endl;
            std::cout << "ID = " << processEntry32.th32ProcessID << std::endl;

            // Dando acesso ao processo que encontramos
            handleProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry32.th32ProcessID);

            // Setando o processID para o processo que encontramos na lista
            processID = processEntry32.th32ProcessID;

            // Confirmando se foi acessado o processo procurado na lista
            if (handleProcess == NULL)
            {
                std::cout << "Falha ao tentar acessar o processo" << std::endl;
            }

            CloseHandle(handleProcessSnapshot);

            return true;
        }
    }

    std::cout << "Não foi possível encontrar o processo: " << processName << " na lista de processos" << std::endl;

    CloseHandle(handleProcessSnapshot);

    return false;
}

// Criando Wrappers para facilitar o uso da função WriteProcessMemory, dessa forma poderemos usa-lá passando poucas informações
template <class dataType>
void wpm(dataType valueToWrite, DWORD addressToWrite)
{
    WriteProcessMemory(handleProcess, (PVOID)addressToWrite, &valueToWrite, sizeof(dataType), 0);
}

// Criando Wrappers para facilitar o uso da função ReadProcessMemory, dessa forma poderemos usa-lá passando poucas informações
template <class dataType>
void rpm(DWORD addressToRead)
{
    dataType rpmBuffer;

    ReadProcessMemory(handleProcess, (PVOID)addressToRead, &rpmBuffer, sizeof(dataType), 0);

    // inf = rpmBuffer;

    return rpmBuffer;
}

int main()
{
    // endereço de memória que fica o valor da variável(count) do teste.exe
    DWORD memoryAddress = 0x0061FF0C;

    attachProcess((char *)"teste.exe");

    while (true)
    {
        wpm<int>(100, memoryAddress);
    }
}