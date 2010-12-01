#define LCDWIDTH				128
#define LCDHEIGHT				64

// messages
#define REMOTELCDINIT 0
#define REMOTELCDCOMMAND 1 // 1 param
#define REMOTELCDDATA 2 // 1 param
#define REMOTELCDSETBRIGHTNESS 3 // 1 param

#define REMOTELCDCLEAR 4 // 0 param
#define REMOTELCDCLEARWHITE 5 // 0 param
#define REMOTELCDSETPIXEL 	6 // 3 params x,y,colour
#define REMOTELCDDRAWLINE		7 // 5 params x1,y1,x2,y2,colour
#define REMOTELCDDRAWRECT 	8 // 5 params x1,y1,x2,y2,colour
#define REMOTELCDFILLRECT 	9 // 5 params x1,y1,x2,y2,colour
#define REMOTELCDDRAWCIRCLE 	10 // 4 params x,y,r,colour
#define REMOTELCDFILLCIRCLE 	11 // 4 params x,y,r,colour
#define REMOTELCDDRAWTRIANGLE 	12 // 7 params x1,y1,x2,y2,x3,y3,colour
#define REMOTELCDFILLTRIANGLE 	13 // 7 params x1,y1,x2,y2,x3,y3,colour
#define REMOTELCDDRAWCHAR 14 // 3 params x,line,char
#define REMOTELCDDRAWSTRING 15 // 2 x,line,length,str.....
#define REMOTELCDUPDATEAREA 16 // 5 params x1,y1,x2,y2,reset(0/1)
#define REMOTELCDSETUPDATEAREA 17 // 5 params  x1,y1,x2,y2,shrink(0/1)
#define REMOTELCDDRAWBMP 18 // 5 + BMP data
#define REMOTELCDDISPLAY 19 // 0 params
#define REMOTELCDCLEARBUF 20 // 0 param