#pragma once

#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>
#include <string.h>
using namespace std;

#define _CRT_SECURE_NO_DEPRICATE
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
 





namespace memory
{
    bool Compare(const BYTE* pData, const BYTE* bMask, const char* szMask)
    {
        for (; *szMask; ++szMask, ++pData, ++bMask)
            if (*szMask == 'x' && *pData != *bMask) return 0;
        return (*szMask) == NULL;
    }

    DWORD FindPattern(DWORD dwAddress, DWORD dwLen, BYTE* bMask, char* szMask)
    {
        for (int i = 0; i < (int)dwLen; i++)
            if (Compare((BYTE*)(dwAddress + (int)i), bMask, szMask))  return (int)(dwAddress + i);
        return 0;
    }

    DWORD scan(char* content)
    {
        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);
        DWORD PageSize = SystemInfo.dwPageSize;
        MEMORY_BASIC_INFORMATION meminfo;
        DWORD Start = (DWORD)SystemInfo.lpMinimumApplicationAddress;
        DWORD End = (DWORD)SystemInfo.lpMaximumApplicationAddress;
        for (DWORD lpAddress = Start; lpAddress <= End; lpAddress += PageSize) //tepigs scanner
        {
            VirtualQuery((void*)lpAddress, &meminfo, PageSize);
            if (meminfo.Type == MEM_MAPPED) continue;
            if (meminfo.Protect == PAGE_READWRITE) {
                DWORD Address = FindPattern(lpAddress, PageSize, (PBYTE)content, (char*)"xxxx");
                if (Address != NULL)
                    return Address;
            }
        }
    }
}



DWORD rbx_L;

DWORD ScriptContext;

#define SetThreadIdentity(rbx_L, Context) *(char*)(rbx_L - 14) = Context 
#define aslr(x)(x - 0x400000 + (DWORD)GetModuleHandleA(0))

#define get_vm_key(rbx_L) decrypt_ptr((int)decrypt_ptr((int)(rbx_L + 8)) + 28) //ckey



int retflag1 = aslr(0x273A8B0);
int retflag2 = aslr(0x273A8A8);

int savedret1, savedret2;






#define set_arg(i,o,n,c)        ((i) = (((i) & MASK0(n, c) | \
                                (((Instruction)o << c) & MASK1(n, c)))))

#define bit(n, i) ((n & (1 << i)) ? 1 : 0)



int tzcnt(int num) {   //broken as of last roblox update
    int streak = 0;
    for (int i = 0; i < 32; i++) {
        if (!(num & (1 << i))) streak++;
        else return streak;
    }
    return streak;
}





#define SCRIPTCONTEXT 0x011AD1EC
#define VM_HACKFLAG 0x1566B98
#define DESERIALIZE_ADDRESS 0x404740
#define GETGLOBALSTATE_ADDRESS 0x64D550
#define SPAWN_ADDRESS 0x657EB0
#define NEWTHREAD_ADDRESS 0x492210




namespace func {
    int decrypt_ptr(int ptr) {
        return *(int*)(ptr)+(ptr);
    }

    int getParent(DWORD Instance) {
        __asm {
            mov ecx, Instance
            mov eax, [ecx + 0x34]
        }
    }

    const char* getClassName(DWORD Instance) {
        __asm {
            mov ecx, Instance
            mov eax, [ecx]
            call dword ptr[eax + 0x10]
        }
    }

    std::string getName(DWORD Instance) {
        std::string* Disgusting;
        __asm {
            mov ecx, Instance
            mov eax, [ecx + 0x28]
            mov Disgusting, eax
        }
        return Disgusting->c_str();
    }

    DWORD getGlobalState(DWORD* ScriptContext, DWORD Index) {
        DWORD* SC = ScriptContext;
        DWORD indx = Index;

        return SC[14 * indx + 41] ^ (DWORD)&SC[14 * indx + 41];
    }

    std::string getInstanceName(DWORD Instance) {
        std::string* Disgusting;
        __asm {
            mov ecx, Instance
            mov eax, [ecx + 0x28]
            mov Disgusting, eax
        }
        return Disgusting->c_str();
    }
}







void Console(const char* N) {



    AllocConsole();
    SetConsoleTitleA(N);
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

}




