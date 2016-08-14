/******************************************************************************
 * Copyright © 1996-99 Klaus Alexander Seistrup @ Magnetic Ink, Copenhagen, DK
 *
 *   Author: Klaus Alexander Seistrup
 *  Created: Saturday, 20th July 1996
 * @(#) $Id: jedstate.h,v 1.8 2001/03/01 15:49:39 kas Exp $
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but with-
 * out any warranty; without even the implied warranty of merchantability or
 * fitness for a particular purpose.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, please write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston MA 02111-1307, USA.
 ******************************************************************************/

#include <sys/stat.h>
#include <alloca.h>
#include <getopt.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <gdbm.h>
#include <time.h>

/**/
#define HAVE_BASENAME	1		/* #undef if you don't */

/**/
#define PGMNAME		"jedstate"
#define PGMVER		"0"
#define PGMREV		"5"
#define PGMPL		"2"
#define PGMDATE		"01-Mar-2001"

#define MakeID(name,ver,rev,pl,date) \
"@(#)" name " " ver "." rev "." pl " (" date ")"

#define MYURL		"http://www.magnetic-ink.dk/cgi-bin/wiki?JedState"

/*
 *  List Node structure.  Each member in a list starts with a Node.
 *  [Yes, my first computers were Amigas... *:)  /kas]
 */
typedef struct Node
{
  struct Node *ln_Succ;		/* Pointer to next (successor) */
  struct Node *ln_Pred;		/* Pointer to previous (predecessor) */
  char        *ln_Name;		/* ID string, null terminated */
}
NODE;

/*
 *  List Header structure.
 */
typedef struct List
{
  struct Node *lh_Head;		/* Pointer to head */
  struct Node *lh_Tail;		/* Pointer to tail */
  struct Node *lh_TailPred;		/* Pointer to tail's predecessor */
}
LIST;

typedef GDBM_FILE	GDB;

#define SECS_PER_MIN	(60)
#define SECS_PER_HOUR	(60 * (SECS_PER_MIN))
#define SECS_PER_DAY	(24 * (SECS_PER_HOUR))
#define SECS_PER_WEEK	( 7 * (SECS_PER_DAY))
#define SECS_PER_YEAR	(31556925)

/*
 * Function prototypes
 */
#ifndef Prototype
# define Prototype	extern
#endif /* !Prototype */

Prototype int   atos (char *ptr);
#ifndef HAVE_BASENAME
Prototype char *basename (char *pathname);
#endif /* !HAVE_BASENAME */
Prototype int   file_exists (char *str);
Prototype int  *fill_in (int *ary, int data, char *type);
Prototype char *get_home_dir (void);
Prototype int   get_jedstate (char *filename);
Prototype char *get_program_name (char *path);
Prototype void  init_jedstate (char *myname);
Prototype GDB   opendb (int rw);
Prototype int   purge_jedstate (int secs);
Prototype int   set_jedstate (char *filename, char *line, char *column);

/**/
#define SHORT_OPTS	"chp:qQuVx::?"
#define END_LONG_OPTS	{NULL,0,NULL,0}

/**/
#define USAGE \
"Usage: %s [OPTIONS]\n"\
"  options are:\n"\
"    -pAGE, --purge=AGE ....... purge database entries older than AGE\n"\
"    -x[N], --example[=N] ..... print sample startup/exit hooks (1|2)\n"\
"    -q, --quiet .............. suppress normal output\n"\
"    -Q, --really-quiet ....... suppress even error messages\n"\
"    -c, --copying-policy ..... show copying policy\n"\
"    -h, --help ............... this brief help text\n"\
"    -V, --version ............ print version information\n"\
"    -u, --url ................ print the URL of Magnetic Ink\n"\
"\n"\
"Syntax:  jedstate --purge[=AGE]                      # purge database\n"\
"  or: getjedstate <filename>                         # fetch info\n"\
"  or: setjedstate <filename> <line> <column>         # store info\n"\
"\n"\
"AGE is age-since-last-view in seconds unless appended with `W' (weeks),\n"\
"`D' (days), `H' (hours) or `M' (minutes), e.g., \"2w\" (2 weeks) or \"19d\"\n"\
"(19 days).  Be careful not to overflow an int.  If AGE is negative no\n"\
"purging will take place.  If age is zero the database will be cleared\n"\
"(this is useful when installing jedstate for the first time)!  To purge\n"\
"only non-existant files set AGE=0x7FFFFFFF or some other large number.\n"

