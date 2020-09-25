/**
* Copyright 2019 Anthony Calandra
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <array>

constexpr size_t TOTAL_BYTECODE_INSTRUCTIONS = 205;

enum class bytecode_tag : uint8_t
{
    NOP = 0, ACONST_NULL, ICONST_M1, ICONST_0, ICONST_1, ICONST_2, ICONST_3, ICONST_4, ICONST_5,
    LCONST_0, LCONST_1, FCONST_0, FCONST_1, FCONST_2, DCONST_0, DCONST_1, BIPUSH, SIPUSH, LDC,
    LDC_W, LDC2_W, ILOAD, LLOAD, FLOAD, DLOAD, ALOAD, ILOAD_0, ILOAD_1, ILOAD_2, ILOAD_3, LLOAD_0,
    LLOAD_1, LLOAD_2, LLOAD_3, FLOAD_0, FLOAD_1, FLOAD_2, FLOAD_3, DLOAD_0, DLOAD_1, DLOAD_2,
    DLOAD_3, ALOAD_0, ALOAD_1, ALOAD_2, ALOAD_3, IALOAD, LALOAD, FALOAD, DALOAD, AALOAD, BALOAD,
    CALOAD, SALOAD, ISTORE, LSTORE, FSTORE, DSTORE, ASTORE, ISTORE_0, ISTORE_1, ISTORE_2, ISTORE_3,
    LSTORE_0, LSTORE_1, LSTORE_2, LSTORE_3, FSTORE_0, FSTORE_1, FSTORE_2, FSTORE_3, DSTORE_0,
    DSTORE_1, DSTORE_2, DSTORE_3, ASTORE_0, ASTORE_1, ASTORE_2, ASTORE_3, IASTORE, LASTORE,
    FASTORE, DASTORE, AASTORE, BASTORE, CASTORE, SASTORE, POP, POP2, DUP, DUP_X1, DUP_X2, DUP2,
    DUP2_X1, DUP2_X2, SWAP, IADD, LADD, FADD, DADD, ISUB, LSUB, FSUB, DSUB, IMUL, LMUL, FMUL, DMUL,
    IDIV, LDIV, FDIV, DDIV, IREM, LREM, FREM, DREM, INEG, LNEG, FNEG, DNEG, ISHL, LSHL, ISHR, LSHR,
    IUSHR, LUSHR, IAND, LAND, IOR, LOR, IXOR, LXOR, IINC, I2L, I2F, I2D, L2I, L2F, L2D, F2I, F2L,
    F2D, D2I, D2L, D2F, I2B, I2C, I2S, LCMP, FCMPL, FCMPG, DCMPL, DCMPG, IFEQ, IFNE, IFLT, IFGE,
    IFGT, IFLE, IF_ICMPEQ, IF_ICMPNE, IF_ICMPLT, IF_ICMPGE, IF_ICMPGT, IF_ICMPLE, IF_ACMPEQ,
    IF_ACMPNE, GOTO, JSR, RET, TABLESWITCH, LOOKUPSWITCH, IRETURN, LRETURN, FRETURN, DRETURN,
    ARETURN, RETURN, GETSTATIC, PUTSTATIC, GETFIELD, PUTFIELD, INVOKEVIRTUAL, INVOKESPECIAL,
    INVOKESTATIC, INVOKEINTERFACE, INVOKEDYNAMIC, NEW, NEWARRAY, ANEWARRAY, ARRAYLENGTH, ATHROW,
    CHECKCAST, INSTANCEOF, MONITORENTER, MONITOREXIT, WIDE, MULTIANEWARRAY, IFNULL, IFNONNULL,
    GOTO_W, JSR_W, BREAKPOINT, IMPDEP1, IMPDEP2
};

// These are all required to be the same size because in other parts of the code we assume indices
// for every table are commutative. e.g. See `find_instructions_cb`.
static_assert(static_cast<size_t>(bytecode_tag::IMPDEP2) + 1 == TOTAL_BYTECODE_INSTRUCTIONS,
    "Bytecode tables differ!");

constexpr std::array<const char* const, TOTAL_BYTECODE_INSTRUCTIONS> instruction_names =
{
    "nop", "aconst_null", "iconst_m1", "iconst_0", "iconst_1",
    "iconst_2", "iconst_3", "iconst_4", "iconst_5", "lconst_0", "lconst_1", "fconst_0", "fconst_1",
    "fconst_2", "dconst_0", "dconst_1", "bipush", "sipush", "ldc", "ldc_w", "ldc2_w",
    "iload", "lload", "fload", "dload", "aload", "iload_0", "iload_1", "iload_2", "iload_3",
    "lload_0", "lload_1", "lload_2", "lload_3", "fload_0", "fload_1", "fload_2", "fload_3",
    "dload_0", "dload_1", "dload_2", "dload_3", "aload_0", "aload_1", "aload_2", "aload_3",
    "iaload", "laload", "faload", "daload", "aaload", "baload", "caload", "saload", "istore",
    "lstore", "fstore", "dstore", "astore", "istore_0", "istore_1", "istore_2", "istore_3",
    "lstore_0", "lstore_1", "lstore_2", "lstore_3", "fstore_0", "fstore_1", "fstore_2", "fstore_3",
    "dstore_0", "dstore_1", "dstore_2", "dstore_3", "astore_0", "astore_1", "astore_2", "astore_3",
    "iastore", "lastore", "fastore", "dastore", "aastore", "bastore", "castore", "sastore", "pop",
    "pop2", "dup", "dup_x1", "dup_x2", "dup2", "dup2_x1", "dup2_x2", "swap", "iadd", "ladd",
    "fadd", "dadd", "isub", "lsub", "fsub", "dsub", "imul", "lmul", "fmul", "dmul", "idiv", "ldiv",
    "fdiv", "ddiv", "irem", "lrem", "frem", "drem", "ineg", "lneg", "fneg", "dneg", "ishl", "lshl",
    "ishr", "lshr", "iushr", "lushr", "iand", "land", "ior", "lor", "ixor", "lxor", "iinc", "i2l",
    "i2f", "i2d", "l2i", "l2f", "l2d", "f2i", "f2l", "f2d", "d2i", "d2l", "d2f", "i2b", "i2c",
    "i2s", "lcmp", "fcmpl", "fcmpg", "dcmpl", "dcmpg", "ifeq", "ifne", "iflt", "ifge", "ifgt",
    "ifle", "if_icmpeq", "if_icmpne", "if_icmplt", "if_icmpge", "if_icmpgt", "if_icmple",
    "if_acmpeq", "if_acmpne", "goto", "jsr", "ret", "tableswitch", "lookupswitch", "ireturn",
    "lreturn", "freturn", "dreturn", "areturn", "return", "getstatic", "putstatic", "getfield",
    "putfield", "invokevirtual", "invokespecial", "invokestatic", "invokeinterface",
    "invokedynamic", "new", "newarray", "anewarray", "arraylength", "athrow", "checkcast",
    "instanceof", "monitorenter", "monitorexit", "wide", "multianewarray", "ifnull", "ifnonnull",
    "goto_w", "jsr_w", "breakpoint", "impdep1", "impdep2"
};

constexpr std::array<size_t, TOTAL_BYTECODE_INSTRUCTIONS> instruction_sizes =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 0, 0, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 5, 5, 3, 2, 3, 1, 1,
    3, 3, 1, 1, 1, 4, 3, 3, 5, 5, 1, 1, 1
};

constexpr size_t get_instruction_size(bytecode_tag instr)
{
    return instruction_sizes[static_cast<size_t>(instr)];
}

constexpr const char* const get_instruction_name(bytecode_tag instr)
{
    return instruction_names[static_cast<size_t>(instr)];
}