int EncryptOpcode(int op, int key) {
    int a = key;
    int b = op;

    int x = (a >> tzcnt(a));
    int y = (x * x) + x - 1;
    int t = y * x;
    y *= 2 - t;
    t = y * x;
    y *= 2 - t;
    t = y * x;
    y *= 2 - t;
    return y * (b >> tzcnt(a));
}


namespace addresses {
    int getGlobalState = aslr(0x1);
    int lua_error = aslr(0x018E72E0);
}

namespace bypasses {
    void saveRetcheck() {
        savedret1 = *(DWORD*)&retflag1;
        savedret2 = *(DWORD*)&retflag2;
    }

    void overrideRetcheck() {
        *(DWORD*)retflag1 = *(DWORD*)savedret1;
        *(DWORD*)retflag2 = *(DWORD*)savedret2;
    }

}


typedef DWORD(__thiscall* clua_getglobalstate)(DWORD, DWORD);
clua_getglobalstate GetGlobalState = (clua_getglobalstate)addresses::getGlobalState; //removed in last update?? 


//use scriptcontext offset to find lua_state.

typedef int(__cdecl* clua_gettop)(int a1);
clua_gettop gettop = (clua_gettop)aslr(0x0143D920); //outdated

typedef int(__cdecl* clua_settop)(int a1);
clua_settop settop = (clua_settop)aslr(0x11B4E30); //works

typedef int(__cdecl* lua_print)(int, const char*);
lua_print printx = (lua_print)aslr(0x65D8E0); //works

typedef int(__cdecl* RPrint)(int, const char*, ...);
RPrint r_Print = (RPrint)aslr(0x65D8E0);

typedef int(__stdcall* clua_getfield)(int, int, const char*);
clua_getfield getfield = (clua_getfield)aslr(0x143D750); //works



int lua_gettop(unsigned int rL)
{
    return (*(DWORD*)(rL + 24) - *(DWORD*)(rL + 12)) >> 4;
}









void dump_hex(int func_start, int end) {
    int amt = 0;
    while (true) {
        amt += 1;
        int i;
        printf("%d", (func_start));
        cout << reinterpret_cast<int*>(*(DWORD*)func_start) << endl;
        if (amt >= end) {
            printf("Size calculation failed - function too large.");
            break;
        }
        func_start = func_start + 0x1;
    }
}




void pushstringcalled(int a, int b, int c) {
    printf(" Function called.");
}

void setcall(DWORD x, int d, char* opcode) {
    DWORD a, b;
    VirtualProtect((LPVOID)x, d, PAGE_EXECUTE_READWRITE, &a);

    cout << "asd";
    *(char*)x = *opcode;
    *(DWORD*)(x + 1) = (DWORD)((char*)pushstringcalled - (char*)(x + 5));
    VirtualProtect((LPVOID)x, d, a, &a);
}

char* copyopcode(int d) {
    return (char*)d;
}


int scanForHexInFunction(int func, int opcode, int data[], int maxsize) {

    int i = 0;
    while (true) {

        cout << ((BYTE*)func)[0] << endl;
        func += 0x1;
        i += 1;
        if (((BYTE*)func)[0] == opcode) {
            for (int a = 1; a < sizeof(data); a++) {
                printf("%d is a", a);
                if (((BYTE*)func)[a] != data[a - 1]) {
                    printf("found first opcode but data doesnt match... rip\n");
                    break;
                }
            }
        }
        if (i == maxsize) {
            break;
        }
         
    }



    return 1;


}

