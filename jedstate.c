/******************************************************************************
 * Copyright © 1996-2001 Klaus Alexander Seistrup @ Magnetic Ink, CPH, DK
 *
 *   Author: Klaus Alexander Seistrup
 *  Created: Saturday, 20th July 1996
 * @(#) $Id: jedstate.c,v 1.7 2001/03/01 15:49:39 kas Exp $
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

#include "jedstate.h"

static const char PgmID[] = MakeID (PGMNAME, PGMVER, PGMREV, PGMPL, PGMDATE);
static const char RCSid[] =
  "$Id: jedstate.c,v 1.7 2001/03/01 15:49:39 kas Exp $";

#ifndef HAVE_BASENAME
 /**/ char *
basename (char *pathname)
{
  char *filename = strrchr (pathname, '/');

  if (filename)
    if (*(filename + 1))
      return filename + 1;

  return pathname;
}
#endif /* not basename */

 /**/ char *
get_program_name (char *path)
{
  static char *__program_name = NULL;

  if (__program_name == NULL)		/* Only if we don't have it already */
    {
      if (path == NULL)
	path = (char *) PGMNAME;

      __program_name = (char *) basename (path);
    }
  return __program_name;
}

 /**/ char *
get_home_dir (void)
{
  static char *__home_dir = NULL;

  if (__home_dir == NULL)		/* Only if we don't have it already */
    {
      __home_dir = (char *) getenv ("HOME");

      if (__home_dir == NULL)		/* If $HOME isn't assigned */
	__home_dir = ".";		/* use current dir instead */
    }
  return __home_dir;
}

 /**/ inline struct Node *
RemHead (struct List *list)
{
  struct Node *node = list->lh_Head->ln_Succ;

  if (node != NULL)
    {
      node->ln_Pred = (struct Node *) list;
      node = list->lh_Head;
      list->lh_Head = node->ln_Succ;
    }
  return node;
}

 /**/ int
atos (char *ptr)
{
  char *endptr = (char *) NULL;
  int secs = strtol (ptr, &endptr, 0);

  if (secs > 0)
    {
      static const char fmt[] =
	"%s: warning: number of %s should be less than %d\n";

      switch (*endptr)
	{
	case 'y':			/* Years */
	case 'Y':
	  if (secs > (0x7FFFFFFF / SECS_PER_YEAR))
	    (void) fprintf (stderr, fmt, get_program_name (NULL), "years",
			    (0x7FFFFFFF / SECS_PER_YEAR) + 1);
	  secs *= SECS_PER_YEAR;
	  break;
	case 'w':			/* Weeks */
	case 'W':
	  if (secs > (0x7FFFFFFF / SECS_PER_WEEK))
	    (void) fprintf (stderr, fmt, get_program_name (NULL), "weeks",
			    (0x7FFFFFFF / SECS_PER_WEEK) + 1);
	  secs *= 7;
	  /* fall through */
	case 'd':			/* Days */
	case 'D':
	  if (secs > (0x7FFFFFFF / SECS_PER_DAY))
	    (void) fprintf (stderr, fmt, get_program_name (NULL), "days",
			    (0x7FFFFFFF / SECS_PER_DAY) + 1);
	  secs *= 24;
	  /* fall through */
	case 'h':			/* Hours */
	case 'H':
	  if (secs > (0x7FFFFFFF / SECS_PER_HOUR))
	    (void) fprintf (stderr, fmt, get_program_name (NULL), "hours",
			    (0x7FFFFFFF / SECS_PER_HOUR) + 1);
	  secs *= 60;
	  /* fall through */
	case 'm':			/* Minutes */
	case 'M':
	  if (secs > (0x7FFFFFFF / SECS_PER_MIN))
	    (void) fprintf (stderr, fmt, get_program_name (NULL), "minutes",
			    (0x7FFFFFFF / SECS_PER_MIN) + 1);
	  secs *= 60;
	  /* fall through */
	case 's':			/* Seconds */
	case 'S':
	case '\0':
	  break;
	default:
	  errno = EDOM;
	  perror (ptr);
	  exit (EXIT_FAILURE);
	}
    }
  return secs;
}

 /**/ int *
