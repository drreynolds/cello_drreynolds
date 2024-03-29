c     See LICENSE_ENZO file for license and copyright information

#include "fortran.h"

c=======================================================================
c/////////////////////////  SUBROUTINE PPM_DE  \\\\\\\\\\\\\\\\\\\\\\\\\
c
      subroutine ppm_de(d, e, u, v, w, ge,
     &                  gravity, gr_xacc, gr_yacc, gr_zacc,
     &                  gamma, dt, nhy, dx, dy, dz,
     &                  rank, in, jn, kn, start, pend,
     &                  iflatten, ipresfree,
     &                  idiff, isteepen, idual, eta1, eta2,
     &                  nsubgrids, lface, rface,
     &                  fistart, fiend, fjstart, fjend,
     &                  array, dindex, eindex,
     &                  uindex, vindex, windex, geindex, tmp,
     &                  ncolor, colorpt, coloff, colindex)
c
c  PERFORMS PPM (DIRECT EULERIAN) UPDATE FOR ONE TIMESTEP (WRAPPER)
c
c  written by: Greg Bryan
c       (some of this is based on code originally written by Jim Stone)
c  date:       June, 1993
c  modified1:  November, 1994; by GB
c              Modified to work as an entirely callable routine within
c              an AMR package.  There is now only one exterior parameter:
c              MAX_ANY_SINGLE_DIRECTION.
c  modified2:  Summer(?), 2002; by Robert Harkness
c              Extended sweeps into ghost zones to update boundary conditions
c              within the directionally split scheme.
c  modified3:  July, 2003; by Alexei Kritsuk
c              Corrected permutation pattern for 2D runs. 
c
c  PURPOSE:  Advances the fluid equations by one timestep using the
c    PPM Direct Eulerian algorithm in 3-D.  This routine is a callable
c    wrapper for the 1d sweeps.  It assumes (in classic fortran style)
c    that the arguments are defined to dimension three; however, it does
c    the right thing if called with one or more collapsed dimensions
c    (i.e. with the width set to 1).
c
c  EXTERNALS:
c    x,y,zeuler_sweep - routines to compute the Eulerian step in
c                       one dimension
c
c  INPUTS:
c     d       - density field (includes boundary zones)
c     dx,y,z  - zone width arrays for each dimension
c     e       - total specific energy field
c     pend     - array (of dimension 3) specifying the end of the active
c               region for reach dimension (zero based)
c     eta1    - (dual) selection parameter for gas energy (typically ~0.1)
c     eta2    - (dual) selection parameter for total energy (typically ~0.001)
c     ge      - gas energy (used when idual = 1)
c     gr_x,y,zacc - gravitational acceleration fields
c     gravity - flag indicating whether or not to use gravity field (1 = yes)
c     i,j,kn  - dimensions of field arrays
c     idiff   - diffusion flag (0 = off)
c     idual   - dual energy formalism flag (0 = off)
c     ipresfree - pressure free flag (0 = off, 1 = on, i.e. p=0)
c     nhy     - cycle number (for better operator splitting)
c     rank    - dimension of problem (unused until modification3)
c     start   - array (of dimension 3) specifying the start of the active
c               region fo reach dimension (zero based)
c     tmp     - temporary work space (30 * largest_slice)
c     u       - x-velocity field
c     v       - y-velocity field
c     w       - z-velocity field
c
c  LOCALS:
c
c-----------------------------------------------------------------------
      implicit NONE
c-----------------------------------------------------------------------
c
c  Arguments
c
      integer gravity, idiff, idual, iflatten, isteepen, nhy, rank,
     &        ipresfree, pend(3), in, jn, kn, nsubgrids, start(3),
     &        ncolor, coloff(ncolor)
      integer fistart(nsubgrids*3), fiend(nsubgrids*3),
     &        fjstart(nsubgrids*3), fjend(nsubgrids*3), 
     &        lface(nsubgrids*3), rface(nsubgrids*3)
      integer dindex(nsubgrids*6), eindex(nsubgrids*6),
     &        uindex(nsubgrids*6), vindex(nsubgrids*6),
     &        windex(nsubgrids*6),geindex(nsubgrids*6),
     &        colindex(nsubgrids*6,ncolor)
      ENZO_REAL d(in,jn,kn), e(in,jn,kn), u(in,jn,kn),
     &     v(in,jn,kn), w(in,jn,kn),ge(in,jn,jn),
     &     gr_xacc(in,jn,kn), gr_yacc(in,jn,kn), gr_zacc(in,jn,kn),
     &        dx,dy,dz
      ENZO_REAL dt, eta1, eta2, gamma, pmin
      ENZO_REAL array(1), colorpt(1)
c
c  Parameters
c
c
c  Locals
c
      integer i, ie, is, ixyz, j, je, js, k, ke, ks,
     &        n, nxz, nyz, nzz, ms
      integer i1, i2, j1, j2, k1, k2
      ENZO_REAL tmp(1)
