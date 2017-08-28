/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _PRIMEFIB_H_RPCGEN
#define _PRIMEFIB_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


#define PRIMEFIB_PROG 0x20000076
#define PRIMEFIB_VERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define PRIME 1
extern  int * prime_1(int *, CLIENT *);
extern  int * prime_1_svc(int *, struct svc_req *);
#define FIBO 2
extern  long * fibo_1(int *, CLIENT *);
extern  long * fibo_1_svc(int *, struct svc_req *);
extern int primefib_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define PRIME 1
extern  int * prime_1();
extern  int * prime_1_svc();
#define FIBO 2
extern  long * fibo_1();
extern  long * fibo_1_svc();
extern int primefib_prog_1_freeresult ();
#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_PRIMEFIB_H_RPCGEN */