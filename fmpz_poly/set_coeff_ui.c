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

    Copyright (C) 2008, 2009 William Hart
   
******************************************************************************/

#include <gmp.h>
#include <stdlib.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_poly.h"

void
fmpz_poly_set_coeff_ui(fmpz_poly_t poly, slong n, ulong x)
{
    fmpz_poly_fit_length(poly, n + 1);

    if (n + 1 > poly->length)   /* insert zeroes between end of poly and new coeff if needed */
    {
        flint_mpn_zero((mp_ptr) (poly->coeffs + poly->length), n - poly->length);
        poly->length = n + 1;
    }

    fmpz_set_ui(poly->coeffs + n, x);
    _fmpz_poly_normalise(poly); /* we may have set leading coefficient to zero */
}
