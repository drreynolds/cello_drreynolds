c     See LICENSE_ENZO file for license and copyright information

#include "fortran.h"
c=======================================================================
c///////////////////////  SUBROUTINE INTEULER  \\\\\\\\\\\\\\\\\\\\\\\\\
c
      subroutine inteuler(
     &            dslice, pslice, gravity, grslice, geslice,
     &            uslice, vslice, wslice, dxi, flatten,
     &            idim, jdim, i1, i2, j1, j2, idual, eta1, eta2,
     &            isteep, iflatten, dt, gamma, ipresfree,
     &            dls, drs, pls, prs, gels, gers,
     &            uls, urs, vls, vrs, wls, wrs,
     &            ncolor, colslice, colls, colrs
     &                    )
c
c  COMPUTES LEFT AND RIGHT EULERIAN INTERFACE VALUES FOR RIEMANN SOLVER
c
c  written by: Jim Stone
c  date:       January,1991
c  modified1:  June, 1993 by Greg Bryan (changed to eulerian)
c  modified2:  July, 1994 by Greg Bryan (changed to slicewise)
c  modified3:  April, 1995 by GB (added gravity)
c
c  PURPOSE:  Uses piecewise parabolic interpolation to compute left-
c    and right interface values to be fed into Riemann solver during a
c    one dimensional sweeps.  This version computes the Eulerian corrections
c    to the left and right states described in section three of Colella &
c    Woodward (1984), JCP.  The routine works on one two dimensional
c    slice at a time.
c
c  INPUT:
c    dslice - extracted 2d slice of the density, d
c    dt     - timestep in problem time
c    dxi    - distance between Eulerian zone edges in sweep direction
c    eta1   - (dual) selection parameter for gas energy (typically ~0.001)
c    flatten - ammount of flattening (calculated in calcdiss)
c    gamma  - ideal gas law constant
c    gravity - gravity flag (0 = off)
c    grslice - acceleration in this direction in this slice
c    i1,i2  - starting and ending addresses for dimension 1
c    idim   - declared leading dimension of slices
c    idual  - dual energy formalism flag (0 = off)
c    iflatten - integer flag for flattener (eq. A1, A2) (0 = off)
c    isteep - integer flag for steepener (eq. 1.14,1.17,3.2) (0 = off)
c    ipresfree - pressure free flag (0 = off, 1 = on, i.e. p=0)
c    j1,j2  - starting and ending addresses for dimension 2
c    jdim   - declared second dimension of slices
c    pslice - extracted 2d slice of the pressure, p
c    uslice - extracted 2d slice of the 1-velocity, u
c    vslice - extracted 2d slice of the 2-velocity, v
c    wslice - extracted 2d slice of the 3-velocity, w
c    
c  OUTPUT:
c    dl,rs  - density at left and right edges of each cell
c    pl,rs  - pressure at left and right edges of each cell
c    ul,rs  - 1-velocity at left and right edges of each cell
c    vl,rs  - 2-velocity at left and right edges of each cell
c    wl,rs  - 3-velocity at left and right edges of each cell
c
c  EXTERNALS:
c
c  LOCALS:
c
c  PARAMETERS:
c    ft     - a constant used in eq. 1.124 (=2*2/3)
c
c-----------------------------------------------------------------------
c
      implicit NONE
c
      integer ijkn
      parameter (ijkn=MAX_ANY_SINGLE_DIRECTION)
c
c-----------------------------------------------------------------------
c
c  argument declarations
c
c     NOT ACCESSED: eta1
      integer gravity, i1, i2, idim, idual, iflatten, ipresfree, isteep,
     &        j1, j2, jdim, ncolor
      ENZO_REAL dt, eta1, eta2, gamma
      ENZO_REAL dslice(idim,jdim),     dxi,  pslice(idim,jdim),
     &       uslice(idim,jdim),  vslice(idim,jdim),  wslice(idim,jdim),
     &      grslice(idim,jdim), geslice(idim,jdim)
      ENZO_REAL
     &          dls(idim,jdim),     drs(idim,jdim), flatten(idim,jdim),
     &          pls(idim,jdim),
     &          prs(idim,jdim),    gels(idim,jdim),    gers(idim,jdim),
     &          uls(idim,jdim),     urs(idim,jdim),     vls(idim,jdim),
     &          vrs(idim,jdim),     wls(idim,jdim),     wrs(idim,jdim)
      ENZO_REAL colslice(idim,jdim,ncolor),   colls(idim,jdim,ncolor),
     &        colrs(idim,jdim,ncolor)
