#include "benchmark/benchmark.h"
#include <iostream>

struct File { int len; const char* str; };

#define E_DSS           {13,  "dxsurface.dss"}
#define E_JPG           {13,  "jpegimage.jpg"}
#define E_PNG           {14,  "IHasAlpha!.png"}
#define E_MATERIAL      {17,  "ogre_mat.material"}
#define E_MESH          {17,  "ogreformat00.mesh"}
#define E_SOUNDSCRIPT   {17,  "yummy.soundscript"}
#define E_WAV           {17,  "listen-to-me!.wav"}
#define E_TRUCK         {13,  "awesome.truck"}
#define E_AIRPLANE      {17,  "superman.airplane"}
#define E_BOAT          {11,  "floaty.boat"}
#define E_MACHINE       {15,  "mystery.machine"}
#define E_LOAD          {18,  "SomewhatHeavy.load"}
#define E_FIXED         {12, "bug123.fixed"}
#define E_CAR           {13, "passenger.car"}
#define E__TERMINATOR__ {-1,  nullptr}

File filelist[] = {
    // ----- typical truck -----
    E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,
    E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,
    E_PNG,E_PNG,E_PNG,E_PNG,
    E_MATERIAL,E_MATERIAL,
    E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,
    E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,
    E_TRUCK,E_TRUCK,E_TRUCK,E_TRUCK,E_TRUCK,
    // ----- typical airplane -----
    E_DSS,E_DSS,
    E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,
    E_PNG,E_PNG,E_PNG,
    E_MATERIAL,
    E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,
    E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,
    E_AIRPLANE,E_AIRPLANE,E_AIRPLANE,
    // ----- typical boat -----
    E_DSS,E_DSS,E_DSS,
    E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,
    E_PNG,E_PNG,
    E_MATERIAL,
    E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,
    E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,
    E_BOAT,E_BOAT,
    // ----- typical car -----
    E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,E_DSS,
    E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,
    E_PNG,E_PNG,E_PNG,
    E_MATERIAL,
    E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,
    E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,
    E_CAR,E_CAR,E_CAR,E_CAR,
    // ----- typical machine -----
    E_DSS,E_DSS,
    E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,E_JPG,
    E_PNG,E_PNG,E_PNG,
    E_MATERIAL,
    E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,
    E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,E_WAV,
    E_MACHINE,
    // ----- typical load -----
    E_DSS,E_DSS,
    E_JPG,E_JPG,E_JPG,E_JPG,
    E_PNG,E_PNG,E_PNG,
    E_MATERIAL,
    E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,E_MESH,
    E_WAV,E_WAV,E_WAV,
    E_LOAD,E_LOAD,
    // ---- typical fixed ----
    E_DSS,E_DSS,
    E_JPG,E_JPG,E_JPG,
    E_PNG,E_PNG,E_PNG,E_PNG,E_PNG,
    E_MATERIAL,
    E_MESH,E_MESH,E_MESH,
    E_WAV,
    E_FIXED,
    E__TERMINATOR__
};

static bool global_b;


// IDEA 1 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// param ext     = lowercase! must include dot! example: ".truck"
// param ext_len = including the dot!
inline bool CheckExtension(const char* ext, unsigned ext_len, const char* filename, unsigned num_chars)
{
    if (num_chars <= ext_len)
    {
        return false;
    }

    unsigned first_fi = num_chars - 1;
    unsigned last_fi  = num_chars - ext_len;
    unsigned ei = ext_len - 1;
    for (unsigned fi = first_fi; fi >= last_fi; --fi) // backwards!
    {
        if (filename[fi] != ext[ei] && filename[fi] != toupper(ext[ei])) { return false; }
        --ei;
    }
}

static void Bench_idea1_CheckExtensionForLoop(benchmark::State& state)
{
    while (state.KeepRunning()) 
    {
        int i = 0;
        while (filelist[i].len != -1) // terminator
        {
            global_b = CheckExtension( ".truck",    6, filelist[i].str, filelist[i].len)
                         || CheckExtension( ".airplane", 9, filelist[i].str, filelist[i].len)
                         || CheckExtension( ".machine",  8, filelist[i].str, filelist[i].len)
                         || CheckExtension( ".car",      4, filelist[i].str, filelist[i].len)
                         || CheckExtension( ".fixed",    6, filelist[i].str, filelist[i].len)
                         || CheckExtension( ".load",     5, filelist[i].str, filelist[i].len)
                         || CheckExtension( ".boat",     5, filelist[i].str, filelist[i].len);
            ++i;
        }
    }
}
BENCHMARK(Bench_idea1_CheckExtensionForLoop);

