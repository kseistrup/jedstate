%% -*- SLang -*-
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Copyright © 1996-99 Klaus Alexander Seistrup @ Magnetic Ink, DK-2860
%%
%%   Author: Klaus Alexander Seistrup
%%  Created: Sun Oct 20 23:48:27 MET DST 1996
%% @(#) $Id: sample-hook-1.sl,v 1.2 1999/07/21 18:57:07 kas Exp $
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

. [Jed_State_File] "HOME" getenv ".jedstate.sl" dircat =Jed_State_File

% Startup hook that utilizes getjedstate
. ( BATCH { % not evaluated for batch processes
.     "getjedstate \"%s\"" buffer_filename 1 Sprintf system pop
.     Jed_State_File dup file_status 1 == { % if we have a .jedstate.sl file
.       dup evalfile pop delete_file % evaluate command, then delete source
.     } if
.     pop
.   } !if
. ) startup_hook

% Exit hook that utilizes setjedstate
. ( BATCH { % not evaluated for batch processes
.     buffer_filename
.     dup strlen { % only if we actually have a file name
.       "setjedstate \"%s\" %d %d" % setjedstate file line col
.       exch what_line what_column 3 Sprintf system
.     } if
.     pop % pops buffer_filename() or system()
.     exit_jed
.   } !if
. ) exit_hook

%% EOF %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
