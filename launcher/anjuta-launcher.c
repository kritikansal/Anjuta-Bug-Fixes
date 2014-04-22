/*
    anjuta-launcher.c
    Copyright (C) 2000  Kh. Naba Kumar Singh

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <glib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* #define DEBUG */

int
main (int argc, char **argv)
{
	pid_t pid;
	int status, i;
	char **arg_v;

	if (argc < 2)
	{
		printf ("\nUsage:\n");
		printf
			("anjuta-launcher program_name [program_parameter ... ]\n\n");
		exit (-1);
	}

	if (strcmp (argv[1], "--version") == 0)
	{
		printf ("anjuta-launcher version 0.1.2\n");
		exit (0);
	}
	if (strcmp (argv[1], "--__debug_terminal") == 0)
	{
		FILE *fp;
		
		if (argc != 3)
		{
			printf ("\nUsage:\n");
			printf ("anjuta-launcher program_name [program_parameter ... ]\n\n");
			exit (-1);
		}
		fp = fopen (argv[2], "w");
		if (fp == NULL)
		{
			g_warning ("Fatal Error: Cannot write to redirection file\n");
		} 
		else
		{
			char* tty_name;
			tty_name = ttyname(0);
			if (tty_name)
			{
				fprintf (fp, "%s\n", tty_name);
				fclose (fp);
				printf ("Debug Terminal for the process:\n");
				printf ("-------------------------------\n");
				while (1) pause ();
				exit (0);
			}
			else
			{
				fprintf (fp, "__ERROR__\n");
				fclose (fp);
				g_warning ("Fatal Error: Unexpected error -- Unknowm tty\n");
			}
		}
		getchar ();
		exit (-1);
	}

	arg_v = g_malloc ((argc) * sizeof (char *));
	printf ("EXECUTING:\n");
	for (i = 1; i < argc; i++)
	{
		printf ("%s ", argv[i]);
		arg_v[i - 1] = g_strdup (argv[i]);
	}
	printf ("\n----------------------------------------------\n");
	arg_v[i - 1] = NULL;

	if ((pid = fork ()) == 0)
	{
		execvp (argv[1], arg_v);
		g_error ("Unable to execute the command (not found)\n");
	}
	if (pid < 0)
	{
		printf ("\n----------------------------------------------\n");
		printf ("There was an error in launching the program\n");
		status = -1;
	}
	else
	{
		waitpid (pid, &status, 0);
		printf ("\n----------------------------------------------\n");

		waitpid (pid, &status, 0);
		
		if (WIFSIGNALED (status)) {
			int signal = WTERMSIG (status);

			printf ("Program has been terminated receiving signal %d (%s)\n", signal, g_strsignal (signal));
		} else if (WIFEXITED (status))
			printf ("Program exited successfully with errcode (%d)\n", WEXITSTATUS (status));
	}
	printf ("Press the Enter key to close this terminal ... \n");
	getchar ();
	return WEXITSTATUS (status);
}