fill_in (int *ary, int data, char *type)
{
  if (data)
    {
      ary[0] = data;
      ary[1] = (int) type;
      ary[2] = (int) (data == 1 ? "" : "s");

      return ary + 3;
    }
  return ary;
}

 /**/ int
file_exists (char *str)
{
  char *name = str ? str : "";
  struct stat *sb = (struct stat *) alloca (sizeof (*sb));

  if (stat (name, sb) == 0)
    {
      int mode = (int) sb->st_mode;

      if (S_ISREG (mode) || S_ISLNK (mode))
	return 1;
    }
  return 0;
}

 /**/ void
init_jedstate (char *myname)
{
  char dbname[PATH_MAX + 1];

  (void) setlocale (LC_ALL, "");
  (void) get_program_name (myname);

  (void) sprintf (dbname, "%s/.jedstate.db", get_home_dir ());

  if (!file_exists (dbname))		/* Create database */
    gdbm_close (opendb (GDBM_NEWDB));

  opterr = 0;
}

 /**/ GDBM_FILE
opendb (int rw)
{
  char dbname[PATH_MAX + 1];
  GDBM_FILE db;

  (void) sprintf (dbname, "%s/.jedstate.db", get_home_dir ());

  if ((db = gdbm_open (dbname, 0, rw, 0640, NULL)) == NULL)
    {
      (void) fprintf (stderr, "%s: cannot open %s\n", get_program_name (NULL),
		      dbname);
      exit (EXIT_FAILURE);
    }
  return db;
}

 /**/ int
set_jedstate (char *filename, char *line, char *column)
{
  datum key;
  int rc = EXIT_SUCCESS;
  GDBM_FILE db = opendb (GDBM_WRCREAT);

  key.dptr = filename;
  key.dsize = strlen (filename) + 1;

  if (file_exists (filename))
    {
      if ((atol (line) > 1) || (atol (column) > 1))
	{
	  datum val;
	  char data[PATH_MAX + 34];	/* Assuming 32-bit ints */

	  val.dptr = data;
	  val.dsize =
	    sprintf (data, "%s\t%s\t%d", column, line, (int) time (NULL)) + 1;

	  if (gdbm_exists (db, key))	/* Replace current entry */
	    rc = gdbm_store (db, key, val, GDBM_REPLACE);
	  else				/* Insert new key=val pair */
	    rc = gdbm_store (db, key, val, GDBM_INSERT);
	}
      else /* We're at bof */ if (gdbm_exists (db, key))	/* If the file has an entry in the database */
	rc = gdbm_delete (db, key);	/* -- dereference it... */
    }
  else /* File doesn't exist */ if (gdbm_exists (db, key))	/* If the file has an entry in the database */
    rc = gdbm_delete (db, key);		/* -- dereference it... */

  gdbm_close (db);

  return rc ? EXIT_FAILURE : EXIT_SUCCESS;
}

 /**/ int
get_jedstate (char *filename)
{
  GDBM_FILE db = opendb (GDBM_READER);
  datum key, val;

  key.dptr = filename;
  key.dsize = strlen (filename) + 1;

  val = gdbm_fetch (db, key);

  if (val.dptr)
    {
      FILE *fp;
      char jsname[PATH_MAX + 1];

      (void) sprintf (jsname, "%s/.jedstate.sl", get_home_dir ());

      if ((fp = fopen (jsname, "w")) == NULL)
	{
	  (void) fprintf (stderr, "%s: cannot open %s\n",
			  get_program_name (NULL), jsname);
	  gdbm_close (db);
	  exit (EXIT_FAILURE);
	}

      (void) fputs ("%% -*- SLang -*-\n", fp);
      (void) fprintf (fp, "%% Jedstate for %s\n", filename);
      (void) fprintf (fp, "%% Generated by %s\n", PgmID + 4);
      (void) fprintf (fp, ". %s pop goto_line goto_column_best_try pop\n",
		      val.dptr);
      (void) fputs ("%% EoF\n", fp);

      (void) fclose (fp);
    }
  gdbm_close (db);

  return EXIT_SUCCESS;
}

 /**/ int
