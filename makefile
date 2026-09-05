
CC=occ
ODIR=o
_OBJ=main.a gteHelp.a tileData.a explosions.a paths.a pathfile.a prize.a highscore.a score.a title.a \
	images.a data.a sound.a control.a mouse.ROOT warp.a
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS= 
LEVELS=levels/level1.xgl levels/level2.xgl levels/level3.xgl levels/level4.xgl levels/level5.xgl levels/level6.xgl \
	levels/level7.xgl levels/level8.xgl levels/level9.xgl levels/level10.xgl levels/level11.xgl levels/level12.xgl \
	levels/level13.xgl levels/level14.xgl levels/level15.xgl 

all:  $(ODIR)/._GalagaGS.r GalagaGS

GalagaGS: $(OBJ)
	$(CC) -o $@ $(OBJ)
	iix chtyp -t S16 -a 0xDB03 $@

$(ODIR)/control.a: control.c control.h 

$(ODIR)/data.a: data.c copyright.h images.h defs.h data.h struct.h 

$(ODIR)/explosions.a: explosions.c copyright.h images.h struct.h data.h proto.h sound.h gte.h 

$(ODIR)/gteHelp.a: gteHelp.c gte.h 

$(ODIR)/highscore.a: highscore.c copyright.h defs.h struct.h proto.h data.h 

$(ODIR)/images.a: images.c defs.h images.h struct.h proto.h data.h gte.h 

$(ODIR)/main.a: main.c copyright.h struct.h data.h images.h defs.h paths.h pathfile.h proto.h sound.h defs.h gte.h gteHelp.h control.h 

$(ODIR)/pathfile.a: pathfile.c copyright.h struct.h paths.h pathfile.h data.h defs.h 

$(ODIR)/paths.a: paths.c copyright.h struct.h defs.h paths.h data.h proto.h images.h 

$(ODIR)/prize.a: prize.c copyright.h images.h struct.h data.h defs.h proto.h sound.h gte.h 

$(ODIR)/score.a: score.c copyright.h struct.h images.h data.h defs.h proto.h gte.h 

$(ODIR)/sound.a: sound.c sound.h data.h defs.h 

$(ODIR)/tileData.a: tileData.c 

$(ODIR)/title.a: title.c copyright.h defs.h images.h struct.h data.h proto.h pathfile.h gte.h

$(ODIR)/%.a: %.c $(DEPS)
	@mkdir -p o
	$(CC) -w -1 -c -O -1 -b --segment=AUTOSEG~~~ -o $@ $< 

$(ODIR)/%.ROOT: %.asm
	@mkdir -p o
	iix macgen $< o:$*.macros 13:AppleUtil:M16.= 13:AInclude:M16.=
	$(CC) -c -o $@ $< 

$(ODIR)/._GalagaGS.r:  galaRez.rez galaRez.equ $(LEVELS)
	@mkdir -p o
	occ -o $(ODIR)/GalagaGS.r galaRez.rez
	cp $@ ._GalagaGS

clean:
	@rm -f $(ODIR)/*.a $(ODIR)/*.root GalagaGS $(ODIR)/gala.r $(ODIR)/._GalagaGS.r ._GalagaGS $(ODIR)/mouse.* $(ODIR)/GalagaGS.r
	@rmdir -p o

