# Chrome Stealth Clicker

Minimal Visual Studio PoC showing that Chrome can still receive scripted keyboard and mouse input while a window’s alpha is set to `1` (effectively invisible). Intended for a [Chrome VRP](https://bughunters.google.com/about/rules/5745167867576320/chrome-vulnerability-reward-program-rules) report.

**Demo:** [YouTube](https://www.youtube.com/watch?v=6Trcj9owDNk)

## Report summary

Chrome windows can be made nearly transparent (`SetLayeredWindowAttributes` with alpha `1`) and still accept focus plus quick keyboard commands. An attacker can use that to change Chrome settings (for example the default search provider) in about a second or two, so the activity looks like a brief glitch rather than a visible UI takeover.

This does **not** fight Chrome’s file integrity and does **not** install an extension. It drives a normal Chrome window through the regular GUI while that window is invisible.

| Field | Value |
| --- | --- |
| Program | Chrome VRP |
| Type | Other (not crash-related) |
| Tested Chrome | 155.0.8059.85 (Official Build) (64-bit) |
| Tested OS | Windows 10 Enterprise 22H2, OS Build 19045.6456 |
| Privileges | PoC exe as administrator (can be piggybacked onto another elevated installer) |

## How the PoC works

1. A high-priority helper thread repeatedly steals focus onto a `Chrome_WidgetWin_1` window and keeps its layered alpha at `1`.
2. While the window is invisible but focused, the main path sends short keyboard sequences (for example open the omnibox / settings flow and change the default search engine).
3. Actions are kept to roughly **1–2 seconds** so the user experience looks like a glitch or crash rather than an interactive session.

Core stealth routine (alpha set to `1`):

```cpp
void stealthAllChromeWindows(BOOL stealth)
{
    HWND fw = FindWindowEx(NULL, NULL, L"Chrome_WidgetWin_1", NULL);

    WCHAR title[4096];
    ZeroMemory(title, 4096 * sizeof WCHAR);

    if (stealth)
    {
        SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(fw, 0, alpha_value, LWA_ALPHA);
    }
    else
    {
        SetLayeredWindowAttributes(fw, 0, 255, LWA_ALPHA);
        SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) | WS_EX_APPWINDOW);
        SetWindowLong(fw, GWL_EXSTYLE, GetWindowLong(fw, GWL_EXSTYLE) & ~WS_EX_TOOLWINDOW);
    }

    SetForegroundWindow(fw);
    SetActiveWindow(fw);
    //BringWindowToTop(fw);

    SetFocus(fw);
    PostMessage(fw, WM_SETFOCUS, NULL, NULL);
    SendMessage(fw, WM_SETFOCUS, NULL, NULL);
}
```

`alpha_value` is `1`. That loop is driven from a separate high-priority thread (`hideThread1`) so the window stays stealthy while input is injected.

## Impact

With elevation, the PoC can perform anything a normal admin user could do through Chrome’s GUI on an already-available window, while remaining invisible. Strengths of this approach:

- No Chrome binary patching
- No malicious extension install
- Uses ordinary Win32 focus + layered-window APIs plus `SendInput`

Limitation: the useful window of action is short. Longer interactions become noticeable.

## Build

This is a **Visual Studio** Win32 C++ project (`stealth_clicker.sln`).

1. Open `stealth_clicker.sln` in Visual Studio (the project targets a modern VS / MSVC toolchain; VS 2022+ is fine).
2. Build the `stealth_clicker` project (Release | x64 recommended).
3. Run the resulting `.exe` **as Administrator** on a machine with Chrome installed.

Project layout:

```
stealth_clicker.sln
stealth_clicker/
  stealth_clicker.cpp   # main PoC logic
  stealth_clicker.vcxproj
  *.rc / *.h / icons    # AppWizard resources
```

## Responsible use

This repository is a **security research PoC** for reporting to Google’s Chrome Vulnerability Reward Program. Use it only on systems you own or are authorized to test, and only for disclosure / reproduction with Chromium security.

## Links

- Repository: https://github.com/fotiosb/Chrome-Stealth-Clicker
- Chrome VRP rules: https://bughunters.google.com/about/rules/5745167867576320/chrome-vulnerability-reward-program-rules
- Demo video: https://www.youtube.com/watch?v=6Trcj9owDNk
