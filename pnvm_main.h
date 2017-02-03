/******************************************************************************
 * These are callbacks for pnvm_plugin to the main interpreter program
 * providing interaction with outer world.
 *
 * @author Michal Minar <xminar06@stud.fit.vutbr.cz>
 * @year 2013
 */

#ifndef PNVM_H
#define PNVM_H

#include "memblock_lib.h"
#ifdef PLATFORM_HEADER
    #include PLATFORM_HEADER
#else

    /**
     * Platform code as a dump of object memory.
     * This can be used to initialize netTemplateCode.
     * This saves the time on initialization by totally avoiding parsing
     * of platform code. Moreover it saves a lot of memory.
     *
     * Original platform code:
     *
     *      "(Nplatform"
     *          // symbols
     *          "(0," // initial address of platform instance
     *              // commands follow
     *              //       1  ,      2  ,    3  ,       4  ,     5  ,
     *              "\"setaddr\",\"pass\",\"load\",\"create\",\"dump\","
     *              //        6
     *              "\"loadinst\","
     *              // output strings
     *              //  7  ,       8  ,       9
     *              "\"ok\",\"failed\",\"badcmd\")"
     *          // places
     *          "(address,nets,idle,message,output)"
     *          // uplinks
     *          "(Uinput(m)(a,c)"       // (message) (address, command)
     *              "(P2,1,I1)"         // remove idle token
     *              "(P0,1,V1)"         // get our address
     *              // check the address, whether it belongs to us
     *              "(G(=(h(V0))(V1)))"
     *              "(A(:(V2)(t(V0))))" // command := tail(msg)
     *              "(O0,1,V1)"         // put address back
     *              "(O3,1,V2))"        // put command to message place
     *          "(Uoutput(x)()"
     *              "(P4,1,V0)"
     *              "(O2,1,I1))"        // return idle token
     *          // initializati on transition
     *          "(I(O0,1,S0)"           // set initial address
     *             "(O2,1,I1))"         // make us idle - accept messages
     *
     *          //transitions
     *          "(Tpass(n,c,m)"     // (net, command, message)
     *              "(P1,1,V0)"     // get any managed instance from nets
     *              "(P3,1,V1)"             // get message
     *              "(G(=(h(V1))(S2)))"     // check, whether cmd=="pass"
     *              // make a message for managed instance
     *              "(G(|(:(V2)(h(t(V1))))(I1)))"
     *              //call 'input' uplink of managed instance
     *              "(Dinput,V0,V2)"
     *              "(O1,1,V0))"        // return instance back
     *
     *          "(Tbadcmd(m,c)"         // (message, command)
     *              "(P3,1,V0)"
     *              "(G(!(|(|(|(=(:(V1)(h(V0)))(S1))"
     *                        "(=(V1)(S2)))"
     *                      "(|(=(V1)(S3))"
     *                        "(=(V1)(S4))))"
     *                    "(|(=(V1)(S5))"
     *                      "(=(V1)(S6))))))"
     *              "(O4,1,S9))"
     *
     *          "(Tout(n,m)"            // (net, message)
     *              "(P1,1,V0)"         // get any managed instance from nets
     *              //get any out put message from it
     *              "(Doutput,0,V1)"
     *              "(O1,1,V0)"         // put back the managed instance
     *              "(O4,1,V1))"        // write the message to output
     *
     *          "(Tdump(c,o)"           // (command, output)
     *              "(P3,1,V0)"         //get command
     *              "(G(=(h(V0))(S5)))" //check, whether cmd=="dump"
     *              "(A(:(V1)(d)))"     //out:=<dumpedplatforminstance>
     *              "(O4,1,V1))"        //write out to output place
     *
     *          "(Tload(c,r,o)"         // (command, result, output)
     *              "(P3,1,V0)"
     *              "(G(=(h(V0))(S3)))"
     *              //load net template and storeits id to out
     *              "(A(:(V1)(l(#(V0)(I1)))))"
     *              "(A(:(V2)(#(a(a(na)(S8))(S7))(V1))))"
     *              "(O4,1,V1))"
     *
     *          "(Tcreate(n,c)"         // (net, command)
     *              "(P3,1,V1)"
     *              "(G(=(h(V1))(S4)))"
     *              "(G(:(V0)(c(#(V1)(I1)))))"
     *              "(O4,1,S7)"
     *              "(O1,1,V0))"
     *
     *          "(Tloadinst(n,c,o)"     // (net, command, output)
     *              "(P3,1,V1)"
     *              "(G(=(h(V1))(S6)))"
     *              "(G(:(V0)(i(#(V1)(I1)))))"
     *              "(O4,1,S7)"
     *              "(O1,1,V0))"
     *
     *          "(Tsetaddr(c,o,a)"      // (command, old, address)
     *              "(P3,1,V0)"
     *              "(P0,1,V1)"
     *              "(G(=(h(V0))(S1)))"
     *              "(A(:(V2)(#(V0)(I1))))"
     *              "(O0,1,V2)"
     *              "(O4,1,S7))"
     *
     *      ")"; 
     */
    #define PLATFORM_CODE \
        { 40  , 78  , 40  , 85  , 105 , 110 , 112 , 117 , 116 , 40   \
        , 109 , 41  , 40  , 97  , 44  , 99  , 41  , 40  , 80  , 2    \
        , 0   , 1   , 0   , 73  , 1   , 0   , 41  , 40  , 80  , 0    \
        , 0   , 1   , 0   , 86  , 1   , 0   , 41  , 40  , 71  , 40   \
        , 61  , 40  , 104 , 40  , 86  , 0   , 0   , 41  , 41  , 40   \
        , 86  , 1   , 0   , 41  , 41  , 41  , 40  , 65  , 40  , 58   \
        , 40  , 86  , 2   , 0   , 41  , 40  , 116 , 40  , 86  , 0    \
        , 0   , 41  , 41  , 41  , 41  , 40  , 79  , 0   , 0   , 1    \
        , 0   , 86  , 1   , 0   , 41  , 40  , 79  , 3   , 0   , 1    \
        , 0   , 86  , 2   , 0   , 41  , 41  , 40  , 85  , 111 , 117  \
        , 116 , 112 , 117 , 116 , 40  , 120 , 41  , 40  , 41  , 40   \
        , 80  , 4   , 0   , 1   , 0   , 86  , 0   , 0   , 41  , 40   \
        , 79  , 2   , 0   , 1   , 0   , 73  , 1   , 0   , 41  , 41   \
        , 40  , 73  , 40  , 79  , 0   , 0   , 1   , 0   , 83  , 0    \
        , 0   , 41  , 40  , 79  , 2   , 0   , 1   , 0   , 73  , 1    \
        , 0   , 41  , 41  , 40  , 84  , 112 , 97  , 115 , 115 , 40   \
        , 110 , 44  , 99  , 44  , 109 , 41  , 40  , 80  , 1   , 0    \
        , 1   , 0   , 86  , 0   , 0   , 41  , 40  , 80  , 3   , 0    \
        , 1   , 0   , 86  , 1   , 0   , 41  , 40  , 71  , 40  , 61   \
        , 40  , 104 , 40  , 86  , 1   , 0   , 41  , 41  , 40  , 83   \
        , 2   , 0   , 41  , 41  , 41  , 40  , 71  , 40  , 124 , 40   \
        , 58  , 40  , 86  , 2   , 0   , 41  , 40  , 104 , 40  , 116  \
        , 40  , 86  , 1   , 0   , 41  , 41  , 41  , 41  , 40  , 73   \
        , 1   , 0   , 41  , 41  , 41  , 40  , 68  , 105 , 110 , 112  \
        , 117 , 116 , 0   , 0   , 0   , 1   , 0   , 86  , 2   , 0    \
        , 41  , 40  , 79  , 1   , 0   , 1   , 0   , 86  , 0   , 0    \
        , 41  , 41  , 40  , 84  , 98  , 97  , 100 , 99  , 109 , 100  \
        , 40  , 109 , 44  , 99  , 41  , 40  , 80  , 3   , 0   , 1    \
        , 0   , 86  , 0   , 0   , 41  , 40  , 71  , 40  , 33  , 40   \
        , 124 , 40  , 124 , 40  , 124 , 40  , 61  , 40  , 58  , 40   \
        , 86  , 1   , 0   , 41  , 40  , 104 , 40  , 86  , 0   , 0    \
        , 41  , 41  , 41  , 40  , 83  , 1   , 0   , 41  , 41  , 40   \
        , 61  , 40  , 86  , 1   , 0   , 41  , 40  , 83  , 2   , 0    \
        , 41  , 41  , 41  , 40  , 124 , 40  , 61  , 40  , 86  , 1    \
        , 0   , 41  , 40  , 83  , 3   , 0   , 41  , 41  , 40  , 61   \
        , 40  , 86  , 1   , 0   , 41  , 40  , 83  , 4   , 0   , 41   \
        , 41  , 41  , 41  , 40  , 124 , 40  , 61  , 40  , 86  , 1    \
        , 0   , 41  , 40  , 83  , 5   , 0   , 41  , 41  , 40  , 61   \
        , 40  , 86  , 1   , 0   , 41  , 40  , 83  , 6   , 0   , 41   \
        , 41  , 41  , 41  , 41  , 41  , 40  , 79  , 4   , 0   , 1    \
        , 0   , 83  , 9   , 0   , 41  , 41  , 40  , 84  , 111 , 117  \
        , 116 , 40  , 110 , 44  , 109 , 41  , 40  , 80  , 1   , 0    \
        , 1   , 0   , 86  , 0   , 0   , 41  , 40  , 68  , 111 , 117  \
        , 116 , 112 , 117 , 116 , 0   , 0   , 0   , 1   , 0   , 86   \
        , 1   , 0   , 41  , 40  , 79  , 1   , 0   , 1   , 0   , 86   \
        , 0   , 0   , 41  , 40  , 79  , 4   , 0   , 1   , 0   , 86   \
        , 1   , 0   , 41  , 41  , 40  , 84  , 100 , 117 , 109 , 112  \
        , 40  , 99  , 44  , 111 , 41  , 40  , 80  , 3   , 0   , 1    \
        , 0   , 86  , 0   , 0   , 41  , 40  , 71  , 40  , 61  , 40   \
        , 104 , 40  , 86  , 0   , 0   , 41  , 41  , 40  , 83  , 5    \
        , 0   , 41  , 41  , 41  , 40  , 65  , 40  , 58  , 40  , 86   \
        , 1   , 0   , 41  , 40  , 100 , 41  , 41  , 41  , 40  , 79   \
        , 4   , 0   , 1   , 0   , 86  , 1   , 0   , 41  , 41  , 40   \
        , 84  , 108 , 111 , 97  , 100 , 40  , 99  , 44  , 114 , 44   \
        , 111 , 41  , 40  , 80  , 3   , 0   , 1   , 0   , 86  , 0    \
        , 0   , 41  , 40  , 71  , 40  , 61  , 40  , 104 , 40  , 86   \
        , 0   , 0   , 41  , 41  , 40  , 83  , 3   , 0   , 41  , 41   \
        , 41  , 40  , 65  , 40  , 58  , 40  , 86  , 1   , 0   , 41   \
        , 40  , 108 , 40  , 35  , 40  , 86  , 0   , 0   , 41  , 40   \
        , 73  , 1   , 0   , 41  , 41  , 41  , 41  , 41  , 40  , 65   \
        , 40  , 58  , 40  , 86  , 2   , 0   , 41  , 40  , 35  , 40   \
        , 97  , 40  , 97  , 40  , 110 , 97  , 41  , 40  , 83  , 8    \
        , 0   , 41  , 41  , 40  , 83  , 7   , 0   , 41  , 41  , 40   \
        , 86  , 1   , 0   , 41  , 41  , 41  , 41  , 40  , 79  , 4    \
        , 0   , 1   , 0   , 86  , 1   , 0   , 41  , 41  , 40  , 84   \
        , 99  , 114 , 101 , 97  , 116 , 101 , 40  , 110 , 44  , 99   \
        , 41  , 40  , 80  , 3   , 0   , 1   , 0   , 86  , 1   , 0    \
        , 41  , 40  , 71  , 40  , 61  , 40  , 104 , 40  , 86  , 1    \
        , 0   , 41  , 41  , 40  , 83  , 4   , 0   , 41  , 41  , 41   \
        , 40  , 71  , 40  , 58  , 40  , 86  , 0   , 0   , 41  , 40   \
        , 99  , 40  , 35  , 40  , 86  , 1   , 0   , 41  , 40  , 73   \
        , 1   , 0   , 41  , 41  , 41  , 41  , 41  , 40  , 79  , 4    \
        , 0   , 1   , 0   , 83  , 7   , 0   , 41  , 40  , 79  , 1    \
        , 0   , 1   , 0   , 86  , 0   , 0   , 41  , 41  , 40  , 84   \
        , 108 , 111 , 97  , 100 , 105 , 110 , 115 , 116 , 40  , 110  \
        , 44  , 99  , 44  , 111 , 41  , 40  , 80  , 3   , 0   , 1    \
        , 0   , 86  , 1   , 0   , 41  , 40  , 71  , 40  , 61  , 40   \
        , 104 , 40  , 86  , 1   , 0   , 41  , 41  , 40  , 83  , 6    \
        , 0   , 41  , 41  , 41  , 40  , 71  , 40  , 58  , 40  , 86   \
        , 0   , 0   , 41  , 40  , 105 , 40  , 35  , 40  , 86  , 1    \
        , 0   , 41  , 40  , 73  , 1   , 0   , 41  , 41  , 41  , 41   \
        , 41  , 40  , 79  , 4   , 0   , 1   , 0   , 83  , 7   , 0    \
        , 41  , 40  , 79  , 1   , 0   , 1   , 0   , 86  , 0   , 0    \
        , 41  , 41  , 40  , 84  , 115 , 101 , 116 , 97  , 100 , 100  \
        , 114 , 40  , 99  , 44  , 111 , 44  , 97  , 41  , 40  , 80   \
        , 3   , 0   , 1   , 0   , 86  , 0   , 0   , 41  , 40  , 80   \
        , 0   , 0   , 1   , 0   , 86  , 1   , 0   , 41  , 40  , 71   \
        , 40  , 61  , 40  , 104 , 40  , 86  , 0   , 0   , 41  , 41   \
        , 40  , 83  , 1   , 0   , 41  , 41  , 41  , 40  , 65  , 40   \
        , 58  , 40  , 86  , 2   , 0   , 41  , 40  , 35  , 40  , 86   \
        , 0   , 0   , 41  , 40  , 73  , 1   , 0   , 41  , 41  , 41   \
        , 41  , 40  , 79  , 0   , 0   , 1   , 0   , 86  , 2   , 0    \
        , 41  , 40  , 79  , 4   , 0   , 1   , 0   , 83  , 7   , 0    \
        , 41  , 41  , 41 }

    #define PLATFORM_NAME "platform"
    #define PLATFORM_PLACE_COUNT 5
    #define PLATFORM_PLACES {"address", "nets", "idle", "message", "output"}
    #define PLATFORM_SYMBOLS "[0,\"setaddr\",\"pass\",\"load\",\"create\"," \
                        "\"dump\",\"loadinst\",\"ok\",\"failed\",\"badcmd\"]"
    #define PLATFORM_CODE_LENGTH 933
    #define PLATFORM_TRANSITIONS 153

#endif // PLATFORM_HEADER

extern MEMBLOCK_bag_t block_workspace;

void dieWith(int code);

#endif /* end of include guard: PNVM_H */
