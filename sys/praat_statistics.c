/* praat_statistics.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/03/07 GPL
 * pb 2002/03/09 OSX no mention of free memory
 * pb 2004/10/18 more big integers
 * pb 2005/03/02 pref string 260 bytes long
 */

#include <time.h>
#include "praatP.h"

static struct {
	long batchSessions, interactiveSessions;
	double memory;
	char dateOfFirstSession [Resources_STRING_BUFFER_SIZE];
} statistics;

void praat_statistics_prefs (void) {
	Resources_addLong ("PraatShell.batchSessions", & statistics.batchSessions);
	Resources_addLong ("PraatShell.interactiveSessions", & statistics.interactiveSessions);
	Resources_addString ("PraatShell.dateOfFirstSession", & statistics.dateOfFirstSession [0]);
	Resources_addDouble ("PraatShell.memory", & statistics.memory);
}

void praat_statistics_prefsChanged (void) {
	if (! statistics.dateOfFirstSession [0]) {
		time_t today = time (NULL);
		char *newLine;
		strcpy (statistics.dateOfFirstSession, ctime (& today));
		newLine = strchr (statistics.dateOfFirstSession, '\n');
		if (newLine) *newLine = '\0';
	}
	if (praat.batch)
		statistics.batchSessions += 1;
	else
		statistics.interactiveSessions += 1;
}

void praat_statistics_exit (void) {
	statistics.memory += Melder_allocationSize ();
}

#if defined (macintosh) && ! defined (__MACH__)
	#include <Memory.h>
#endif
void praat_memoryInfo (void) {
	MelderInfo_open ();
	MelderInfo_writeLine2 ("Currently allocated: ", Melder_bigInteger (Melder_allocationCount () - Melder_deallocationCount ()));
	MelderInfo_writeLine5 ("   Things: ", Melder_integer (Thing_getTotalNumberOfThings ()),
		" (objects in list: ", Melder_integer (praat.n), ")");
	MelderInfo_writeLine2 ("   Arrays: ", Melder_integer (NUM_getTotalNumberOfArrays ()));
	#if defined (macintosh) && ! defined (__MACH__)
	{
		long grow;
		MelderInfo_writeLine5 ("\nFree memory: ", Melder_bigInteger (FreeMem ()), " bytes (contiguous: ",
			Melder_bigInteger (MaxMem (& grow)), " bytes)");
	}
	#endif
	MelderInfo_writeLine5 (
		"\nHistory of this session:\n"
		"   Allocation: ", Melder_bigInteger (Melder_allocationCount ()), " (", Melder_bigInteger (Melder_allocationSize ()), " bytes)");
	MelderInfo_writeLine2 ("   Deallocation: ", Melder_bigInteger (Melder_deallocationCount ()));
	MelderInfo_writeLine3 ("\nHistory of all sessions from ", statistics.dateOfFirstSession, " until today:");
	MelderInfo_writeLine4 ("   Interactive sessions: ", Melder_integer (statistics.interactiveSessions),
		" batch sessions: ", Melder_integer (statistics.batchSessions));
	MelderInfo_writeLine3 ("   Total memory use: ", Melder_bigInteger (statistics.memory + Melder_allocationSize ()), " bytes");
	MelderInfo_close ();
}

/* End of file praat_statistics.c */

