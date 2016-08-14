%% -*- SLang -*-
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Copyright © 1996-99 Klaus Alexander Seistrup @ Magnetic Ink, DK-2860
%%
%%   Author: Klaus Alexander Seistrup
%%  Created: Sun Oct 20 23:48:26 MET DST 1996
%% @(#) $Id: sample-hook-2.sl,v 1.2 1999/07/21 18:57:07 kas Exp $
%%
%% This program is free software; you can redistribute it and/or modify it
%% under the terms of the GNU General Public License as published by the Free
%% Software Foundation; either version 2 of the License, or (at your option)
%% any later version.
%%
%% This program is distributed in the hope that it will be useful, but with-
%% out any warranty; without even the implied warranty of merchantability or
%% fitness for a particular purpose.  See the GNU General Public License for
%% more details.
%%
%% You should have received a copy of the GNU General Public License along
%% with this program; if not, please write to the Free Software Foundation,
%% Inc., 59 Temple Place, Suite 330, Boston MA 02111-1307, USA.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

variable Jed_State_File = dircat (getenv ("HOME"), ".jedstate.sl");

% Startup hook that utilizes getjedstate
define startup_hook ()
{
  !if (BATCH)				% not evaluated for batch processes
    {
      $1 = Sprintf ("getjedstate \"%s\"", buffer_filename, 1);
      () = system ($1);

      if (file_status (Jed_State_File) == 1)    % if we have a .jedstate.sl
	{
	  () = evalfile (Jed_State_File);       % evaluate command
	  () = delete_file (Jed_State_File);    % - then delete source
	}
    }
}

% Exit hook that utilizes setjedstate
define exit_hook ()
{
  !if (BATCH)                           % not evaluated for batch processes
    {
      variable fname = buffer_filename ();

      if (strlen (fname))		% only if we actually have a file name
	{
	  variable line = what_line ();	% active line
	  variable col = what_column ();% - and column

	  $1 = Sprintf ("setjedstate \"%s\" %d %d", fname, line, col, 3);
	  () = system ($1);		% do it
	}
      exit_jed ();
    }
}

%% EOF %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
