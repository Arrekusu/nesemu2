#pragma once
#include "nes_cpu.h"

opEntry opsTable[] = {
        {"BRK", nes_addr_mode::IMP, 1, 0, 7}, //0x0, 0
        {"ORA", nes_addr_mode::INDX, 2, 0, 6}, //0x1, 1
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x2, 2
        {"SLO", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x3, 3
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x4, 4
        {"ORA", nes_addr_mode::ZP, 2, 0, 3}, //0x5, 5
        {"ASL", nes_addr_mode::ZP, 2, 0, 5}, //0x6, 6
        {"SLO", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x7, 7
        {"PHP", nes_addr_mode::IMP, 1, 0, 3}, //0x8, 8
        {"ORA", nes_addr_mode::IMD, 2, 0, 2}, //0x9, 9
        {"ASL", nes_addr_mode::ACC, 1, 0, 2}, //0xA, 10
        {"ANC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xB, 11
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xC, 12
        {"ORA", nes_addr_mode::ABS, 3, 1, 4}, //0xD, 13
        {"ASL", nes_addr_mode::ABS, 3, 1, 6}, //0xE, 14
        {"SLO", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xF, 15
        {"BPL", nes_addr_mode::REL, 2, 0, 2}, //0x10, 16
        {"ORA", nes_addr_mode::INDY, 2, 0, 5}, //0x11, 17
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x12, 18
        {"SLO", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x13, 19
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x14, 20
        {"ORA", nes_addr_mode::ZPX, 2, 0, 4}, //0x15, 21
        {"ASL", nes_addr_mode::ZPX, 2, 0, 6}, //0x16, 22
        {"SLO", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x17, 23
        {"CLC", nes_addr_mode::IMP, 1, 0, 2}, //0x18, 24
        {"ORA", nes_addr_mode::ABSY, 3, 0, 4}, //0x19, 25
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x1A, 26
        {"SLO", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x1B, 27
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x1C, 28
        {"ORA", nes_addr_mode::ABSX, 3, 0, 4}, //0x1D, 29
        {"ASL", nes_addr_mode::ABSX, 3, 0, 7}, //0x1E, 30
        {"SLO", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x1F, 31
        {"JSR", nes_addr_mode::ABS, 3, 0, 6}, //0x20, 32
        {"AND", nes_addr_mode::INDX, 2, 0, 6}, //0x21, 33
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x22, 34
        {"RLA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x23, 35
        {"BIT", nes_addr_mode::ZP, 2, 0, 3}, //0x24, 36
        {"AND", nes_addr_mode::ZP, 2, 0, 3}, //0x25, 37
        {"ROL", nes_addr_mode::ZP, 2, 0, 5}, //0x26, 38
        {"RLA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x27, 39
        {"PLP", nes_addr_mode::IMP, 1, 0, 4}, //0x28, 40
        {"AND", nes_addr_mode::IMD, 2, 0, 2}, //0x29, 41
        {"ROL", nes_addr_mode::ACC, 1, 0, 2}, //0x2A, 42
        {"ANC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x2B, 43
        {"BIT", nes_addr_mode::ABS, 3, 1, 4}, //0x2C, 44
        {"AND", nes_addr_mode::ABS, 3, 1, 4}, //0x2D, 45
        {"ROL", nes_addr_mode::ABS, 3, 1, 6}, //0x2E, 46
        {"RLA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x2F, 47
        {"BMI", nes_addr_mode::REL, 2, 0, 2}, //0x30, 48
        {"AND", nes_addr_mode::INDY, 2, 0, 5}, //0x31, 49
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x32, 50
        {"RLA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x33, 51
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x34, 52
        {"AND", nes_addr_mode::ZPX, 2, 0, 4}, //0x35, 53
        {"ROL", nes_addr_mode::ZPX, 2, 0, 6}, //0x36, 54
        {"RLA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x37, 55
        {"SEC", nes_addr_mode::IMP, 1, 0, 2}, //0x38, 56
        {"AND", nes_addr_mode::ABSY, 3, 0, 4}, //0x39, 57
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x3A, 58
        {"RLA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x3B, 59
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x3C, 60
        {"AND", nes_addr_mode::ABSX, 3, 0, 4}, //0x3D, 61
        {"ROL", nes_addr_mode::ABSX, 3, 0, 7}, //0x3E, 62
        {"RLA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x3F, 63
        {"RTI", nes_addr_mode::IMP, 1, 0, 6}, //0x40, 64
        {"EOR", nes_addr_mode::INDX, 2, 0, 6}, //0x41, 65
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x42, 66
        {"SRE", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x43, 67
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x44, 68
        {"EOR", nes_addr_mode::ZP, 2, 0, 3}, //0x45, 69
        {"LSR", nes_addr_mode::ZP, 2, 0, 5}, //0x46, 70
        {"SRE", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x47, 71
        {"PHA", nes_addr_mode::IMP, 1, 0, 3}, //0x48, 72
        {"EOR", nes_addr_mode::IMD, 2, 0, 2}, //0x49, 73
        {"LSR", nes_addr_mode::ACC, 1, 0, 2}, //0x4A, 74
        {"ALR", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x4B, 75
        {"JMP", nes_addr_mode::ABS, 3, 0, 3}, //0x4C, 76
        {"EOR", nes_addr_mode::ABS, 3, 1, 4}, //0x4D, 77
        {"LSR", nes_addr_mode::ABS, 3, 1, 6}, //0x4E, 78
        {"SRE", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x4F, 79
        {"BVC", nes_addr_mode::REL, 2, 0, 2}, //0x50, 80
        {"EOR", nes_addr_mode::INDY, 2, 0, 5}, //0x51, 81
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x52, 82
        {"SRE", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x53, 83
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x54, 84
        {"EOR", nes_addr_mode::ZPX, 2, 0, 4}, //0x55, 85
        {"LSR", nes_addr_mode::ZPX, 2, 0, 6}, //0x56, 86
        {"SRE", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x57, 87
        {"CLI", nes_addr_mode::IMP, 1, 0, 2}, //0x58, 88
        {"EOR", nes_addr_mode::ABSY, 3, 0, 4}, //0x59, 89
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x5A, 90
        {"SRE", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x5B, 91
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x5C, 92
        {"EOR", nes_addr_mode::ABSX, 3, 0, 4}, //0x5D, 93
        {"LSR", nes_addr_mode::ABSX, 3, 0, 7}, //0x5E, 94
        {"SRE", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x5F, 95
        {"RTS", nes_addr_mode::IMP, 1, 0, 6}, //0x60, 96
        {"ADC", nes_addr_mode::INDX, 2, 0, 6}, //0x61, 97
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x62, 98
        {"RRA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x63, 99
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x64, 100
        {"ADC", nes_addr_mode::ZP, 2, 0, 3}, //0x65, 101
        {"ROR", nes_addr_mode::ZP, 2, 0, 5}, //0x66, 102
        {"RRA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x67, 103
        {"PLA", nes_addr_mode::IMP, 1, 0, 4}, //0x68, 104
        {"ADC", nes_addr_mode::IMD, 2, 0, 2}, //0x69, 105
        {"ROR", nes_addr_mode::ACC, 1, 0, 2}, //0x6A, 106
        {"ARR", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x6B, 107
        {"JMP", nes_addr_mode::IND, 3, 0, 5}, //0x6C, 108
        {"ADC", nes_addr_mode::ABS, 3, 1, 4}, //0x6D, 109
        {"ROR", nes_addr_mode::ABS, 3, 1, 6}, //0x6E, 110
        {"RRA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x6F, 111
        {"BVS", nes_addr_mode::REL, 2, 0, 2}, //0x70, 112
        {"ADC", nes_addr_mode::INDY, 2, 0, 5}, //0x71, 113
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x72, 114
        {"RRA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x73, 115
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x74, 116
        {"ADC", nes_addr_mode::ZPX, 2, 0, 4}, //0x75, 117
        {"ROR", nes_addr_mode::ZPX, 2, 0, 6}, //0x76, 118
        {"RRA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x77, 119
        {"SEI", nes_addr_mode::IMP, 1, 0, 2}, //0x78, 120
        {"ADC", nes_addr_mode::ABSY, 3, 0, 4}, //0x79, 121
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x7A, 122
        {"RRA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x7B, 123
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x7C, 124
        {"ADC", nes_addr_mode::ABSX, 3, 0, 4}, //0x7D, 125
        {"ROR", nes_addr_mode::ABSX, 3, 0, 7}, //0x7E, 126
        {"RRA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x7F, 127
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x80, 128
        {"STA", nes_addr_mode::INDX, 2, 1, 6}, //0x81, 129
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x82, 130
        {"SAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x83, 131
        {"STY", nes_addr_mode::ZP, 2, 1, 3}, //0x84, 132
        {"STA", nes_addr_mode::ZP, 2, 1, 3}, //0x85, 133
        {"STX", nes_addr_mode::ZP, 2, 1, 3}, //0x86, 134
        {"SAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x87, 135
        {"DEY", nes_addr_mode::IMP, 1, 0, 2}, //0x88, 136
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x89, 137
        {"TXA", nes_addr_mode::IMP, 1, 0, 2}, //0x8A, 138
        {"XAA", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x8B, 139
        {"STY", nes_addr_mode::ABS, 3, 1, 4}, //0x8C, 140
        {"STA", nes_addr_mode::ABS, 3, 1, 4}, //0x8D, 141
        {"STX", nes_addr_mode::ABS, 3, 1, 4}, //0x8E, 142
        {"SAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x8F, 143
        {"BCC", nes_addr_mode::REL, 2, 0, 2}, //0x90, 144
        {"STA", nes_addr_mode::INDY, 2, 1, 6}, //0x91, 145
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x92, 146
        {"AHX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x93, 147
        {"STY", nes_addr_mode::ZPX, 2, 1, 4}, //0x94, 148
        {"STA", nes_addr_mode::ZPX, 2, 1, 4}, //0x95, 149
        {"STX", nes_addr_mode::ZPY, 2, 1, 4}, //0x96, 150
        {"SAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x97, 151
        {"TYA", nes_addr_mode::IMP, 1, 0, 2}, //0x98, 152
        {"STA", nes_addr_mode::ABSY, 3, 1, 5}, //0x99, 153
        {"TXS", nes_addr_mode::IMP, 1, 0, 2}, //0x9A, 154
        {"TAS", nes_addr_mode::IMP, 1, 0, 2}, //0x9B, 155
        {"SHY", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x9C, 156
        {"STA", nes_addr_mode::ABSX, 3, 1, 5}, //0x9D, 157
        {"SHX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x9E, 158
        {"AHX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0x9F, 159
        {"LDY", nes_addr_mode::IMD, 2, 1, 2}, //0xA0, 160
        {"LDA", nes_addr_mode::INDX, 2, 1, 6}, //0xA1, 161
        {"LDX", nes_addr_mode::IMD, 2, 1, 2}, //0xA2, 162
        {"LAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xA3, 163
        {"LDY", nes_addr_mode::ZP, 2, 1, 3}, //0xA4, 164
        {"LDA", nes_addr_mode::ZP, 2, 1, 3}, //0xA5, 165
        {"LDX", nes_addr_mode::ZP, 2, 1, 3}, //0xA6, 166
        {"LAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xA7, 167
        {"TAY", nes_addr_mode::IMP, 1, 0, 2}, //0xA8, 168
        {"LDA", nes_addr_mode::IMD, 2, 1, 2}, //0xA9, 169
        {"TAX", nes_addr_mode::IMP, 1, 0, 2}, //0xAA, 170
        {"LAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xAB, 171
        {"LDY", nes_addr_mode::ABS, 3, 1, 4}, //0xAC, 172
        {"LDA", nes_addr_mode::ABS, 3, 1, 4}, //0xAD, 173
        {"LDX", nes_addr_mode::ABS, 3, 1, 4}, //0xAE, 174
        {"LAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xAF, 175
        {"BCS", nes_addr_mode::REL, 2, 0, 2}, //0xB0, 176
        {"LDA", nes_addr_mode::INDY, 2, 1, 5}, //0xB1, 177
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xB2, 178
        {"LAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xB3, 179
        {"LDY", nes_addr_mode::ZPX, 2, 1, 4}, //0xB4, 180
        {"LDA", nes_addr_mode::ZPX, 2, 1, 4}, //0xB5, 181
        {"LDX", nes_addr_mode::ZPY, 2, 1, 4}, //0xB6, 182
        {"LAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xB7, 183
        {"CLV", nes_addr_mode::IMP, 1, 0, 2}, //0xB8, 184
        {"LDA", nes_addr_mode::ABSY, 3, 1, 4}, //0xB9, 185
        {"TSX", nes_addr_mode::IMP, 1, 0, 2}, //0xBA, 186
        {"LAS", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xBB, 187
        {"LDY", nes_addr_mode::ABSX, 3, 1, 4}, //0xBC, 188
        {"LDA", nes_addr_mode::ABSX, 3, 1, 4}, //0xBD, 189
        {"LDX", nes_addr_mode::ABSY, 3, 1, 4}, //0xBE, 190
        {"LAX", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xBF, 191
        {"CPY", nes_addr_mode::IMD, 2, 0, 2}, //0xC0, 192
        {"CMP", nes_addr_mode::INDX, 2, 0, 6}, //0xC1, 193
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xC2, 194
        {"DCP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xC3, 195
        {"CPY", nes_addr_mode::ZP, 2, 0, 3}, //0xC4, 196
        {"CMP", nes_addr_mode::ZP, 2, 0, 3}, //0xC5, 197
        {"DEC", nes_addr_mode::ZP, 2, 0, 5}, //0xC6, 198
        {"DCP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xC7, 199
        {"INY", nes_addr_mode::IMP, 1, 0, 2}, //0xC8, 200
        {"CMP", nes_addr_mode::IMD, 2, 0, 2}, //0xC9, 201
        {"DEX", nes_addr_mode::IMP, 1, 0, 2}, //0xCA, 202
        {"AXS", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xCB, 203
        {"CPY", nes_addr_mode::ABS, 3, 1, 4}, //0xCC, 204
        {"CMP", nes_addr_mode::ABS, 3, 1, 4}, //0xCD, 205
        {"DEC", nes_addr_mode::ABS, 3, 1, 6}, //0xCE, 206
        {"DCP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xCF, 207
        {"BNE", nes_addr_mode::REL, 2, 0, 2}, //0xD0, 208
        {"CMP", nes_addr_mode::INDY, 2, 0, 5}, //0xD1, 209
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xD2, 210
        {"DCP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xD3, 211
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xD4, 212
        {"CMP", nes_addr_mode::ZPX, 2, 0, 4}, //0xD5, 213
        {"DEC", nes_addr_mode::ZPX, 2, 0, 6}, //0xD6, 214
        {"DCP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xD7, 215
        {"CLD", nes_addr_mode::IMP, 1, 0, 2}, //0xD8, 216
        {"CMP", nes_addr_mode::ABSY, 3, 0, 4}, //0xD9, 217
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xDA, 218
        {"DCP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xDB, 219
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xDC, 220
        {"CMP", nes_addr_mode::ABSX, 3, 0, 4}, //0xDD, 221
        {"DEC", nes_addr_mode::ABSX, 3, 0, 7}, //0xDE, 222
        {"DCP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xDF, 223
        {"CPX", nes_addr_mode::IMD, 2, 0, 2}, //0xE0, 224
        {"SBC", nes_addr_mode::INDX, 2, 0, 6}, //0xE1, 225
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xE2, 226
        {"ISC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xE3, 227
        {"CPX", nes_addr_mode::ZP, 2, 0, 3}, //0xE4, 228
        {"SBC", nes_addr_mode::ZP, 2, 0, 3}, //0xE5, 229
        {"INC", nes_addr_mode::ZP, 2, 0, 5}, //0xE6, 230
        {"ISC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xE7, 231
        {"INX", nes_addr_mode::IMP, 1, 0, 2}, //0xE8, 232
        {"SBC", nes_addr_mode::IMD, 2, 0, 2}, //0xE9, 233
        {"NOP", nes_addr_mode::IMP, 1, 0, 2}, //0xEA, 234
        {"SBC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xEB, 235
        {"CPX", nes_addr_mode::ABS, 3, 1, 4}, //0xEC, 236
        {"SBC", nes_addr_mode::ABS, 3, 1, 4}, //0xED, 237
        {"INC", nes_addr_mode::ABS, 3, 1, 6}, //0xEE, 238
        {"ISC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xEF, 239
        {"BEQ", nes_addr_mode::REL, 2, 0, 2}, //0xF0, 240
        {"SBC", nes_addr_mode::INDY, 2, 0, 5}, //0xF1, 241
        {"STP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xF2, 242
        {"ISC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xF3, 243
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xF4, 244
        {"SBC", nes_addr_mode::ZPX, 2, 0, 4}, //0xF5, 245
        {"INC", nes_addr_mode::ZPX, 2, 0, 6}, //0xF6, 246
        {"ISC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xF7, 247
        {"SED", nes_addr_mode::IMP, 1, 0, 2}, //0xF8, 248
        {"SBC", nes_addr_mode::ABSY, 3, 0, 4}, //0xF9, 249
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xFA, 250
        {"ISC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xFB, 251
        {"NOP", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xFC, 252
        {"SBC", nes_addr_mode::ABSX, 3, 0, 4}, //0xFD, 253
        {"INC", nes_addr_mode::ABSX, 3, 0, 7}, //0xFE, 254
        {"ISC", nes_addr_mode::UNKNOWN, 0, 0, 0}, //0xFF, 255
};