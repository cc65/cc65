#include <windows.h>

int main (void)
{
    /* Broadcast the WM_SETTINGCHANGE message with the lParam argument set
     * to a pointer to the string "Environment" (one of the many undocumented
     * Microsoft kludges).
     */
    SendMessage (HWND_BROADCAST, WM_SETTINGCHANGE, NULL, (LONG) "Environment");
    return 0;
}



