## -*- Makefile -*-
###############################################################################
## Simple stupid Makefile to make jedstate et al.
###############################################################################
## Copyright © 1999 Klaus Alexander Seistrup @ Magnetic Ink, Copenhagen, DK
##
##   Author: Klaus Alexander Seistrup
##  Created: Wednesday, 21st July 1999
## @(#) $Id: Makefile,v 1.2 1999/07/21 20:24:44 kas Exp $
##
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 of the License, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful, but with-
## out any warranty; without even the implied warranty of merchantability or
## fitness for a particular purpose.  See the GNU General Public License for
## more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, please write to the Free Software Foundation,
## Inc., 59 Temple Place, Suite 330, Boston MA 02111-1307, USA.
###############################################################################

    TARGET = jedstate
    SOURCE = jedstate.c
   OBJECTS = jedstate.o

        CC = gcc
    CFLAGS = -s -Wall -O2 -m486 -fomit-frame-pointer -fstrength-reduce
   LDFLAGS = -s

      LIBS = -lgdbm
    LIBDIR =
    INCDIR = -I.

        RM = rm -f
        LN = ln -s
   INSTALL = install -o root -g root

INSTALLDIR = /usr/local/bin

#

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(INCDIR) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBDIR) $(LIBS)

mrproper: clean
	$(RM) *.bak $(TARGET)

clean:
	$(RM) a.out core *~ $(OBJECTS)

install: $(TARGET)
	$(INSTALL) -m 0755 -s $(TARGET) $(INSTALLDIR)
	#(cd $(INSTALLDIR) && $(LN) $(TARGET) setjedstate)
	#(cd $(INSTALLDIR) && $(LN) $(TARGET) getjedstate)
	#$(TARGET) --example >> ~/.jedrc
	#$(TARGET) --purge=0

## Soli Deo Gloria ############################################################