purge_jedstate (int secs)
{
  int rc;
  time_t now;
  GDBM_FILE db;
  datum key, val;
  LIST *list;

  if (secs < 0)				/* Keep all references */
    return EXIT_SUCCESS;

  if (secs == 0)			/* Clear database */
    {
      gdbm_close (opendb (GDBM_NEWDB));
      return EXIT_SUCCESS;
    }
  rc = 0;
  db = opendb (GDBM_READER);
  key = gdbm_firstkey (db);
  list = (LIST *) alloca (sizeof (*list));

  /* AKA NewList(list) */
  list->lh_Head = (NODE *) & list->lh_Tail;
  list->lh_Tail = (NODE *) NULL;
  list->lh_TailPred = (NODE *) & list->lh_Head;

  now = time (NULL);

  while (key.dptr)
    {
      int keep_it = file_exists (key.dptr);

      if (keep_it)
	{
	  int then = now;
	  char *ptr;

	  val = gdbm_fetch (db, key);
	  ptr = strrchr (val.dptr, '\t');

	  if (ptr)
	    if (*(ptr + 1))
	      then = atol (ptr + 1);

	  if ((now - then) > secs)
	    keep_it = 0;
	}

      if (!keep_it)
	{
	  NODE *node = (NODE *) alloca (sizeof (*node) + key.dsize);

	  node->ln_Name = (char *) (node + 1);
	  (void) memcpy (node->ln_Name, key.dptr, key.dsize);

	  /* AKA AddTail(list,node) */
	  node->ln_Pred = list->lh_TailPred;
	  node->ln_Succ = (NODE *) & list->lh_Tail;
	  list->lh_TailPred = node;
	  node->ln_Pred->ln_Succ = node;
	}
      key = gdbm_nextkey (db, key);
    }
  gdbm_close (db);

  /* Is list empty? */
  if (list->lh_TailPred != (NODE *) list)
    {
      static char *tpls[] = {
	"0 seconds",
	"%d %s%s",
	"%d %s%s and %d %s%s",
	"%d %s%s, %d %s%s and %d %s%s",
	"%d %s%s, %d %s%s, %d %s%s and %d %s%s",
	"%d %s%s, %d %s%s, %d %s%s, %d %s%s and %d %s%s",
	"a very long time"		/* ;-) */
      };
      char *done, *fmt_tpl, fmt_str[256];
      NODE *node;
      int num = 0;
      int ary[16];
      int *ptr;
      int weeks, days, hours, mins;

      weeks = secs / SECS_PER_WEEK;
      secs %= SECS_PER_WEEK;
      days = secs / SECS_PER_DAY;
      secs %= SECS_PER_DAY;
      hours = secs / SECS_PER_HOUR;
      secs %= SECS_PER_HOUR;
      mins = secs / SECS_PER_MIN;
      secs %= SECS_PER_MIN;

      if (weeks > 1000)
	fmt_tpl = tpls[6];
      else
	fmt_tpl =
	  tpls[(weeks > 0) + (days > 0) + (hours > 0) + (mins > 0) + (secs >
								      0)];

      ptr = memset (ary, 0, sizeof (ary));

      ptr = fill_in (ptr, weeks, "week");
      ptr = fill_in (ptr, days, "day");
      ptr = fill_in (ptr, hours, "hour");
      ptr = fill_in (ptr, mins, "minute");
      ptr = fill_in (ptr, secs, "second");

      (void) sprintf (fmt_str, "Purging entries not viewed for %s:\n\n",
		      fmt_tpl);

      db = opendb (GDBM_WRITER);

      (void) vprintf (fmt_str, ary);

      done =
	isatty (fileno (stdout)) ? "\r\x1B[K - dereferenced `%s'\n" : NULL;

      while ((node = RemHead (list)) != NULL)
	{
	  key.dptr = node->ln_Name;
	  key.dsize = strlen (node->ln_Name) + 1;

	  (void) printf (" - dereferencing `%s'...", node->ln_Name);
	  (void) fflush (stdout);

	  rc += gdbm_delete (db, key);

	  if (done)
	    (void) printf (done, node->ln_Name);
	  else
	    (void) putchar ('\n');

	  ++num;
	}
      (void) putchar ('\n');

      rc += gdbm_reorganize (db);
      gdbm_close (db);

      (void) printf ("Purged %d name%s.\n", num, num == 1 ? "" : "s");
    }
  return rc ? EXIT_FAILURE : EXIT_SUCCESS;
}

 /**/ int