c
c\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\////////////////////////////////
c=======================================================================
c
c  Error check
c     
C      print*, 'd = ',d(i1,j1,k1)
C      print*, 'e = ',e(i1,j1,k1)
C      print*, 'u = ',u(i1,j1,k1)
C      print*, 'v = ',v(i1,j1,k1)
C      print*, 'w = ',w(i1,j1,k1)
C      print*, 'ge = ',ge(i1,j1,k1)
C      print*, 'gravity = ',gravity
CC      print*, 'gr_xacc = ',gr_xacc(i1,j1,k1)
CC      print*, 'gr_yacc = ',gr_yacc(i1,j1,k1)
CC      print*, 'gr_zacc = ',gr_zacc(i1,j1,k1)
C      print*, 'gamma = ',gamma
C      print*, 'dt = ',dt
C      print*, 'nhy = ',nhy
C      print*, 'dx = ',dx
C      print*, 'dy = ',dy
C      print*, 'dz = ',dz
C      print*, 'rank = ',rank
C      print*, 'in = ',in
C      print*, 'jn = ',jn
C      print*, 'kn = ',kn
C      print*, 'start = ',start(1),start(2),start(3)
C      print*, 'pend = ',pend(1),pend(2),pend(3)
C      print*, 'iflatten = ',iflatten
C      print*, 'ipresfree = ',ipresfree
C      print*, 'idiff = ',idiff
C      print*, 'isteepen = ',isteepen
C      print*, 'idual = ',idual
C      print*, 'eta1 = ',eta1
C      print*, 'eta2 = ',eta2
C      print*, 'nsubgrids = ',nsubgrids
C      print*, 'lface = ',lface
C      print*, 'rface = ',rface
C      print*, 'fistart = ',fistart
C      print*, 'fiend = ',fiend
C      print*, 'fjstart = ',fjstart
C      print*, 'fjend = ',fjend
C      print*, 'array = ',array
C      print*, 'dindex = ',dindex
C      print*, 'eindex = ',eindex
C      print*, 'uindex = ',uindex
C      print*, 'vindex = ',vindex
C      print*, 'windex = ',windex
C      print*, 'geindex = ',geindex
C      print*, 'tmp = ',tmp
C      print*, 'ncolor = ',ncolor
C      print*, 'colorpt = ',colorpt
C      print*, 'coloff = ',coloff
C      print*, 'colindex = ',colindex
      
      if (max(in,jn,kn) .gt. MAX_ANY_SINGLE_DIRECTION) then
         write(6,*) 'PPM_DE: A grid dimension is too long.'
         write(6,*) '   (increase MAX_ANY_SINGLE_DIRECTION.)'
         write(6,*) 'in=',in,'jn=',jn,'kn=',kn,'max=',
     *        MAX_ANY_SINGLE_DIRECTION
         stop 
      endif
c
c  Convert arguments to usable form
c
      is = start(1) + 1
      js = start(2) + 1
      ks = start(3) + 1
      ie = pend(1) + 1
      je = pend(2) + 1
      ke = pend(3) + 1
      ms = max(in*jn, jn*kn, kn*in)
      nxz = ie-is+1
      nyz = je-js+1
      nzz = ke-ks+1

      i1 = 1
      i2 = in
      j1 = 1
      j2 = jn
      k1 = 1
      k2 = kn

!     i1 = is
!     i2 = ie
!     j1 = js
!     j2 = je
!     k1 = ks
!     k2 = ke



!     write(*,'("AAARRGS",9i5)') is,ie,in,js,je,jn,ks,ke,kn
c
c  Set minimum pressure (better if it were a parameter)
c
      pmin = tiny
c
c  Loop over directions, using a Strang-type splitting
c
!      ixyz = mod(nhy,3)
!      do n=ixyz,ixyz+2
      ixyz = mod(nhy,rank)
      do n=ixyz,ixyz+rank-1
c
c  Update in x-direction
c
!        if (mod(n,3) .eq. 0 .and. nxz .gt. 1) then
        if (mod(n,rank) .eq. 0 .and. nxz .gt. 1) then
c
c         write (6,*) 'SOLVER: starting x-sweep'
c
c*$* ASSERT CONCURRENT CALL
c$DOACROSS LOCAL(k)
!         do k=ks, ke
          do k=k1, k2
            call xeuler_sweep(k, d, e, u, v, w, ge, in, jn, kn,
     &        gravity, gr_xacc, idual, eta1, eta2,
     &        is, ie, js, je, ks, ke,
     &        gamma, pmin, dt, dx, dy, dz,
     &        idiff, iflatten, isteepen, ipresfree,
     &        nsubgrids, lface, rface,
     &        fistart, fiend, fjstart, fjend,
     &        dindex, eindex, geindex,
     &        uindex, vindex, windex, array,
     &        ncolor, colorpt, coloff, colindex,
     &        tmp(1+ms*0),  tmp(1+ms*1),  tmp(1+ms*2),  tmp(1+ms*3),
     &        tmp(1+ms*4),  tmp(1+ms*5),  tmp(1+ms*6),  tmp(1+ms*7),
     &        tmp(1+ms*8),  tmp(1+ms*9),  tmp(1+ms*10), tmp(1+ms*11),
     &        tmp(1+ms*12), tmp(1+ms*13), tmp(1+ms*14), tmp(1+ms*15),
     &        tmp(1+ms*16), tmp(1+ms*17), tmp(1+ms*18), tmp(1+ms*19),
     &        tmp(1+ms*20), tmp(1+ms*21), tmp(1+ms*22), tmp(1+ms*23),
     &        tmp(1+ms*24), tmp(1+ms*25), tmp(1+ms*26), tmp(1+ms*27),
     &        tmp(1+ms*28), tmp(1+ms*29),
     &        tmp(1+ms*(30+0*ncolor)), tmp(1+ms*(30+1*ncolor)), 
     &        tmp(1+ms*(30+2*ncolor)), tmp(1+ms*(30+3*ncolor))
     &                       )
          enddo
