param([int]$TargetProcessId, [int]$DurationSeconds = 12, [string]$OutputName = 'release_main_thread_wall.json')
$ErrorActionPreference = 'Stop'
$profileSource = @"
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;

public static class CgFrameStackSampler {
    [DllImport("kernel32.dll", SetLastError=true)] static extern IntPtr OpenProcess(uint access, bool inherit, int pid);
    [DllImport("kernel32.dll", SetLastError=true)] static extern IntPtr OpenThread(uint access, bool inherit, uint tid);
    [DllImport("kernel32.dll", SetLastError=true)] static extern uint SuspendThread(IntPtr thread);
    [DllImport("kernel32.dll", SetLastError=true)] static extern uint ResumeThread(IntPtr thread);
    [DllImport("kernel32.dll", SetLastError=true)] static extern bool GetThreadContext(IntPtr thread, IntPtr context);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr handle);
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode)] static extern IntPtr GetModuleHandle(string name);
    [DllImport("kernel32.dll", CharSet=CharSet.Ansi)] static extern IntPtr GetProcAddress(IntPtr module, string name);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr window, out uint pid);
    [DllImport("dbghelp.dll", CharSet=CharSet.Unicode, SetLastError=true)] static extern bool SymInitializeW(IntPtr process, string searchPath, bool invade);
    [DllImport("dbghelp.dll")] static extern bool SymCleanup(IntPtr process);
    [DllImport("dbghelp.dll")] static extern uint SymSetOptions(uint options);
    [DllImport("dbghelp.dll", SetLastError=true)] static extern bool StackWalk64(uint machine, IntPtr process, IntPtr thread, IntPtr frame, IntPtr context, IntPtr readMemory, IntPtr functionTable, IntPtr moduleBase, IntPtr translate);
    [DllImport("dbghelp.dll", SetLastError=true)] static extern bool SymFromAddrW(IntPtr process, ulong address, out ulong displacement, IntPtr symbol);
    public class Sample { public double elapsedMs; public double suspensionMs; public List<string> addresses = new List<string>(); }
    public class Result { public int processId; public uint threadId; public string startUtc; public double durationMs; public double cpuSecondsBefore; public double cpuSecondsAfter; public List<Sample> samples = new List<Sample>(); public Dictionary<string,string> symbols = new Dictionary<string,string>(); public List<object> modules = new List<object>(); }
    static void Address(IntPtr frame, int offset, long value) { Marshal.WriteInt64(frame, offset, value); Marshal.WriteInt32(frame, offset+12, 3); }
    public static Result Capture(int pid, int seconds) {
        var process = Process.GetProcessById(pid);
        uint owner; uint tid = GetWindowThreadProcessId(process.MainWindowHandle, out owner);
        if (tid == 0 || owner != (uint)pid) throw new Exception("目标窗口或主线程无效。");
        var result = new Result { processId=pid, threadId=tid, startUtc=DateTime.UtcNow.ToString("o"), cpuSecondsBefore=process.TotalProcessorTime.TotalSeconds };
        foreach (ProcessModule module in process.Modules) result.modules.Add(new { name=module.ModuleName, path=module.FileName, baseAddress="0x"+module.BaseAddress.ToInt64().ToString("x"), size=module.ModuleMemorySize });
        IntPtr hProcess = OpenProcess(0x410, false, pid);
        if (hProcess == IntPtr.Zero) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
        IntPtr hThread = IntPtr.Zero, rawContext = IntPtr.Zero, frame = IntPtr.Zero;
        bool symbolsInitialized = false;
        try {
            hThread = OpenThread(0x4A, false, tid);
            if (hThread == IntPtr.Zero) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
            SymSetOptions(0x80000 | 0x200 | 0x4 | 0x2);
            if (!SymInitializeW(hProcess, "", true)) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
            symbolsInitialized = true;
            IntPtr dbghelp = GetModuleHandle("dbghelp.dll");
            IntPtr functionTable = GetProcAddress(dbghelp, "SymFunctionTableAccess64");
            IntPtr moduleBase = GetProcAddress(dbghelp, "SymGetModuleBase64");
            rawContext = Marshal.AllocHGlobal(2064);
            IntPtr context = new IntPtr((rawContext.ToInt64()+15)&~15L);
            frame = Marshal.AllocHGlobal(1024);
            byte[] zeroContext = new byte[2048], zeroFrame = new byte[1024];
            var clock = Stopwatch.StartNew();
            while (clock.Elapsed.TotalSeconds < seconds) {
                process.Refresh();
                if (process.HasExited) break;
                Marshal.Copy(zeroContext, 0, context, zeroContext.Length);
                Marshal.Copy(zeroFrame, 0, frame, zeroFrame.Length);
                // 依据本机 Windows SDK 的 x64 CONTEXT 和 ADDRESS64 布局。
                Marshal.WriteInt32(context, 48, 0x10000B);
                var sample = new Sample { elapsedMs=clock.Elapsed.TotalMilliseconds };
                var suspension = Stopwatch.StartNew();
                uint previousSuspendCount = SuspendThread(hThread);
                if (previousSuspendCount == uint.MaxValue) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
                try {
                    if (!GetThreadContext(hThread, context)) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
                    long rip = Marshal.ReadInt64(context, 248);
                    Address(frame, 0, rip); Address(frame, 32, Marshal.ReadInt64(context, 160)); Address(frame, 48, Marshal.ReadInt64(context, 152));
                    sample.addresses.Add("0x"+rip.ToString("x"));
                    for (int depth=0; depth<64; depth++) {
                        if (!StackWalk64(0x8664, hProcess, hThread, frame, context, IntPtr.Zero, functionTable, moduleBase, IntPtr.Zero)) break;
                        long pc = Marshal.ReadInt64(frame, 0);
                        if (pc == 0) break;
                        string value = "0x"+pc.ToString("x");
                        if (sample.addresses[sample.addresses.Count-1] != value) sample.addresses.Add(value);
                    }
                } finally {
                    if (ResumeThread(hThread) == uint.MaxValue) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
                }
                sample.suspensionMs = suspension.Elapsed.TotalMilliseconds;
                result.samples.Add(sample);
                double delay = 10.0 - suspension.Elapsed.TotalMilliseconds;
                if (delay > 0) Thread.Sleep((int)Math.Ceiling(delay));
            }
            result.durationMs = clock.Elapsed.TotalMilliseconds;
            process.Refresh(); result.cpuSecondsAfter = process.TotalProcessorTime.TotalSeconds;
            IntPtr symbol = Marshal.AllocHGlobal(4096);
            try {
                foreach (Sample sample in result.samples) foreach (string text in sample.addresses) {
                    if (result.symbols.ContainsKey(text)) continue;
                    Marshal.Copy(new byte[4096], 0, symbol, 4096);
                    Marshal.WriteInt32(symbol, 0, 88); Marshal.WriteInt32(symbol, 80, 512);
                    ulong displacement;
                    if (SymFromAddrW(hProcess, Convert.ToUInt64(text.Substring(2),16), out displacement, symbol)) result.symbols[text] = Marshal.PtrToStringUni(new IntPtr(symbol.ToInt64()+84), Marshal.ReadInt32(symbol,76)) + "+0x"+displacement.ToString("x");
                    else result.symbols[text] = "";
                }
            } finally { Marshal.FreeHGlobal(symbol); }
            return result;
        } finally {
            if (frame != IntPtr.Zero) Marshal.FreeHGlobal(frame);
            if (rawContext != IntPtr.Zero) Marshal.FreeHGlobal(rawContext);
            if (symbolsInitialized) SymCleanup(hProcess);
            if (hThread != IntPtr.Zero) CloseHandle(hThread);
            CloseHandle(hProcess);
        }
    }
}
"@
Add-Type -TypeDefinition $profileSource -Language CSharp
$profileResult = [CgFrameStackSampler]::Capture($TargetProcessId, $DurationSeconds)
$profileResult | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath (Join-Path $PSScriptRoot $OutputName) -Encoding UTF8
[PSCustomObject]@{ ProcessId=$profileResult.processId; ThreadId=$profileResult.threadId; Samples=$profileResult.samples.Count; DurationMs=$profileResult.durationMs; CPUSeconds=($profileResult.cpuSecondsAfter-$profileResult.cpuSecondsBefore); MaxSuspensionMs=($profileResult.samples.suspensionMs | Measure-Object -Maximum).Maximum; FirstStacks=$profileResult.samples[0].addresses } | ConvertTo-Json -Depth 6 -Compress