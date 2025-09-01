/*
 * MIT License
 *
 * Copyright (c) 2025 KU Leuven - COSIC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

.text
.thumb
.syntax unified

.align 2

@ input: uint8_t rand1
.global ClearAND
.func ClearAND, ClearAND
.type ClearAND, %function 
ClearAND:
    and r1, r0, r0, ror #16
    mov r0, r1
    bx lr
.endfunc

@ input: uint8_t rand1
.global ClearXOR
.func ClearXOR, ClearXOR
.type ClearXOR, %function 
ClearXOR:
    eor r1, r0, r0, ror #16
    mov r0, r1
    bx lr
.endfunc

@ input: uint8_t rand1, rand2
.global ClearMult
.func ClearMult, ClearMult
.type ClearMult, %function 
ClearMult:
    mul r2, r0, r1
    mov r0, r2
    bx lr
.endfunc

@ input: uint8_t rand1, rand2
.global ClearMultXOR
.func ClearMultXOR, ClearMultXOR
.type ClearMultXOR, %function 
ClearMultXOR:
    mul r2, r0, r1
    eor r0, r2, r2, ror #16
    bx lr
.endfunc

@ input : size_t len
.global ClearStack
.func ClearStack, ClearStack
.type ClearStack, %function
ClearStack:
    push {r4, lr}
    mov r2, r0
    mov r4, r0
    sub sp, sp, r4
    mov r0, sp
    ldr r1, =randclear
    ldr r1, [r1]
    bl memset
    add sp, sp, r4
    pop {r4, pc}
.endfunc