c
c  local declarations
c
      integer iii,jjj
      integer i, j, ic
      ENZO_REAL steepen(ijkn),
     &     tmp1(ijkn),tmp2(ijkn),tmp3(ijkn),tmp4(ijkn)
      ENZO_REAL qa,qb,qc,qd,s1,s2      
      ENZO_REAL  c1, c2, c3, c4, c5, c6
     &    ,dp(ijkn),pl(ijkn),pr(ijkn),p6(ijkn)
     &    ,du(ijkn),ul(ijkn),ur(ijkn),u6(ijkn)
     &    ,dla(ijkn),dra(ijkn),pla(ijkn),pra(ijkn),ula(ijkn),ura(ijkn)
     &    ,vla(ijkn),vra(ijkn),wla(ijkn),wra(ijkn)
     &    ,plm(ijkn),prm(ijkn),ulm(ijkn),urm(ijkn)
     &    ,dl0(ijkn),dr0(ijkn),pl0(ijkn),pr0(ijkn)
     &    ,plp(ijkn),prp(ijkn),ulp(ijkn),urp(ijkn),ul0(ijkn),ur0(ijkn)
     &    ,vl0(ijkn),vr0(ijkn),wl0(ijkn),wr0(ijkn)
     &    , cs(ijkn),d2d(ijkn),dxb(ijkn)
     &    , cm(ijkn), c0(ijkn), cp(ijkn),char1(ijkn),char2(ijkn)
     &    ,betalp(ijkn),betalm(ijkn),betal0(ijkn),cla(ijkn)
     &    ,betarp(ijkn),betarm(ijkn),betar0(ijkn),cra(ijkn)
     &    ,gela(ijkn),gera(ijkn),gel0(ijkn),ger0(ijkn)
     &    ,colla(ijkn,MAX_COLOR),colra(ijkn,MAX_COLOR)
     &     ,coll0(ijkn,MAX_COLOR),colr0(ijkn,MAX_COLOR)
c     
c     parameters
c     
      ENZO_REAL ft
      parameter(ft = 4.0/3.0)
c     
c\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/////////////////////////////////////
c=======================================================================
c     
c     write(6,*) 'INTEULER: dt =',dt,' isteep =',isteep
c     write(6,*) 'INTEULER: iflatten =',iflatten
c     write(6,*) 'INTEULER: idim =',idim,' jdim =',jdim
c     write(6,*) 'INTEULER: i1   =',i1,  ' i2   =',i2
c     write(6,*) 'INTEULER: j1   =',j1,  ' j2   =',j2
c     
c     Compute coefficients used in interpolation formulae (from eq. 1.6)
c     

      qa    = dxi/(dxi + dxi + dxi)
      c1 = qa*(2.0*dxi + dxi)/(dxi + dxi)
      c2 = qa*(2.0*dxi + dxi)/(dxi + dxi)
c     
      qa    = dxi + dxi + dxi + dxi
      qb    = dxi/(dxi + dxi)
      qc    = (dxi + dxi)/(2.0*dxi + dxi)
      qd    = (dxi + dxi)/(2.0*dxi + dxi)
      qb    = qb + 2.0*dxi*qb/qa*(qc-qd)
      c3 = 1.0 - qb
      c4 = qb
      c5 =  dxi/qa*qd
      c6 = -dxi/qa*qc
c
c    Loop over sweep lines (in this slice)
c
      do 400 j=j1, j2
c
c     Precompute steepening coefficients if needed (eqns 1.14-1.17, plus 3.2)
c
      if (isteep .ne. 0) then
         do i=i1-2,i2+2
            qa     = dxi + dxi + dxi
            d2d(i) = (dslice(i+1,j) - dslice(i,j))/(dxi + dxi)
            d2d(i) = (d2d(i) - (dslice(i,j)-dslice(i-1,j))
     &               /(dxi+dxi))/qa
            dxb(i) = 0.5*(dxi + dxi)
         enddo
         do i=i1-1,i2+1
            qc = abs(dslice(i+1,j) - dslice(i-1,j))
     &           - 0.01*min(abs(dslice(i+1,j)),abs(dslice(i-1,j)))
            s1 = (d2d(i-1) - d2d(i+1))*(dxb(i-1)**3 + dxb(i)**3)
     &           /((dxb(i) + dxb(i-1))*
     &           (dslice(i+1,j) - dslice(i-1,j) + tiny))
            if (d2d(i+1)*d2d(i-1) .gt. 0.0) s1 = 0.0
            if (qc .le. 0.0) s1 = 0.0
            s2 = max(0.0, min(20.0*(s1-0.05), 1.0))
            qa = abs(dslice(i+1,j) - dslice(i-1,j))/
     &           min(dslice(i+1,j),  dslice(i-1,j))
            qb = abs(pslice(i+1,j) - pslice(i-1,j))/
     &           min(pslice(i+1,j),  pslice(i-1,j))
            if (gamma*0.1*qa .ge. qb) then
               steepen(i) = s2
            else
               steepen(i) = 0.0
            endif
         enddo
      endif
