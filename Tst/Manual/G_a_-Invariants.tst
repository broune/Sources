LIB "tst.lib"; tst_init();
  LIB "ainvar.lib";
  int n=5;
  int i;
  ring s=32003,(x(1..n)),wp(1,2,3,4,5);
  // definition of the vectorfield m=sum m[i,1]*d/dx(i)
  matrix m[n][1];
  for (i=1;i<=n-1;i=i+1)
  {
     m[i+1,1]=x(i);
  }
  // computation of the ring of invariants
  ideal in=invariantRing(m,x(2),x(1),0);
  in;   //invariant ring is generated by 5 invariants
  ring q=32003,(x,y,z,u,v,w),dp;
  matrix m[6][1];
  m[2,1]=x;
  m[3,1]=y;
  m[5,1]=u;
  m[6,1]=v;
  // the vectorfield is: xd/dy+yd/dz+ud/dv+vd/dw
  ideal in=invariantRing(m,y,x,0);
  in; //invariant ring is generated by 6 invariants
tst_status(1);$