// IDEA 2 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Test one (C)haracter
#define CTEST(_OFFS_, _CHAR_)               (filename[len - (_OFFS_) - 1] == (_CHAR_))
// Test one (P)osition
#define PTEST(_OFFS_, _CHAR_a_, _CHAR_A_)   (CTEST((_OFFS_), (_CHAR_a_)) || CTEST((_OFFS_), (_CHAR_A_)))
// Test letters
#define Dot(_OFFS_)                         (CTEST((_OFFS_),'.'))
#define A(_OFFS_)                           (PTEST((_OFFS_),'a','A'))
#define B(_OFFS_)                           (PTEST((_OFFS_),'b','B'))
#define C(_OFFS_)                           (PTEST((_OFFS_),'c','C'))
#define D(_OFFS_)                           (PTEST((_OFFS_),'d','D'))
#define E(_OFFS_)                           (PTEST((_OFFS_),'e','E'))
#define F(_OFFS_)                           (PTEST((_OFFS_),'f','F'))
#define H(_OFFS_)                           (PTEST((_OFFS_),'h','H'))
#define I(_OFFS_)                           (PTEST((_OFFS_),'i','I'))
#define K(_OFFS_)                           (PTEST((_OFFS_),'k','K'))
#define L(_OFFS_)                           (PTEST((_OFFS_),'l','L'))
#define M(_OFFS_)                           (PTEST((_OFFS_),'m','M'))
#define N(_OFFS_)                           (PTEST((_OFFS_),'n','N'))
#define O(_OFFS_)                           (PTEST((_OFFS_),'o','O'))
#define P(_OFFS_)                           (PTEST((_OFFS_),'p','P'))
#define R(_OFFS_)                           (PTEST((_OFFS_),'r','R'))
#define T(_OFFS_)                           (PTEST((_OFFS_),'t','T'))
#define U(_OFFS_)                           (PTEST((_OFFS_),'u','U'))
#define X(_OFFS_)                           (PTEST((_OFFS_),'x','X'))

#define MK_CHECK_EXT_FN(_EXT_, _BODY_)  inline bool CheckExt##_EXT_(const char* filename, unsigned len) { _BODY_ }

// Length includes dot. Example: ".truck" = 6 chars

MK_CHECK_EXT_FN(Truck,   {return (len > 6) && Dot(5) && T(4) && R(3) && U(2) && C(1) && K(0)                        ;})
MK_CHECK_EXT_FN(Car,     {return (len > 4) && Dot(3) && C(2) && A(1) && R(0)                                        ;})
MK_CHECK_EXT_FN(Airplane,{return (len > 9) && Dot(8) && A(7) && I(6) && R(5) && P(4) && L(3) && A(2) && N(1) && E(0);})
MK_CHECK_EXT_FN(Boat,    {return (len > 5) && Dot(4) && B(3) && O(2) && A(1) && T(0)                                ;})
MK_CHECK_EXT_FN(Machine, {return (len > 8) && Dot(7) && M(6) && A(5) && C(4) && H(3) && I(2) && N(1) && E(0)        ;})
MK_CHECK_EXT_FN(Fixed,   {return (len > 6) && Dot(5) && F(4) && I(3) && X(2) && E(1) && D(0)                        ;})
MK_CHECK_EXT_FN(Load,    {return (len > 5) && Dot(4) && L(3) && O(2) && A(1) && D(0)                                ;})