c
c     Precompute left and right characteristic distances
c
      do i=i1-1,i2+1
         cs(i) = sqrt(gamma*pslice(i,j)/dslice(i,j))
         if (ipresfree .eq. 1) cs(i) = tiny
         char1(i) = max(0.0, dt*(uslice(i,j)+cs(i)))/(2.0*dxi)
         char2(i) = max(0.0,-dt*(uslice(i,j)-cs(i)))/(2.0*dxi)
      enddo
c
c Brought dt and dx terms here to avoid possible overflows [BWO v1_1_6]
c
      do i=i1-1,i2+1
         cm(i) = (uslice(i,j)-cs(i))*(dt/(2.0*dxi))
         c0(i) = (uslice(i,j)      )*(dt/(2.0*dxi))
         cp(i) = (uslice(i,j)+cs(i))*(dt/(2.0*dxi))
      enddo
c
c     Compute left and right states for each variable
c       (steepening, if requested, is only applied to density)
c
      call intvar(dslice(1,j), idim, i1, i2, isteep, steepen, iflatten, 
     &            flatten, c1, c2, c3, c4, c5, c6, char1, char2, c0,
     &            tmp1, tmp2, tmp3, tmp4, dla, dra, dl0, dr0)
c
      call intvar(pslice(1,j), idim, i1, i2, 0     , steepen, iflatten, 
     &            flatten, c1, c2, c3, c4, c5, c6, char1, char2, c0,
     &            dp, pl, pr, p6, pla, pra, pl0, pr0)
c
      call intvar(uslice(1,j), idim, i1, i2, 0     , steepen, iflatten, 
     &            flatten, c1, c2, c3, c4, c5, c6, char1, char2, c0,
     &            du, ul, ur, u6, ula, ura, ul0, ur0)
c
      call intvar(vslice(1,j), idim, i1, i2, 0     , steepen, iflatten, 
     &            flatten, c1, c2, c3, c4, c5, c6, char1, char2, c0,
     &            tmp1, tmp2, tmp3, tmp4, vla, vra, vl0, vr0)
c
      call intvar(wslice(1,j), idim, i1, i2, 0     , steepen, iflatten, 
     &            flatten, c1, c2, c3, c4, c5, c6, char1, char2, c0,
     &            tmp1, tmp2, tmp3, tmp4, wla, wra, wl0, wr0)
c
      if (idual .eq. 1)
     & call intvar(geslice(1,j), idim, i1, i2, 0   , steepen, iflatten, 
     &            flatten, c1, c2, c3, c4, c5, c6, char1, char2, c0,
     &            tmp1, tmp2, tmp3, tmp4, gela, gera, gel0, ger0)
c
      do ic=1,ncolor
        call intvar(colslice(1,j,ic), idim, i1,i2, 0, steepen, iflatten,
     &            flatten, c1, c2, c3, c4, c5, c6, char1, char2, c0,
     &            tmp1, tmp2, tmp3, tmp4, colla(1,ic), colra(1,ic),
     &            coll0(1,ic), colr0(1,ic))
      enddo
c
#ifdef UNUSED
      do i=i1,i2+1
         if (dla(i)/dslice(i-1,j) .gt. 4.0) then
            write(6,*) 'inteuler left:',i,j
            write(6,*) dla(i-1),dla(i),dla(i+1)
            write(6,*) dslice(i-1,j),dslice(i,j),dslice(i+1,j)
            write(6,*) dra(i-1),dra(i),dra(i+1)
            write(6,*) ula(i-1),ula(i),ula(i+1)
            write(6,*) pla(i-1),pla(i),pla(i+1)
            write(6,*) uslice(i-1,j),uslice(i,j),uslice(i+1,j)
         endif
      enddo
#endif /* UNUSED */
c
c
c Correct the initial guess from the linearized gas equations
c
c     First, compute averge over characteristic domain of dependance (3.5)
c
c
      do i=i1,i2+1
        plm(i)= pr(i-1)-cm(i-1)*(dp(i-1)-(1.0-ft*cm(i-1))*p6(i-1))
        prm(i)= pl(i  )-cm(i  )*(dp(i  )+(1.0+ft*cm(i  ))*p6(i  ))
        plp(i)= pr(i-1)-cp(i-1)*(dp(i-1)-(1.0-ft*cp(i-1))*p6(i-1))
        prp(i)= pl(i  )-cp(i  )*(dp(i  )+(1.0+ft*cp(i  ))*p6(i  ))
      enddo
