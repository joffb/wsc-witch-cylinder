#include "cylinder.h"

/*
// y offset for curve
var out = []; 
for (var i = 0; i < 144; i++) {
    theta = Math.acos((72-i)/72);
    y = Math.sin((Math.PI * 1.5) + theta);
    out.push(Math.floor(y * 128) + 128);
}

// remove the most extreme samples
JSON.stringify(out);
*/

const unsigned char cylinder[144] = {
0,1,3,5,7,8,10,12,14,16,17,19,21,23,24,26,28,30,32,33,35,37,39,40,42,44,46,47,49,51,53,55,56,58,60,62,64,65,67,69,71,72,74,76,78,79,81,83,85,87,88,90,92,94,95,97,99,101,103,104,106,108,110,111,113,115,117,119,120,122,124,126,127,129,131,133,135,136,138,140,142,143,145,147,149,151,152,154,156,158,159,161,163,165,167,168,170,172,174,175,177,179,181,183,184,186,188,190,192,193,195,197,199,200,202,204,206,207,209,211,213,215,216,218,220,222,224,225,227,229,231,232,234,236,238,239,241,243,245,247,248,250,252,254
};

/*
// x offset for curve 
var out = []; 
for (var i = 0; i < 144; i++) {
    // work out the angle for the y value
    // hypotenuse/radius is 72 as it's half the screen height (144/2)
    theta = Math.acos((i-72)/72);
    x = Math.floor(Math.sin(theta) * 72) >> 1;
    out.push(x);
}
JSON.stringify(out) 
*/

const unsigned char cylinder_edge[144] = {
0,5,8,10,11,13,14,15,16,17,18,19,19,20,21,21,22,23,23,24,24,25,25,26,26,27,27,28,28,28,29,29,29,30,30,30,31,31,31,31,32,32,32,32,33,33,33,33,33,34,34,34,34,34,34,34,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,36,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,35,34,34,34,34,34,34,34,33,33,33,33,33,32,32,32,32,31,31,31,31,30,30,30,29,29,29,28,28,28,27,27,26,26,25,25,24,24,23,23,22,21,21,20,19,19,18,17,16,15,14,13,11,10,8,5
};