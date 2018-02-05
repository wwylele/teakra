#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <malloc.h>

#include <fcntl.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cdc_bin.h"

void MoveCursor(unsigned row, unsigned col) {
    printf("\x1b[%u;%uH", row + 1, col + 1);
}

enum Color {
    Reset = 0,
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magnenta = 35,
    Cyan = 36,
    white = 37,
};
void SetColor(Color color) {
    printf("\x1b[%dm", (int)color);
}

class IReg {
public:
    IReg(const std::string& name, const std::string& flags): name(name), flags(flags) {}
    virtual ~IReg() = default;
    virtual unsigned GetLength() = 0;
    virtual unsigned GetSrcDigit(unsigned pos) = 0;
    virtual unsigned GetDstDigit(unsigned pos) = 0;
    virtual void SetSrcDigit(unsigned pos, unsigned value) = 0;
    virtual unsigned GetDigitRange() = 0;
    void Print(unsigned row, unsigned col, unsigned selected) {
        SetColor(Cyan);
        MoveCursor(row, col - name.size());
        printf("%s", name.c_str());
        unsigned len = GetLength();
        for (unsigned i = 0; i < len; ++i) {
            unsigned src = GetSrcDigit(len - i - 1);
            unsigned dst = GetDstDigit(len - i - 1);
            MoveCursor(row, col + i);
            SetColor(len - i - 1 == selected ? Yellow : Reset);
            if (flags.empty())
                printf("%X", src);
            else
                printf("%c", src ? flags[i] : '-');
            MoveCursor(row + 1, col + i);
            SetColor(src == dst ? Blue : Green);
            if (flags.empty())
                printf("%X", dst);
            else
                printf("%c", dst ? flags[i] : '-');
        }
    }
private:
    std::string name, flags;
};

class HexReg : public IReg{
public:
    HexReg(const std::string& name, u16& src, u16& dst) : IReg(name, ""), src(src), dst(dst) {}
    unsigned GetLength() override {
        return 4;
    }
    unsigned GetSrcDigit(unsigned pos) override {
        return (src >> (pos * 4)) & 0xF;
    }
    unsigned GetDstDigit(unsigned pos) override {
        return (dst >> (pos * 4)) & 0xF;
    }
    void SetSrcDigit(unsigned pos, unsigned value) override {
        src &= ~(0xF << (pos * 4));
        src |= value << (pos * 4);
    }
    unsigned GetDigitRange() override {
        return 16;
    }
private:
    u16& src;
    u16& dst;
};

class BinReg : public IReg{
public:
    BinReg(const std::string& name, u16& src, u16& dst, const std::string flags) : IReg(name, flags), src(src), dst(dst) {}
    unsigned GetLength() override {
        return 16;
    }
    unsigned GetSrcDigit(unsigned pos) override {
        return (src >> pos) & 1;
    }
    unsigned GetDstDigit(unsigned pos) override {
        return (dst >> pos) & 1;
    }
    void SetSrcDigit(unsigned pos, unsigned value) override {
        src &= ~(1 << pos);
        src |= value << pos;
    }
    unsigned GetDigitRange() override {
        return 2;
    }
private:
    u16& src;
    u16& dst;
};

u16* dspP = (u16*)0x1FF00000;
u16* dspD = (u16*)0x1FF40000;

u16* srcBase = &dspD[0x2000];
u16* dstBase = &dspD[0x2FD0];

IReg* MakeHexReg(const std::string& name, unsigned offset) {
    return new HexReg(name, srcBase[offset], dstBase[offset]);
}

IReg* MakeBinReg(const std::string& name, unsigned offset, const std::string flags = "") {
    return new BinReg(name, srcBase[offset], dstBase[offset], flags);
}

constexpr unsigned t_row = 15;
constexpr unsigned t_col = 4;

IReg* grid[t_row][t_col] = {};

unsigned c_row = 0, c_col = 0, c_pos = 0;

void PrintGrid(unsigned row, unsigned col) {
    IReg* r = grid[row][col];
    if(!r)
        return;
    r->Print(row * 2, 4 + col * 9, row == c_row && col == c_col ? c_pos : 0xFFFFFFFF);
}

void PrintAll() {
    for (unsigned row = 0; row < t_row; ++row)
        for (unsigned col = 0; col < t_col; ++col)
            PrintGrid(row, col);
}

void FlushCache(void* ptr, u32 size) {
    svcFlushProcessDataCache(CUR_PROCESS_HANDLE, ptr, size);
}

void InvalidateCache(void* ptr, u32 size) {
    svcInvalidateProcessDataCache(CUR_PROCESS_HANDLE, ptr, size);
}

void StartDspProgram() {
    dspD[1] = 1;
    FlushCache(&dspD[1], 2);
    while(dspD[1])InvalidateCache(&dspD[1], 2);
}

