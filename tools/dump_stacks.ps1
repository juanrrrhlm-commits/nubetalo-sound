# Herramienta de diagnóstico: imprime la pila de cada hilo de un proceso (símbolos desde el PDB junto al exe).
param([int]$ProcId, [string]$Exe)
Add-Type -TypeDefinition @'
using System; using System.Runtime.InteropServices; using System.Text; using System.Collections.Generic;
public static class SW {
  [DllImport("kernel32.dll")] static extern IntPtr OpenThread(uint a, bool i, uint id);
  [DllImport("kernel32.dll")] static extern uint SuspendThread(IntPtr h);
  [DllImport("kernel32.dll")] static extern uint ResumeThread(IntPtr h);
  [DllImport("kernel32.dll")] static extern bool GetThreadContext(IntPtr h, IntPtr ctx);
  [DllImport("kernel32.dll")] static extern IntPtr OpenProcess(uint a, bool i, uint pid);
  [DllImport("kernel32.dll")] static extern bool ReadProcessMemory(IntPtr p, IntPtr a, byte[] b, IntPtr n, out IntPtr r);
  [DllImport("dbghelp.dll", SetLastError=true)] static extern bool SymInitialize(IntPtr p, string path, bool inv);
  [DllImport("dbghelp.dll", CharSet=CharSet.Ansi)] static extern ulong SymLoadModuleEx(IntPtr p, IntPtr f, string img, string mod, ulong b, uint sz, IntPtr d, uint fl);
  [DllImport("dbghelp.dll")] static extern bool SymFromAddr(IntPtr p, ulong a, out ulong disp, IntPtr sym);
  [DllImport("dbghelp.dll")] static extern bool StackWalk64(uint mt, IntPtr p, IntPtr t, IntPtr frame, IntPtr ctx, IntPtr rd, IntPtr fa, IntPtr gm, IntPtr tr);
  [DllImport("dbghelp.dll")] static extern IntPtr SymFunctionTableAccess64(IntPtr p, ulong a);
  [DllImport("dbghelp.dll")] static extern ulong SymGetModuleBase64(IntPtr p, ulong a);
  [DllImport("kernel32.dll")] static extern IntPtr LoadLibrary(string n);
  [DllImport("kernel32.dll")] static extern IntPtr GetProcAddress(IntPtr m, string n);
  delegate bool RM(IntPtr p, ulong a, IntPtr buf, uint n, out uint r);
  public static string Dump(uint pid, uint tid) {
    var sb = new StringBuilder();
    IntPtr hp = OpenProcess(0x1F0FFF, false, pid), ht = OpenThread(0x1F03FF, false, tid);
    if (ht == IntPtr.Zero) return "no thread";
    SuspendThread(ht);
    IntPtr ctx = Marshal.AllocHGlobal(1232 + 16); ctx = (IntPtr)(((long)ctx + 15) & ~15L);
    for (int i = 0; i < 1232; i++) Marshal.WriteByte(ctx, i, 0);
    Marshal.WriteInt32(ctx, 0x30, 0x10001F); // CONTEXT_ALL
    if (!GetThreadContext(ht, ctx)) { ResumeThread(ht); return "ctx fail"; }
    ulong rip = (ulong)Marshal.ReadInt64(ctx, 0xF8), rsp = (ulong)Marshal.ReadInt64(ctx, 0x98), rbp = (ulong)Marshal.ReadInt64(ctx, 0xA0);
    IntPtr frame = Marshal.AllocHGlobal(256); for (int i = 0; i < 256; i++) Marshal.WriteByte(frame, i, 0);
    Marshal.WriteInt64(frame, 0, (long)rip); Marshal.WriteInt32(frame, 12, 3);
    Marshal.WriteInt64(frame, 32, (long)rbp); Marshal.WriteInt32(frame, 44, 3);
    Marshal.WriteInt64(frame, 48, (long)rsp); Marshal.WriteInt32(frame, 60, 3);
    IntPtr sym = Marshal.AllocHGlobal(512);
    for (int n = 0; n < 40; n++) {
      IntPtr lib = LoadLibrary("dbghelp.dll");
      if (!StackWalk64(0x8664, hp, ht, frame, ctx, IntPtr.Zero, GetProcAddress(lib, "SymFunctionTableAccess64"), GetProcAddress(lib, "SymGetModuleBase64"), IntPtr.Zero)) break;
      ulong pc = (ulong)Marshal.ReadInt64(frame, 0); if (pc == 0) break;
      for (int i = 0; i < 512; i++) Marshal.WriteByte(sym, i, 0);
      Marshal.WriteInt32(sym, 0, 88); Marshal.WriteInt32(sym, 8 * 0 + 4 * 0 + 80, 0);
      Marshal.WriteInt32(sym, 80, 256); // MaxNameLen at offset 80 (SYMBOL_INFO: SizeOfStruct@0, MaxNameLen@80)
      ulong d; string name = SymFromAddr(hp, pc, out d, sym) ? Marshal.PtrToStringAnsi(sym + 84) : "?";
      sb.AppendLine("   " + pc.ToString("X") + " " + name + "+0x" + d.ToString("X"));
    }
    ResumeThread(ht); return sb.ToString();
  }
  public static void Init(uint pid, string exe, string pdbdir) {
    IntPtr hp = OpenProcess(0x1F0FFF, false, pid);
    SymInitialize(hp, pdbdir + ";srv*", false);
    SymLoadModuleEx(hp, IntPtr.Zero, exe, null, 0, 0, IntPtr.Zero, 0);
  }
}
'@
[SW]::Init($ProcId, $Exe, (Split-Path $Exe))
foreach ($t in (Get-Process -Id $ProcId).Threads) {
  "--- hilo $($t.Id) $($t.ThreadState) $($t.WaitReason)"
  [SW]::Dump($ProcId, $t.Id)
}
