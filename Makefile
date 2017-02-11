all:
	bison -d myson.y
	flex kk.l
	gcc -o calc myson.tab.c lex.yy.c
	./calc
clean:
	rm myson.tab.c myson.tab.h calc lex.yy.c