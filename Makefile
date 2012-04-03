### Makefile ---

## Author: jardonnet@lrde.epita.fr
## Version: $Id: Makefile,v 0.0 2008/07/22 21:42:04 ugo Exp $
## Keywords: Boosting
## X-URL:

-include Makefile.rules

PROJ=boosting
BIN=test
VERSION=0.5

TAR_FILE=$(PROJ)-$(VERSION).tar.bz2
SRC=main.cc
#HDR=adaboost.hh classification_tree.hh tree.hh observation.hh misclass.hh
OBJ=$(SRC:.cc=.o)
VPATH=src
LFLAGS=-Isrc
vpath %.o obj

FILES=src README

all: $(BIN)

$(BIN): $(OBJ) $(HDR)
	$(CCXX) $(CFLAGS) $(patsubst %, obj/%,$(OBJ)) -o $(BIN) $(LFLAGS)

%.o : %.cc Makefile
	$(CCXX) -c $(CFLAGS) $< -o obj/$@

clean:
	rm -f $(BIN)
	rm -f obj/*.o
	rm -f .deps

dist: all clean fixme AUTHORS
	rm -rf $(PROJ)
	mkdir $(PROJ)
	cp -r configure Makefile AUTHORS FIXME $(FILES) $(PROJ)
	-find $(PROJ) -name ".svn" -exec rm -rf {} \;
	-find $(PROJ) -name "*~" -exec rm -rf {} \;
	-find $(PROJ) -name "#*" -exec rm -rf {} \;
	tar cjvf $(TAR_FILE) $(PROJ)
	rm -rf $(PROJ)
	chmod 644 $(TAR_FILE)

svn_check:
	@svn st | grep \? ; [ $$? = "1" ] \
	|| (echo "----------------------------------------------"\
	&& echo "SOME FILES ARE MISSING FROM THE SVN REPOSITORY"\
	&& echo "----------------------------------------------");

commit:
	@svn up
	@rm -f diff.patch
	@echo "  * " >> diff.patch
	@svn st | grep "^?" -v >> diff.patch
	@echo ""  >> diff.patch
	@echo "||| Not editing commit log cancels commit! |||"
	@export F1=`openssl dgst -md5 diff.patch | cut -d' ' -f 2`	;\
	$(EDITOR) diff.patch						;\
	export F2=`openssl dgst -md5 diff.patch | cut -d' ' -f 2`	;\
	if [ ! "$$F1" = "$$F2" ] ; then 	\
	  svn ci -F diff.patch ; 		\
	else					\
	  echo "[[[ Commit aborted ]]]" ;	\
	fi
	@rm -f diff.patch

fixme:
	-rm -f FIXME
	-grep "FIXME" -r . --line-number \
        --exclude="Makefile" --exclude-dir=".svn" \
	                           --exclude-dir=".git" > FIXME

.deps:
	g++ -MM $(SRC:%=src/%) > .deps

.PHONY: doc check .deps gui

-include .deps

### Makefile ends here
