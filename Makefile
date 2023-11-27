


#On Msys2 it should be...
#SDL_LIBS= -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lopengl32


default:
	cbas funbas.cbas foobar.c
	cc -O3 foobar.c -o fun -lm -lSDL2 -lGL -lSDL2_mixer -fwrapv -Wall -Wno-unused

win:
	cbas funbas.cbas
	cc -O3 auto_out.c -o fun -lm -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lopengl32 -fwrapv -Wall -Wno-unused

bld:
	cc -O3 auto_out.c -o fun -lm -lSDL2 -lGL -lSDL2_mixer -fwrapv -Wall -Wno-unused
	
bld_win:
	cc -O3 auto_out.c -o fun -lm -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lopengl32 -fwrapv -Wall -Wno-unused

helloworld:
	cbas helloworld.cbas
	cc -O3 auto_out.c -o helloworld -lm -lSDL2 -lGL -lSDL2_mixer -fwrapv -Wall -Wno-unused

snake:
	cbas snake.cbas
	cc -O3 auto_out.c -o snake -lm -lSDL2 -lGL -lSDL2_mixer -fwrapv -Wall -Wno-unused

copy:
	cp fun.hbas /home/gek/work/seabass/library/toc/
	cp funbas.hbas /home/gek/work/seabass/library/toc/


clean:
	rm -f *.out *.exe fun auto_out.c foobar.c helloworld snake
	
push: clean
	git add .
	git commit -m "make push"
	git push -f
