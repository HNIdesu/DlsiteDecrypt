#include <MinHook.h>
#include "Message.h"
#include <unordered_map>
#include <windows.h>
#include "TcpClient.h"

using namespace std;

static BOOL gIsHookStarted = FALSE;
static TcpClient* gTcpClient = nullptr;

static void SendRemoteMessage(Message& msg) {
    if (gTcpClient == nullptr || gTcpClient->IsClosed())return;
    char* buffer = new char[msg.len1+msg.len2+msg.len3+1+4+3*4];
    int start = 0;
    try {
        memcpy(buffer + start, msg.signature, 4);//signature
        start += 4;
        memcpy(buffer + start, (const char*)&msg.len1, 4);//len1
        start += 4;
        memcpy(buffer + start, (const char*)&msg.len2, 4);//len2
        start += 4;
        memcpy(buffer + start, (const char*)&msg.len3, 4);//len3
        start += 4;
        if (msg.len1 > 0){
            memcpy(buffer + start, msg.data1, msg.len1);//data1
            start += msg.len1;
        }
        if (msg.len2 > 0) {
            memcpy(buffer + start, msg.data2, msg.len2);//data2
            start += msg.len2;
        }
        if (msg.len3 > 0) {
            memcpy(buffer + start, msg.data3, msg.len3);//data3
            start += msg.len3;
        }
        buffer[start] = msg.checksum;
        start += 1;
        gTcpClient->Send(buffer, start);
    }
    catch (exception) {}
    delete[] buffer;
}

struct HookEntry {
    LPVOID pTarget;
    LPVOID pDetour;
    LPVOID ppOriginal;
};

static unordered_map<LPVOID,HookEntry> HookEntryList;
static int __stdcall ctcrypt_27(int a1, int key, int key_length, int a4, int a5) {
    HookEntry entry = HookEntryList[ctcrypt_27];
    auto result = ((int (__stdcall*)(int, int, int, int, int))entry.ppOriginal)(a1, key, key_length, a4, a5);
    SendRemoteMessage(Message(3, "KEY",key_length, (const char*)key));
    return result;
}

static BOOL StartHook(){
    if (!gIsHookStarted)
    {
        if (MH_Initialize() == MH_OK) {
            auto hModule = GetModuleHandleW(L"ctcrypt.dll");
            if (hModule == NULL) {
                SendRemoteMessage(Message("LOG", "ctcrypt.dll not found,try to load"));
                hModule = LoadLibraryW(L"ctcrypt.dll");
                if (hModule == NULL) {
                    SendRemoteMessage(Message("LOG", "Failed to load ctcrypt.dll"));
                    return FALSE;
                }
            }
            auto pCtcrypt27 = GetProcAddress(hModule, MAKEINTRESOURCE(27));
            auto entry = HookEntry();
            entry.pTarget = pCtcrypt27;
            if (MH_CreateHook(pCtcrypt27, ctcrypt_27, &entry.ppOriginal) != MH_OK
                || MH_EnableHook(pCtcrypt27) != MH_OK) {
                SendRemoteMessage(Message("LOG", "Failed to create and enable hook"));
                return FALSE;
            }
            entry.pDetour = ctcrypt_27;
            HookEntryList[ctcrypt_27] = entry;
            gIsHookStarted = TRUE;
        }
        else
            SendRemoteMessage(Message("LOG", "Failed to initialize minhook"));
        
    }
    return gIsHookStarted;
}

static BOOL StopHook(){
    if (gIsHookStarted) {
        for (pair<LPVOID,HookEntry> pair : HookEntryList) {
            MH_RemoveHook(pair.second.pTarget);
        }
        HookEntryList.clear();
        gIsHookStarted = (MH_Uninitialize() != MH_OK);
    }
    return !gIsHookStarted;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    BOOL result = TRUE;
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            gTcpClient = new TcpClient();
            result = gTcpClient->Connect("127.0.0.1", 35701) && StartHook();
            break;
        case DLL_PROCESS_DETACH:
            if (gTcpClient != nullptr) {
                delete gTcpClient;
                gTcpClient = nullptr;
            }
            result = StopHook();
            break;
    }
    return result;
}

