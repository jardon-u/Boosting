### Makefile ---

## Author: jardonnet@lrde.epita.fr
## Version: $Id: Makefile,v 0.0 2008/07/22 21:42:04 ugo Exp $
## Keywords: Boosting
## X-URL:

include Makefile.rules

PROJ=compBio
BIN=$(PROJ)
SRC=main.cc
#HDR=adaboost.hh classification_tree.hh classification_tree.hxx tree.hh observation.hh misclass.hh
OBJ=$(SRC:.cc=.o)
VPATH=src
vpath %.o obj

FILES=src configure check obj README

$(BIN): $(OBJ) $(HDR)
	$(CC) $(FLAG) $(patsubst %,obj/%,$(OBJ)) -o $(BIN)

%.o : %.cc Makefile
	$(CC) -c $(FLAG) $< -o obj/$@

gui:
	cd gui && make

check:
	cd check && ./moulinette.sh

clean:
	rm -f $(BIN)
	rm -f obj/*.o

dist: clean fixme AUTHORS svn_check
	rm -rf $(PROJ)
	mkdir $(PROJ)
	cp -r Makefile AUTHORS FIXME $(FILES) $(PROJ)
	-find $(PROJ) -name ".svn" -exec rm -rf {} \;
	-find $(PROJ) -name "*~" -exec rm -rf {} \;
	-find $(PROJ) -name "#*" -exec rm -rf {} \;
	tar cjvf $(PROJ).tar.bz2 $(PROJ)
	rm -rf $(PROJ)
	chmod 644 $(PROJ).tar.bz2

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
        --exclude="Makefile" --exclude-dir=".svn" > FIXME

.deps:
	gcc -MM $(SRC:%=src/%) > .deps

.PHONY: doc check .deps gui

-include .deps

### Makefile ends here