c
      do i=i1,i2+1
        ulm(i)= ur(i-1)-cm(i-1)*(du(i-1)-(1.0-ft*cm(i-1))*u6(i-1))
        urm(i)= ul(i  )-cm(i  )*(du(i  )+(1.0+ft*cm(i  ))*u6(i  ))
        ulp(i)= ur(i-1)-cp(i-1)*(du(i-1)-(1.0-ft*cp(i-1))*u6(i-1))
        urp(i)= ul(i  )-cp(i  )*(du(i  )+(1.0+ft*cp(i  ))*u6(i  ))
      enddo
c
c     Compute correction terms (3.7)
c
c     Changed the order of operations here to prevent overflow [BWO v1_1_6]

      do i = i1, i2+1
c     cla(i) = sqrt(max(gamma*pla(i)*dla(i), 0.0))
c     cra(i) = sqrt(max(gamma*pra(i)*dra(i), 0.0))
         cla(i) = sqrt(max(gamma*pla(i),0.0)) * sqrt(max(dla(i),0.0))
         cra(i) = sqrt(max(gamma*pra(i),0.0)) * sqrt(max(dra(i),0.0))
         if ( (cla(i) .gt. huge) .or.
     &        (cla(i) .eq.0)) then
            write(6,*) 'inteuler: cla', i,cla(i), pla(i),dla(i)
            write(6,*) 'inteuler: i1,i2',i1,i2
            do iii=1,idim
            do jjj=1,jdim
               write(6,*) 'dslice ',iii,jjj,dslice(iii,jjj)
            end do
            end do
            stop
         endif
         if ( (cra(i) .gt. huge) .or.
     &        (cra(i).eq.0)) then
            write(6,*) 'inteuler: cra', i,cra(i), pra(i),dra(i)
            stop
         endif
      enddo
c
c     a) left side
c
      do i = i1, i2+1
         betalp(i) = (ula(i)-ulp(i)) + (pla(i)-plp(i))/cla(i)
         betalm(i) = (ula(i)-ulm(i)) - (pla(i)-plm(i))/cla(i)
         betal0(i) = ((pla(i)-pl0(i))/cla(i))/cla(i) + 
     &               1.0/dla(i) - 1.0/dl0(i)
      enddo
c
c     Add gravity component
c      
      if (gravity .eq. 1) then
        do i = i1, i2+1
c          if (cla(i) .gt. 0.3*ula(i)) then
c          if (gamma*pla(i)/dla(i) .gt. eta2*ula(i)**2) then
           betalp(i) = betalp(i) - 0.25*dt*(grslice(i-1,j)+grslice(i,j))
           betalm(i) = betalm(i) - 0.25*dt*(grslice(i-1,j)+grslice(i,j))
c          endif
        enddo
      endif
c
      do i = i1, i2+1
         betalp(i) = -betalp(i)/(2.0*cla(i))
         betalm(i) = +betalm(i)/(2.0*cla(i))
      enddo
c
      do i = i1, i2+1
         if (cp(i-1) .le. 0) betalp(i) = 0
         if (cm(i-1) .le. 0) betalm(i) = 0
         if (c0(i-1) .le. 0) betal0(i) = 0
      enddo
c
c     b) right side
c
      do i = i1, i2+1
         betarp(i) = (ura(i)-urp(i)) + (pra(i)-prp(i))/cra(i)
         betarm(i) = (ura(i)-urm(i)) - (pra(i)-prm(i))/cra(i)
         betar0(i) = ((pra(i)-pr0(i))/cra(i))/cra(i) + 
     &               1.0/dra(i) - 1.0/dr0(i)
      enddo
c
c     Add gravity component
c      
      if (gravity .eq. 1) then
        do i = i1, i2+1
c          if (cra(i) .gt. 0.3*ura(i)) then
c           if (gamma*pra(i)/dra(i) .gt. eta2*ura(i)**2) then
           betarp(i) = betarp(i) - 0.25*dt*(grslice(i-1,j)+grslice(i,j))
           betarm(i) = betarm(i) - 0.25*dt*(grslice(i-1,j)+grslice(i,j))
c          endif
        enddo
      endif
c
      do i = i1, i2+1
         betarp(i) = -betarp(i)/(2.0*cra(i))
         betarm(i) = +betarm(i)/(2.0*cra(i))
      enddo
