
#include <sys/types.h>
#include "armral.h"
#include <string.h>


#ifndef __PS_NR_PDCCH_H
#define __PS_NR_PDCCH_H

#define MAX_PDCCH_AGGREGATION               (16)
#define NCCE                                (6)
#define MAX_PDCCH_AGGREGATION_LEVELS        (3)
#define PDCCH_DATA_SC_PER_RB                (9)
#define PDCCH_DMRS_SC_PER_RB                (3)
#define N_RE_PER_RB                         (12)
#define N_DMRS_RE_PDCCH                     (3)
#define MAX_INT_16                          (1<<15)

typedef struct {
    uint32_t fdra;
    uint32_t tdra;
    bool mapping;
    uint32_t mcs;
    uint32_t rv;
    bool si_ind;
}dci_1_0_si_sib_1_t;

/*armral_cmplx_int16_t phase_derot_jitt_guard_rest = {32768,0,29792,13646,21403,24812,9127,31471,-4808,32413,-17869,27467,-27684,17531,-32470,4410,-31357,-9512,-24548,-21706,-13279,-29957,402,-32766,14010,-29622,25073,-21097,31581,-8740,32352,5205,27246,18205,17190,27897,4011,32522,-9896,31238,-22006,24279,-30118,12910,-32758,-804,-29448,-14373,-20788,-25330,-8351,-31686,5602,-32286,18538,-27020,28106,-16846,32568,-3612,31114,10279,24008,22302,12540,30274,-1206,32746,-14733,29269,-25583,20475,-31786,7962,-32214,-5998,-26791,-18868,-16500,-28311,-3212,-32610,10660,-30986,22595,-23732,30425,-12167,32729,1608,29086,15091,20160,25833,7571,31881,-6393,32138,-19195,26557,-28511,16151,-32647,2811,-30853,-11039,-23453,-22884,-11793,-30572,2009,-32706,15447,-28899,26078,-19841,31972,-7180,32058,6787,26320,19520,15800,28707,2411,32679,-11417,30715,-23170,23170,-30715,11417,-32679,-2411,-28707,-15800,-19520,-26320,-6787,-32058,7180,-31972,19841,-26078,28899,-15447,32706,-2009,30572,11793,22884,23453,11039,30853,-2811,32647,-16151,28511,-26557,19195,-32138,6393,-31881,-7571,-25833,-20160,-15091,-29086,-1608,-32729,12167,-30425,23732,-22595,30986,-10660,32610,3212,28311,16500,18868,26791,5998,32214,-7962,31786,-20475,25583,-29269,14733,-32746,1206,-30274,-12540,-22302,-24008,-10279,-31114,3612,-32568,16846,-28106,27020,-18538,32286,-5602,31686,8351,25330,20788,14373,29448,804,32758,-12910,30118,-24279,22006,-31238,9896,-32522,-4011,-27897,-17190,-18205,-27246,-5205,-32352,8740,-31581,21097,-25073,29622,-14010,32766,-402,29957,13279,21706,24548,9512,31357,-4410,32470,-17531,27684,-27467,17869,-32413,4808,-31471,-9127,-24812,-21403,-13646,-29792,
0,-32768,13646,-29792,24812,-21403,31471,-9127,32413,4808,27467,17869,17531,27684,4410,32470,-9512,31357,-21706,24548,-29957,13279,-32766,-402,-29622,-14010,-21097,-25073,-8740,-31581,5205,-32352,18205,-27246,27897,-17190,32522,-4011,31238,9896,24279,22006,12910,30118,-804,32758,-14373,29448,-25330,20788,-31686,8351,-32286,-5602,-27020,-18538,-16846,-28106,-3612,-32568,10279,-31114,22302,-24008,30274,-12540,32746,1206,29269,14733,20475,25583,7962,31786,-5998,32214,-18868,26791,-28311,16500,-32610,3212,-30986,-10660,-23732,-22595,-12167,-30425,1608,-32729,15091,-29086,25833,-20160,31881,-7571,32138,6393,26557,19195,16151,28511,2811,32647,-11039,30853,-22884,23453,-30572,11793,-32706,-2009,-28899,-15447,-19841,-26078,-7180,-31972,6787,-32058,19520,-26320,28707,-15800,32679,-2411,30715,11417,23170,23170,11417,30715,-2411,32679,-15800,28707,-26320,19520,-32058,6787,-31972,-7180,-26078,-19841,-15447,-28899,-2009,-32706,11793,-30572,23453,-22884,30853,-11039,32647,2811,28511,16151,19195,26557,6393,32138,-7571,31881,-20160,25833,-29086,15091,-32729,1608,-30425,-12167,-22595,-23732,-10660,-30986,3212,-32610,16500,-28311,26791,-18868,32214,-5998,31786,7962,25583,20475,14733,29269,1206,32746,-12540,30274,-24008,22302,-31114,10279,-32568,-3612,-28106,-16846,-18538,-27020,-5602,-32286,8351,-31686,20788,-25330,29448,-14373,32758,-804,30118,12910,22006,24279,9896,31238,-4011,32522,-17190,27897,-27246,18205,-32352,5205,-31581,-8740,-25073,-21097,-14010,-29622,-402,-32766,13279,-29957,24548,-21706,31357,-9512,32470,4410,27684,17531,17869,27467,4808,32413,-9127,31471,-21403,24812,-29792,13646,
-32768,0,-29792,-13646,-21403,-24812,-9127,-31471,4808,-32413,17869,-27467,27684,-17531,32470,-4410,31357,9512,24548,21706,13279,29957,-402,32766,-14010,29622,-25073,21097,-31581,8740,-32352,-5205,-27246,-18205,-17190,-27897,-4011,-32522,9896,-31238,22006,-24279,30118,-12910,32758,804,29448,14373,20788,25330,8351,31686,-5602,32286,-18538,27020,-28106,16846,-32568,3612,-31114,-10279,-24008,-22302,-12540,-30274,1206,-32746,14733,-29269,25583,-20475,31786,-7962,32214,5998,26791,18868,16500,28311,3212,32610,-10660,30986,-22595,23732,-30425,12167,-32729,-1608,-29086,-15091,-20160,-25833,-7571,-31881,6393,-32138,19195,-26557,28511,-16151,32647,-2811,30853,11039,23453,22884,11793,30572,-2009,32706,-15447,28899,-26078,19841,-31972,7180,-32058,-6787,-26320,-19520,-15800,-28707,-2411,-32679,11417,-30715,23170,-23170,30715,-11417,32679,2411,28707,15800,19520,26320,6787,32058,-7180,31972,-19841,26078,-28899,15447,-32706,2009,-30572,-11793,-22884,-23453,-11039,-30853,2811,-32647,16151,-28511,26557,-19195,32138,-6393,31881,7571,25833,20160,15091,29086,1608,32729,-12167,30425,-23732,22595,-30986,10660,-32610,-3212,-28311,-16500,-18868,-26791,-5998,-32214,7962,-31786,20475,-25583,29269,-14733,32746,-1206,30274,12540,22302,24008,10279,31114,-3612,32568,-16846,28106,-27020,18538,-32286,5602,-31686,-8351,-25330,-20788,-14373,-29448,-804,-32758,12910,-30118,24279,-22006,31238,-9896,32522,4011,27897,17190,18205,27246,5205,32352,-8740,31581,-21097,25073,-29622,14010,-32766,402,-29957,-13279,-21706,-24548,-9512,-31357,4410,-32470,17531,-27684,27467,-17869,32413,-4808,31471,9127,24812,21403,13646,29792,
0,32768,-13646,29792,-24812,21403,-31471,9127,-32413,-4808,-27467,-17869,-17531,-27684,-4410,-32470,9512,-31357,21706,-24548,29957,-13279,32766,402,29622,14010,21097,25073,8740,31581,-5205,32352,-18205,27246,-27897,17190,-32522,4011,-31238,-9896,-24279,-22006,-12910,-30118,804,-32758,14373,-29448,25330,-20788,31686,-8351,32286,5602,27020,18538,16846,28106,3612,32568,-10279,31114,-22302,24008,-30274,12540,-32746,-1206,-29269,-14733,-20475,-25583,-7962,-31786,5998,-32214,18868,-26791,28311,-16500,32610,-3212,30986,10660,23732,22595,12167,30425,-1608,32729,-15091,29086,-25833,20160,-31881,7571,-32138,-6393,-26557,-19195,-16151,-28511,-2811,-32647,11039,-30853,22884,-23453,30572,-11793,32706,2009,28899,15447,19841,26078,7180,31972,-6787,32058,-19520,26320,-28707,15800,-32679,2411,-30715,-11417,-23170,-23170,-11417,-30715,2411,-32679,15800,-28707,26320,-19520,32058,-6787,31972,7180,26078,19841,15447,28899,2009,32706,-11793,30572,-23453,22884,-30853,11039,-32647,-2811,-28511,-16151,-19195,-26557,-6393,-32138,7571,-31881,20160,-25833,29086,-15091,32729,-1608,30425,12167,22595,23732,10660,30986,-3212,32610,-16500,28311,-26791,18868,-32214,5998,-31786,-7962,-25583,-20475,-14733,-29269,-1206,-32746,12540,-30274,24008,-22302,31114,-10279,32568,3612,28106,16846,18538,27020,5602,32286,-8351,31686,-20788,25330,-29448,14373,-32758,804,-30118,-12910,-22006,-24279,-9896,-31238,4011,-32522,17190,-27897,27246,-18205,32352,-5205,31581,8740,25073,21097,14010,29622,402,32766,-13279,29957,-24548,21706,-31357,9512,-32470,-4410,-27684,-17531,-17869,-27467,-4808,-32413,9127,-31471,21403,-24812,29792,
-13646,32768,0,29792,13646,21403,24812,9127,31471,-4808,32413,-17869,27467,-27684,17531,-32470,4410,-31357,-9512,-24548,-21706,-13279,-29957,402,-32766,14010,-29622,25073,-21097,31581,-8740,32352,5205,27246,18205,17190,27897,4011,32522,-9896,31238,-22006,24279,-30118,12910,-32758,-804,-29448,-14373,-20788,-25330,-8351,-31686,5602,-32286,18538,-27020,28106,-16846,32568,-3612,31114,10279,24008,22302,12540,30274,-1206,32746,-14733,29269,-25583,20475,-31786,7962,-32214,-5998,-26791,-18868,-16500,-28311,-3212,-32610,10660,-30986,22595,-23732,30425,-12167,32729,1608,29086,15091,20160,25833,7571,31881,-6393,32138,-19195,26557,-28511,16151,-32647,2811,-30853,-11039,-23453,-22884,-11793,-30572,2009,-32706,15447,-28899,26078,-19841,31972,-7180,32058,6787,26320,19520,15800,28707,2411,32679,-11417,30715,-23170,23170,-30715,11417,-32679,-2411,-28707,-15800,-19520,-26320,-6787,-32058,7180,-31972,19841,-26078,28899,-15447,32706,-2009,30572,11793,22884,23453,11039,30853,-2811,32647,-16151,28511,-26557,19195,-32138,6393,-31881,-7571,-25833,-20160,-15091,-29086,-1608,-32729,12167,-30425,23732,-22595,30986,-10660,32610,3212,28311,16500,18868,26791,5998,32214,-7962,31786,-20475,25583,-29269,14733,-32746,1206,-30274,-12540,-22302,-24008,-10279,-31114,3612,-32568,16846,-28106,27020,-18538,32286,-5602,31686,8351,25330,20788,14373,29448,804,32758,-12910,30118,-24279,22006,-31238,9896,-32522,-4011,-27897,-17190,-18205,-27246,-5205,-32352,8740,-31581,21097,-25073,29622,-14010,32766,-402,29957,13279,21706,24548,9512,31357,-4410,32470,-17531,27684,-27467,17869,-32413,4808,-31471,-9127,-24812,-21403,-13646,
-29792,0,-32768,13646,-29792,24812,-21403,31471,-9127,32413,4808,27467,17869,17531,27684,4410,32470,-9512,31357,-21706,24548,-29957,13279,-32766,-402,-29622,-14010,-21097,-25073,-8740,-31581,5205,-32352,18205,-27246,27897,-17190,32522,-4011,31238,9896,24279,22006,12910,30118,-804,32758,-14373,29448,-25330,20788,-31686,8351,-32286,-5602,-27020,-18538,-16846,-28106,-3612,-32568,10279,-31114,22302,-24008,30274,-12540,32746,1206,29269,14733,20475,25583,7962,31786,-5998,32214,-18868,26791,-28311,16500,-32610,3212,-30986,-10660,-23732,-22595,-12167,-30425,1608,-32729,15091,-29086,25833,-20160,31881,-7571,32138,6393,26557,19195,16151,28511,2811,32647,-11039,30853,-22884,23453,-30572,11793,-32706,-2009,-28899,-15447,-19841,-26078,-7180,-31972,6787,-32058,19520,-26320,28707,-15800,32679,-2411,30715,11417,23170,23170,11417,30715,-2411,32679,-15800,28707,-26320,19520,-32058,6787,-31972,-7180,-26078,-19841,-15447,-28899,-2009,-32706,11793,-30572,23453,-22884,30853,-11039,32647,2811,28511,16151,19195,26557,6393,32138,-7571,31881,-20160,25833,-29086,15091,-32729,1608,-30425,-12167,-22595,-23732,-10660,-30986,3212,-32610,16500,-28311,26791,-18868,32214,-5998,31786,7962,25583,20475,14733,29269,1206,32746,-12540,30274,-24008,22302,-31114,10279,-32568,-3612,-28106,-16846,-18538,-27020,-5602,-32286,8351,-31686,20788,-25330,29448,-14373,32758,-804,30118,12910,22006,24279,9896,31238,-4011,32522,-17190,27897,-27246,18205,-32352,5205,-31581,-8740,-25073,-21097,-14010,-29622,-402,-32766,13279,-29957,24548,-21706,31357,-9512,32470,4410,27684,17531,17869,27467,4808,32413,-9127,31471,-21403,24812,-29792,
13646,-32768,0,-29792,-13646,-21403,-24812,-9127,-31471,4808,-32413,17869,-27467,27684,-17531,32470,-4410,31357,9512,24548,21706,13279,29957,-402,32766,-14010,29622,-25073,21097,-31581,8740,-32352,-5205,-27246,-18205,-17190,-27897,-4011,-32522,9896,-31238,22006,-24279,30118,-12910,32758,804,29448,14373,20788,25330,8351,31686,-5602,32286,-18538,27020,-28106,16846,-32568,3612,-31114,-10279,-24008,-22302,-12540,-30274,1206,-32746,14733,-29269,25583,-20475,31786,-7962,32214,5998,26791,18868,16500,28311,3212,32610,-10660,30986,-22595,23732,-30425,12167,-32729,-1608,-29086,-15091,-20160,-25833,-7571,-31881,6393,-32138,19195,-26557,28511,-16151,32647,-2811,30853,11039,23453,22884,11793,30572,-2009,32706,-15447,28899,-26078,19841,-31972,7180,-32058,-6787,-26320,-19520,-15800,-28707,-2411,-32679,11417,-30715,23170,-23170,30715,-11417,32679,2411,28707,15800,19520,26320,6787,32058,-7180,31972,-19841,26078,-28899,15447,-32706,2009,-30572,-11793,-22884,-23453,-11039,-30853,2811,-32647,16151,-28511,26557,-19195,32138,-6393,31881,7571,25833,20160,15091,29086,1608,32729,-12167,30425,-23732,22595,-30986,10660,-32610,-3212,-28311,-16500,-18868,-26791,-5998,-32214,7962,-31786,20475,-25583,29269,-14733,32746,-1206,30274,12540,22302,24008,10279,31114,-3612,32568,-16846,28106,-27020,18538,-32286,5602,-31686,-8351,-25330,-20788,-14373,-29448,-804,-32758,12910,-30118,24279,-22006,31238,-9896,32522,4011,27897,17190,18205,27246,5205,32352,-8740,31581,-21097,25073,-29622,14010,-32766,402,-29957,-13279,-21706,-24548,-9512,-31357,4410,-32470,17531,-27684,27467,-17869,32413,-4808,31471,9127,24812,21403,13646,
29792,0,32768,-13646,29792,-24812,21403,-31471,9127,-32413,-4808,-27467,-17869,-17531,-27684,-4410,-32470,9512,-31357,21706,-24548,29957,-13279,32766,402,29622,14010,21097,25073,8740,31581,-5205,32352,-18205,27246,-27897,17190,-32522,4011,-31238,-9896,-24279,-22006,-12910,-30118,804,-32758,14373,-29448,25330,-20788,31686,-8351,32286,5602,27020,18538,16846,28106,3612,32568,-10279,31114,-22302,24008,-30274,12540,-32746,-1206,-29269,-14733,-20475,-25583,-7962,-31786,5998,-32214,18868,-26791,28311,-16500,32610,-3212,30986,10660,23732,22595,12167,30425,-1608,32729,-15091,29086,-25833,20160,-31881,7571,-32138,-6393,-26557,-19195,-16151,-28511,-2811,-32647,11039,-30853,22884,-23453,30572,-11793,32706,2009,28899,15447,19841,26078,7180,31972,-6787,32058,-19520,26320,-28707,15800,-32679,2411,-30715,-11417,-23170,-23170,-11417,-30715,2411,-32679,15800,-28707,26320,-19520,32058,-6787,31972,7180,26078,19841,15447,28899,2009,32706,-11793,30572,-23453,22884,-30853,11039,-32647,-2811,-28511,-16151,-19195,-26557,-6393,-32138,7571,-31881,20160,-25833,29086,-15091,32729,-1608,30425,12167,22595,23732,10660,30986,-3212,32610,-16500,28311,-26791,18868,-32214,5998,-31786,-7962,-25583,-20475,-14733,-29269,-1206,-32746,12540,-30274,24008,-22302,31114,-10279,32568,3612,28106,16846,18538,27020,5602,32286,-8351,31686,-20788,25330,-29448,14373,-32758,804,-30118,-12910,-22006,-24279,-9896,-31238,4011,-32522,17190,-27897,27246,-18205,32352,-5205,31581,8740,25073,21097,14010,29622,402,32766,-13279,29957,-24548,21706,-31357,9512,-32470,-4410,-27684,-17531,-17869,-27467,-4808,-32413,9127,-31471,21403,-24812,29792,-13646 };

armral_cmplx_int16_t phase_derot_jitt_guard_first = { 32768,0,30118,12910,22595,23732,11417,30715,-1608,32729,-14373,29448,-24812,21403,-31238,9896,-32610,-3212,-28707,-15800,-20160,-25833,-8351,-31686,4808,-32413,17190,-27897,26791,-18868,32058,-6787,32138,6393,27020,18538,17531,27684,5205,32352,-7962,31786,-19841,26078,-28511,16151,-32568,3612,-31357,-9512,-25073,-21097,-14733,-29269,-2009,-32706,11039,-30853,22302,-24008,29957,-13279,32766,-402,30274,12540,22884,23453,11793,30572,-1206,32746,-14010,29622,-24548,21706,-31114,10279,-32647,-2811,-28899,-15447,-20475,-25583,-8740,-31581,4410,-32470,16846,-28106,26557,-19195,31972,-7180,32214,5998,27246,18205,17869,27467,5602,32286,-7571,31881,-19520,26320,-28311,16500,-32522,4011,-31471,-9127,-25330,-20788,-15091,-29086,-2411,-32679,10660,-30986,22006,-24279,29792,-13646,32758,-804,30425,12167,23170,23170,12167,30425,-804,32758,-13646,29792,-24279,22006,-30986,10660,-32679,-2411,-29086,-15091,-20788,-25330,-9127,-31471,4011,-32522,16500,-28311,26320,-19520,31881,-7571,32286,5602,27467,17869,18205,27246,5998,32214,-7180,31972,-19195,26557,-28106,16846,-32470,4410,-31581,-8740,-25583,-20475,-15447,-28899,-2811,-32647,10279,-31114,21706,-24548,29622,-14010,32746,-1206,30572,11793,23453,22884,12540,30274,-402,32766,-13279,29957,-24008,22302,-30853,11039,-32706,-2009,-29269,-14733,-21097,-25073,-9512,-31357,3612,-32568,16151,-28511,26078,-19841,31786,-7962,32352,5205,27684,17531,18538,27020,6393,32138,-6787,32058,-18868,26791,-27897,17190,-32413,4808,-31686,-8351,-25833,-20160,-15800,-28707,-3212,-32610,9896,-31238,21403,-24812,29448,-14373,32729,-1608,30715,11417,23732,22595,12910,30118,
0,32768,-12910,30118,-23732,22595,-30715,11417,-32729,-1608,-29448,-14373,-21403,-24812,-9896,-31238,3212,-32610,15800,-28707,25833,-20160,31686,-8351,32413,4808,27897,17190,18868,26791,6787,32058,-6393,32138,-18538,27020,-27684,17531,-32352,5205,-31786,-7962,-26078,-19841,-16151,-28511,-3612,-32568,9512,-31357,21097,-25073,29269,-14733,32706,-2009,30853,11039,24008,22302,13279,29957,402,32766,-12540,30274,-23453,22884,-30572,11793,-32746,-1206,-29622,-14010,-21706,-24548,-10279,-31114,2811,-32647,15447,-28899,25583,-20475,31581,-8740,32470,4410,28106,16846,19195,26557,7180,31972,-5998,32214,-18205,27246,-27467,17869,-32286,5602,-31881,-7571,-26320,-19520,-16500,-28311,-4011,-32522,9127,-31471,20788,-25330,29086,-15091,32679,-2411,30986,10660,24279,22006,13646,29792,804,32758,-12167,30425,-23170,23170,-30425,12167,-32758,-804,-29792,-13646,-22006,-24279,-10660,-30986,2411,-32679,15091,-29086,25330,-20788,31471,-9127,32522,4011,28311,16500,19520,26320,7571,31881,-5602,32286,-17869,27467,-27246,18205,-32214,5998,-31972,-7180,-26557,-19195,-16846,-28106,-4410,-32470,8740,-31581,20475,-25583,28899,-15447,32647,-2811,31114,10279,24548,21706,14010,29622,1206,32746,-11793,30572,-22884,23453,-30274,12540,-32766,-402,-29957,-13279,-22302,-24008,-11039,-30853,2009,-32706,14733,-29269,25073,-21097,31357,-9512,32568,3612,28511,16151,19841,26078,7962,31786,-5205,32352,-17531,27684,-27020,18538,-32138,6393,-32058,-6787,-26791,-18868,-17190,-27897,-4808,-32413,8351,-31686,20160,-25833,28707,-15800,32610,-3212,31238,9896,24812,21403,14373,29448,1608,32729,-11417,30715,-22595,23732,-30118,12910,-32768,
0,-30118,-12910,-22595,-23732,-11417,-30715,1608,-32729,14373,-29448,24812,-21403,31238,-9896,32610,3212,28707,15800,20160,25833,8351,31686,-4808,32413,-17190,27897,-26791,18868,-32058,6787,-32138,-6393,-27020,-18538,-17531,-27684,-5205,-32352,7962,-31786,19841,-26078,28511,-16151,32568,-3612,31357,9512,25073,21097,14733,29269,2009,32706,-11039,30853,-22302,24008,-29957,13279,-32766,402,-30274,-12540,-22884,-23453,-11793,-30572,1206,-32746,14010,-29622,24548,-21706,31114,-10279,32647,2811,28899,15447,20475,25583,8740,31581,-4410,32470,-16846,28106,-26557,19195,-31972,7180,-32214,-5998,-27246,-18205,-17869,-27467,-5602,-32286,7571,-31881,19520,-26320,28311,-16500,32522,-4011,31471,9127,25330,20788,15091,29086,2411,32679,-10660,30986,-22006,24279,-29792,13646,-32758,804,-30425,-12167,-23170,-23170,-12167,-30425,804,-32758,13646,-29792,24279,-22006,30986,-10660,32679,2411,29086,15091,20788,25330,9127,31471,-4011,32522,-16500,28311,-26320,19520,-31881,7571,-32286,-5602,-27467,-17869,-18205,-27246,-5998,-32214,7180,-31972,19195,-26557,28106,-16846,32470,-4410,31581,8740,25583,20475,15447,28899,2811,32647,-10279,31114,-21706,24548,-29622,14010,-32746,1206,-30572,-11793,-23453,-22884,-12540,-30274,402,-32766,13279,-29957,24008,-22302,30853,-11039,32706,2009,29269,14733,21097,25073,9512,31357,-3612,32568,-16151,28511,-26078,19841,-31786,7962,-32352,-5205,-27684,-17531,-18538,-27020,-6393,-32138,6787,-32058,18868,-26791,27897,-17190,32413,-4808,31686,8351,25833,20160,15800,28707,3212,32610,-9896,31238,-21403,24812,-29448,14373,-32729,1608,-30715,-11417,-23732,-22595,-12910,-30118,
0,-32768,12910,-30118,23732,-22595,30715,-11417,32729,1608,29448,14373,21403,24812,9896,31238,-3212,32610,-15800,28707,-25833,20160,-31686,8351,-32413,-4808,-27897,-17190,-18868,-26791,-6787,-32058,6393,-32138,18538,-27020,27684,-17531,32352,-5205,31786,7962,26078,19841,16151,28511,3612,32568,-9512,31357,-21097,25073,-29269,14733,-32706,2009,-30853,-11039,-24008,-22302,-13279,-29957,-402,-32766,12540,-30274,23453,-22884,30572,-11793,32746,1206,29622,14010,21706,24548,10279,31114,-2811,32647,-15447,28899,-25583,20475,-31581,8740,-32470,-4410,-28106,-16846,-19195,-26557,-7180,-31972,5998,-32214,18205,-27246,27467,-17869,32286,-5602,31881,7571,26320,19520,16500,28311,4011,32522,-9127,31471,-20788,25330,-29086,15091,-32679,2411,-30986,-10660,-24279,-22006,-13646,-29792,-804,-32758,12167,-30425,23170,-23170,30425,-12167,32758,804,29792,13646,22006,24279,10660,30986,-2411,32679,-15091,29086,-25330,20788,-31471,9127,-32522,-4011,-28311,-16500,-19520,-26320,-7571,-31881,5602,-32286,17869,-27467,27246,-18205,32214,-5998,31972,7180,26557,19195,16846,28106,4410,32470,-8740,31581,-20475,25583,-28899,15447,-32647,2811,-31114,-10279,-24548,-21706,-14010,-29622,-1206,-32746,11793,-30572,22884,-23453,30274,-12540,32766,402,29957,13279,22302,24008,11039,30853,-2009,32706,-14733,29269,-25073,21097,-31357,9512,-32568,-3612,-28511,-16151,-19841,-26078,-7962,-31786,5205,-32352,17531,-27684,27020,-18538,32138,-6393,32058,6787,26791,18868,17190,27897,4808,32413,-8351,31686,-20160,25833,-28707,15800,-32610,3212,-31238,-9896,-24812,-21403,-14373,-29448,-1608,-32729,11417,-30715,22595,-23732,30118,-12910,32768,
0,30118,12910,22595,23732,11417,30715,-1608,32729,-14373,29448,-24812,21403,-31238,9896,-32610,-3212,-28707,-15800,-20160,-25833,-8351,-31686,4808,-32413,17190,-27897,26791,-18868,32058,-6787,32138,6393,27020,18538,17531,27684,5205,32352,-7962,31786,-19841,26078,-28511,16151,-32568,3612,-31357,-9512,-25073,-21097,-14733,-29269,-2009,-32706,11039,-30853,22302,-24008,29957,-13279,32766,-402,30274,12540,22884,23453,11793,30572,-1206,32746,-14010,29622,-24548,21706,-31114,10279,-32647,-2811,-28899,-15447,-20475,-25583,-8740,-31581,4410,-32470,16846,-28106,26557,-19195,31972,-7180,32214,5998,27246,18205,17869,27467,5602,32286,-7571,31881,-19520,26320,-28311,16500,-32522,4011,-31471,-9127,-25330,-20788,-15091,-29086,-2411,-32679,10660,-30986,22006,-24279,29792,-13646,32758,-804,30425,12167,23170,23170,12167,30425,-804,32758,-13646,29792,-24279,22006,-30986,10660,-32679,-2411,-29086,-15091,-20788,-25330,-9127,-31471,4011,-32522,16500,-28311,26320,-19520,31881,-7571,32286,5602,27467,17869,18205,27246,5998,32214,-7180,31972,-19195,26557,-28106,16846,-32470,4410,-31581,-8740,-25583,-20475,-15447,-28899,-2811,-32647,10279,-31114,21706,-24548,29622,-14010,32746,-1206,30572,11793,23453,22884,12540,30274,-402,32766,-13279,29957,-24008,22302,-30853,11039,-32706,-2009,-29269,-14733,-21097,-25073,-9512,-31357,3612,-32568,16151,-28511,26078,-19841,31786,-7962,32352,5205,27684,17531,18538,27020,6393,32138,-6787,32058,-18868,26791,-27897,17190,-32413,4808,-31686,-8351,-25833,-20160,-15800,-28707,-3212,-32610,9896,-31238,21403,-24812,29448,-14373,32729,-1608,30715,11417,23732,22595,12910,30118,
0,32768,-12910,30118,-23732,22595,-30715,11417,-32729,-1608,-29448,-14373,-21403,-24812,-9896,-31238,3212,-32610,15800,-28707,25833,-20160,31686,-8351,32413,4808,27897,17190,18868,26791,6787,32058,-6393,32138,-18538,27020,-27684,17531,-32352,5205,-31786,-7962,-26078,-19841,-16151,-28511,-3612,-32568,9512,-31357,21097,-25073,29269,-14733,32706,-2009,30853,11039,24008,22302,13279,29957,402,32766,-12540,30274,-23453,22884,-30572,11793,-32746,-1206,-29622,-14010,-21706,-24548,-10279,-31114,2811,-32647,15447,-28899,25583,-20475,31581,-8740,32470,4410,28106,16846,19195,26557,7180,31972,-5998,32214,-18205,27246,-27467,17869,-32286,5602,-31881,-7571,-26320,-19520,-16500,-28311,-4011,-32522,9127,-31471,20788,-25330,29086,-15091,32679,-2411,30986,10660,24279,22006,13646,29792,804,32758,-12167,30425,-23170,23170,-30425,12167,-32758,-804,-29792,-13646,-22006,-24279,-10660,-30986,2411,-32679,15091,-29086,25330,-20788,31471,-9127,32522,4011,28311,16500,19520,26320,7571,31881,-5602,32286,-17869,27467,-27246,18205,-32214,5998,-31972,-7180,-26557,-19195,-16846,-28106,-4410,-32470,8740,-31581,20475,-25583,28899,-15447,32647,-2811,31114,10279,24548,21706,14010,29622,1206,32746,-11793,30572,-22884,23453,-30274,12540,-32766,-402,-29957,-13279,-22302,-24008,-11039,-30853,2009,-32706,14733,-29269,25073,-21097,31357,-9512,32568,3612,28511,16151,19841,26078,7962,31786,-5205,32352,-17531,27684,-27020,18538,-32138,6393,-32058,-6787,-26791,-18868,-17190,-27897,-4808,-32413,8351,-31686,20160,-25833,28707,-15800,32610,-3212,31238,9896,24812,21403,14373,29448,1608,32729,-11417,30715,-22595,23732,-30118,12910,-32768,
0,-30118,-12910,-22595,-23732,-11417,-30715,1608,-32729,14373,-29448,24812,-21403,31238,-9896,32610,3212,28707,15800,20160,25833,8351,31686,-4808,32413,-17190,27897,-26791,18868,-32058,6787,-32138,-6393,-27020,-18538,-17531,-27684,-5205,-32352,7962,-31786,19841,-26078,28511,-16151,32568,-3612,31357,9512,25073,21097,14733,29269,2009,32706,-11039,30853,-22302,24008,-29957,13279,-32766,402,-30274,-12540,-22884,-23453,-11793,-30572,1206,-32746,14010,-29622,24548,-21706,31114,-10279,32647,2811,28899,15447,20475,25583,8740,31581,-4410,32470,-16846,28106,-26557,19195,-31972,7180,-32214,-5998,-27246,-18205,-17869,-27467,-5602,-32286,7571,-31881,19520,-26320,28311,-16500,32522,-4011,31471,9127,25330,20788,15091,29086,2411,32679,-10660,30986,-22006,24279,-29792,13646,-32758,804,-30425,-12167,-23170,-23170,-12167,-30425,804,-32758,13646,-29792,24279,-22006,30986,-10660,32679,2411,29086,15091,20788,25330,9127,31471,-4011,32522,-16500,28311,-26320,19520,-31881,7571,-32286,-5602,-27467,-17869,-18205,-27246,-5998,-32214,7180,-31972,19195,-26557,28106,-16846,32470,-4410,31581,8740,25583,20475,15447,28899,2811,32647,-10279,31114,-21706,24548,-29622,14010,-32746,1206,-30572,-11793,-23453,-22884,-12540,-30274,402,-32766,13279,-29957,24008,-22302,30853,-11039,32706,2009,29269,14733,21097,25073,9512,31357,-3612,32568,-16151,28511,-26078,19841,-31786,7962,-32352,-5205,-27684,-17531,-18538,-27020,-6393,-32138,6787,-32058,18868,-26791,27897,-17190,32413,-4808,31686,8351,25833,20160,15800,28707,3212,32610,-9896,31238,-21403,24812,-29448,14373,-32729,1608,-30715,-11417,-23732,-22595,-12910,-30118,
0,-32768,12910,-30118,23732,-22595,30715,-11417,32729,1608,29448,14373,21403,24812,9896,31238,-3212,32610,-15800,28707,-25833,20160,-31686,8351,-32413,-4808,-27897,-17190,-18868,-26791,-6787,-32058,6393,-32138,18538,-27020,27684,-17531,32352,-5205,31786,7962,26078,19841,16151,28511,3612,32568,-9512,31357,-21097,25073,-29269,14733,-32706,2009,-30853,-11039,-24008,-22302,-13279,-29957,-402,-32766,12540,-30274,23453,-22884,30572,-11793,32746,1206,29622,14010,21706,24548,10279,31114,-2811,32647,-15447,28899,-25583,20475,-31581,8740,-32470,-4410,-28106,-16846,-19195,-26557,-7180,-31972,5998,-32214,18205,-27246,27467,-17869,32286,-5602,31881,7571,26320,19520,16500,28311,4011,32522,-9127,31471,-20788,25330,-29086,15091,-32679,2411,-30986,-10660,-24279,-22006,-13646,-29792,-804,-32758,12167,-30425,23170,-23170,30425,-12167,32758,804,29792,13646,22006,24279,10660,30986,-2411,32679,-15091,29086,-25330,20788,-31471,9127,-32522,-4011,-28311,-16500,-19520,-26320,-7571,-31881,5602,-32286,17869,-27467,27246,-18205,32214,-5998,31972,7180,26557,19195,16846,28106,4410,32470,-8740,31581,-20475,25583,-28899,15447,-32647,2811,-31114,-10279,-24548,-21706,-14010,-29622,-1206,-32746,11793,-30572,22884,-23453,30274,-12540,32766,402,29957,13279,22302,24008,11039,30853,-2009,32706,-14733,29269,-25073,21097,-31357,9512,-32568,-3612,-28511,-16151,-19841,-26078,-7962,-31786,5205,-32352,17531,-27684,27020,-18538,32138,-6393,32058,6787,26791,18868,17190,27897,4808,32413,-8351,31686,-20160,25833,-28707,15800,-32610,3212,-31238,-9896,-24812,-21403,-14373,-29448,-1608,-32729,11417,-30715,22595,-23732,30118,-12910 };
*/
void ps_nr_pdcch_coreset_extraction(armral_cmplx_int16_t *inSym,uint8_t nSym,uint8_t symb_offset,uint8_t rb_offset,uint8_t rbs,uint8_t kssb,armral_cmplx_int16_t *outSym, void * scratchBuf);
void ps_nr_pdcch_channel_estimator(armral_cmplx_int16_t *dmrs_re,armral_cmplx_int16_t *dmrs_ref_re,armral_cmplx_int16_t *chan_est_re, void* scratchBuf,int16_t n_re);
void ps_nr_pdcch_equalizer(armral_cmplx_int16_t *in_data_re,armral_cmplx_int16_t *chan_est_re,armral_cmplx_int16_t *eq_data_re, int16_t n_re);
void ps_nr_pseudo_random_generator_init(uint32_t cInit, uint32_t* pnX1, uint32_t* pnX2);
void ps_nr_pseudo_random_generator(uint32_t seqLen, uint8_t* c, uint8_t ssbIndex, uint32_t* pnX1, uint32_t* pnX2);
void ps_nr_pdcch_deratematching(int16_t* llr, float* llrOut, uint16_t N, uint16_t E, uint16_t K, void* scratchBuf);
void ps_nr_pdcch_sym_descrambler(int16_t* llrDeScr, int16_t* llr, uint32_t seqLen, uint8_t* pScrCw);
void ps_nr_qpsk_demodulation(armral_cmplx_int16_t* eqDataRe, uint32_t seqLen, int16_t* llr);
void ps_nr_pdcch_gen_rb_interleaving_indices(uint8_t numCoresetRBs, uint8_t numCoresetSym, uint16_t cellId, uint8_t* cceRegInterleaving);
void ps_nr_pdcch_gen_rb_indices(uint8_t* cceRegInterleaverIndices, uint8_t aggrLevel, uint8_t numCoresetSym, uint8_t numCoresetRBs, uint8_t* cceCandidates, uint8_t* rbCandPerAgg);
void ps_nr_pdcch_gen_cce_candidates(uint8_t aggLvl, uint8_t maxNumCand, uint8_t numCCEs, uint8_t candIdx, uint8_t* cceCandidates);
void sort(uint8_t *inputIndices, uint8_t len);
void swap(uint8_t* a, uint8_t* b);
void ps_nr_pdcch_reg_to_re_demapping(uint8_t* regCandPerAgg,  uint8_t aggrLevel, armral_cmplx_int16_t* out1, armral_cmplx_int16_t* out2, armral_cmplx_int16_t* in);
void ps_nr_pdcch_dmrs_gen(armral_cmplx_int16_t* dmrsRefOut, uint16_t cellId, uint16_t slotnum, uint8_t dmrsStartSymNum, uint8_t numCoresetSym, uint8_t numCoresetRBs, uint8_t aggrLvl, uint8_t* rbCandPerAgg, void * scratch_buf);
void ps_nr_parse_dci_1_0_si_sib_1( uint32_t* payload, uint32_t payloadlen, dci_1_0_si_sib_1_t* dci_1_0_si_sib_1);
bool ps_nr_pdcch_crc_check( uint32_t* bitsIn, uint32_t rnti, uint32_t K);
void ps_nr_pdcch_polar_decoder(float *llr, uint32_t *infoBits, uint32_t *info_bit_pattern, uint8_t *interleaver_pattern, void *scratchBuf, uint16_t N, uint8_t K); //int16_t* llrOut
void bitSum(uint8_t *bits, uint8_t *bits_1, uint16_t node, uint16_t temp);
void g_repetition(float *L_depth, float *L_depth_1, uint8_t *bits, uint16_t node, uint16_t temp);
void f_min_sum(float *L_depth, float *L_depth_1, uint16_t node, uint16_t temp);
void ps_nr_pdcch_calculate_K_N_E( uint8_t aggrLvl, uint8_t coreset_rbs, uint8_t* K, uint16_t* E, uint16_t* N);
void ps_nr_polar_information_bit_pattern(uint8_t K, uint16_t E, uint16_t N, uint32_t* info_bit_pattern, void* pScrCw1, void* pScrCw2, void* pScrCw3); 
void ps_nr_polar_bit_interleaver_pattern(uint8_t K, uint16_t I_il, uint8_t* interleaver_pattern);
uint32_t reverseBits(uint32_t num);

#endif /* __NMM_PDCCH_H */