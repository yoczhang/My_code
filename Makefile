
default: lib transport
all: lib transport

examples.zip: ${FILES}
	@zip -9 -u -y $@ $^

clean:
	-/bin/rm -f *.o *.tmp plot.eps PI[0-9]* \
		*.G.* *.A.* *.Aalpha.* *.Abeta.* \
		*.Gx.* *.Gy.* *.Gz.* *.x.* *.y.* *.z.* *.b.* *.x0.*\
		*.lua *.txt

distclean: clean
	-/bin/rm -f poisson simplest mesh*.*  transport int_associated_legendre_ployns handle_boundary_conditions

lib:
	@(cd ../src; $(MAKE))

include ../Makefile.inc

#poisson.o:	../src/libphg${LIB_SUFFIX} functions.h poisson.c
#simplest.o:	../src/libphg${LIB_SUFFIX} functions.h simplest.c
#vector_test.o:	../src/libphg${LIB_SUFFIX} functions.h vector_test.c
#maxwell-complex.o: ../src/libphg${LIB_SUFFIX} maxwell-complex.c
#navier-stokes.o: ../src/libphg${LIB_SUFFIX} navier-stokes.c navier-stokes.h
transport.o:	../src/libphg${LIB_SUFFIX} functions.h other_functions.h transport.c
#matrix_test.o:	../src/libphg${LIB_SUFFIX} functions.h other_functions.h matrix_test.c
other_functions.o:	../src/libphg${LIB_SUFFIX} other_functions.h other_functions.c
int_associated_legendre_polyns.o:	../src/libphg${LIB_SUFFIX} int_associated_legendre_polyns.h int_associated_legendre_polyns.c
handle_boundary_conditions.o:	../src/libphg${LIB_SUFFIX} handle_boundary_conditions.h handle_boundary_conditions.c
#myDebug_simplest.o:	../src/libphg${LIB_SUFFIX} functions.h myDebug_simplest.c
#matrix_test: matrix_test.o other_functions.o
#	${LINKER} ${USER_LDFLAGS} ${LDFLAGS} -o matrix_test matrix_test.o other_functions.o ${USER_LIBS} ${LIBS}
transport: transport.o other_functions.o int_associated_legendre_polyns.o handle_boundary_conditions.o
	${LINKER} ${USER_LDFLAGS} ${LDFLAGS} -o transport transport.o other_functions.o int_associated_legendre_polyns.o handle_boundary_conditions.o ${USER_LIBS}${LIBS}


.PHONY: default all clean distclean lib
