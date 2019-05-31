/* ----------------------------------------------------------------------------- 
 * Created by * lallous <lallousx86@yahoo.com> *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ----------------------------------------------------------------------------- 


 History
 ---------
 09/09/2004  - added IsInsideVPC()
 02/22/2005  - added IsInsideVMWare()
 03/26/2005  - . added C++ friendy version of IsInsideVPC() and renamed old IsInsideVPC()
               . rewritten IsInsideVmWare()


 Special thanks to Ken Kato <chitchat-lj@infoseek.jp> for his VMWare Backdoor information
 (http://chitchat.at.infoseek.co.jp/vmware/vmtools.html)

*/

/*#include <windows.h>

// IsInsideVPC's exception filter
DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)
{
  PCONTEXT ctx = ep->ContextRecord;

  ctx->Rbx = -1; // Not running VPC
  ctx->Rip += 4; // skip past the "call VPC" opcodes
  return EXCEPTION_CONTINUE_EXECUTION; // we can safely resume execution since we skipped faulty instruction
}

// high level language friendly version of IsInsideVPC()
bool IsInsideVPC()
{
  bool rc = false;

  __try
  {
    _asm push ebx
    _asm mov  ebx, 0 // Flag
    _asm mov  eax, 1 // VPC function number

    // call VPC 
    _asm __emit 0Fh
    _asm __emit 3Fh
    _asm __emit 07h
    _asm __emit 0Bh

    _asm test ebx, ebx
    _asm setz [rc]
    _asm pop ebx
  }
  // The except block shouldn't get triggered if VPC is running!!
  __except(IsInsideVPC_exceptionFilter(GetExceptionInformation()))
  {
  }

  return rc;
}

bool IsInsideVMWare()
{
  bool rc = true;

  __try
  {
    __asm
    {
      push   edx
      push   ecx
      push   ebx

      mov    eax, 'VMXh'
      mov    ebx, 0 // any value but not the MAGIC VALUE
      mov    ecx, 10 // get VMWare version
      mov    edx, 'VX' // port number

      in     eax, dx // read port
                     // on return EAX returns the VERSION
      cmp    ebx, 'VMXh' // is it a reply from VMWare?
      setz   [rc] // set return value

      pop    ebx
      pop    ecx
      pop    edx
    }
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    rc = false;
  }

  return rc;
}

/*

// pure ASM version of IsInsideVPC()
__declspec(naked) bool IsInsideVPC_asm()
{
  __asm
  {
    push ebp
    mov  ebp, esp

    mov  ecx, offset exception_handler

    push ebx
    push ecx

    push dword ptr fs:[0]
    mov  dword ptr fs:[0], esp

    mov  ebx, 0 // Flag
    mov  eax, 1 // VPC function number
  }

    // call VPC 
   _asm __emit 0Fh
   _asm __emit 3Fh
   _asm __emit 07h
   _asm __emit 0Bh

  _asm
  {
    mov eax, dword ptr ss:[esp]
    mov dword ptr fs:[0], eax

    add esp, 8

    test ebx, ebx
    
    setz al

    lea esp, dword ptr ss:[ebp-4]
    mov ebx, dword ptr ss:[esp]
    mov ebp, dword ptr ss:[esp+4]

    add esp, 8

    jmp ret1
   exception_handler:
    mov ecx, [esp+0Ch]
    mov dword ptr [ecx+0A4h], -1 // EBX = -1 -> not running, ebx = 0 -> running
    add dword ptr [ecx+0B8h], 4 // -> skip past the call to VPC
    xor eax, eax // exception is handled
    ret
   ret1:
    ret
  }
}


bool IsInsideVMWare_()
{
  bool r;
  _asm
  {
    push   edx
    push   ecx
    push   ebx

    mov    eax, 'VMXh'
    mov    ebx, 0 // any value but MAGIC VALUE
    mov    ecx, 10 // get VMWare version
    mov    edx, 'VX' // port number
    in     eax, dx // read port
                   // on return EAX returns the VERSION
    cmp    ebx, 'VMXh' // is it a reply from VMWare?
    setz   [r] // set return value

    pop    ebx
    pop    ecx
    pop    edx
  }
  return r;
}

bool IsInsideVMWare()
{
  __try
  {
    return IsInsideVMWare_();
  }
  __except(1) // 1 = EXCEPTION_EXECUTE_HANDLER
  {
    return false;
  }
}
*/
