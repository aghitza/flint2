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

    Copyright (C) 2010-2012 Fredrik Johansson

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "nmod_poly_mat.h"
#include "perm.h"
#include "ulong_extras.h"

/* checks that the rref has the right form */
int check_rref(const nmod_poly_mat_t A, const nmod_poly_t den, slong rank)
{
    slong i, j, k, prev_pivot;

    /* bottom should be zero */
    for (i = rank; i < A->r; i++)
        for (j = 0; j < A->c; j++)
            if (!nmod_poly_is_zero(nmod_poly_mat_entry(A, i, j)))
                return 0;

    prev_pivot = -1;

    for (i = 0; i < rank; i++)
    {
        for (j = 0; j < A->c; j++)
        {
            if (!nmod_poly_is_zero(nmod_poly_mat_entry(A, i, j)))
            {
                /* pivot should have a higher column index than previous */
                if (j <= prev_pivot)
                    return 0;

                /* column should be 0 ... 0 1 0 ... 0 */
                for (k = 0; k < rank; k++)
                {
                    if (i == k && !nmod_poly_equal(nmod_poly_mat_entry(A, k, j), den))
                        return 0;
                    if (i != k && !nmod_poly_is_zero(nmod_poly_mat_entry(A, k, j)))
                        return 0;
                }

                prev_pivot = j;
                break;
            }
        }
    }

    return 1;
}

int
main(void)
{
    slong iter;
    flint_rand_t state;

    printf("rref....");
    fflush(stdout);

    flint_randinit(state);

    for (iter = 0; iter < 1000 * flint_test_multiplier(); iter++)
    {
        nmod_poly_mat_t A, R, B, R2;
        nmod_poly_t den, c, den2;
        slong j, k, m, n, deg, rank1, rank2;
        slong *perm;
        float density;
        int equal;
        mp_limb_t p;

        m = n_randint(state, 10);
        n = n_randint(state, 10);
        deg = 1 + n_randint(state, 5);
        density = n_randint(state, 100) * 0.01;
        p = n_randtest_prime(state, 0);

        nmod_poly_mat_init(A, m, n, p);
        nmod_poly_mat_init(R, m, n, p);
        nmod_poly_mat_init(B, 2 * m, n, p);
        nmod_poly_mat_init(R2, 2 * m, n, p);

        nmod_poly_init(c, p);
        nmod_poly_init(den, p);
        nmod_poly_init(den2, p);

        perm = _perm_init(2 * m);

        nmod_poly_mat_randtest_sparse(A, state, deg, density);

        rank1 = nmod_poly_mat_rref(R, den, A);

        check_rref(R, den, rank1);

        /* Concatenate the original matrix with the rref, scramble the rows,
            and check that the rref is the same */
        _perm_randtest(perm, 2 * m, state);

        for (j = 0; j < m; j++)
        {
            nmod_poly_randtest_not_zero(c, state, deg);
            for (k = 0; k < n; k++)
                nmod_poly_mul(nmod_poly_mat_entry(B, perm[j], k), nmod_poly_mat_entry(A, j, k), c);
        }

        for (j = 0; j < m; j++)
        {
            nmod_poly_randtest_not_zero(c, state, deg);
            for (k = 0; k < n; k++)
                nmod_poly_mul(nmod_poly_mat_entry(B, perm[m + j], k), nmod_poly_mat_entry(R, j, k), c);
        }

        rank2 = nmod_poly_mat_rref(R2, den2, B);
        equal = (rank1 == rank2);

        if (equal)
        {
            nmod_poly_mat_scalar_mul_nmod_poly(R, R, den2);
            nmod_poly_mat_scalar_mul_nmod_poly(R2, R2, den);

            for (j = 0; j < rank2; j++)
                for (k = 0; k < n; k++)
                    equal = equal &&
                        nmod_poly_equal(nmod_poly_mat_entry(R, j, k), nmod_poly_mat_entry(R2, j, k));
            for (j = rank2; j < 2 * rank2; j++)
                for (k = 0; k < n; k++)
                    equal = equal && nmod_poly_is_zero(nmod_poly_mat_entry(R2, j, k));
        }

        if (!equal)
        {
            printf("FAIL (rank1 = %ld, rank2 = %ld)!\n", rank1, rank2);
            nmod_poly_mat_print(A, "x"); printf("\n\n");
            nmod_poly_mat_print(R, "x"); printf("\n\n");
            nmod_poly_mat_print(R2, "x"); printf("\n\n");
            abort();
        }

        nmod_poly_clear(c);
        nmod_poly_clear(den);
        nmod_poly_clear(den2);

        _perm_clear(perm);

        nmod_poly_mat_clear(A);
        nmod_poly_mat_clear(R);
        nmod_poly_mat_clear(B);
        nmod_poly_mat_clear(R2);
    }

    flint_randclear(state);
    printf("PASS\n");
    return 0;
}