MK_CHECK_EXT_FN(BitTruck,   {return (len > 6) & Dot(5) & T(4) & R(3) & U(2) & C(1) & K(0)                     ;})
MK_CHECK_EXT_FN(BitCar,     {return (len > 4) & Dot(3) & C(2) & A(1) & R(0)                                   ;})
MK_CHECK_EXT_FN(BitAirplane,{return (len > 9) & Dot(8) & A(7) & I(6) & R(5) & P(4) & L(3) & A(2) & N(1) & E(0);})
MK_CHECK_EXT_FN(BitBoat,    {return (len > 5) & Dot(4) & B(3) & O(2) & A(1) & T(0)                            ;})
MK_CHECK_EXT_FN(BitMachine, {return (len > 8) & Dot(7) & M(6) & A(5) & C(4) & H(3) & I(2) & N(1) & E(0)       ;})
MK_CHECK_EXT_FN(BitFixed,   {return (len > 6) & Dot(5) & F(4) & I(3) & X(2) & E(1) & D(0)                     ;})
MK_CHECK_EXT_FN(BitLoad,    {return (len > 5) & Dot(4) & L(3) & O(2) & A(1) & D(0)                            ;})


static void Bench_idea2_MacroMadness(benchmark::State& state)
{
    while (state.KeepRunning()) 
    {
        int i = 0;
        static int b;
        while (filelist[i].len != -1) // terminator
        {
            //static File entry = filelist[i];
            global_b = CheckExtTruck   ( filelist[i].str, filelist[i].len)
                         || CheckExtAirplane( filelist[i].str, filelist[i].len)
                         || CheckExtMachine ( filelist[i].str, filelist[i].len)
                         || CheckExtCar     ( filelist[i].str, filelist[i].len)
                         || CheckExtFixed   ( filelist[i].str, filelist[i].len)
                         || CheckExtLoad    ( filelist[i].str, filelist[i].len)
                         || CheckExtBoat    ( filelist[i].str, filelist[i].len)
                ;
            ++i;
        }
    }
}
BENCHMARK(Bench_idea2_MacroMadness);

static void Bench_idea2b_MacroMadnessBitAdd(benchmark::State& state)
{
    while (state.KeepRunning()) 
    {
        int i = 0;
        static int b;
        while (filelist[i].len != -1) // terminator
        {
            //static File entry = filelist[i];
            global_b = CheckExtBitTruck   ( filelist[i].str, filelist[i].len)
                    || CheckExtBitAirplane( filelist[i].str, filelist[i].len)
                    || CheckExtBitMachine ( filelist[i].str, filelist[i].len)
                    || CheckExtBitCar     ( filelist[i].str, filelist[i].len)
                    || CheckExtBitFixed   ( filelist[i].str, filelist[i].len)
                    || CheckExtBitLoad    ( filelist[i].str, filelist[i].len)
                    || CheckExtBitBoat    ( filelist[i].str, filelist[i].len)
                ;
            ++i;
        }
    }
}
BENCHMARK(Bench_idea2b_MacroMadnessBitAdd);

static void Bench_EmptyFilenameLoop(benchmark::State& state)
{
    while (state.KeepRunning()) 
    {
        int i = 0;
        static int b;
        while (filelist[i].len != -1) // terminator
        {
            global_b = (filelist[i].str[5] > 'f' || filelist[i].len > 14);
            ++i;
        }
    }
}
BENCHMARK(Bench_EmptyFilenameLoop);


// MAIN //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  int main(int argc, char** argv) {        
      // validate
      using namespace std;
      cout << "Validating CheckExtTruck   : " << ((CheckExtTruck("heavy.truck", 11) == true) ? "ok" : "FAILED" ) << endl;
      cout << "Validating CheckExtCar     : " << ((CheckExtCar("light.car",    9) == true) ? "ok" : "FAILED" ) << endl;
      cout << "Validating CheckExtAirplane: " << ((CheckExtAirplane("sky.airplane", 12) == true) ? "ok" : "FAILED" ) << endl;
      cout << "Validating CheckExtBoat    : " << ((CheckExtBoat("floaty.boat", 11) == true) ? "ok" : "FAILED" ) << endl;
      cout << "Validating CheckExtFixed   : " << ((CheckExtFixed("bug.fixed", 9) == true) ? "ok" : "FAILED" ) << endl;
      cout << "Validating CheckExtLoad    : " << ((CheckExtLoad("heavy.load", 10) == true) ? "ok" : "FAILED" ) << endl;
      cout << endl;


      // benchmark
    ::benchmark::Initialize(&argc, argv);  
    ::benchmark::RunSpecifiedBenchmarks(); 
    system("pause");
    return (int) global_b; // Without this, EmptyFilename BM returns 2ns (which means "optimized out")
  }