main (int argc, char *const *argv)
{
  int secs, opt, idx;
  char *program_name;
  static const struct option long_opts[] = {
    {"copying-policy", 0, NULL, 'c'},	/* print copying policy */
    {"example", 2, NULL, 'x'},		/* print example hooks to stdout */
    {"help", 0, NULL, 'h'},		/* print brief help text */
    {"purge", 1, NULL, 'p'},		/* purge entries older than argument */
    {"quiet", 0, NULL, 'q'},		/* suppress stdout */
    {"really-quiet", 0, NULL, 'Q'},	/* suppress stdout and stderr */
    {"url", 0, NULL, 'u'},		/* print URL of homepage */
    {"version", 0, NULL, 'V'},		/* print version information */
    END_LONG_OPTS
  };

  secs = idx = -1;
  init_jedstate (argv[0]);

  while ((opt = getopt_long (argc, argv, SHORT_OPTS, long_opts, &idx)) != EOF)
    {
      switch (opt)
	{
	case 'p':
	  if (optarg == NULL)
	    secs = 0x7FFFFFFF;
	  else
	    secs = atos (optarg);
	  break;
	case 'x':
	  if (optarg && (optarg[0] == '2' && optarg[1] == 0))
	    puts (SAMPLE_HOOK_2);
	  else
	    puts (SAMPLE_HOOK_1);
	  exit (EXIT_SUCCESS);
	  /* break; */
	case 'Q':
	  if (freopen ("/dev/null", "w", stderr) == NULL)
	    {
	      (void) fprintf (stderr,
			      "%s: cannot assign stderr to /dev/null\n",
			      get_program_name (NULL));
	      exit (EXIT_FAILURE);
	    }
	  /* Fall through */
	case 'q':
	  if (freopen ("/dev/null", "w", stdout) == NULL)
	    {
	      (void) fprintf (stderr,
			      "%s: cannot assign stdout to /dev/null\n",
			      get_program_name (NULL));
	      exit (EXIT_FAILURE);
	    }
	  break;
	case 'u':
	  (void) puts (MYURL);
	  exit (EXIT_SUCCESS);
	  /* break; */
	case 'c':
	  (void) puts (MINIGPL);
	  exit (EXIT_SUCCESS);
	  /* break; */
	case 'h':
	  (void) printf (USAGE, get_program_name (NULL));
	  exit (EXIT_SUCCESS);
	  /* break; */
	case 'V':
	  (void) puts (PgmID + 4);
	  exit (EXIT_SUCCESS);
	  /* break; */
	default:
	  (void) fprintf (stderr, "Try `%s --help' for more information.\n",
			  get_program_name (NULL));
	  exit (EXIT_FAILURE);
	  /* break; */
	}
    }
  argv += optind;
  argc -= optind;

  program_name = get_program_name (NULL);

  if (strcmp (program_name, "getjedstate") == 0)
    {
      if (argc == 1)
	idx = get_jedstate (argv[0]);
      else
	{
	  (void) fputs ("syntax: getjedstate <filename>\n", stderr);
	  idx = EXIT_FAILURE;
	}
    }
  else if (strcmp (program_name, "setjedstate") == 0)
    {
      if (argc == 3)
	idx = set_jedstate (argv[0], argv[1], argv[2]);
      else
	{
	  (void) fputs ("syntax: setjedstate <filename> <line> <column>\n",
			stderr);
	  idx = EXIT_FAILURE;
	}
    }
  else
    idx = purge_jedstate (secs);

  exit (idx);
  /* NOTREACHED */
  return -1;
}

/* Soli Deo Gloria */