void StopDspProgram() {
    dspD[2] = 1;
    FlushCache(&dspD[2], 2);
    while(dspD[2])InvalidateCache(&dspD[2], 2);
}

void UploadDspProgram(const std::vector<u16>& code) {
    StopDspProgram();
    InvalidateCache(&dspP[0x2000], 0x2000);

    memcpy(&dspP[0x2000], code.data(), code.size() * 2);
    unsigned end = 0x2000 + code.size();
    dspP[end + 0] = 0x0000; // nop
    dspP[end + 1] = 0x0000; // nop
    dspP[end + 2] = 0x0000; // nop
    dspP[end + 3] = 0x4180; // br 0x1800
    dspP[end + 4] = 0x1800;

    FlushCache(&dspP[0x2000], 0x2000);
    StartDspProgram();
}

int udp_s;

void UdpInit() {
    #define SOC_ALIGN       0x1000
    #define SOC_BUFFERSIZE  0x100000
    static u32 *SOC_buffer;
    // allocate buffer for SOC service
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    if(SOC_buffer == NULL) {
        printf("memalign: failed to allocate\n");
        return;
    }

    Result ret;
    if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
        printf("socInit: 0x%08lX\n", ret);
        return;
    }

    sockaddr_in si_me;

    //create a UDP socket
    if((udp_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("socket() failed");
        return;
    }

    // zero out the structure
    memset(&si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(8888);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if(bind(udp_s, (sockaddr*)&si_me, sizeof(si_me)) == -1) {
        printf("bind() failed");
        return;
    }
}

std::vector<u16> CheckProgramPackage() {
    constexpr unsigned BUFLEN = 512;
    char buf[BUFLEN];
    sockaddr_in si_other;
    socklen_t slen = sizeof(si_other);
    int recv_len;
    if((recv_len = recvfrom(udp_s, buf, BUFLEN, MSG_DONTWAIT, (sockaddr*)&si_other, &slen)) < 4)
        return {};
    u16 magic;
    memcpy(&magic, buf, 2);
    if (magic != 0xD590)
        return {};
    std::vector<u16> program_package((recv_len - 2) / 2);
    memcpy(program_package.data(), buf + 2, program_package.size() * 2);
    return program_package;
}

int main() {
    aptInit();
    gfxInitDefault();

    PrintConsole topScreen, bottomScreen;
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    consoleSelect(&bottomScreen);
    printf("Hello!\n");

    UdpInit();
    //LightLock_Init(&udp_lock);
    //udp_thread = threadCreate(UdpThreadEntry, 0, (4 * 1024), 24, -2, false);

    printf("dspInit: %08lX\n", dspInit());
    bool loaded = false;
    printf("DSP_LoadComponent: %08lX\n", DSP_LoadComponent(cdc_bin, cdc_bin_size, 0xFF, 0xFF, &loaded));
    printf("loaded = %d\n", loaded);
    consoleSelect(&topScreen);

    grid[7][0] = MakeHexReg("r7", 0);
    grid[6][0] = MakeHexReg("r6", 1);
    grid[5][0] = MakeHexReg("r5", 2);
    grid[4][0] = MakeHexReg("r4", 3);
    grid[3][0] = MakeHexReg("r3", 4);
    grid[2][0] = MakeHexReg("r2", 5);
    grid[1][0] = MakeHexReg("r1", 6);
    grid[0][0] = MakeHexReg("r0", 7);

    grid[6][1] = MakeHexReg("mixp", 8);
    grid[7][1] = MakeHexReg("repc", 9);
    grid[5][1] = MakeHexReg("stj0", 0xA);
    grid[5][2] = MakeHexReg("sti0", 0xB);
    grid[7][2] = MakeHexReg("lc", 0xC);

    grid[3][1] = MakeHexReg("p1h", 0xD);
    grid[3][2] = MakeHexReg("p1l", 0xE);
    grid[2][2] = MakeHexReg("y1", 0xF);
    grid[2][1] = MakeHexReg("x1", 0x10);
    grid[1][1] = MakeHexReg("p0h", 0x11);
    grid[1][2] = MakeHexReg("p0l", 0x12);
    grid[0][2] = MakeHexReg("y0", 0x13);
    grid[0][1] = MakeHexReg("x0", 0x14);

    grid[6][2] = MakeHexReg("sv", 0x15);

    grid[12][1] = MakeHexReg("b1h", 0x16);
    grid[12][2] = MakeHexReg("b1l", 0x17);
    grid[12][0] = MakeHexReg("b1e", 0x18);
    grid[11][1] = MakeHexReg("b0h", 0x19);
    grid[11][2] = MakeHexReg("b0l", 0x1A);
    grid[11][0] = MakeHexReg("b0e", 0x1B);
    grid[10][1] = MakeHexReg("a1h", 0x1C);
    grid[10][2] = MakeHexReg("a1l", 0x1D);
    grid[10][0] = MakeHexReg("a1e", 0x1E);
    grid[9][1] = MakeHexReg("a0h", 0x1F);
    grid[9][2] = MakeHexReg("a0l", 0x20);
    grid[9][0] = MakeHexReg("a0e", 0x21);

    grid[13][3] = MakeBinReg("arp3", 0x22, "#RR#RRiiiiijjjjj");
    grid[12][3] = MakeBinReg("arp2", 0x23, "#RR#RRiiiiijjjjj");
    grid[11][3] = MakeBinReg("arp1", 0x24, "#RR#RRiiiiijjjjj");
    grid[10][3] = MakeBinReg("arp0", 0x25, "#RR#RRiiiiijjjjj");
    grid[9][3] = MakeBinReg("ar1", 0x26, "RRRRRRoosssoosss");
    grid[8][3] = MakeBinReg("ar0", 0x27, "RRRRRRoosssoosss");
    grid[7][3] = MakeBinReg("mod2", 0x28, "7654321m7654321M");
    grid[6][3] = MakeBinReg("mod1", 0x29, "???B####pppppppp");
    grid[5][3] = MakeBinReg("mod0", 0x2A, "#QQ#PPooSYY###SS");
    grid[4][3] = MakeBinReg("stt2", 0x2B, "LBBB####mm##V21I");
    grid[3][3] = MakeBinReg("stt1", 0x2C, "QP#########R####");
    grid[2][3] = MakeBinReg("stt0", 0x2D, "####C###ZMNVCELL");
    grid[1][3] = MakeBinReg("cfgj", 0x2E, "mmmmmmmmmsssssss");
    grid[0][3] = MakeBinReg("cfgi", 0x2F, "mmmmmmmmmsssssss");

    // Main loop
    while (aptMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break; // break in order to return to hbmenu

        if (kDown & KEY_DOWN) {
            for (int next = (int)c_row + 1; next < (int)t_row; ++next) {
                if (grid[next][c_col]) {
                    c_row = next;
                    break;
                }
            }
        }

        if (kDown & KEY_UP) {
            for (int next = (int)c_row - 1; next >= 0; --next) {
                if (grid[next][c_col]) {
                    c_row = next;
                    break;
                }
            }
        }

        if (kDown & KEY_LEFT) {
            if(c_pos == grid[c_row][c_col]->GetLength() - 1) {
                for (int next = (int)c_col - 1; next >= 0; --next) {
                    if (grid[c_row][next]) {
                        c_col = next;
                        c_pos = 0;
                        break;
                    }
                }
            } else {
                ++c_pos;
            }
        }

        if (kDown & KEY_RIGHT) {
            if(c_pos == 0) {
                for (int next = (int)c_col + 1; next < (int)t_col; ++next) {
                    if (grid[c_row][next]) {
                        c_col = next;
                        c_pos = grid[c_row][c_col]->GetLength() - 1;
                        break;
                    }
                }
            } else {
                --c_pos;
            }
        }

        if (kDown & KEY_A) {
            unsigned v = grid[c_row][c_col]->GetSrcDigit(c_pos);
            ++v;
            if (v == grid[c_row][c_col]->GetDigitRange()) v = 0;
            grid[c_row][c_col]->SetSrcDigit(c_pos, v);
        }

        if (kDown & KEY_B) {
            unsigned v = grid[c_row][c_col]->GetSrcDigit(c_pos);
            if (v == 0) v = grid[c_row][c_col]->GetDigitRange();
            --v;
            grid[c_row][c_col]->SetSrcDigit(c_pos, v);
        }

        if (kDown & KEY_X) {
            UploadDspProgram({0x86A0}); // add r0, a0
        }

        if (kDown & KEY_Y) {
           FlushCache(&dspD[0x3000], 0x40000 - 0x6000);
           for (u32 i = 0x3000; i < 0x20000; ++i) {
               dspD[i] = i >> 4;
           }
           InvalidateCache(&dspD[0x3000], 0x40000 - 0x6000);
        }

        FlushCache(&dspD[0x2000], 0x1000);
        InvalidateCache(&dspD[0x2000], 0x2000);
        PrintAll();

        auto program_package = CheckProgramPackage();

        if (program_package.size()) {
            consoleSelect(&bottomScreen);
            printf("--------\nNew program received!\n");
            for (u16 code : program_package) {
                printf("%04X ", code);
            }
            UploadDspProgram(program_package);
            printf("\nUploaded!\n");

            consoleSelect(&topScreen);
        }

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();

        //Wait for VBlank
        gspWaitForVBlank();
    }
    socExit();
    dspExit();
    gfxExit();
    aptExit();
    return 0;
}
