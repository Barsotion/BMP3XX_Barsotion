CC = ${PREFIX}-gcc
AR = ${PREFIX}-ar
OBJDUMP = ${PREFIX}-objdump
SIZE = ${PREFIX}-size

.PHONY: build
build: clean
	@echo '=====< Building BMP3XX library >================'
	@rm -rf build
	@mkdir build
	${CC} ${FLAGS} -I./inc -c src/bmp3xx_barsotion.c -o build/bmp3xx_barsotion.o
	@echo '=====< Creating an archive >===================='
	${AR} rcs libbarsotion-bmp3xx.a build/*
	@echo '=====< Totals >================================='
	@${SIZE} -t libbarsotion-bmp3xx.a

clean:
	rm -rf build
	rm -f *.a
