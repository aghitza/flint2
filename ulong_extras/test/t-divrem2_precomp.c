/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2009 William Hart

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "ulong_extras.h"

int main(void)
{
   int result;
   ulong i;
   flint_rand_t state;
   flint_randinit(state);

   printf("divrem2_precomp....");
   fflush(stdout);

   for (i = 0; i < 100000 * flint_test_multiplier(); i++)
   {
      mp_limb_t d, n, r1, r2, q1, q2;
      double dpre;

      d = n_randtest(state);
      if (d == 0UL) d++;
  
      n = n_randtest(state);
      
      dpre = n_precompute_inverse(d);

      r1 = n_divrem2_precomp(&q1, n, d, dpre);
      r2 = n%d;
      q2 = n/d;

      result = ((r1 == r2) && (q1 == q2));
      if (!result)
      {
         printf("FAIL:\n");
         printf("n = %lu, d = %lu, dpre = %f\n", n, d, dpre); 
         printf("q1 = %lu, q2 = %lu, r1 = %lu, r2 = %lu\n", q1, q2, r1, r2);
         abort();
      }
   }

   flint_randclear(state);

   printf("PASS\n");
   return 0;
}