string int2hex(int x) {
    std::stringstream stream;
    stream << std::hex << std::uppercase << x;
    std::string result(stream.str());
    return result;
}
int vscanForBytes(int addy, int bytes[], int max) {
    int origaddy = addy;
    for (int a = 1; a < max; a++) {
        BYTE* add = (BYTE*)(((BYTE*)addy)[0]);
        int addx = (int)add; // single hex byte of memory

        cout << int2hex(addy) << ": " << int2hex(addx) << endl;

        if (addx == bytes[0]) {
            cout << "found one byte " << endl;
            int tempaddy = addy;
            int startaddy = tempaddy;
            for (int b = 0; b <= sizeof(bytes) - 1; b++) {

                int comp = (int)(BYTE*)(((BYTE*)tempaddy + b)[0]);
                cout << "comparing " << bytes[b] << " to " << comp << endl;

                if (comp == bytes[b] || bytes[b] == 0) {
                    cout << "continuing, it's gold " << endl;
                }
                else
                {
                    break;
                }
                if (b == sizeof(bytes) - 1) {
                    int comp = (int)(BYTE*)(((BYTE*)tempaddy + b)[0]);
                    cout << "found at " << int2hex(comp) << " which is at " << int2hex(tempaddy+b) << endl;
                    return tempaddy + b;
                }
            }
        }
        addy += 1;
    }
    return 1;
}
int scanForBytes(int addy, int bytes[], int max) {
    int origaddy = addy;
    for (int a = 1; a < max; a++) {
        BYTE* add = (BYTE*)(((BYTE*)addy)[0]);
        int addx = (int)add; // single hex byte of memory
        
        //cout << int2hex(addy) << ": " << int2hex(addx) << endl;

        if (addx == bytes[0]) {
           // cout << "found one byte " << endl;
            int tempaddy = addy;
            int startaddy = tempaddy;
            for (int b = 0; b <= sizeof(bytes)-1; b++) {

                int comp = (int)(BYTE*)(((BYTE*)tempaddy + b)[0]);
              //  cout << "comparing " << bytes[b] << " to " << comp << endl;

                if (comp == bytes[b] || bytes[b] == 0) {
                    //cout << "continuing, it's gold " << endl;
                }
                else
                {
                    break;
                }
                if (b == sizeof(bytes)-1) {
                    int comp = (int)(BYTE*)(((BYTE*)tempaddy + b)[0]);
                    //cout << "found at " << int2hex(comp) << " which is at " << int2hex(tempaddy+b) << endl;
                    return tempaddy+b;
                }
            }
        }
        addy += 1;
    }
    return 1;
}


DWORD Scan(char* cn) {
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    DWORD psize = sysinfo.dwPageSize;
    MEMORY_BASIC_INFORMATION minfo;
    DWORD addr;
    for (DWORD lp = (DWORD)sysinfo.lpMinimumApplicationAddress; lp <= (DWORD)sysinfo.lpMaximumApplicationAddress; lp += psize) {
        VirtualQuery((void*)lp, &minfo, psize);
        if (minfo.Protect == PAGE_READWRITE) {
            for (int i = 0; i < (int)psize; i++) {
                for (; *(char*)"xxxx"; ++ * (char*)"xxxx", ++ * (BYTE*)(lp + (int)i), ++(*(PBYTE)cn)) {
                    if (*(char*)"xxxx" == 'x' && *(BYTE*)(lp + (int)i) != *(PBYTE)cn) {
                        DWORD addr = (int)(lp + i);
                        break;
                    }
                }
            }
            if (addr != 0)
                return addr;
        }
    }
}


bool confirmRetcheckExists(int addr) {
    int retbytes[] = { 0xE8, 0x85, 0x95, 0x27 };
    int end = scanForBytes((addr), retbytes, 1500);
    if (end > 1) {
        return true;
    }
    int antiretbytes[] = { 0x90, 0x90, 0x90, 0x90 };
    end = scanForBytes((addr), antiretbytes, 1500);
    if (end > 1) {
        return false;
    }
    cout << "CANNOT FIND EITHER, FUNCTION IS MISSING ENABLED/DISABLED RETCHECK." << endl;
    return false;

}



int* setRetcheck(int addr) {
    int retbytes[] = { 0x72, 0x0, 0x56, 0xE8 };
    int end = vscanForBytes((addr), retbytes, 1500);
    int* old = new int[5];
    old[0] = (int)(((BYTE*)end)[0]);
    old[1] = (int)(((BYTE*)(end + 1))[0]);
    old[2] = (int)(((BYTE*)(end + 2))[0]);
    old[3] = (int)(((BYTE*)(end + 3))[0]);
    old[4] = (int)(((BYTE*)(end + 4))[0]);
    for (int i = 0; i != 5; i++) {
        cout << (old[i]) << " set at " << int2hex(end+i) << endl;
    }
    
    
    cout << endl << endl << "OLD STARTING: " << endl;
    for (int i = 0; i < 5; i++) {
        cout << "old " << i << " : " << int2hex(old[i]) << endl;
    }
    if (end == 0) { 
        
        cout << "\ncannot set retcheck to invalid address" << endl; 
        return 0;
    }
    else {

        DWORD a, b;
        VirtualProtect((LPVOID)end, 5, PAGE_EXECUTE_READWRITE, &a);
        *(char*)end = 0x90;
        *(char*)(end+1) = 0x90;
        *(char*)(end+2) = 0x90;
        *(char*)(end+3) = 0x90;
        *(char*)(end+4) = 0x90;
        VirtualProtect((LPVOID)end, 5, a, &a);
        int retbytes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        int endd = scanForBytes((end), retbytes, 15);
        return old;
    }
}