c
      do i = i1, i2+1
         if (cp(i) .ge. 0) betarp(i) = 0
         if (cm(i) .ge. 0) betarm(i) = 0
         if (c0(i) .ge. 0) betar0(i) = 0
      enddo
c
c     Finally, combine to create corrected left/right states (eq. 3.6)
c
      do i=i1,i2+1
c   Changed order of operations to avoid overflow in 32 bits [BWO v1_1_6]
         pls(i,j) = pla(i) + ((betalp(i)+betalm(i))*cla(i))*cla(i)
         prs(i,j) = pra(i) + ((betarp(i)+betarm(i))*cra(i))*cra(i)
c
         uls(i,j) = ula(i) + (betalp(i)-betalm(i))*cla(i)
         urs(i,j) = ura(i) + (betarp(i)-betarm(i))*cra(i)
c
         dls(i,j) = 1.0/(1.0/dla(i) - (betal0(i)+betalp(i)+betalm(i)))
         drs(i,j) = 1.0/(1.0/dra(i) - (betar0(i)+betarp(i)+betarm(i)))
      enddo
c
c     Take the appropriate state from the advected variables
c
      do i=i1,i2+1
         if (uslice(i-1,j) .le. 0.0) then
            vls(i,j)  = vla(i)
            wls(i,j)  = wla(i)
            gels(i,j) = gela(i)
         else
            vls(i,j)  = vl0(i)
            wls(i,j)  = wl0(i)
            gels(i,j) = gel0(i)
         endif
c
         if (uslice(i  ,j) .ge. 0.0) then
            vrs(i,j)  = vra(i)
            wrs(i,j)  = wra(i)
            gers(i,j) = gera(i)
         else
            vrs(i,j)  = vr0(i)
            wrs(i,j)  = wr0(i)
            gers(i,j) = ger0(i)
         endif
      enddo
c
      do ic=1,ncolor
         do i=i1,i2+1
            if (uslice(i-1,j) .le. 0.0) then
               colls(i,j,ic) = colla(i,ic)
            else
               colls(i,j,ic) = coll0(i,ic)
            endif
c
            if (uslice(i  ,j) .ge. 0.0) then
               colrs(i,j,ic) = colra(i,ic)
            else
               colrs(i,j,ic) = colr0(i,ic)
            endif
         enddo
      enddo
c
c  Dual energy formalism: if sound speed squared is less than eta1*v^2 
c    then discard the corrections and use pla, ula, dla.  This amounts
c     to assuming that we are outside the shocked region but the flow is
c     hypersonic so this should be true.  This is inserted because the
c     corrections are inaccurate for hypersonic flows.
c
      if (idual .eq. 1) then
         do i=i1, i2+1
            if (gamma*pla(i)/dla(i) .lt. (eta2*ula(i))*ula(i) .or.
     &          max(abs(cm(i-1)),abs(c0(i-1)),abs(cp(i-1))) .lt. 
     &          1.0e-3
     &          .or. dls(i,j)/dla(i) .gt. 5.0) then
               pls(i,j) = pla(i)
               uls(i,j) = ula(i)
               dls(i,j) = dla(i)
            endif
            if (gamma*pra(i)/dra(i) .lt. (eta2*ura(i))*ura(i) .or.
     &          max(abs(cm(i)),abs(c0(i)),abs(cp(i))) .lt. 1.0e-3
     &          .or. drs(i,j)/dra(i) .gt. 5.0) then
               prs(i,j) = pra(i)
               urs(i,j) = ura(i)
               drs(i,j) = dra(i)
            endif
         enddo
      endif
c
c     Testing code
c
#define CORRECTION
c
#ifdef NO_CORRECTION
      do i=i1, i2+1
         pls(i,j) = pla(i)
         uls(i,j) = ula(i)
         dls(i,j) = dla(i)
         prs(i,j) = pra(i)
         urs(i,j) = ura(i)
         drs(i,j) = dra(i)
      enddo
#endif /* NO_CORRECTION */
c
c     Enforce minimum values.
c
      do i=i1, i2+1
         pls(i,j) = max(pls(i,j), tiny)
         prs(i,j) = max(prs(i,j), tiny)
         dls(i,j) = max(dls(i,j), tiny)
         drs(i,j) = max(drs(i,j), tiny)
      enddo
c
c     If approximating pressure free conditions, then the density should be
c       reset to the pre-corrected state.
c
      if (ipresfree .eq. 1) then
         do i=i1, i2+1
            dls(i,j) = dla(i)
            drs(i,j) = dra(i)
         enddo
      endif
c
400   continue
c
      return
      end