/**/
#define MINIGPL	\
"Copyright © 1996-2001 Klaus Alexander Seistrup @ Magnetic Ink, Copenhagen, DK\n"\
"\n"\
"This program is free software; you can redistribute it and/or modify it\n"\
"under the terms of the GNU General Public License as published by the Free\n"\
"Software Foundation; either version 2 of the License, or (at your option)\n"\
"any later version.\n"\
"\n"\
"This program is distributed in the hope that it will be useful, but with-\n"\
"out any warranty; without even the implied warranty of merchantability or\n"\
"fitness for a particular purpose.  See the GNU General Public License for\n"\
"more details.\n"\
"\n"\
"You should have received a copy of the GNU General Public License along\n"\
"with this program; if not, please write to the Free Software Foundation,\n"\
"Inc., 59 Temple Place, Suite 330, Boston MA 02111-1307, USA."

/*
 * Sample hooks (looks really messy until printed) to interface with jed
 */

/* Sample hook #1 ("normal" S-Lang) */
#define SAMPLE_HOOK_1 \
"\n"\
"variable Jed_State_File = dircat (getenv (\"HOME\"), \".jedstate.sl\");\n"\
"\n"\
"% Startup hook that utilizes jedstate invoked as getjedstate\n"\
"define startup_hook ()\n"\
"{\n"\
"  !if (BATCH)                           % not evaluated for batch processes\n"\
"    {\n"\
"      $1 = Sprintf (\"getjedstate \\\"%s\\\"\", buffer_filename, 1);\n"\
"      () = system ($1);\n"\
"\n"\
"      if (file_status (Jed_State_File) == 1)    % if we have a .jedstate.sl file\n"\
"        {\n"\
"          () = evalfile (Jed_State_File);       % evaluate command\n"\
"          () = delete_file (Jed_State_File);    % - then delete source\n"\
"        }\n"\
"    }\n"\
"}\n"\
"\n"\
"% Exit hook that utilizes jedstate invoked as setjedstate\n"\
"define exit_hook ()\n"\
"{\n"\
"  !if (BATCH)                           % not evaluated for batch processes\n"\
"    {\n"\
"      variable fname = buffer_filename ();\n"\
"\n"\
"      if (strlen (fname))               % only if we actually have a file name\n"\
"        {\n"\
"          variable line = what_line (); % active line\n"\
"          variable col = what_column ();% - and column\n"\
"\n"\
"          $1 = Sprintf (\"setjedstate \\\"%s\\\" %d %d\", fname, line, col, 3);\n"\
"          () = system ($1);             % do it\n"\
"        }\n"\
"      exit_jed ();\n"\
"    }\n"\
"}\n"

/* Sample hook #2 (same as #1, only in RPN) */
#define SAMPLE_HOOK_2 \
"\n. [Jed_State_File] \"HOME\" getenv \".jedstate.sl\" dircat =Jed_State_File\n"\
"\n"\
"% Startup hook that utilizes jedstate invoked as getjedstate\n"\
". ( BATCH { % not evaluated for batch processes\n"\
".     \"getjedstate \\\"%s\\\"\" buffer_filename 1 Sprintf system pop\n"\
".     Jed_State_File dup file_status 1 == { % if we have a .jedstate.sl file\n"\
".       dup evalfile pop delete_file % evaluate command, then delete source\n"\
".     } if\n"\
".     pop\n"\
".   } !if\n"\
". ) startup_hook\n"\
"\n"\
"% Exit hook that utilizes jedstate invoked as setjedstate\n"\
". ( BATCH { % not evaluated for batch processes\n"\
".     buffer_filename\n"\
".     dup strlen { % only if we actually have a file name\n"\
".       \"setjedstate \\\"%s\\\" %d %d\" % setjedstate file line col\n"\
".       exch what_line what_column 3 Sprintf system\n"\
".     } if\n"\
".     pop % pops buffer_filename() or system()\n"\
".     exit_jed\n"\
".   } !if\n"\
". ) exit_hook\n"

/* Soli Deo Gloria */