void restoreRetcheck(int addr, int * bytes) {
    int retbytes[] = { 0x90, 0x90, 0x90, 0x90 };
    int end = scanForBytes((addr), retbytes, 1500);

    if (end == 0) {
        cout << "\ncannot set retcheck to invalid address" << endl;
    }
    else {
        DWORD a, b;
        end = end - 3;
        VirtualProtect((LPVOID)(end), 5, PAGE_EXECUTE_READWRITE, &a);
        
        for (int i = 0; i != 5; i++) {
            *(char*)(end+i) = (bytes[i]);
            cout << (bytes[i]) << " set at " << int2hex(end+i)  << " to " << (bytes[i]) << endl;
        }

        VirtualProtect((LPVOID)(end), 5, a, &a);
    }
}

int* setRet(int addr) {
    int* oldb;
    oldb = setRetcheck(addr);
    cout << "set retcheck" << endl;

    if (confirmRetcheckExists((addr))) {
        cout << "RETCHECK for " << int2hex(addr) << " successfully bypassed." << endl;
    }
    return oldb;
}

void restRet(int addr, int * oldb) {
    addr -= 1;
    restoreRetcheck((addr), oldb);
    
    if (!confirmRetcheckExists((addr))) {
        cout << "RETCHECK for " << int2hex(addr) << " successfully restored." << endl;
    }
}

typedef void(__cdecl* clua_pushstring)(int, const char*);
clua_pushstring pushstring = (clua_pushstring)aslr(0x143EE30); //works

int main() {

    Console("BlackStar");


    int scriptc = aslr(0x01E63178);
    
    DWORD* scriptContext = (DWORD*)memory::scan((char*)&scriptc); //SCRIPTCONTEXT IS DYNAMIC DUE TO vFtable NOW MOVING ON A PER-GAME
                                                                  //BASIS! NEVER USE WITHOUT SCANNING*************

    DWORD* state = (DWORD*)((char*)&scriptContext[45] + scriptContext[45]); // static RLUA_STATE offset from *SCANNED* scriptcontext
    int rstate = (int)state;
    int gett = gettop(rstate);
    cout << "Initialization completed. State: " << int2hex(rstate) << ". Top is " << gett << "." << endl;

    //cout << "top: " << gett << endl;
    int retbytes[] = { 0x11111 };
    vscanForBytes(aslr(0x143EE30)+90, retbytes, 30); //.text page +0x90 from function start -> +30 bytes just dump hex
    cout << "start here" << endl;
    int* by = setRet(aslr(0x143EE30));


    cout << endl << endl << "OLD STARTING: " << endl;
    for (int i = 0; i != 5; i++) {
        cout << "old " << i << " : " << int2hex(*(by+i)) << endl;
    }


    pushstring(rstate, "hi");
    restRet(aslr(0x143EE30),by);

    vscanForBytes(aslr(0x143EE30) + 90, retbytes, 30);
    /*int* oldb;
    oldb = setRetcheck(aslr(0x143D750));
    if (!confirmRetcheckExists(aslr(0x143D750)))
      cout << "RETCHECK for " << int2hex(0x143D750) << " successfully bypassed." << endl;


    cout << endl << "starting topcheck... good output = 0,1,2,3 (each getfield):" << endl;
    cout << "top: " << gettop(rstate) << endl;
    getfield(rstate, -10002, "game");
    cout << "top: " << gettop(rstate) << endl;
    getfield(rstate, -10002, "game");
    cout << "top: " << gettop(rstate) << endl;
    getfield(rstate, -10002, "game");
    cout << "top: " << gettop(rstate) << endl;
    getfield(rstate, -10002, "game");
    cout << "finished topcheck, restoring retcheck" << endl;
    restoreRetcheck(aslr(0x143D750), oldb);
    if (confirmRetcheckExists(aslr(0x143D750)))
        cout << "RETCHECK for " << int2hex(0x143D750) << " successfully restored." << endl;*/

    return 1;
}









BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        main();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

