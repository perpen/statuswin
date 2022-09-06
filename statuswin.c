#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <draw.h>
#include <cursor.h>
#include <event.h>
#include <regexp.h>
#include <keyboard.h>
#include	<bio.h>

typedef struct Win Win;
struct Win {
	Rectangle r;
};

Win *win;
int linescount;
int rows, cols;
Image *color;
char *path;
// Set by getlines()
char *lines[128];
int linescount;
Reprog *seprx = nil;

enum {
	PAD = 3,
	MARGIN = 5
};

void*
erealloc(void *v, ulong n)
{
	v = realloc(v, n);
	if(v == nil)
		sysfatal("out of memory reallocating %lud", n);
	return v;
}

void
getlines(void) {
	int file = open(path, OREAD);
	if(file == -1) {
		sysfatal("cannot open");
	}
	int n;
	int off = seek(file, 0, 0);
	if(off == -1) sysfatal("unable to seek");
	char buf[Bsize];
	n = read(file, buf, Bsize);
	if(n == -1) sysfatal("cannot read");
	close(file);
	if(n == Bsize) fprintf(stderr, "content truncated\n");
	buf[n] = '\0';

	char *cur = buf;
	printf("cur=%s\n", cur);
	int count = 0, found;
	Resub match; //FIXME user may use groups
	for(;;) {
		found = regexec(seprx, cur, &match, 1);
		if(!found) break;
		*match.sp = '\0';
		lines[count++] = buf;
		cur = match.ep;
	}
	linescount = count;

	for(int i=0; i<linescount; i++){
		printf("line %d: %s\n", i, lines[i]);
	}
	exits("printed");
}

void
refreshwin(void)
{
	getlines();
	if(linescount == 0) return;
	win = erealloc(win, linescount*sizeof(win[0]));
	for(int i=0; i<linescount; i++){
		win[i].r = Rect(0,0,0,0);
	}
}

void
drawwin(int i)
{
	char *line = lines[i];
	draw(screen, win[i].r, color, nil, ZP);
	_string(screen, addpt(win[i].r.min, Pt(2,0)), display->black, ZP,
		font, line, nil, strlen(line), 
		win[i].r, nil, ZP, SoverD);
	border(screen, win[i].r, 1, display->black, ZP);	
}

void
geometry(void)
{
	int i, ncols;
	Rectangle r;

	rows = (Dy(screen->r)-2*MARGIN+PAD)/(font->height+PAD);
	if(rows <= 0)
		rows = 1;
	if(rows*cols < linescount || rows*cols >= linescount*2){
		ncols = linescount <= 0 ? 1 : (linescount+rows-1)/rows;
		if(ncols != cols){
			cols = ncols;
		}
	}

	r = Rect(0,0,(Dx(screen->r)-2*MARGIN+PAD)/cols-PAD, font->height);
	for(i=0; i<linescount; i++)
		win[i].r = rectaddpt(rectaddpt(r, Pt(MARGIN+(PAD+Dx(r))*(i/rows),
					MARGIN+(PAD+Dy(r))*(i%rows))), screen->r.min);
}

void
redraw(Image *screen)
{
	geometry();	
	int i;
	draw(screen, screen->r, color, nil, ZP);
	if(linescount == 0) return;
	for(i=0; i<linescount; i++)
		drawwin(i);
}

void
eresized(int new)
{
	if(new && getwindow(display, Refmesg) < 0)
		fprint(2,"can't reattach to window");
	geometry();
	redraw(screen);
}

void
usage(void)
{
	fprint(2, "usage: statuswin path seprx\n");
	exits("usage");
}

void
main(int argc, char **argv)
{
	if(argc != 3)
		usage();
	path = argv[0];
	seprx = regcomp(argv[1]);
	if(seprx == NULL) exits("invalid regex");
	printf("path=%s seprx=%x\n", path, seprx);

	char *fontname = nil;
	if(initdraw(0, fontname, "statuswin") < 0)
		sysfatal("initdraw: %r");
	color = allocimagemix(display, DPalebluegreen, DWhite);

	if(color == nil)
		sysfatal("allocimage: %r");

	refreshwin();
	redraw(screen);
	einit(Emouse|Ekeyboard);
	int Etimer = etimer(0, 3000);
	Event e;

	for(;;){
		switch(eread(Emouse|Ekeyboard|Etimer, &e)){
		case Ekeyboard:
			if(e.kbdc==Kdel || e.kbdc=='q')
				exits(0);
			break;
		default:	/* Etimer */
			refreshwin();
			redraw(screen);
			break;
		}
	}
}