c
        endif
c
c  Update in y-direction
c
!        if (mod(n,3) .eq. 1 .and. nyz .gt. 1) then
        if (mod(n,rank) .eq. 1 .and. nyz .gt. 1) then
c
c         write (6,*) 'SOLVER: starting y-sweep'
c
c*$* ASSERT CONCURRENT CALL
c$DOACROSS LOCAL(i)
!         do i=is, ie
          do i=i1, i2
            call yeuler_sweep(i, d, e, u, v, w, ge, in, jn, kn,
     &        gravity, gr_yacc, idual, eta1, eta2,
     &        is, ie, js, je, ks, ke,
     &        gamma, pmin, dt, dx, dy, dz,
     &        idiff, iflatten, isteepen, ipresfree,
     &        nsubgrids, lface, rface,
     &        fistart, fiend, fjstart, fjend,
     &        dindex, eindex, geindex,
     &        uindex, vindex, windex, array,
     &        ncolor, colorpt, coloff, colindex,
     &        tmp(1+ms*0),  tmp(1+ms*1),  tmp(1+ms*2),  tmp(1+ms*3),
     &        tmp(1+ms*4),  tmp(1+ms*5),  tmp(1+ms*6),  tmp(1+ms*7),
     &        tmp(1+ms*8),  tmp(1+ms*9),  tmp(1+ms*10), tmp(1+ms*11),
     &        tmp(1+ms*12), tmp(1+ms*13), tmp(1+ms*14), tmp(1+ms*15),
     &        tmp(1+ms*16), tmp(1+ms*17), tmp(1+ms*18), tmp(1+ms*19),
     &        tmp(1+ms*20), tmp(1+ms*21), tmp(1+ms*22), tmp(1+ms*23),
     &        tmp(1+ms*24), tmp(1+ms*25), tmp(1+ms*26), tmp(1+ms*27),
     &        tmp(1+ms*28), tmp(1+ms*29),
     &        tmp(1+ms*(30+0*ncolor)), tmp(1+ms*(30+1*ncolor)), 
     &        tmp(1+ms*(30+2*ncolor)), tmp(1+ms*(30+3*ncolor))
     &                       )
          enddo
c
        endif
c
c  Update in z-direction
c
!        if (mod(n,3) .eq. 2 .and. nzz .gt. 1) then
        if (mod(n,rank) .eq. 2 .and. nzz .gt. 1) then
c
c         write (6,*) 'SOLVER: starting z-sweep'
c
c*$* ASSERT CONCURRENT CALL
c$DOACROSS LOCAL(j)
!         do j=js, je
          do j=j1, j2
            call zeuler_sweep(j, d, e, u, v, w, ge, in, jn, kn,
     &        gravity, gr_zacc, idual, eta1, eta2,
     &        is, ie, js, je, ks, ke,
     &        gamma, pmin, dt, dx, dy, dz,
     &        idiff, iflatten, isteepen, ipresfree,
     &        nsubgrids, lface, rface,
     &        fistart, fiend, fjstart, fjend,
     &        dindex, eindex, geindex,
     &        uindex, vindex, windex, array,
     &        ncolor, colorpt, coloff, colindex,
     &        tmp(1+ms*0),  tmp(1+ms*1),  tmp(1+ms*2),  tmp(1+ms*3),
     &        tmp(1+ms*4),  tmp(1+ms*5),  tmp(1+ms*6),  tmp(1+ms*7),
     &        tmp(1+ms*8),  tmp(1+ms*9),  tmp(1+ms*10), tmp(1+ms*11),
     &        tmp(1+ms*12), tmp(1+ms*13), tmp(1+ms*14), tmp(1+ms*15),
     &        tmp(1+ms*16), tmp(1+ms*17), tmp(1+ms*18), tmp(1+ms*19),
     &        tmp(1+ms*20), tmp(1+ms*21), tmp(1+ms*22), tmp(1+ms*23),
     &        tmp(1+ms*24), tmp(1+ms*25), tmp(1+ms*26), tmp(1+ms*27),
     &        tmp(1+ms*28), tmp(1+ms*29),
     &        tmp(1+ms*(30+0*ncolor)), tmp(1+ms*(30+1*ncolor)), 
     &        tmp(1+ms*(30+2*ncolor)), tmp(1+ms*(30+3*ncolor))
     &                       )
          enddo
c
        endif
c
      enddo
c
      return
      end
